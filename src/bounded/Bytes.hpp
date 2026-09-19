#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <vector>

#include "CapacityTraits.hpp"
#include "MemoryBoundedTraits.hpp"
#include "TypeConversionAdapter.hpp"
#include "detail/ByteOperationsPolicy.hpp"
#include "detail/CapacityValidation.hpp"
#include "detail/SizeCounter.hpp"
#include "detail/TypeNormalization.hpp"

namespace ESPressio::Bounded {

    /// Describes the outcome of assigning a complete candidate byte sequence to bounded Bytes.
    enum class BytesAssignmentResult : std::uint8_t {
        Succeeded = 0,
        CapacityExceeded = 1,
        NullSourcePointer = 2
    };

    /// Describes the outcome of appending a candidate byte sequence to bounded Bytes.
    enum class BytesAppendResult : std::uint8_t {
        Succeeded = 0,
        CapacityExceeded = 1,
        NullSourcePointer = 2
    };

    /// Describes the outcome of inserting a candidate byte sequence into bounded Bytes.
    enum class BytesInsertResult : std::uint8_t {
        Succeeded = 0,
        CapacityExceeded = 1,
        NullSourcePointer = 2,
        IndexOutOfRange = 3
    };

    /// Describes the outcome of erasing a logical byte range from bounded Bytes.
    enum class BytesEraseResult : std::uint8_t {
        Succeeded = 0,
        IndexOutOfRange = 1,
        RangeOutOfBounds = 2
    };

    /// Describes the outcome of appending one byte to bounded Bytes.
    enum class BytesPushBackResult : std::uint8_t {
        Succeeded = 0,
        CapacityExceeded = 1
    };

    /// Describes the outcome of removing the final logical byte from bounded Bytes.
    enum class BytesPopBackResult : std::uint8_t {
        Succeeded = 0,
        Empty = 1
    };

    /// Describes the outcome of changing the logical length of bounded Bytes.
    enum class BytesResizeResult : std::uint8_t {
        Succeeded = 0,
        CapacityExceeded = 1
    };

    /// Describes the outcome of checked byte access.
    enum class BytesAccessResult : std::uint8_t {
        Succeeded = 0,
        IndexOutOfRange = 1
    };

    /// Stores a variable-length byte sequence with compile-time capacity, fully inline storage, and no dynamic allocation.
    /// @tparam TCapacity Maximum logical capacity in bytes.
    /// @tparam TByteOperationsProvider Stateless EDP-Memory ByteOperations provider selected at compile time.
    template<
        std::size_t TCapacity,
        class TByteOperationsProvider = Detail::DefaultByteOperationsProvider
    >
    class Bytes final {

    private:

        /// Compile-time zero-state raw byte-operation policy used by these bounded Bytes.
        using ByteOperations = Detail::ByteOperationsPolicy<TByteOperationsProvider>;

        // Storage state.

        /// Stores the complete maximum byte payload inline.
        std::array<std::uint8_t, TCapacity> _storage{};

        /// Stores the current logical byte length using the smallest suitable unsigned integer Type.
        Detail::SizeCounter<TCapacity> _size = 0;

        // Source-overlap helpers.

        /// Reports whether a complete source range resides inside this Bytes instance's current logical payload.
        bool IsSourceInsideActiveStorage(
            const std::uint8_t* source,
            std::size_t length,
            std::size_t& sourceOffset
        ) const noexcept {
            if (length == 0U) {
                sourceOffset = 0U;
                return false;
            }

            const auto storageBegin = reinterpret_cast<std::uintptr_t>(_storage.data());
            const auto storageEnd = storageBegin + Size();
            const auto sourceBegin = reinterpret_cast<std::uintptr_t>(source);
            const auto sourceEnd = sourceBegin + length;

            if (
                sourceBegin >= storageBegin &&
                sourceEnd <= storageEnd &&
                sourceEnd >= sourceBegin
            ) {
                sourceOffset = static_cast<std::size_t>(sourceBegin - storageBegin);
                return true;
            }

            sourceOffset = 0U;
            return false;
        }

        // Internal mutation helpers.

        /// Copies a known-valid bounded Bytes payload without performing fallible validation.
        /// @tparam TSourceCapacity Compile-time capacity of the source bounded value.
        /// @tparam TSourceByteOperationsProvider ByteOperations provider selected by the source bounded value.
        template<std::size_t TSourceCapacity, class TSourceByteOperationsProvider>
        void CopyFromKnownValid(
            const Bytes<TSourceCapacity, TSourceByteOperationsProvider>& source
        ) noexcept {
            const auto sourceSize = source.Size();

            if (sourceSize > 0U) {
                ByteOperations::MoveBytes(
                    _storage.data(),
                    source.Data(),
                    sourceSize
                );
            }

            _size = static_cast<Detail::SizeCounter<TCapacity>>(sourceSize);
        }

        /// Assigns an explicitly sized byte sequence after validating pointer safety and capacity.
        BytesAssignmentResult AssignBytes(
            const std::uint8_t* source,
            std::size_t length
        ) noexcept {
            if (source == nullptr && length > 0U) { return BytesAssignmentResult::NullSourcePointer; }

            if (ValidateCapacity<TCapacity>(length) != CapacityValidationResult::WithinCapacity) {
                return BytesAssignmentResult::CapacityExceeded;
            }

            if (length > 0U) {
                ByteOperations::MoveBytes(
                    _storage.data(),
                    source,
                    length
                );
            }

            _size = static_cast<Detail::SizeCounter<TCapacity>>(length);
            return BytesAssignmentResult::Succeeded;
        }

        /// Appends an explicitly sized byte sequence after validating pointer safety and resulting capacity.
        BytesAppendResult AppendBytes(
            const std::uint8_t* source,
            std::size_t length
        ) noexcept {
            if (source == nullptr && length > 0U) { return BytesAppendResult::NullSourcePointer; }

            if (length > TCapacity - Size()) { return BytesAppendResult::CapacityExceeded; }

            const auto previousSize = Size();

            if (length > 0U) {
                ByteOperations::MoveBytes(
                    _storage.data() + previousSize,
                    source,
                    length
                );
            }

            _size = static_cast<Detail::SizeCounter<TCapacity>>(previousSize + length);
            return BytesAppendResult::Succeeded;
        }

        /// Inserts an explicitly sized byte sequence while preserving a source range that aliases the current payload.
        BytesInsertResult InsertBytes(
            std::size_t index,
            const std::uint8_t* source,
            std::size_t length
        ) noexcept {
            const auto previousSize = Size();

            if (index > previousSize) { return BytesInsertResult::IndexOutOfRange; }

            if (source == nullptr && length > 0U) { return BytesInsertResult::NullSourcePointer; }

            if (length > TCapacity - previousSize) { return BytesInsertResult::CapacityExceeded; }

            if (length == 0U) { return BytesInsertResult::Succeeded; }

            std::size_t sourceOffset = 0U;
            const bool sourceAliasesPayload = IsSourceInsideActiveStorage(
                source,
                length,
                sourceOffset
            );

            ByteOperations::MoveBytes(
                _storage.data() + index + length,
                _storage.data() + index,
                previousSize - index
            );

            if (!sourceAliasesPayload) {
                ByteOperations::MoveBytes(
                    _storage.data() + index,
                    source,
                    length
                );
            } else if (sourceOffset + length <= index) {
                ByteOperations::MoveBytes(
                    _storage.data() + index,
                    _storage.data() + sourceOffset,
                    length
                );
            } else if (sourceOffset >= index) {
                ByteOperations::MoveBytes(
                    _storage.data() + index,
                    _storage.data() + sourceOffset + length,
                    length
                );
            } else {
                const auto prefixLength = index - sourceOffset;
                const auto suffixLength = length - prefixLength;

                if (prefixLength > 0U) {
                    ByteOperations::MoveBytes(
                        _storage.data() + index,
                        _storage.data() + sourceOffset,
                        prefixLength
                    );
                }

                if (suffixLength > 0U) {
                    ByteOperations::MoveBytes(
                        _storage.data() + index + prefixLength,
                        _storage.data() + index + length,
                        suffixLength
                    );
                }
            }

            _size = static_cast<Detail::SizeCounter<TCapacity>>(previousSize + length);
            return BytesInsertResult::Succeeded;
        }

    public:

        /// Creates an empty bounded byte sequence with all maximum storage already reserved inline.
        constexpr Bytes() noexcept = default;

        /// Copies a bounded byte sequence of identical capacity.
        Bytes(
            const Bytes& source
        ) noexcept {
            CopyFromKnownValid(source);
        }

        /// Moves a bounded byte sequence of identical capacity and leaves the source empty.
        Bytes(
            Bytes&& source
        ) noexcept {
            CopyFromKnownValid(source);
            source.Clear();
        }

        /// Copies bounded Bytes whose compile-time capacity cannot exceed this destination's capacity.
        /// @tparam TSourceCapacity Compile-time capacity of the source bounded value.
        /// @tparam TSourceByteOperationsProvider ByteOperations provider selected by the source bounded value.
        template<std::size_t TSourceCapacity, class TSourceByteOperationsProvider, std::enable_if_t<(TSourceCapacity <= TCapacity), int> = 0>
        Bytes(
            const Bytes<TSourceCapacity, TSourceByteOperationsProvider>& source
        ) noexcept {
            CopyFromKnownValid(source);
        }

        /// Moves bounded Bytes whose compile-time capacity cannot exceed this destination's capacity and leaves the source empty.
        /// @tparam TSourceCapacity Compile-time capacity of the source bounded value.
        /// @tparam TSourceByteOperationsProvider ByteOperations provider selected by the source bounded value.
        template<std::size_t TSourceCapacity, class TSourceByteOperationsProvider, std::enable_if_t<(TSourceCapacity <= TCapacity), int> = 0>
        Bytes(
            Bytes<TSourceCapacity, TSourceByteOperationsProvider>&& source
        ) noexcept {
            CopyFromKnownValid(source);
            source.Clear();
        }

        // Assignment operators.

        /// Copies bounded Bytes of identical capacity.
        Bytes& operator=(
            const Bytes& source
        ) noexcept {
            if (this != &source) {
                CopyFromKnownValid(source);
            }

            return *this;
        }

        /// Moves bounded Bytes of identical capacity and leaves the source empty.
        Bytes& operator=(
            Bytes&& source
        ) noexcept {
            if (this != &source) {
                CopyFromKnownValid(source);
                source.Clear();
            }

            return *this;
        }

        /// Copies bounded Bytes whose compile-time capacity cannot exceed this destination's capacity.
        /// @tparam TSourceCapacity Compile-time capacity of the source bounded value.
        /// @tparam TSourceByteOperationsProvider ByteOperations provider selected by the source bounded value.
        template<std::size_t TSourceCapacity, class TSourceByteOperationsProvider, std::enable_if_t<(TSourceCapacity <= TCapacity), int> = 0>
        Bytes& operator=(
            const Bytes<TSourceCapacity, TSourceByteOperationsProvider>& source
        ) noexcept {
            CopyFromKnownValid(source);
            return *this;
        }

        /// Moves bounded Bytes whose compile-time capacity cannot exceed this destination's capacity and leaves the source empty.
        /// @tparam TSourceCapacity Compile-time capacity of the source bounded value.
        /// @tparam TSourceByteOperationsProvider ByteOperations provider selected by the source bounded value.
        template<std::size_t TSourceCapacity, class TSourceByteOperationsProvider, std::enable_if_t<(TSourceCapacity <= TCapacity), int> = 0>
        Bytes& operator=(
            Bytes<TSourceCapacity, TSourceByteOperationsProvider>&& source
        ) noexcept {
            CopyFromKnownValid(source);
            source.Clear();
            return *this;
        }

        // Capacity and state inspection.

        /// Returns the maximum number of logical bytes this container can retain.
        static constexpr std::size_t Capacity() noexcept {
            return TCapacity;
        }

        /// Returns the current logical byte count.
        constexpr std::size_t Size() const noexcept {
            return static_cast<std::size_t>(_size);
        }

        /// Reports whether the logical byte sequence is empty.
        constexpr bool IsEmpty() const noexcept {
            return Size() == 0U;
        }

        /// Reports whether the logical byte sequence occupies its complete capacity.
        constexpr bool IsFull() const noexcept {
            return Size() == TCapacity;
        }

        // Direct logical-byte access.

        /// Returns a mutable pointer to the inline byte storage.
        std::uint8_t* Data() noexcept {
            return _storage.data();
        }

        /// Returns a read-only pointer to the inline byte storage.
        const std::uint8_t* Data() const noexcept {
            return _storage.data();
        }

        /// Returns a mutable reference to a logical byte at an index that must already satisfy index < Size().
        std::uint8_t& operator[](
            std::size_t index
        ) noexcept {
            return _storage[index];
        }

        /// Returns a read-only reference to a logical byte at an index that must already satisfy index < Size().
        const std::uint8_t& operator[](
            std::size_t index
        ) const noexcept {
            return _storage[index];
        }

        /// Retrieves a mutable pointer to one logical byte through a checked access path.
        BytesAccessResult GetAt(
            std::size_t index,
            std::uint8_t*& value
        ) noexcept {
            if (index >= Size()) {
                value = nullptr;
                return BytesAccessResult::IndexOutOfRange;
            }

            value = _storage.data() + index;
            return BytesAccessResult::Succeeded;
        }

        /// Retrieves a read-only pointer to one logical byte through a checked access path.
        BytesAccessResult GetAt(
            std::size_t index,
            const std::uint8_t*& value
        ) const noexcept {
            if (index >= Size()) {
                value = nullptr;
                return BytesAccessResult::IndexOutOfRange;
            }

            value = _storage.data() + index;
            return BytesAccessResult::Succeeded;
        }

        // Iteration.

        /// Returns a mutable iterator to the first logical byte.
        std::uint8_t* begin() noexcept {
            return _storage.data();
        }

        /// Returns a mutable iterator one position beyond the final logical byte.
        std::uint8_t* end() noexcept {
            return _storage.data() + Size();
        }

        /// Returns a read-only iterator to the first logical byte.
        const std::uint8_t* begin() const noexcept {
            return _storage.data();
        }

        /// Returns a read-only iterator one position beyond the final logical byte.
        const std::uint8_t* end() const noexcept {
            return _storage.data() + Size();
        }

        /// Returns a read-only iterator to the first logical byte.
        const std::uint8_t* cbegin() const noexcept {
            return begin();
        }

        /// Returns a read-only iterator one position beyond the final logical byte.
        const std::uint8_t* cend() const noexcept {
            return end();
        }

        // Complete-value assignment.

        /// Assigns another bounded byte sequence after validating this destination's runtime capacity.
        /// @tparam TSourceCapacity Compile-time capacity of the source bounded value.
        /// @tparam TSourceByteOperationsProvider ByteOperations provider selected by the source bounded value.
        template<std::size_t TSourceCapacity, class TSourceByteOperationsProvider>
        BytesAssignmentResult Assign(
            const Bytes<TSourceCapacity, TSourceByteOperationsProvider>& source
        ) noexcept {
            if (source.Size() > TCapacity) { return BytesAssignmentResult::CapacityExceeded; }

            CopyFromKnownValid(source);
            return BytesAssignmentResult::Succeeded;
        }

        /// Assigns a fixed standard byte array.
        /// @tparam TArraySize Fixed source array extent.
        template<std::size_t TArraySize>
        BytesAssignmentResult Assign(
            const std::array<std::uint8_t, TArraySize>& source
        ) noexcept {
            return AssignBytes(
                source.data(),
                source.size()
            );
        }

        /// Assigns a standard dynamic byte vector without retaining its dynamic storage.
        BytesAssignmentResult Assign(
            const std::vector<std::uint8_t>& source
        ) noexcept {
            return AssignBytes(
                source.data(),
                source.size()
            );
        }

        /// Assigns an explicitly sized byte range.
        BytesAssignmentResult Assign(
            const std::uint8_t* source,
            std::size_t length
        ) noexcept {
            return AssignBytes(
                source,
                length
            );
        }

        // Append operations.

        /// Appends another bounded byte sequence without truncation.
        /// @tparam TSourceCapacity Compile-time capacity of the source bounded value.
        /// @tparam TSourceByteOperationsProvider ByteOperations provider selected by the source bounded value.
        template<std::size_t TSourceCapacity, class TSourceByteOperationsProvider>
        BytesAppendResult Append(
            const Bytes<TSourceCapacity, TSourceByteOperationsProvider>& source
        ) noexcept {
            return AppendBytes(
                source.Data(),
                source.Size()
            );
        }

        /// Appends a fixed standard byte array without truncation.
        /// @tparam TArraySize Fixed source array extent.
        template<std::size_t TArraySize>
        BytesAppendResult Append(
            const std::array<std::uint8_t, TArraySize>& source
        ) noexcept {
            return AppendBytes(
                source.data(),
                source.size()
            );
        }

        /// Appends a standard dynamic byte vector without retaining its dynamic storage.
        BytesAppendResult Append(
            const std::vector<std::uint8_t>& source
        ) noexcept {
            return AppendBytes(
                source.data(),
                source.size()
            );
        }

        /// Appends an explicitly sized byte range.
        BytesAppendResult Append(
            const std::uint8_t* source,
            std::size_t length
        ) noexcept {
            return AppendBytes(
                source,
                length
            );
        }

        // Insert operations.

        /// Inserts another bounded byte sequence at a checked logical index.
        /// @tparam TSourceCapacity Compile-time capacity of the source bounded value.
        /// @tparam TSourceByteOperationsProvider ByteOperations provider selected by the source bounded value.
        template<std::size_t TSourceCapacity, class TSourceByteOperationsProvider>
        BytesInsertResult Insert(
            std::size_t index,
            const Bytes<TSourceCapacity, TSourceByteOperationsProvider>& source
        ) noexcept {
            return InsertBytes(
                index,
                source.Data(),
                source.Size()
            );
        }

        /// Inserts a fixed standard byte array at a checked logical index.
        /// @tparam TArraySize Fixed source array extent.
        template<std::size_t TArraySize>
        BytesInsertResult Insert(
            std::size_t index,
            const std::array<std::uint8_t, TArraySize>& source
        ) noexcept {
            return InsertBytes(
                index,
                source.data(),
                source.size()
            );
        }

        /// Inserts a standard dynamic byte vector at a checked logical index.
        BytesInsertResult Insert(
            std::size_t index,
            const std::vector<std::uint8_t>& source
        ) noexcept {
            return InsertBytes(
                index,
                source.data(),
                source.size()
            );
        }

        /// Inserts an explicitly sized byte range at a checked logical index.
        BytesInsertResult Insert(
            std::size_t index,
            const std::uint8_t* source,
            std::size_t length
        ) noexcept {
            return InsertBytes(
                index,
                source,
                length
            );
        }

        // Range and length mutation.

        /// Erases a strict logical byte range without silently clamping an invalid request.
        BytesEraseResult Erase(
            std::size_t index,
            std::size_t count
        ) noexcept {
            const auto currentSize = Size();

            if (index > currentSize) { return BytesEraseResult::IndexOutOfRange; }

            if (count > currentSize - index) { return BytesEraseResult::RangeOutOfBounds; }

            if (count == 0U) { return BytesEraseResult::Succeeded; }

            ByteOperations::MoveBytes(
                _storage.data() + index,
                _storage.data() + index + count,
                currentSize - index - count
            );

            _size = static_cast<Detail::SizeCounter<TCapacity>>(currentSize - count);
            return BytesEraseResult::Succeeded;
        }

        /// Appends one byte when capacity remains available.
        BytesPushBackResult PushBack(
            std::uint8_t value
        ) noexcept {
            if (IsFull()) { return BytesPushBackResult::CapacityExceeded; }

            _storage[Size()] = value;
            _size = static_cast<Detail::SizeCounter<TCapacity>>(Size() + 1U);

            return BytesPushBackResult::Succeeded;
        }

        /// Removes the final logical byte when the sequence is non-empty.
        BytesPopBackResult PopBack() noexcept {
            if (IsEmpty()) { return BytesPopBackResult::Empty; }

            _size = static_cast<Detail::SizeCounter<TCapacity>>(Size() - 1U);
            return BytesPopBackResult::Succeeded;
        }

        /// Changes logical size, initializing every newly exposed byte to the supplied fill value.
        BytesResizeResult Resize(
            std::size_t newSize,
            std::uint8_t fillValue = 0U
        ) noexcept {
            if (newSize > TCapacity) { return BytesResizeResult::CapacityExceeded; }

            const auto previousSize = Size();

            if (newSize > previousSize) {
                ByteOperations::FillBytes(
                    _storage.data() + previousSize,
                    fillValue,
                    newSize - previousSize
                );
            }

            _size = static_cast<Detail::SizeCounter<TCapacity>>(newSize);
            return BytesResizeResult::Succeeded;
        }

        /// Clears the logical byte sequence while retaining all preallocated storage.
        void Clear() noexcept {
            _size = 0;
        }

        // Explicit external-Type conversion.

        /// Converts this bounded byte sequence to a target Type through the target Type owner's compile-time adapter specialization.
        /// @tparam TTarget Explicit conversion target Type.
        template<class TTarget>
        auto CastTo(
            TTarget& target
        ) const noexcept(
            TypeConversionAdapter<
                Bytes<TCapacity, TByteOperationsProvider>,
                Detail::NormalizedType<TTarget>
            >::IsNoexcept
        ) -> typename TypeConversionAdapter<
            Bytes<TCapacity, TByteOperationsProvider>,
            Detail::NormalizedType<TTarget>
        >::ResultType {
            using Adapter = TypeConversionAdapter<
                Bytes<TCapacity, TByteOperationsProvider>,
                Detail::NormalizedType<TTarget>
            >;

            static_assert(
                Adapter::IsAvailable,
                "No TypeConversionAdapter is available for this bounded Bytes target Type."
            );

            return Adapter::Convert(
                *this,
                target
            );
        }

        /// Converts a source Type into this bounded byte sequence through the source Type owner's compile-time adapter specialization.
        /// @tparam TSource Explicit conversion source Type.
        template<class TSource>
        auto CastFrom(
            const TSource& source
        ) noexcept(
            TypeConversionAdapter<
                Detail::NormalizedType<TSource>,
                Bytes<TCapacity, TByteOperationsProvider>
            >::IsNoexcept
        ) -> typename TypeConversionAdapter<
            Detail::NormalizedType<TSource>,
            Bytes<TCapacity, TByteOperationsProvider>
        >::ResultType {
            using Adapter = TypeConversionAdapter<
                Detail::NormalizedType<TSource>,
                Bytes<TCapacity, TByteOperationsProvider>
            >;

            static_assert(
                Adapter::IsAvailable,
                "No TypeConversionAdapter is available for this source Type and bounded Bytes target."
            );

            return Adapter::Convert(
                source,
                *this
            );
        }

        // Logical-value comparisons.

        /// Reports whether two bounded byte sequences contain identical logical bytes.
        /// @tparam TOtherCapacity Compile-time capacity of the compared bounded value.
        /// @tparam TOtherByteOperationsProvider ByteOperations provider selected by the compared bounded value.
        template<std::size_t TOtherCapacity, class TOtherByteOperationsProvider>
        bool operator==(
            const Bytes<TOtherCapacity, TOtherByteOperationsProvider>& other
        ) const noexcept {
            if (Size() != other.Size()) { return false; }

            return ByteOperations::CompareBytes(
                _storage.data(),
                other.Data(),
                Size()
            ) == ESPressio::Memory::ByteComparison::Equal;
        }

        /// Reports whether two bounded byte sequences contain different logical bytes.
        /// @tparam TOtherCapacity Compile-time capacity of the compared bounded value.
        /// @tparam TOtherByteOperationsProvider ByteOperations provider selected by the compared bounded value.
        template<std::size_t TOtherCapacity, class TOtherByteOperationsProvider>
        bool operator!=(
            const Bytes<TOtherCapacity, TOtherByteOperationsProvider>& other
        ) const noexcept {
            return !(
                *this ==
                other
            );
        }

        /// Reports whether this byte sequence sorts lexicographically before another sequence.
        /// @tparam TOtherCapacity Compile-time capacity of the compared bounded value.
        /// @tparam TOtherByteOperationsProvider ByteOperations provider selected by the compared bounded value.
        template<std::size_t TOtherCapacity, class TOtherByteOperationsProvider>
        bool operator<(
            const Bytes<TOtherCapacity, TOtherByteOperationsProvider>& other
        ) const noexcept {
            const auto commonSize = Size() < other.Size()
                ? Size()
                : other.Size();

            const auto comparison = ByteOperations::CompareBytes(
                _storage.data(),
                other.Data(),
                commonSize
            );

            if (comparison == ESPressio::Memory::ByteComparison::Less) { return true; }
            if (comparison == ESPressio::Memory::ByteComparison::Greater) { return false; }

            return Size() < other.Size();
        }

        /// Reports whether this byte sequence is lexicographically before or equal to another sequence.
        /// @tparam TOtherCapacity Compile-time capacity of the compared bounded value.
        /// @tparam TOtherByteOperationsProvider ByteOperations provider selected by the compared bounded value.
        template<std::size_t TOtherCapacity, class TOtherByteOperationsProvider>
        bool operator<=(
            const Bytes<TOtherCapacity, TOtherByteOperationsProvider>& other
        ) const noexcept {
            return !(
                other <
                *this
            );
        }

        /// Reports whether this byte sequence sorts lexicographically after another sequence.
        /// @tparam TOtherCapacity Compile-time capacity of the compared bounded value.
        /// @tparam TOtherByteOperationsProvider ByteOperations provider selected by the compared bounded value.
        template<std::size_t TOtherCapacity, class TOtherByteOperationsProvider>
        bool operator>(
            const Bytes<TOtherCapacity, TOtherByteOperationsProvider>& other
        ) const noexcept {
            return other < *this;
        }

        /// Reports whether this byte sequence is lexicographically after or equal to another sequence.
        /// @tparam TOtherCapacity Compile-time capacity of the compared bounded value.
        /// @tparam TOtherByteOperationsProvider ByteOperations provider selected by the compared bounded value.
        template<std::size_t TOtherCapacity, class TOtherByteOperationsProvider>
        bool operator>=(
            const Bytes<TOtherCapacity, TOtherByteOperationsProvider>& other
        ) const noexcept {
            return !(
                *this <
                other
            );
        }

    };

    /// Certifies every bounded Bytes specialization as a self-contained memory-bounded value.
    /// @tparam TCapacity Compile-time capacity of the bounded value.
    /// @tparam TByteOperationsProvider Stateless ByteOperations provider selected by the bounded value.
    template<std::size_t TCapacity, class TByteOperationsProvider>
    struct MemoryBoundedTraits<Bytes<TCapacity, TByteOperationsProvider>> : MemoryBoundedValueDeclaration<false> {
    };

    /// Exposes the compile-time byte capacity of bounded Bytes.
    /// @tparam TCapacity Compile-time capacity of the bounded value.
    /// @tparam TByteOperationsProvider Stateless ByteOperations provider selected by the bounded value.
    template<std::size_t TCapacity, class TByteOperationsProvider>
    struct CapacityTraits<Bytes<TCapacity, TByteOperationsProvider>> {

        /// Bounded Bytes capacity is known at compile time.
        static constexpr bool HasStaticCapacity = true;

        /// Bounded Bytes logical capacity is measured in bytes.
        static constexpr CapacityUnit Unit = CapacityUnit::Bytes;

        /// Reports the maximum number of logical bytes retained by the container.
        static constexpr std::size_t Capacity = TCapacity;

    };

} // ESPressio::Bounded
