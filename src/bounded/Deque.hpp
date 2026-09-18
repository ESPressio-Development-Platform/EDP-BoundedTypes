#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <utility>

#include "CapacityTraits.hpp"
#include "MemoryBoundedTraits.hpp"
#include "detail/IndexedIterator.hpp"
#include "detail/RawSlotStorage.hpp"
#include "detail/SizeCounter.hpp"

namespace ESPressio::Bounded {

    /// Describes the outcome of inserting one value at the front of a bounded Deque.
    enum class DequePushFrontResult : std::uint8_t {
        Succeeded = 0,
        CapacityExceeded = 1
    };

    /// Describes the outcome of inserting one value at the back of a bounded Deque.
    enum class DequePushBackResult : std::uint8_t {
        Succeeded = 0,
        CapacityExceeded = 1
    };

    /// Describes the outcome of in-place construction at the front of a bounded Deque.
    enum class DequeEmplaceFrontResult : std::uint8_t {
        Succeeded = 0,
        CapacityExceeded = 1
    };

    /// Describes the outcome of in-place construction at the back of a bounded Deque.
    enum class DequeEmplaceBackResult : std::uint8_t {
        Succeeded = 0,
        CapacityExceeded = 1
    };

    /// Describes the outcome of removing the front value from a bounded Deque.
    enum class DequePopFrontResult : std::uint8_t {
        Succeeded = 0,
        Empty = 1
    };

    /// Describes the outcome of removing the back value from a bounded Deque.
    enum class DequePopBackResult : std::uint8_t {
        Succeeded = 0,
        Empty = 1
    };

    /// Describes the outcome of checked bounded Deque element access.
    enum class DequeAccessResult : std::uint8_t {
        Succeeded = 0,
        Empty = 1,
        IndexOutOfRange = 2
    };

    /// Stores a variable-length double-ended logical sequence in fully preallocated inline ring storage.
    template<class TValue, std::size_t TCapacity>
    class Deque final {

    private:

        /// Allows logical iterators to access already-live elements without exposing ring storage publicly.
        template<class, class, bool>
        friend class Detail::IndexedIterator;

        // Storage state.

        /// Reserves the complete maximum raw slot storage without constructing inactive TValue objects.
        Detail::RawSlotStorage<TValue, TCapacity> _storage{};

        /// Stores the current number of active values.
        Detail::SizeCounter<TCapacity> _size = 0;

        /// Stores the physical slot index containing the current logical front value.
        Detail::SizeCounter<TCapacity> _head = 0;

        // Index translation.

        /// Maps a valid logical index to its current physical ring slot.
        std::size_t PhysicalIndex(
            std::size_t logicalIndex
        ) const noexcept {
            if constexpr (TCapacity == 0U) {
                return 0U;
            } else {
                return (
                    static_cast<std::size_t>(_head) +
                    logicalIndex
                ) % TCapacity;
            }
        }

        /// Returns the physical slot immediately preceding the current head with ring wrap.
        std::size_t PreviousHeadIndex() const noexcept {
            if constexpr (TCapacity == 0U) {
                return 0U;
            } else {
                return (
                    static_cast<std::size_t>(_head) +
                    TCapacity -
                    1U
                ) % TCapacity;
            }
        }

        /// Returns the physical slot immediately following the current head with ring wrap.
        std::size_t NextHeadIndex() const noexcept {
            if constexpr (TCapacity == 0U) {
                return 0U;
            } else {
                return (
                    static_cast<std::size_t>(_head) +
                    1U
                ) % TCapacity;
            }
        }

        // Internal element access.

        /// Returns a mutable reference to an already-live element at a valid logical index.
        TValue& ElementAtUnchecked(
            std::size_t index
        ) noexcept {
            return _storage.Get(
                PhysicalIndex(index)
            );
        }

        /// Returns a read-only reference to an already-live element at a valid logical index.
        const TValue& ElementAtUnchecked(
            std::size_t index
        ) const noexcept {
            return _storage.Get(
                PhysicalIndex(index)
            );
        }

        // Lifetime helpers.

        /// Destroys every active ring element and resets logical ring state.
        void DestroyAll() noexcept {
            const auto currentSize = Size();

            for (std::size_t index = 0U; index < currentSize; ++index)
                _storage.Destroy(
                    PhysicalIndex(index)
                );

            _size = 0;
            _head = 0;
        }

        /// Copies all source values in logical order into an empty destination ring.
        void CopyFrom(
            const Deque& source
        ) noexcept {
            static_assert(
                std::is_nothrow_copy_constructible_v<TValue>,
                "Deque copy operations require TValue to be nothrow copy-constructible."
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

        /// Moves all source values in logical order into an empty destination ring and empties the source.
        void MoveFrom(
            Deque& source
        ) noexcept {
            static_assert(
                std::is_nothrow_move_constructible_v<TValue>,
                "Deque move operations require TValue to be nothrow move-constructible."
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

    public:

        static_assert(
            IsMemoryBoundedValue<TValue>,
            "Bounded::Deque TValue must be a certified memory-bounded value."
        );

        static_assert(
            std::is_nothrow_destructible_v<TValue>,
            "Bounded::Deque TValue must be nothrow-destructible."
        );

        /// Identifies the logical element Type retained by this Deque.
        using ValueType = TValue;

        /// Provides mutable random-access logical iteration over ring storage.
        using Iterator = Detail::IndexedIterator<
            Deque<TValue, TCapacity>,
            TValue,
            false
        >;

        /// Provides read-only random-access logical iteration over ring storage.
        using ConstIterator = Detail::IndexedIterator<
            Deque<TValue, TCapacity>,
            TValue,
            true
        >;

        /// Creates an empty bounded Deque without constructing any TValue instances.
        constexpr Deque() noexcept = default;

        /// Copies a bounded Deque when TValue supports nothrow copy construction.
        Deque(
            const Deque& source
        ) noexcept {
            CopyFrom(source);
        }

        /// Moves a bounded Deque when TValue supports nothrow move construction and leaves the source empty.
        Deque(
            Deque&& source
        ) noexcept {
            MoveFrom(source);
        }

        /// Destroys every active TValue while releasing no external storage.
        ~Deque() noexcept {
            DestroyAll();
        }

        // Assignment operators.

        /// Copies a bounded Deque when TValue supports nothrow copy construction.
        Deque& operator=(
            const Deque& source
        ) noexcept {
            if (this != &source) {
                CopyFrom(source);
            }

            return *this;
        }

        /// Moves a bounded Deque when TValue supports nothrow move construction and leaves the source empty.
        Deque& operator=(
            Deque&& source
        ) noexcept {
            if (this != &source) {
                MoveFrom(source);
            }

            return *this;
        }

        // Capacity and state inspection.

        /// Returns the maximum number of TValue instances this Deque can contain.
        static constexpr std::size_t Capacity() noexcept {
            return TCapacity;
        }

        /// Returns the current number of live logical elements.
        constexpr std::size_t Size() const noexcept {
            return static_cast<std::size_t>(_size);
        }

        /// Reports whether this Deque currently contains no logical elements.
        constexpr bool IsEmpty() const noexcept {
            return Size() == 0U;
        }

        /// Reports whether this Deque currently occupies every available logical slot.
        constexpr bool IsFull() const noexcept {
            return Size() == TCapacity;
        }

        // Front/back access.

        /// Returns a mutable reference to the logical front value; the Deque must already be non-empty.
        TValue& Front() noexcept {
            return ElementAtUnchecked(0U);
        }

        /// Returns a read-only reference to the logical front value; the Deque must already be non-empty.
        const TValue& Front() const noexcept {
            return ElementAtUnchecked(0U);
        }

        /// Returns a mutable reference to the logical back value; the Deque must already be non-empty.
        TValue& Back() noexcept {
            return ElementAtUnchecked(Size() - 1U);
        }

        /// Returns a read-only reference to the logical back value; the Deque must already be non-empty.
        const TValue& Back() const noexcept {
            return ElementAtUnchecked(Size() - 1U);
        }

        /// Retrieves a mutable front-value pointer through a checked access path.
        DequeAccessResult GetFront(
            TValue*& value
        ) noexcept {
            if (IsEmpty()) {
                value = nullptr;
                return DequeAccessResult::Empty;
            }

            value = std::addressof(
                Front()
            );
            return DequeAccessResult::Succeeded;
        }

        /// Retrieves a read-only front-value pointer through a checked access path.
        DequeAccessResult GetFront(
            const TValue*& value
        ) const noexcept {
            if (IsEmpty()) {
                value = nullptr;
                return DequeAccessResult::Empty;
            }

            value = std::addressof(
                Front()
            );
            return DequeAccessResult::Succeeded;
        }

        /// Retrieves a mutable back-value pointer through a checked access path.
        DequeAccessResult GetBack(
            TValue*& value
        ) noexcept {
            if (IsEmpty()) {
                value = nullptr;
                return DequeAccessResult::Empty;
            }

            value = std::addressof(
                Back()
            );
            return DequeAccessResult::Succeeded;
        }

        /// Retrieves a read-only back-value pointer through a checked access path.
        DequeAccessResult GetBack(
            const TValue*& value
        ) const noexcept {
            if (IsEmpty()) {
                value = nullptr;
                return DequeAccessResult::Empty;
            }

            value = std::addressof(
                Back()
            );
            return DequeAccessResult::Succeeded;
        }

        // Indexed access.

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
        DequeAccessResult GetAt(
            std::size_t index,
            TValue*& value
        ) noexcept {
            if (index >= Size()) {
                value = nullptr;
                return DequeAccessResult::IndexOutOfRange;
            }

            value = std::addressof(
                ElementAtUnchecked(index)
            );
            return DequeAccessResult::Succeeded;
        }

        /// Retrieves a read-only element pointer through a checked logical index.
        DequeAccessResult GetAt(
            std::size_t index,
            const TValue*& value
        ) const noexcept {
            if (index >= Size()) {
                value = nullptr;
                return DequeAccessResult::IndexOutOfRange;
            }

            value = std::addressof(
                ElementAtUnchecked(index)
            );
            return DequeAccessResult::Succeeded;
        }

        // Iteration.

        /// Returns a mutable logical iterator to the front value.
        Iterator begin() noexcept {
            return Iterator(
                this,
                0U
            );
        }

        /// Returns a mutable logical iterator one position beyond the back value.
        Iterator end() noexcept {
            return Iterator(
                this,
                Size()
            );
        }

        /// Returns a read-only logical iterator to the front value.
        ConstIterator begin() const noexcept {
            return ConstIterator(
                this,
                0U
            );
        }

        /// Returns a read-only logical iterator one position beyond the back value.
        ConstIterator end() const noexcept {
            return ConstIterator(
                this,
                Size()
            );
        }

        /// Returns a read-only logical iterator to the front value.
        ConstIterator cbegin() const noexcept {
            return begin();
        }

        /// Returns a read-only logical iterator one position beyond the back value.
        ConstIterator cend() const noexcept {
            return end();
        }

        // Front insertion.

        /// Copies one value into the logical front when capacity remains available.
        DequePushFrontResult PushFront(
            const TValue& value
        ) noexcept {
            static_assert(
                std::is_nothrow_copy_constructible_v<TValue>,
                "Deque::PushFront(const TValue&) requires TValue to be nothrow copy-constructible."
            );

            if (IsFull()) { return DequePushFrontResult::CapacityExceeded; }

            const auto newHead = IsEmpty()
                ? 0U
                : PreviousHeadIndex();

            _storage.Construct(
                newHead,
                value
            );
            _head = static_cast<Detail::SizeCounter<TCapacity>>(newHead);
            ++_size;

            return DequePushFrontResult::Succeeded;
        }

        /// Moves one value into the logical front when capacity remains available.
        DequePushFrontResult PushFront(
            TValue&& value
        ) noexcept {
            static_assert(
                std::is_nothrow_move_constructible_v<TValue>,
                "Deque::PushFront(TValue&&) requires TValue to be nothrow move-constructible."
            );

            if (IsFull()) { return DequePushFrontResult::CapacityExceeded; }

            const auto newHead = IsEmpty()
                ? 0U
                : PreviousHeadIndex();

            _storage.Construct(
                newHead,
                std::move(
                    value
                )
            );
            _head = static_cast<Detail::SizeCounter<TCapacity>>(newHead);
            ++_size;

            return DequePushFrontResult::Succeeded;
        }

        /// Constructs one value directly in the next logical front slot.
        template<class... TArgumentTypes>
        DequeEmplaceFrontResult EmplaceFront(
            TArgumentTypes&&... arguments
        ) noexcept {
            static_assert(
                std::is_nothrow_constructible_v<TValue, TArgumentTypes...>,
                "Deque::EmplaceFront requires TValue to be nothrow-constructible from the supplied arguments."
            );

            if (IsFull()) { return DequeEmplaceFrontResult::CapacityExceeded; }

            const auto newHead = IsEmpty()
                ? 0U
                : PreviousHeadIndex();

            _storage.Construct(
                newHead,
                std::forward<TArgumentTypes>(arguments)...
            );
            _head = static_cast<Detail::SizeCounter<TCapacity>>(newHead);
            ++_size;

            return DequeEmplaceFrontResult::Succeeded;
        }

        // Back insertion.

        /// Copies one value into the logical back when capacity remains available.
        DequePushBackResult PushBack(
            const TValue& value
        ) noexcept {
            static_assert(
                std::is_nothrow_copy_constructible_v<TValue>,
                "Deque::PushBack(const TValue&) requires TValue to be nothrow copy-constructible."
            );

            if (IsFull()) { return DequePushBackResult::CapacityExceeded; }

            const auto physicalIndex = PhysicalIndex(Size());
            _storage.Construct(
                physicalIndex,
                value
            );
            ++_size;

            return DequePushBackResult::Succeeded;
        }

        /// Moves one value into the logical back when capacity remains available.
        DequePushBackResult PushBack(
            TValue&& value
        ) noexcept {
            static_assert(
                std::is_nothrow_move_constructible_v<TValue>,
                "Deque::PushBack(TValue&&) requires TValue to be nothrow move-constructible."
            );

            if (IsFull()) { return DequePushBackResult::CapacityExceeded; }

            const auto physicalIndex = PhysicalIndex(Size());
            _storage.Construct(
                physicalIndex,
                std::move(
                    value
                )
            );
            ++_size;

            return DequePushBackResult::Succeeded;
        }

        /// Constructs one value directly in the next logical back slot.
        template<class... TArgumentTypes>
        DequeEmplaceBackResult EmplaceBack(
            TArgumentTypes&&... arguments
        ) noexcept {
            static_assert(
                std::is_nothrow_constructible_v<TValue, TArgumentTypes...>,
                "Deque::EmplaceBack requires TValue to be nothrow-constructible from the supplied arguments."
            );

            if (IsFull()) { return DequeEmplaceBackResult::CapacityExceeded; }

            const auto physicalIndex = PhysicalIndex(Size());
            _storage.Construct(
                physicalIndex,
                std::forward<TArgumentTypes>(arguments)...
            );
            ++_size;

            return DequeEmplaceBackResult::Succeeded;
        }

        // Removal.

        /// Removes and destroys the logical front value when the Deque is non-empty.
        DequePopFrontResult PopFront() noexcept {
            if (IsEmpty()) { return DequePopFrontResult::Empty; }

            _storage.Destroy(
                static_cast<std::size_t>(_head)
            );

            if (Size() == 1U) {
                _size = 0;
                _head = 0;
            } else {
                _head = static_cast<Detail::SizeCounter<TCapacity>>(
                    NextHeadIndex()
                );
                --_size;
            }

            return DequePopFrontResult::Succeeded;
        }

        /// Removes and destroys the logical back value when the Deque is non-empty.
        DequePopBackResult PopBack() noexcept {
            if (IsEmpty()) { return DequePopBackResult::Empty; }

            _storage.Destroy(
                PhysicalIndex(
                    Size() - 1U
                )
            );
            --_size;

            if (IsEmpty()) {
                _head = 0;
            }

            return DequePopBackResult::Succeeded;
        }

        /// Destroys all active values and returns the Deque to its empty state.
        void Clear() noexcept {
            DestroyAll();
        }

        // Logical-value comparisons.

        /// Reports whether two bounded Deques contain identical logical sequences.
        bool operator==(
            const Deque& other
        ) const noexcept {
            static_assert(
                noexcept(
                    std::declval<const TValue&>() ==
                    std::declval<const TValue&>()
                ),
                "Deque equality requires TValue equality comparison to be noexcept."
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

        /// Reports whether two bounded Deques contain different logical sequences.
        bool operator!=(
            const Deque& other
        ) const noexcept {
            return !(
                *this ==
                other
            );
        }

    };

    /// Certifies a bounded Deque when its recursively owned element Type is memory-bounded.
    template<class TValue, std::size_t TCapacity>
    struct MemoryBoundedTraits<Deque<TValue, TCapacity>> : MemoryBoundedValueDeclaration<
        false,
        TValue
    > {
    };

    /// Exposes the compile-time element capacity of a bounded Deque.
    template<class TValue, std::size_t TCapacity>
    struct CapacityTraits<Deque<TValue, TCapacity>> {

        /// Bounded Deque capacity is known at compile time.
        static constexpr bool HasStaticCapacity = true;

        /// Bounded Deque logical capacity is measured in elements.
        static constexpr CapacityUnit Unit = CapacityUnit::Elements;

        /// Reports the maximum number of logical elements retained by the Deque.
        static constexpr std::size_t Capacity = TCapacity;

    };

} // ESPressio::Bounded
