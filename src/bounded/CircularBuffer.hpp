#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

#include "CapacityTraits.hpp"
#include "Deque.hpp"
#include "MemoryBoundedTraits.hpp"

namespace ESPressio::Bounded {

    /// Describes the outcome of adding one value to a CircularBuffer without overwrite permission.
    enum class CircularBufferPushResult : std::uint8_t {
        Succeeded = 0,
        CapacityExceeded = 1
    };

    /// Describes the outcome of explicitly adding one value with permission to replace the oldest value.
    enum class CircularBufferPushOverwriteResult : std::uint8_t {
        Succeeded = 0,
        CapacityUnavailable = 1
    };

    /// Describes the outcome of removing the oldest value from a bounded CircularBuffer.
    enum class CircularBufferPopResult : std::uint8_t {
        Succeeded = 0,
        Empty = 1
    };

    /// Describes the outcome of checked CircularBuffer element access.
    enum class CircularBufferAccessResult : std::uint8_t {
        Succeeded = 0,
        IndexOutOfRange = 1
    };

    /// Stores oldest-to-newest values in bounded ring storage with explicit reject or overwrite insertion semantics.
    template<class TValue, std::size_t TCapacity>
    class CircularBuffer final {

    private:

        // Underlying bounded ring storage.

        /// Stores CircularBuffer values in logical oldest-to-newest order.
        Deque<TValue, TCapacity> _values{};

    public:

        static_assert(
            IsMemoryBoundedValue<TValue>,
            "Bounded::CircularBuffer TValue must be a certified memory-bounded value."
        );

        static_assert(
            std::is_nothrow_destructible_v<TValue>,
            "Bounded::CircularBuffer TValue must be nothrow-destructible."
        );

        /// Identifies mutable logical iteration over oldest-to-newest values.
        using Iterator = typename Deque<TValue, TCapacity>::Iterator;

        /// Identifies read-only logical iteration over oldest-to-newest values.
        using ConstIterator = typename Deque<TValue, TCapacity>::ConstIterator;

        /// Creates an empty bounded CircularBuffer.
        constexpr CircularBuffer() noexcept = default;

        /// Copies a bounded CircularBuffer.
        CircularBuffer(
            const CircularBuffer& source
        ) noexcept = default;

        /// Moves a bounded CircularBuffer and leaves the source empty.
        CircularBuffer(
            CircularBuffer&& source
        ) noexcept = default;

        /// Copies a bounded CircularBuffer.
        CircularBuffer& operator=(
            const CircularBuffer& source
        ) noexcept = default;

        /// Moves a bounded CircularBuffer and leaves the source empty.
        CircularBuffer& operator=(
            CircularBuffer&& source
        ) noexcept = default;

        // Capacity and state inspection.

        /// Returns the maximum number of values this CircularBuffer can retain.
        static constexpr std::size_t Capacity() noexcept {
            return TCapacity;
        }

        /// Returns the current number of retained values.
        constexpr std::size_t Size() const noexcept {
            return _values.Size();
        }

        /// Reports whether this CircularBuffer currently contains no values.
        constexpr bool IsEmpty() const noexcept {
            return _values.IsEmpty();
        }

        /// Reports whether this CircularBuffer currently occupies every available slot.
        constexpr bool IsFull() const noexcept {
            return _values.IsFull();
        }

        // Element access.

        /// Returns a mutable reference to the oldest retained value; the buffer must already be non-empty.
        TValue& Front() noexcept {
            return _values.Front();
        }

        /// Returns a read-only reference to the oldest retained value; the buffer must already be non-empty.
        const TValue& Front() const noexcept {
            return _values.Front();
        }

        /// Returns a mutable reference to the newest retained value; the buffer must already be non-empty.
        TValue& Back() noexcept {
            return _values.Back();
        }

        /// Returns a read-only reference to the newest retained value; the buffer must already be non-empty.
        const TValue& Back() const noexcept {
            return _values.Back();
        }

        /// Returns a mutable reference at an oldest-to-newest index that must already satisfy index < Size().
        TValue& operator[](
            std::size_t index
        ) noexcept {
            return _values[index];
        }

        /// Returns a read-only reference at an oldest-to-newest index that must already satisfy index < Size().
        const TValue& operator[](
            std::size_t index
        ) const noexcept {
            return _values[index];
        }

        /// Retrieves a mutable element pointer through a checked oldest-to-newest index.
        CircularBufferAccessResult GetAt(
            std::size_t index,
            TValue*& value
        ) noexcept {
            if (_values.GetAt(
                index,
                value
            ) != DequeAccessResult::Succeeded) {
                value = nullptr;
                return CircularBufferAccessResult::IndexOutOfRange;
            }

            return CircularBufferAccessResult::Succeeded;
        }

        /// Retrieves a read-only element pointer through a checked oldest-to-newest index.
        CircularBufferAccessResult GetAt(
            std::size_t index,
            const TValue*& value
        ) const noexcept {
            if (_values.GetAt(
                index,
                value
            ) != DequeAccessResult::Succeeded) {
                value = nullptr;
                return CircularBufferAccessResult::IndexOutOfRange;
            }

            return CircularBufferAccessResult::Succeeded;
        }

        // Iteration.

        /// Returns a mutable iterator to the oldest retained value.
        Iterator begin() noexcept {
            return _values.begin();
        }

        /// Returns a mutable iterator one position beyond the newest retained value.
        Iterator end() noexcept {
            return _values.end();
        }

        /// Returns a read-only iterator to the oldest retained value.
        ConstIterator begin() const noexcept {
            return _values.begin();
        }

        /// Returns a read-only iterator one position beyond the newest retained value.
        ConstIterator end() const noexcept {
            return _values.end();
        }

        /// Returns a read-only iterator to the oldest retained value.
        ConstIterator cbegin() const noexcept {
            return _values.cbegin();
        }

        /// Returns a read-only iterator one position beyond the newest retained value.
        ConstIterator cend() const noexcept {
            return _values.cend();
        }

        // Reject-on-full insertion.

        /// Copies one value into the newest position and rejects insertion when full.
        CircularBufferPushResult Push(
            const TValue& value
        ) noexcept {
            if (_values.PushBack(value) != DequePushBackResult::Succeeded) { return CircularBufferPushResult::CapacityExceeded; }

            return CircularBufferPushResult::Succeeded;
        }

        /// Moves one value into the newest position and rejects insertion when full.
        CircularBufferPushResult Push(
            TValue&& value
        ) noexcept {
            const auto result = _values.PushBack(
                std::move(
                    value
                )
            );

            if (result != DequePushBackResult::Succeeded) { return CircularBufferPushResult::CapacityExceeded; }

            return CircularBufferPushResult::Succeeded;
        }

        /// Constructs one newest value directly and rejects insertion when full.
        template<class... TArgumentTypes>
        CircularBufferPushResult Emplace(
            TArgumentTypes&&... arguments
        ) noexcept {
            if (_values.EmplaceBack(
                std::forward<TArgumentTypes>(arguments)...
            ) != DequeEmplaceBackResult::Succeeded) {
                return CircularBufferPushResult::CapacityExceeded;
            }

            return CircularBufferPushResult::Succeeded;
        }

        // Explicit overwrite insertion.

        /// Copies one value as newest, replacing the oldest value first when the buffer is full.
        CircularBufferPushOverwriteResult PushOverwrite(
            const TValue& value
        ) noexcept {
            static_assert(
                std::is_nothrow_copy_constructible_v<TValue>,
                "CircularBuffer::PushOverwrite(const TValue&) requires TValue to be nothrow copy-constructible."
            );

            if constexpr (TCapacity == 0U) {
                return CircularBufferPushOverwriteResult::CapacityUnavailable;
            }

            if (IsFull()) {
                _values.PopFront();
            }

            _values.PushBack(value);
            return CircularBufferPushOverwriteResult::Succeeded;
        }

        /// Moves one non-aliasing value as newest, replacing the oldest value first when the buffer is full.
        CircularBufferPushOverwriteResult PushOverwrite(
            TValue&& value
        ) noexcept {
            static_assert(
                std::is_nothrow_move_constructible_v<TValue>,
                "CircularBuffer::PushOverwrite(TValue&&) requires TValue to be nothrow move-constructible."
            );

            if constexpr (TCapacity == 0U) {
                return CircularBufferPushOverwriteResult::CapacityUnavailable;
            }

            if (IsFull()) {
                _values.PopFront();
            }

            _values.PushBack(
                std::move(
                    value
                )
            );
            return CircularBufferPushOverwriteResult::Succeeded;
        }

        /// Constructs one newest value after explicitly discarding the oldest value when the buffer is full.
        template<class... TArgumentTypes>
        CircularBufferPushOverwriteResult EmplaceOverwrite(
            TArgumentTypes&&... arguments
        ) noexcept {
            static_assert(
                std::is_nothrow_constructible_v<TValue, TArgumentTypes...>,
                "CircularBuffer::EmplaceOverwrite requires TValue to be nothrow-constructible from the supplied arguments."
            );

            if constexpr (TCapacity == 0U) {
                return CircularBufferPushOverwriteResult::CapacityUnavailable;
            }

            if (IsFull()) {
                _values.PopFront();
            }

            _values.EmplaceBack(
                std::forward<TArgumentTypes>(arguments)...
            );
            return CircularBufferPushOverwriteResult::Succeeded;
        }

        // Removal and clear.

        /// Removes the oldest retained value when the CircularBuffer is non-empty.
        CircularBufferPopResult PopFront() noexcept {
            if (_values.PopFront() != DequePopFrontResult::Succeeded) { return CircularBufferPopResult::Empty; }

            return CircularBufferPopResult::Succeeded;
        }

        /// Destroys all retained values and returns the CircularBuffer to its empty state.
        void Clear() noexcept {
            _values.Clear();
        }

    };

    /// Certifies a bounded CircularBuffer when its recursively owned element Type is memory-bounded.
    template<class TValue, std::size_t TCapacity>
    struct MemoryBoundedTraits<CircularBuffer<TValue, TCapacity>> : MemoryBoundedValueDeclaration<
        false,
        TValue
    > {
    };

    /// Exposes the compile-time element capacity of a bounded CircularBuffer.
    template<class TValue, std::size_t TCapacity>
    struct CapacityTraits<CircularBuffer<TValue, TCapacity>> {

        /// Bounded CircularBuffer capacity is known at compile time.
        static constexpr bool HasStaticCapacity = true;

        /// Bounded CircularBuffer logical capacity is measured in elements.
        static constexpr CapacityUnit Unit = CapacityUnit::Elements;

        /// Reports the maximum number of retained values.
        static constexpr std::size_t Capacity = TCapacity;

    };

} // ESPressio::Bounded
