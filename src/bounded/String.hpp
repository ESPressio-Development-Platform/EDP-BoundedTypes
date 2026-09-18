#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include "CapacityTraits.hpp"
#include "MemoryBoundedTraits.hpp"
#include "TypeConversionAdapter.hpp"
#include "detail/CapacityValidation.hpp"
#include "detail/SizeCounter.hpp"
#include "detail/TypeNormalization.hpp"

namespace ESPressio::Bounded {

    /// Describes the outcome of assigning a complete candidate value to a bounded String.
    enum class StringAssignmentResult : std::uint8_t {
        Succeeded = 0,
        CapacityExceeded = 1,
        EmbeddedNullNotPermitted = 2,
        NullSourcePointer = 3,
        SourceNotNullTerminated = 4
    };

    /// Describes the outcome of appending a candidate value to a bounded String.
    enum class StringAppendResult : std::uint8_t {
        Succeeded = 0,
        CapacityExceeded = 1,
        EmbeddedNullNotPermitted = 2,
        NullSourcePointer = 3,
        SourceNotNullTerminated = 4
    };

    /// Describes the outcome of inserting a candidate value into a bounded String.
    enum class StringInsertResult : std::uint8_t {
        Succeeded = 0,
        CapacityExceeded = 1,
        EmbeddedNullNotPermitted = 2,
        NullSourcePointer = 3,
        SourceNotNullTerminated = 4,
        IndexOutOfRange = 5
    };

    /// Describes the outcome of erasing a logical range from a bounded String.
    enum class StringEraseResult : std::uint8_t {
        Succeeded = 0,
        IndexOutOfRange = 1,
        RangeOutOfBounds = 2
    };

    /// Describes the outcome of replacing a logical character in a bounded String.
    enum class StringSetAtResult : std::uint8_t {
        Succeeded = 0,
        IndexOutOfRange = 1,
        NullCharacterNotPermitted = 2
    };

    /// Describes the outcome of appending one character to a bounded String.
    enum class StringPushBackResult : std::uint8_t {
        Succeeded = 0,
        CapacityExceeded = 1,
        NullCharacterNotPermitted = 2
    };

    /// Describes the outcome of removing the final logical character from a bounded String.
    enum class StringPopBackResult : std::uint8_t {
        Succeeded = 0,
        Empty = 1
    };

    /// Describes the outcome of checked character access on a bounded String.
    enum class StringAccessResult : std::uint8_t {
        Succeeded = 0,
        IndexOutOfRange = 1
    };

    /// Stores a variable-length null-terminated text byte sequence with a compile-time payload capacity and no dynamic allocation.
    template<std::size_t TCapacity>
    class String final {

    private:

        // Storage state.

        /// Stores the complete maximum payload and one permanently reserved implicit null terminator.
        std::array<char, TCapacity + 1U> _storage{};

        /// Stores the current logical payload length using the smallest suitable unsigned integer Type.
        Detail::SizeCounter<TCapacity> _size = 0;

        // Validation helpers.

        /// Reports whether the candidate text bytes contain an embedded null character.
        static bool ContainsEmbeddedNull(
            const char* source,
            std::size_t length
        ) noexcept {
            for (std::size_t index = 0; index < length; ++index)
                if (source[index] == '\0') { return true; }

            return false;
        }

        /// Reports whether a complete source range resides inside this String's current logical payload.
        bool IsSourceInsideActiveStorage(
            const char* source,
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

        /// Copies a known-valid bounded String payload without performing fallible validation.
        template<std::size_t TSourceCapacity>
        void CopyFromKnownValid(
            const String<TSourceCapacity>& source
        ) noexcept {
            const auto sourceSize = source.Size();

            if (sourceSize > 0U) {
                std::memmove(
                    _storage.data(),
                    source.Data(),
                    sourceSize
                );
            }

            _size = static_cast<Detail::SizeCounter<TCapacity>>(sourceSize);
            _storage[sourceSize] = '\0';
        }

        /// Assigns candidate text bytes after validating capacity, pointer safety, and the embedded-null invariant.
        StringAssignmentResult AssignBytes(
            const char* source,
            std::size_t length
        ) noexcept {
            if (source == nullptr && length > 0U) { return StringAssignmentResult::NullSourcePointer; }

            if (ValidateCapacity<TCapacity>(length) != CapacityValidationResult::WithinCapacity) {
                return StringAssignmentResult::CapacityExceeded;
            }

            if (length > 0U && ContainsEmbeddedNull(
                source,
                length
            )) {
                return StringAssignmentResult::EmbeddedNullNotPermitted;
            }

            if (length > 0U) {
                std::memmove(
                    _storage.data(),
                    source,
                    length
                );
            }

            _size = static_cast<Detail::SizeCounter<TCapacity>>(length);
            _storage[length] = '\0';

            return StringAssignmentResult::Succeeded;
        }

        /// Appends candidate text bytes after validating the complete resulting logical size and text invariant.
        StringAppendResult AppendBytes(
            const char* source,
            std::size_t length
        ) noexcept {
            if (source == nullptr && length > 0U) { return StringAppendResult::NullSourcePointer; }

            if (length > TCapacity - Size()) { return StringAppendResult::CapacityExceeded; }

            if (length > 0U && ContainsEmbeddedNull(
                source,
                length
            )) {
                return StringAppendResult::EmbeddedNullNotPermitted;
            }

            const auto previousSize = Size();

            if (length > 0U) {
                std::memmove(
                    _storage.data() + previousSize,
                    source,
                    length
                );
            }

            const auto newSize = previousSize + length;
            _size = static_cast<Detail::SizeCounter<TCapacity>>(newSize);
            _storage[newSize] = '\0';

            return StringAppendResult::Succeeded;
        }

        /// Inserts candidate text bytes while preserving any source range that aliases this String's active payload.
        StringInsertResult InsertBytes(
            std::size_t index,
            const char* source,
            std::size_t length
        ) noexcept {
            const auto previousSize = Size();

            if (index > previousSize) { return StringInsertResult::IndexOutOfRange; }

            if (source == nullptr && length > 0U) { return StringInsertResult::NullSourcePointer; }

            if (length > TCapacity - previousSize) { return StringInsertResult::CapacityExceeded; }

            if (length > 0U && ContainsEmbeddedNull(
                source,
                length
            )) {
                return StringInsertResult::EmbeddedNullNotPermitted;
            }

            if (length == 0U) { return StringInsertResult::Succeeded; }

            std::size_t sourceOffset = 0U;
            const bool sourceAliasesPayload = IsSourceInsideActiveStorage(
                source,
                length,
                sourceOffset
            );

            std::memmove(
                _storage.data() + index + length,
                _storage.data() + index,
                previousSize - index
            );

            if (!sourceAliasesPayload) {
                std::memmove(
                    _storage.data() + index,
                    source,
                    length
                );
            } else if (sourceOffset + length <= index) {
                std::memmove(
                    _storage.data() + index,
                    _storage.data() + sourceOffset,
                    length
                );
            } else if (sourceOffset >= index) {
                std::memmove(
                    _storage.data() + index,
                    _storage.data() + sourceOffset + length,
                    length
                );
            } else {
                const auto prefixLength = index - sourceOffset;
                const auto suffixLength = length - prefixLength;

                if (prefixLength > 0U) {
                    std::memmove(
                        _storage.data() + index,
                        _storage.data() + sourceOffset,
                        prefixLength
                    );
                }

                if (suffixLength > 0U) {
                    std::memmove(
                        _storage.data() + index + prefixLength,
                        _storage.data() + index + length,
                        suffixLength
                    );
                }
            }

            const auto newSize = previousSize + length;
            _size = static_cast<Detail::SizeCounter<TCapacity>>(newSize);
            _storage[newSize] = '\0';

            return StringInsertResult::Succeeded;
        }

        // Conversion dispatch helpers.

        /// Dispatches an explicitly requested conversion from this bounded String to a target Type.
        template<class TTarget>
        auto CastToImplementation(
            TTarget& target
        ) const noexcept(
            TypeConversionAdapter<
                String<TCapacity>,
                Detail::NormalizedType<TTarget>
            >::IsNoexcept
        ) -> typename TypeConversionAdapter<
            String<TCapacity>,
            Detail::NormalizedType<TTarget>
        >::ResultType {
            using Adapter = TypeConversionAdapter<
                String<TCapacity>,
                Detail::NormalizedType<TTarget>
            >;

            static_assert(
                Adapter::IsAvailable,
                "No TypeConversionAdapter is available for this bounded String target Type."
            );

            return Adapter::Convert(
                *this,
                target
            );
        }

        /// Dispatches an explicitly requested conversion from a source Type into this bounded String.
        template<class TSource>
        auto CastFromImplementation(
            const TSource& source
        ) noexcept(
            TypeConversionAdapter<
                Detail::NormalizedType<TSource>,
                String<TCapacity>
            >::IsNoexcept
        ) -> typename TypeConversionAdapter<
            Detail::NormalizedType<TSource>,
            String<TCapacity>
        >::ResultType {
            using Adapter = TypeConversionAdapter<
                Detail::NormalizedType<TSource>,
                String<TCapacity>
            >;

            static_assert(
                Adapter::IsAvailable,
                "No TypeConversionAdapter is available for this source Type and bounded String target."
            );

            return Adapter::Convert(
                source,
                *this
            );
        }

    public:

        static_assert(
            TCapacity < static_cast<std::size_t>(-1),
            "Bounded::String capacity must leave addressable storage for the implicit null terminator."
        );

        /// Creates an empty bounded String with its implicit terminator already established.
        constexpr String() noexcept = default;

        /// Copies a bounded String of identical capacity.
        String(
            const String& source
        ) noexcept {
            CopyFromKnownValid(source);
        }

        /// Moves a bounded String of identical capacity and leaves the source empty.
        String(
            String&& source
        ) noexcept {
            CopyFromKnownValid(source);
            source.Clear();
        }

        /// Copies a bounded String whose compile-time capacity cannot exceed this destination's capacity.
        template<std::size_t TSourceCapacity, std::enable_if_t<(TSourceCapacity <= TCapacity && TSourceCapacity != TCapacity), int> = 0>
        String(
            const String<TSourceCapacity>& source
        ) noexcept {
            CopyFromKnownValid(source);
        }

        /// Moves a bounded String whose compile-time capacity cannot exceed this destination's capacity and leaves the source empty.
        template<std::size_t TSourceCapacity, std::enable_if_t<(TSourceCapacity <= TCapacity && TSourceCapacity != TCapacity), int> = 0>
        String(
            String<TSourceCapacity>&& source
        ) noexcept {
            CopyFromKnownValid(source);
            source.Clear();
        }

        // Assignment operators.

        /// Copies a bounded String of identical capacity.
        String& operator=(
            const String& source
        ) noexcept {
            if (this != &source) {
                CopyFromKnownValid(source);
            }

            return *this;
        }

        /// Moves a bounded String of identical capacity and leaves the source empty.
        String& operator=(
            String&& source
        ) noexcept {
            if (this != &source) {
                CopyFromKnownValid(source);
                source.Clear();
            }

            return *this;
        }

        /// Copies a bounded String whose compile-time capacity cannot exceed this destination's capacity.
        template<std::size_t TSourceCapacity, std::enable_if_t<(TSourceCapacity <= TCapacity && TSourceCapacity != TCapacity), int> = 0>
        String& operator=(
            const String<TSourceCapacity>& source
        ) noexcept {
            CopyFromKnownValid(source);
            return *this;
        }

        /// Moves a bounded String whose compile-time capacity cannot exceed this destination's capacity and leaves the source empty.
        template<std::size_t TSourceCapacity, std::enable_if_t<(TSourceCapacity <= TCapacity && TSourceCapacity != TCapacity), int> = 0>
        String& operator=(
            String<TSourceCapacity>&& source
        ) noexcept {
            CopyFromKnownValid(source);
            source.Clear();
            return *this;
        }

        // Capacity and state inspection.

        /// Returns the maximum number of payload bytes this String can retain, excluding the implicit null terminator.
        static constexpr std::size_t Capacity() noexcept {
            return TCapacity;
        }

        /// Returns the current number of logical payload bytes, excluding the implicit null terminator.
        constexpr std::size_t Size() const noexcept {
            return static_cast<std::size_t>(_size);
        }

        /// Reports whether the String currently contains no payload bytes.
        constexpr bool IsEmpty() const noexcept {
            return Size() == 0U;
        }

        /// Reports whether the String currently occupies its complete logical payload capacity.
        constexpr bool IsFull() const noexcept {
            return Size() == TCapacity;
        }

        // Read-only character storage access.

        /// Returns a read-only pointer to the first payload byte, followed by the guaranteed implicit null terminator.
        const char* Data() const noexcept {
            return _storage.data();
        }

        /// Returns a read-only C-compatible null-terminated pointer to this String's payload.
        const char* CStr() const noexcept {
            return _storage.data();
        }

        /// Returns a non-owning standard view over the logical payload bytes, excluding the implicit terminator.
        std::string_view View() const noexcept {
            return std::string_view(
                _storage.data(),
                Size()
            );
        }

        /// Returns the logical character at an index that must already satisfy index < Size().
        char operator[](
            std::size_t index
        ) const noexcept {
            return _storage[index];
        }

        /// Retrieves one logical character through a checked access path.
        StringAccessResult GetAt(
            std::size_t index,
            char& value
        ) const noexcept {
            if (index >= Size()) { return StringAccessResult::IndexOutOfRange; }

            value = _storage[index];
            return StringAccessResult::Succeeded;
        }

        // Read-only iteration.

        /// Returns a read-only iterator to the first logical payload character.
        const char* begin() const noexcept {
            return _storage.data();
        }

        /// Returns a read-only iterator one position beyond the final logical payload character.
        const char* end() const noexcept {
            return _storage.data() + Size();
        }

        /// Returns a read-only iterator to the first logical payload character.
        const char* cbegin() const noexcept {
            return begin();
        }

        /// Returns a read-only iterator one position beyond the final logical payload character.
        const char* cend() const noexcept {
            return end();
        }

        // Complete-value assignment.

        /// Assigns another bounded String after validating this destination's runtime capacity.
        template<std::size_t TSourceCapacity>
        StringAssignmentResult Assign(
            const String<TSourceCapacity>& source
        ) noexcept {
            if (source.Size() > TCapacity) { return StringAssignmentResult::CapacityExceeded; }

            CopyFromKnownValid(source);
            return StringAssignmentResult::Succeeded;
        }

        /// Assigns a standard string view without truncation.
        StringAssignmentResult Assign(
            std::string_view source
        ) noexcept {
            return AssignBytes(
                source.data(),
                source.size()
            );
        }

        /// Assigns a standard dynamic String without retaining any dynamic ownership.
        StringAssignmentResult Assign(
            const std::string& source
        ) noexcept {
            return AssignBytes(
                source.data(),
                source.size()
            );
        }

        /// Assigns an explicitly sized character range without performing an unbounded null scan.
        StringAssignmentResult Assign(
            const char* source,
            std::size_t length
        ) noexcept {
            return AssignBytes(
                source,
                length
            );
        }

        /// Assigns a null-terminated fixed character array after validating its terminator and payload.
        template<std::size_t TArraySize>
        StringAssignmentResult Assign(
            const char (&source)[TArraySize]
        ) noexcept {
            static_assert(
                TArraySize > 0U,
                "A fixed character array used as String input must provide storage for a null terminator."
            );

            if (source[TArraySize - 1U] != '\0') { return StringAssignmentResult::SourceNotNullTerminated; }

            for (std::size_t index = 0U; index + 1U < TArraySize; ++index)
                if (source[index] == '\0') { return StringAssignmentResult::EmbeddedNullNotPermitted; }

            return AssignBytes(
                source,
                TArraySize - 1U
            );
        }

        // Append operations.

        /// Appends another bounded String without truncation.
        template<std::size_t TSourceCapacity>
        StringAppendResult Append(
            const String<TSourceCapacity>& source
        ) noexcept {
            return AppendBytes(
                source.Data(),
                source.Size()
            );
        }

        /// Appends a standard string view without truncation.
        StringAppendResult Append(
            std::string_view source
        ) noexcept {
            return AppendBytes(
                source.data(),
                source.size()
            );
        }

        /// Appends a standard dynamic String without retaining any dynamic ownership.
        StringAppendResult Append(
            const std::string& source
        ) noexcept {
            return AppendBytes(
                source.data(),
                source.size()
            );
        }

        /// Appends an explicitly sized character range without performing an unbounded null scan.
        StringAppendResult Append(
            const char* source,
            std::size_t length
        ) noexcept {
            return AppendBytes(
                source,
                length
            );
        }

        /// Appends a null-terminated fixed character array after validating its terminator and payload.
        template<std::size_t TArraySize>
        StringAppendResult Append(
            const char (&source)[TArraySize]
        ) noexcept {
            static_assert(
                TArraySize > 0U,
                "A fixed character array used as String input must provide storage for a null terminator."
            );

            if (source[TArraySize - 1U] != '\0') { return StringAppendResult::SourceNotNullTerminated; }

            for (std::size_t index = 0U; index + 1U < TArraySize; ++index)
                if (source[index] == '\0') { return StringAppendResult::EmbeddedNullNotPermitted; }

            return AppendBytes(
                source,
                TArraySize - 1U
            );
        }

        // Insert operations.

        /// Inserts another bounded String at a checked logical index.
        template<std::size_t TSourceCapacity>
        StringInsertResult Insert(
            std::size_t index,
            const String<TSourceCapacity>& source
        ) noexcept {
            return InsertBytes(
                index,
                source.Data(),
                source.Size()
            );
        }

        /// Inserts a standard string view at a checked logical index.
        StringInsertResult Insert(
            std::size_t index,
            std::string_view source
        ) noexcept {
            return InsertBytes(
                index,
                source.data(),
                source.size()
            );
        }

        /// Inserts a standard dynamic String at a checked logical index.
        StringInsertResult Insert(
            std::size_t index,
            const std::string& source
        ) noexcept {
            return InsertBytes(
                index,
                source.data(),
                source.size()
            );
        }

        /// Inserts an explicitly sized character range at a checked logical index.
        StringInsertResult Insert(
            std::size_t index,
            const char* source,
            std::size_t length
        ) noexcept {
            return InsertBytes(
                index,
                source,
                length
            );
        }

        /// Inserts a null-terminated fixed character array after validating its terminator and payload.
        template<std::size_t TArraySize>
        StringInsertResult Insert(
            std::size_t index,
            const char (&source)[TArraySize]
        ) noexcept {
            static_assert(
                TArraySize > 0U,
                "A fixed character array used as String input must provide storage for a null terminator."
            );

            if (source[TArraySize - 1U] != '\0') { return StringInsertResult::SourceNotNullTerminated; }

            for (std::size_t sourceIndex = 0U; sourceIndex + 1U < TArraySize; ++sourceIndex)
                if (source[sourceIndex] == '\0') { return StringInsertResult::EmbeddedNullNotPermitted; }

            return InsertBytes(
                index,
                source,
                TArraySize - 1U
            );
        }

        // Character and range mutation.

        /// Erases a strict logical range without silently clamping an invalid request.
        StringEraseResult Erase(
            std::size_t index,
            std::size_t count
        ) noexcept {
            const auto currentSize = Size();

            if (index > currentSize) { return StringEraseResult::IndexOutOfRange; }

            if (count > currentSize - index) { return StringEraseResult::RangeOutOfBounds; }

            if (count == 0U) { return StringEraseResult::Succeeded; }

            std::memmove(
                _storage.data() + index,
                _storage.data() + index + count,
                currentSize - index - count
            );

            const auto newSize = currentSize - count;
            _size = static_cast<Detail::SizeCounter<TCapacity>>(newSize);
            _storage[newSize] = '\0';

            return StringEraseResult::Succeeded;
        }

        /// Replaces one existing logical character while preserving the no-embedded-null invariant.
        StringSetAtResult SetAt(
            std::size_t index,
            char value
        ) noexcept {
            if (index >= Size()) { return StringSetAtResult::IndexOutOfRange; }

            if (value == '\0') { return StringSetAtResult::NullCharacterNotPermitted; }

            _storage[index] = value;
            return StringSetAtResult::Succeeded;
        }

        /// Appends one non-null character when capacity remains available.
        StringPushBackResult PushBack(
            char value
        ) noexcept {
            if (value == '\0') { return StringPushBackResult::NullCharacterNotPermitted; }

            if (IsFull()) { return StringPushBackResult::CapacityExceeded; }

            const auto previousSize = Size();
            _storage[previousSize] = value;
            _size = static_cast<Detail::SizeCounter<TCapacity>>(previousSize + 1U);
            _storage[previousSize + 1U] = '\0';

            return StringPushBackResult::Succeeded;
        }

        /// Removes the final logical character when the String is non-empty.
        StringPopBackResult PopBack() noexcept {
            if (IsEmpty()) { return StringPopBackResult::Empty; }

            const auto newSize = Size() - 1U;
            _size = static_cast<Detail::SizeCounter<TCapacity>>(newSize);
            _storage[newSize] = '\0';

            return StringPopBackResult::Succeeded;
        }

        /// Clears the logical payload while retaining all preallocated storage.
        void Clear() noexcept {
            _size = 0;
            _storage[0] = '\0';
        }

        // Explicit external-Type conversion.

        /// Converts this bounded String to a target Type through the target Type owner's compile-time adapter specialization.
        template<class TTarget>
        auto CastTo(
            TTarget& target
        ) const noexcept(
            TypeConversionAdapter<
                String<TCapacity>,
                Detail::NormalizedType<TTarget>
            >::IsNoexcept
        ) -> typename TypeConversionAdapter<
            String<TCapacity>,
            Detail::NormalizedType<TTarget>
        >::ResultType {
            return CastToImplementation(target);
        }

        /// Converts a source Type into this bounded String through the source Type owner's compile-time adapter specialization.
        template<class TSource>
        auto CastFrom(
            const TSource& source
        ) noexcept(
            TypeConversionAdapter<
                Detail::NormalizedType<TSource>,
                String<TCapacity>
            >::IsNoexcept
        ) -> typename TypeConversionAdapter<
            Detail::NormalizedType<TSource>,
            String<TCapacity>
        >::ResultType {
            return CastFromImplementation(source);
        }

        // Logical-value comparisons.

        /// Reports whether two bounded Strings contain identical logical payload bytes.
        template<std::size_t TOtherCapacity>
        bool operator==(
            const String<TOtherCapacity>& other
        ) const noexcept {
            if (Size() != other.Size()) { return false; }

            for (std::size_t index = 0U; index < Size(); ++index)
                if (_storage[index] != other[index]) { return false; }

            return true;
        }

        /// Reports whether two bounded Strings contain different logical payload bytes.
        template<std::size_t TOtherCapacity>
        bool operator!=(
            const String<TOtherCapacity>& other
        ) const noexcept {
            return !(
                *this ==
                other
            );
        }

        /// Reports whether this String sorts lexicographically before another bounded String by payload byte value.
        template<std::size_t TOtherCapacity>
        bool operator<(
            const String<TOtherCapacity>& other
        ) const noexcept {
            const auto commonSize = Size() < other.Size()
                ? Size()
                : other.Size();

            for (std::size_t index = 0U; index < commonSize; ++index) {
                const auto left = static_cast<unsigned char>(_storage[index]);
                const auto right = static_cast<unsigned char>(other[index]);

                if (left < right) { return true; }

                if (left > right) { return false; }
            }

            return Size() < other.Size();
        }

        /// Reports whether this String is lexicographically before or equal to another bounded String.
        template<std::size_t TOtherCapacity>
        bool operator<=(
            const String<TOtherCapacity>& other
        ) const noexcept {
            return !(
                other <
                *this
            );
        }

        /// Reports whether this String sorts lexicographically after another bounded String.
        template<std::size_t TOtherCapacity>
        bool operator>(
            const String<TOtherCapacity>& other
        ) const noexcept {
            return other < *this;
        }

        /// Reports whether this String is lexicographically after or equal to another bounded String.
        template<std::size_t TOtherCapacity>
        bool operator>=(
            const String<TOtherCapacity>& other
        ) const noexcept {
            return !(
                *this <
                other
            );
        }

    };

    /// Certifies every bounded String specialization as a self-contained memory-bounded value.
    template<std::size_t TCapacity>
    struct MemoryBoundedTraits<String<TCapacity>> : MemoryBoundedValueDeclaration<false> {
    };

    /// Exposes the compile-time payload-byte capacity of a bounded String.
    template<std::size_t TCapacity>
    struct CapacityTraits<String<TCapacity>> {

        /// Bounded String capacity is known at compile time.
        static constexpr bool HasStaticCapacity = true;

        /// Bounded String logical capacity is measured in payload bytes.
        static constexpr CapacityUnit Unit = CapacityUnit::Bytes;

        /// Reports the maximum number of payload bytes retained by the String.
        static constexpr std::size_t Capacity = TCapacity;

    };

} // ESPressio::Bounded
