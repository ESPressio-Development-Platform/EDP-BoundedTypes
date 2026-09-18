#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "CapacityTraits.hpp"
#include "MemoryBoundedTraits.hpp"
#include "detail/IndexedIterator.hpp"
#include "detail/RawSlotStorage.hpp"
#include "detail/SizeCounter.hpp"

namespace ESPressio::Bounded {

    /// Describes the outcome of assigning a complete logical sequence to a bounded Vector.
    enum class VectorAssignmentResult : std::uint8_t {
        Succeeded = 0,
        CapacityExceeded = 1
    };

    /// Describes the outcome of appending one value to a bounded Vector.
    enum class VectorPushBackResult : std::uint8_t {
        Succeeded = 0,
        CapacityExceeded = 1
    };

    /// Describes the outcome of in-place construction at the end of a bounded Vector.
    enum class VectorEmplaceBackResult : std::uint8_t {
        Succeeded = 0,
        CapacityExceeded = 1
    };

    /// Describes the outcome of inserting one value into a bounded Vector.
    enum class VectorInsertResult : std::uint8_t {
        Succeeded = 0,
        CapacityExceeded = 1,
        IndexOutOfRange = 2
    };

    /// Describes the outcome of erasing a logical range from a bounded Vector.
    enum class VectorEraseResult : std::uint8_t {
        Succeeded = 0,
        IndexOutOfRange = 1,
        RangeOutOfBounds = 2
    };

    /// Describes the outcome of removing the final logical value from a bounded Vector.
    enum class VectorPopBackResult : std::uint8_t {
        Succeeded = 0,
        Empty = 1
    };

    /// Describes the outcome of changing the logical size of a bounded Vector.
    enum class VectorResizeResult : std::uint8_t {
        Succeeded = 0,
        CapacityExceeded = 1
    };

    /// Describes the outcome of checked bounded Vector element access.
    enum class VectorAccessResult : std::uint8_t {
        Succeeded = 0,
        IndexOutOfRange = 1
    };

    /// Stores a variable-length indexed sequence in fully preallocated inline slots without dynamic allocation.
    template<class TValue, std::size_t TCapacity>
    class Vector final {

    private:

        /// Allows logical iterators to access already-live elements without exposing raw slot storage publicly.
        template<class, class, bool>
        friend class Detail::IndexedIterator;

        // Storage state.

        /// Reserves the complete maximum raw slot storage without constructing inactive TValue objects.
        Detail::RawSlotStorage<TValue, TCapacity> _storage{};

        /// Stores the current number of live logical elements using the smallest suitable unsigned integer Type.
        Detail::SizeCounter<TCapacity> _size = 0;

        // Internal element access.

        /// Returns a mutable reference to an already-live element at a valid logical index.
        TValue& ElementAtUnchecked(
            std::size_t index
        ) noexcept {
            return _storage.Get(index);
        }

        /// Returns a read-only reference to an already-live element at a valid logical index.
        const TValue& ElementAtUnchecked(
            std::size_t index
        ) const noexcept {
            return _storage.Get(index);
        }

        // Internal lifetime helpers.

        /// Destroys every active element without changing the preallocated raw slot capacity.
        void DestroyAll() noexcept {
            while (_size > 0) {
                const auto finalIndex = Size() - 1U;
                _storage.Destroy(finalIndex);
                --_size;
            }
        }

        /// Copies all elements from a source whose logical size is already known to fit.
        template<std::size_t TSourceCapacity>
        void CopyFromKnownFit(
            const Vector<TValue, TSourceCapacity>& source
        ) noexcept {
            static_assert(
                std::is_nothrow_copy_constructible_v<TValue>,
                "Vector copy operations require TValue to be nothrow copy-constructible."
            );

            DestroyAll();

            for (std::size_t index = 0U; index < source.Size(); ++index) {
                _storage.Construct(
                    index,
                    source[index]
                );
                ++_size;
            }
        }

        /// Moves all elements from a source whose logical size is already known to fit and empties that source.
        template<std::size_t TSourceCapacity>
        void MoveFromKnownFit(
            Vector<TValue, TSourceCapacity>& source
        ) noexcept {
            static_assert(
                std::is_nothrow_move_constructible_v<TValue>,
                "Vector move operations require TValue to be nothrow move-constructible."
            );

            DestroyAll();

            for (std::size_t index = 0U; index < source.Size(); ++index) {
                _storage.Construct(
                    index,
                    std::move(
                        source[index]
                    )
                );
                ++_size;
            }

            source.Clear();
        }

        /// Finds whether a source reference identifies an element already owned by this Vector.
        bool FindOwnedElementIndex(
            const TValue* value,
            std::size_t& index
        ) const noexcept {
            for (std::size_t candidate = 0U; candidate < Size(); ++candidate)
                if (std::addressof(
                    ElementAtUnchecked(candidate)
                ) == value) {
                    index = candidate;
                    return true;
                }

            index = 0U;
            return false;
        }

        /// Opens one raw slot at a logical insertion point by nothrow move-constructing existing values backward.
        void ShiftRightForInsertion(
            std::size_t index
        ) noexcept {
            static_assert(
                std::is_nothrow_move_constructible_v<TValue>,
                "Vector insertion requires TValue to be nothrow move-constructible."
            );

            for (std::size_t destination = Size(); destination > index; --destination) {
                _storage.Construct(
                    destination,
                    std::move(
                        _storage.Get(destination - 1U)
                    )
                );
                _storage.Destroy(destination - 1U);
            }
        }

        /// Closes a logical gap by nothrow move-constructing trailing values forward into destroyed slots.
        void ShiftLeftAfterErase(
            std::size_t index,
            std::size_t count
        ) noexcept {
            static_assert(
                std::is_nothrow_move_constructible_v<TValue>,
                "Vector erase requires TValue to be nothrow move-constructible."
            );

            const auto previousSize = Size();

            for (std::size_t destination = index; destination + count < previousSize; ++destination) {
                _storage.Construct(
                    destination,
                    std::move(
                        _storage.Get(destination + count)
                    )
                );
                _storage.Destroy(destination + count);
            }
        }

    public:

        static_assert(
            IsMemoryBoundedValue<TValue>,
            "Bounded::Vector TValue must be a certified memory-bounded value."
        );

        static_assert(
            std::is_nothrow_destructible_v<TValue>,
            "Bounded::Vector TValue must be nothrow-destructible."
        );

        /// Identifies the logical element Type retained by this Vector.
        using ValueType = TValue;

        /// Provides mutable random-access logical iteration without claiming contiguous TValue pointer semantics.
        using Iterator = Detail::IndexedIterator<
            Vector<TValue, TCapacity>,
            TValue,
            false
        >;

        /// Provides read-only random-access logical iteration without claiming contiguous TValue pointer semantics.
        using ConstIterator = Detail::IndexedIterator<
            Vector<TValue, TCapacity>,
            TValue,
            true
        >;

        /// Creates an empty bounded Vector without constructing any TValue instances.
        constexpr Vector() noexcept = default;

        /// Copies a bounded Vector of identical capacity when TValue supports nothrow copy construction.
        Vector(
            const Vector& source
        ) noexcept {
            static_assert(
                std::is_nothrow_copy_constructible_v<TValue>,
                "Vector copy construction requires TValue to be nothrow copy-constructible."
            );

            CopyFromKnownFit(source);
        }

        /// Moves a bounded Vector of identical capacity when TValue supports nothrow move construction and leaves the source empty.
        Vector(
            Vector&& source
        ) noexcept {
            static_assert(
                std::is_nothrow_move_constructible_v<TValue>,
                "Vector move construction requires TValue to be nothrow move-constructible."
            );

            MoveFromKnownFit(source);
        }

        /// Copies a bounded Vector whose compile-time capacity cannot exceed this destination's capacity.
        template<std::size_t TSourceCapacity, std::enable_if_t<(TSourceCapacity <= TCapacity && TSourceCapacity != TCapacity), int> = 0>
        Vector(
            const Vector<TValue, TSourceCapacity>& source
        ) noexcept {
            static_assert(
                std::is_nothrow_copy_constructible_v<TValue>,
                "Vector widening copy construction requires TValue to be nothrow copy-constructible."
            );

            CopyFromKnownFit(source);
        }

        /// Moves a bounded Vector whose compile-time capacity cannot exceed this destination's capacity and leaves the source empty.
        template<std::size_t TSourceCapacity, std::enable_if_t<(TSourceCapacity <= TCapacity && TSourceCapacity != TCapacity), int> = 0>
        Vector(
            Vector<TValue, TSourceCapacity>&& source
        ) noexcept {
            static_assert(
                std::is_nothrow_move_constructible_v<TValue>,
                "Vector widening move construction requires TValue to be nothrow move-constructible."
            );

            MoveFromKnownFit(source);
        }

        /// Destroys every active TValue while releasing no external storage.
        ~Vector() noexcept {
            DestroyAll();
        }

        // Assignment operators.

        /// Copies a bounded Vector of identical capacity.
        Vector& operator=(
            const Vector& source
        ) noexcept {
            static_assert(
                std::is_nothrow_copy_constructible_v<TValue>,
                "Vector copy assignment requires TValue to be nothrow copy-constructible."
            );

            if (this != &source) {
                CopyFromKnownFit(source);
            }

            return *this;
        }

        /// Moves a bounded Vector of identical capacity and leaves the source empty.
        Vector& operator=(
            Vector&& source
        ) noexcept {
            static_assert(
                std::is_nothrow_move_constructible_v<TValue>,
                "Vector move assignment requires TValue to be nothrow move-constructible."
            );

            if (this != &source) {
                MoveFromKnownFit(source);
            }

            return *this;
        }

        /// Copies a bounded Vector whose compile-time capacity cannot exceed this destination's capacity.
        template<std::size_t TSourceCapacity, std::enable_if_t<(TSourceCapacity <= TCapacity && TSourceCapacity != TCapacity), int> = 0>
        Vector& operator=(
            const Vector<TValue, TSourceCapacity>& source
        ) noexcept {
            static_assert(
                std::is_nothrow_copy_constructible_v<TValue>,
                "Vector widening copy assignment requires TValue to be nothrow copy-constructible."
            );

            CopyFromKnownFit(source);
            return *this;
        }

        /// Moves a bounded Vector whose compile-time capacity cannot exceed this destination's capacity and leaves the source empty.
        template<std::size_t TSourceCapacity, std::enable_if_t<(TSourceCapacity <= TCapacity && TSourceCapacity != TCapacity), int> = 0>
        Vector& operator=(
            Vector<TValue, TSourceCapacity>&& source
        ) noexcept {
            static_assert(
                std::is_nothrow_move_constructible_v<TValue>,
                "Vector widening move assignment requires TValue to be nothrow move-constructible."
            );

            MoveFromKnownFit(source);
            return *this;
        }

        // Capacity and state inspection.

        /// Returns the maximum number of TValue instances this Vector can contain.
        static constexpr std::size_t Capacity() noexcept {
            return TCapacity;
        }

        /// Returns the current number of live logical elements.
        constexpr std::size_t Size() const noexcept {
            return static_cast<std::size_t>(_size);
        }

        /// Reports whether this Vector currently contains no logical elements.
        constexpr bool IsEmpty() const noexcept {
            return Size() == 0U;
        }

        /// Reports whether this Vector currently occupies every available logical slot.
        constexpr bool IsFull() const noexcept {
            return Size() == TCapacity;
        }

        // Element access.

        /// Returns a mutable reference to an element at an index that must already satisfy index < Size().
        TValue& operator[](
            std::size_t index
        ) noexcept {
            return ElementAtUnchecked(index);
        }

        /// Returns a read-only reference to an element at an index that must already satisfy index < Size().
        const TValue& operator[](
            std::size_t index
        ) const noexcept {
            return ElementAtUnchecked(index);
        }

        /// Retrieves a mutable element pointer through a checked logical index.
        VectorAccessResult GetAt(
            std::size_t index,
            TValue*& value
        ) noexcept {
            if (index >= Size()) {
                value = nullptr;
                return VectorAccessResult::IndexOutOfRange;
            }

            value = std::addressof(
                ElementAtUnchecked(index)
            );
            return VectorAccessResult::Succeeded;
        }

        /// Retrieves a read-only element pointer through a checked logical index.
        VectorAccessResult GetAt(
            std::size_t index,
            const TValue*& value
        ) const noexcept {
            if (index >= Size()) {
                value = nullptr;
                return VectorAccessResult::IndexOutOfRange;
            }

            value = std::addressof(
                ElementAtUnchecked(index)
            );
            return VectorAccessResult::Succeeded;
        }

        // Iteration.

        /// Returns a mutable logical iterator to the first element.
        Iterator begin() noexcept {
            return Iterator(
                this,
                0U
            );
        }

        /// Returns a mutable logical iterator one position beyond the final element.
        Iterator end() noexcept {
            return Iterator(
                this,
                Size()
            );
        }

        /// Returns a read-only logical iterator to the first element.
        ConstIterator begin() const noexcept {
            return ConstIterator(
                this,
                0U
            );
        }

        /// Returns a read-only logical iterator one position beyond the final element.
        ConstIterator end() const noexcept {
            return ConstIterator(
                this,
                Size()
            );
        }

        /// Returns a read-only logical iterator to the first element.
        ConstIterator cbegin() const noexcept {
            return begin();
        }

        /// Returns a read-only logical iterator one position beyond the final element.
        ConstIterator cend() const noexcept {
            return end();
        }

        // Complete-sequence assignment.

        /// Assigns another bounded Vector after validating this destination's runtime capacity.
        template<std::size_t TSourceCapacity>
        VectorAssignmentResult Assign(
            const Vector<TValue, TSourceCapacity>& source
        ) noexcept {
            static_assert(
                std::is_nothrow_copy_constructible_v<TValue>,
                "Vector assignment requires TValue to be nothrow copy-constructible."
            );

            if (source.Size() > TCapacity) { return VectorAssignmentResult::CapacityExceeded; }

            CopyFromKnownFit(source);
            return VectorAssignmentResult::Succeeded;
        }

        /// Assigns a fixed standard array after validating this destination's runtime capacity.
        template<std::size_t TArraySize>
        VectorAssignmentResult Assign(
            const std::array<TValue, TArraySize>& source
        ) noexcept {
            static_assert(
                std::is_nothrow_copy_constructible_v<TValue>,
                "Vector assignment from std::array requires TValue to be nothrow copy-constructible."
            );

            if (source.size() > TCapacity) { return VectorAssignmentResult::CapacityExceeded; }

            DestroyAll();

            for (std::size_t index = 0U; index < source.size(); ++index) {
                _storage.Construct(
                    index,
                    source[index]
                );
                ++_size;
            }

            return VectorAssignmentResult::Succeeded;
        }

        /// Assigns a standard dynamic Vector after validating size before mutating this bounded destination.
        VectorAssignmentResult Assign(
            const std::vector<TValue>& source
        ) noexcept {
            static_assert(
                std::is_nothrow_copy_constructible_v<TValue>,
                "Vector assignment from std::vector requires TValue to be nothrow copy-constructible."
            );

            if (source.size() > TCapacity) { return VectorAssignmentResult::CapacityExceeded; }

            DestroyAll();

            for (std::size_t index = 0U; index < source.size(); ++index) {
                _storage.Construct(
                    index,
                    source[index]
                );
                ++_size;
            }

            return VectorAssignmentResult::Succeeded;
        }

        // End insertion.

        /// Appends a copied value when capacity remains available.
        VectorPushBackResult PushBack(
            const TValue& value
        ) noexcept {
            static_assert(
                std::is_nothrow_copy_constructible_v<TValue>,
                "Vector::PushBack(const TValue&) requires TValue to be nothrow copy-constructible."
            );

            if (IsFull()) { return VectorPushBackResult::CapacityExceeded; }

            _storage.Construct(
                Size(),
                value
            );
            ++_size;

            return VectorPushBackResult::Succeeded;
        }

        /// Appends a moved value when capacity remains available.
        VectorPushBackResult PushBack(
            TValue&& value
        ) noexcept {
            static_assert(
                std::is_nothrow_move_constructible_v<TValue>,
                "Vector::PushBack(TValue&&) requires TValue to be nothrow move-constructible."
            );

            if (IsFull()) { return VectorPushBackResult::CapacityExceeded; }

            _storage.Construct(
                Size(),
                std::move(
                    value
                )
            );
            ++_size;

            return VectorPushBackResult::Succeeded;
        }

        /// Constructs a new final value directly in its preallocated raw slot.
        template<class... TArgumentTypes>
        VectorEmplaceBackResult EmplaceBack(
            TArgumentTypes&&... arguments
        ) noexcept {
            static_assert(
                std::is_nothrow_constructible_v<TValue, TArgumentTypes...>,
                "Vector::EmplaceBack requires TValue to be nothrow-constructible from the supplied arguments."
            );

            if (IsFull()) { return VectorEmplaceBackResult::CapacityExceeded; }

            _storage.Construct(
                Size(),
                std::forward<TArgumentTypes>(arguments)...
            );
            ++_size;

            return VectorEmplaceBackResult::Succeeded;
        }

        // Indexed insertion.

        /// Inserts a copied value at a checked logical index, including safe copy insertion from this Vector's own elements.
        VectorInsertResult Insert(
            std::size_t index,
            const TValue& value
        ) noexcept {
            static_assert(
                std::is_nothrow_copy_constructible_v<TValue>,
                "Vector::Insert copy requires TValue to be nothrow copy-constructible."
            );

            static_assert(
                std::is_nothrow_move_constructible_v<TValue>,
                "Vector::Insert requires TValue to be nothrow move-constructible for slot relocation."
            );

            if (index > Size()) { return VectorInsertResult::IndexOutOfRange; }

            if (IsFull()) { return VectorInsertResult::CapacityExceeded; }

            std::size_t ownedSourceIndex = 0U;
            const bool sourceIsOwned = FindOwnedElementIndex(
                std::addressof(
                    value
                ),
                ownedSourceIndex
            );

            ShiftRightForInsertion(index);

            if (sourceIsOwned && ownedSourceIndex >= index) {
                _storage.Construct(
                    index,
                    _storage.Get(ownedSourceIndex + 1U)
                );
            } else {
                _storage.Construct(
                    index,
                    value
                );
            }

            ++_size;
            return VectorInsertResult::Succeeded;
        }

        /// Inserts a moved non-aliasing value at a checked logical index.
        VectorInsertResult Insert(
            std::size_t index,
            TValue&& value
        ) noexcept {
            static_assert(
                std::is_nothrow_move_constructible_v<TValue>,
                "Vector::Insert move requires TValue to be nothrow move-constructible."
            );

            if (index > Size()) { return VectorInsertResult::IndexOutOfRange; }

            if (IsFull()) { return VectorInsertResult::CapacityExceeded; }

            ShiftRightForInsertion(index);
            _storage.Construct(
                index,
                std::move(
                    value
                )
            );
            ++_size;

            return VectorInsertResult::Succeeded;
        }

        /// Constructs one value at a checked logical index after opening a raw slot.
        template<class... TArgumentTypes>
        VectorInsertResult Emplace(
            std::size_t index,
            TArgumentTypes&&... arguments
        ) noexcept {
            static_assert(
                std::is_nothrow_move_constructible_v<TValue>,
                "Vector::Emplace requires TValue to be nothrow move-constructible for slot relocation."
            );

            static_assert(
                std::is_nothrow_constructible_v<TValue, TArgumentTypes...>,
                "Vector::Emplace requires TValue to be nothrow-constructible from the supplied arguments."
            );

            if (index > Size()) { return VectorInsertResult::IndexOutOfRange; }

            if (IsFull()) { return VectorInsertResult::CapacityExceeded; }

            ShiftRightForInsertion(index);
            _storage.Construct(
                index,
                std::forward<TArgumentTypes>(arguments)...
            );
            ++_size;

            return VectorInsertResult::Succeeded;
        }

        // Erase and size mutation.

        /// Erases a strict logical range while preserving order.
        VectorEraseResult Erase(
            std::size_t index,
            std::size_t count
        ) noexcept {
            static_assert(
                std::is_nothrow_move_constructible_v<TValue>,
                "Vector::Erase requires TValue to be nothrow move-constructible."
            );

            const auto previousSize = Size();

            if (index > previousSize) { return VectorEraseResult::IndexOutOfRange; }

            if (count > previousSize - index) { return VectorEraseResult::RangeOutOfBounds; }

            if (count == 0U) { return VectorEraseResult::Succeeded; }

            for (std::size_t eraseIndex = index; eraseIndex < index + count; ++eraseIndex)
                _storage.Destroy(eraseIndex);

            ShiftLeftAfterErase(
                index,
                count
            );

            _size = static_cast<Detail::SizeCounter<TCapacity>>(previousSize - count);
            return VectorEraseResult::Succeeded;
        }

        /// Removes the final logical value when the Vector is non-empty.
        VectorPopBackResult PopBack() noexcept {
            if (IsEmpty()) { return VectorPopBackResult::Empty; }

            const auto finalIndex = Size() - 1U;
            _storage.Destroy(finalIndex);
            --_size;

            return VectorPopBackResult::Succeeded;
        }

        /// Changes logical size, default-constructing new elements when growth is requested.
        VectorResizeResult Resize(
            std::size_t newSize
        ) noexcept {
            static_assert(
                std::is_nothrow_default_constructible_v<TValue>,
                "Vector::Resize(newSize) requires TValue to be nothrow default-constructible."
            );

            if (newSize > TCapacity) { return VectorResizeResult::CapacityExceeded; }

            while (Size() > newSize) {
                const auto finalIndex = Size() - 1U;
                _storage.Destroy(finalIndex);
                --_size;
            }

            while (Size() < newSize) {
                _storage.Construct(Size());
                ++_size;
            }

            return VectorResizeResult::Succeeded;
        }

        /// Changes logical size, copy-constructing any new elements from one fill value.
        VectorResizeResult Resize(
            std::size_t newSize,
            const TValue& fillValue
        ) noexcept {
            static_assert(
                std::is_nothrow_copy_constructible_v<TValue>,
                "Vector::Resize(newSize, fillValue) requires TValue to be nothrow copy-constructible."
            );

            if (newSize > TCapacity) { return VectorResizeResult::CapacityExceeded; }

            while (Size() > newSize) {
                const auto finalIndex = Size() - 1U;
                _storage.Destroy(finalIndex);
                --_size;
            }

            while (Size() < newSize) {
                _storage.Construct(
                    Size(),
                    fillValue
                );
                ++_size;
            }

            return VectorResizeResult::Succeeded;
        }

        /// Destroys all active values and returns the Vector to its empty state.
        void Clear() noexcept {
            DestroyAll();
        }

        // Logical-value comparisons.

        /// Reports whether two bounded Vectors contain identical logical sequences.
        template<std::size_t TOtherCapacity>
        bool operator==(
            const Vector<TValue, TOtherCapacity>& other
        ) const noexcept {
            static_assert(
                noexcept(
                    std::declval<const TValue&>() ==
                    std::declval<const TValue&>()
                ),
                "Vector equality requires TValue equality comparison to be noexcept."
            );

            if (Size() != other.Size()) { return false; }

            for (std::size_t index = 0U; index < Size(); ++index)
                if (!(
                    ElementAtUnchecked(index) ==
                    other[index]
                )) {
                    return false;
                }

            return true;
        }

        /// Reports whether two bounded Vectors contain different logical sequences.
        template<std::size_t TOtherCapacity>
        bool operator!=(
            const Vector<TValue, TOtherCapacity>& other
        ) const noexcept {
            return !(
                *this ==
                other
            );
        }

        /// Reports whether this Vector sorts lexicographically before another logical sequence.
        template<std::size_t TOtherCapacity>
        bool operator<(
            const Vector<TValue, TOtherCapacity>& other
        ) const noexcept {
            static_assert(
                noexcept(
                    std::declval<const TValue&>() <
                    std::declval<const TValue&>()
                ),
                "Vector ordering requires TValue less-than comparison to be noexcept."
            );

            const auto commonSize = Size() < other.Size()
                ? Size()
                : other.Size();

            for (std::size_t index = 0U; index < commonSize; ++index) {
                if (ElementAtUnchecked(index) < other[index]) { return true; }

                if (other[index] < ElementAtUnchecked(index)) { return false; }
            }

            return Size() < other.Size();
        }

        /// Reports whether this Vector is lexicographically before or equal to another logical sequence.
        template<std::size_t TOtherCapacity>
        bool operator<=(
            const Vector<TValue, TOtherCapacity>& other
        ) const noexcept {
            return !(
                other <
                *this
            );
        }

        /// Reports whether this Vector sorts lexicographically after another logical sequence.
        template<std::size_t TOtherCapacity>
        bool operator>(
            const Vector<TValue, TOtherCapacity>& other
        ) const noexcept {
            return other < *this;
        }

        /// Reports whether this Vector is lexicographically after or equal to another logical sequence.
        template<std::size_t TOtherCapacity>
        bool operator>=(
            const Vector<TValue, TOtherCapacity>& other
        ) const noexcept {
            return !(
                *this <
                other
            );
        }

    };

    /// Certifies a bounded Vector when its recursively owned element Type is memory-bounded.
    template<class TValue, std::size_t TCapacity>
    struct MemoryBoundedTraits<Vector<TValue, TCapacity>> : MemoryBoundedValueDeclaration<
        false,
        TValue
    > {
    };

    /// Exposes the compile-time element capacity of a bounded Vector.
    template<class TValue, std::size_t TCapacity>
    struct CapacityTraits<Vector<TValue, TCapacity>> {

        /// Bounded Vector capacity is known at compile time.
        static constexpr bool HasStaticCapacity = true;

        /// Bounded Vector logical capacity is measured in elements.
        static constexpr CapacityUnit Unit = CapacityUnit::Elements;

        /// Reports the maximum number of logical elements retained by the Vector.
        static constexpr std::size_t Capacity = TCapacity;

    };

} // ESPressio::Bounded
