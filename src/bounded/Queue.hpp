#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <utility>

#include "CapacityTraits.hpp"
#include "Deque.hpp"
#include "MemoryBoundedTraits.hpp"

namespace ESPressio::Bounded {

    /// Describes the outcome of adding one value to a bounded FIFO Queue.
    enum class QueueEnqueueResult : std::uint8_t {
        Succeeded = 0,
        CapacityExceeded = 1
    };

    /// Describes the outcome of in-place construction at the back of a bounded FIFO Queue.
    enum class QueueEmplaceResult : std::uint8_t {
        Succeeded = 0,
        CapacityExceeded = 1
    };

    /// Describes the outcome of removing the front value from a bounded FIFO Queue.
    enum class QueueDequeueResult : std::uint8_t {
        Succeeded = 0,
        Empty = 1
    };

    /// Describes the outcome of checked access to the front value of a bounded FIFO Queue.
    enum class QueueAccessResult : std::uint8_t {
        Succeeded = 0,
        Empty = 1
    };

    /// Provides a bounded FIFO abstraction backed by fully preallocated inline ring storage.
    template<class TValue, std::size_t TCapacity>
    class Queue final {

    private:

        // Underlying bounded storage.

        /// Stores FIFO values in a bounded ring while keeping random-access details outside the public Queue surface.
        Deque<TValue, TCapacity> _values{};

    public:

        static_assert(
            IsMemoryBoundedValue<TValue>,
            "Bounded::Queue TValue must be a certified memory-bounded value."
        );

        static_assert(
            std::is_nothrow_destructible_v<TValue>,
            "Bounded::Queue TValue must be nothrow-destructible."
        );

        /// Creates an empty bounded Queue.
        constexpr Queue() noexcept = default;

        /// Copies a bounded Queue when TValue supports the Deque copy requirements.
        Queue(
            const Queue& source
        ) noexcept = default;

        /// Moves a bounded Queue and leaves the source empty.
        Queue(
            Queue&& source
        ) noexcept = default;

        /// Copies a bounded Queue.
        Queue& operator=(
            const Queue& source
        ) noexcept = default;

        /// Moves a bounded Queue and leaves the source empty.
        Queue& operator=(
            Queue&& source
        ) noexcept = default;

        // Capacity and state inspection.

        /// Returns the maximum number of values this Queue can retain.
        static constexpr std::size_t Capacity() noexcept {
            return TCapacity;
        }

        /// Returns the current number of queued values.
        constexpr std::size_t Size() const noexcept {
            return _values.Size();
        }

        /// Reports whether this Queue currently contains no values.
        constexpr bool IsEmpty() const noexcept {
            return _values.IsEmpty();
        }

        /// Reports whether this Queue currently occupies every available slot.
        constexpr bool IsFull() const noexcept {
            return _values.IsFull();
        }

        // Front access.

        /// Returns a mutable reference to the front value; the Queue must already be non-empty.
        TValue& Front() noexcept {
            return _values.Front();
        }

        /// Returns a read-only reference to the front value; the Queue must already be non-empty.
        const TValue& Front() const noexcept {
            return _values.Front();
        }

        /// Retrieves a mutable front-value pointer through a checked access path.
        QueueAccessResult GetFront(
            TValue*& value
        ) noexcept {
            if (_values.GetFront(value) != DequeAccessResult::Succeeded) {
                value = nullptr;
                return QueueAccessResult::Empty;
            }

            return QueueAccessResult::Succeeded;
        }

        /// Retrieves a read-only front-value pointer through a checked access path.
        QueueAccessResult GetFront(
            const TValue*& value
        ) const noexcept {
            if (_values.GetFront(value) != DequeAccessResult::Succeeded) {
                value = nullptr;
                return QueueAccessResult::Empty;
            }

            return QueueAccessResult::Succeeded;
        }

        // Enqueue operations.

        /// Copies one value into the FIFO back when capacity remains available.
        QueueEnqueueResult Enqueue(
            const TValue& value
        ) noexcept {
            const auto result = _values.PushBack(value);

            if (result != DequePushBackResult::Succeeded) { return QueueEnqueueResult::CapacityExceeded; }

            return QueueEnqueueResult::Succeeded;
        }

        /// Moves one value into the FIFO back when capacity remains available.
        QueueEnqueueResult Enqueue(
            TValue&& value
        ) noexcept {
            const auto result = _values.PushBack(
                std::move(
                    value
                )
            );

            if (result != DequePushBackResult::Succeeded) { return QueueEnqueueResult::CapacityExceeded; }

            return QueueEnqueueResult::Succeeded;
        }

        /// Constructs one value directly in the FIFO back slot.
        template<class... TArgumentTypes>
        QueueEmplaceResult Emplace(
            TArgumentTypes&&... arguments
        ) noexcept {
            const auto result = _values.EmplaceBack(
                std::forward<TArgumentTypes>(arguments)...
            );

            if (result != DequeEmplaceBackResult::Succeeded) { return QueueEmplaceResult::CapacityExceeded; }

            return QueueEmplaceResult::Succeeded;
        }

        // Dequeue operations.

        /// Removes the front value when the Queue is non-empty.
        QueueDequeueResult Dequeue() noexcept {
            if (_values.PopFront() != DequePopFrontResult::Succeeded) { return QueueDequeueResult::Empty; }

            return QueueDequeueResult::Succeeded;
        }

        /// Moves the front value into caller-owned output and then removes it from the Queue.
        QueueDequeueResult Dequeue(
            TValue& output
        ) noexcept {
            static_assert(
                std::is_nothrow_move_assignable_v<TValue>,
                "Queue::Dequeue(output) requires TValue to be nothrow move-assignable."
            );

            if (IsEmpty()) { return QueueDequeueResult::Empty; }

            output = std::move(
                _values.Front()
            );
            _values.PopFront();

            return QueueDequeueResult::Succeeded;
        }

        /// Destroys all queued values and returns the Queue to its empty state.
        void Clear() noexcept {
            _values.Clear();
        }

    };

    /// Certifies a bounded Queue when its recursively owned element Type is memory-bounded.
    template<class TValue, std::size_t TCapacity>
    struct MemoryBoundedTraits<Queue<TValue, TCapacity>> : MemoryBoundedValueDeclaration<
        false,
        TValue
    > {
    };

    /// Exposes the compile-time element capacity of a bounded Queue.
    template<class TValue, std::size_t TCapacity>
    struct CapacityTraits<Queue<TValue, TCapacity>> {

        /// Bounded Queue capacity is known at compile time.
        static constexpr bool HasStaticCapacity = true;

        /// Bounded Queue logical capacity is measured in elements.
        static constexpr CapacityUnit Unit = CapacityUnit::Elements;

        /// Reports the maximum number of queued values.
        static constexpr std::size_t Capacity = TCapacity;

    };

} // ESPressio::Bounded
