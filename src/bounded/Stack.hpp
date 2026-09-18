#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

#include "CapacityTraits.hpp"
#include "MemoryBoundedTraits.hpp"
#include "Vector.hpp"

namespace ESPressio::Bounded {

    /// Describes the outcome of adding one value to a bounded LIFO Stack.
    enum class StackPushResult : std::uint8_t {
        Succeeded = 0,
        CapacityExceeded = 1
    };

    /// Describes the outcome of in-place construction at the top of a bounded LIFO Stack.
    enum class StackEmplaceResult : std::uint8_t {
        Succeeded = 0,
        CapacityExceeded = 1
    };

    /// Describes the outcome of removing the top value from a bounded LIFO Stack.
    enum class StackPopResult : std::uint8_t {
        Succeeded = 0,
        Empty = 1
    };

    /// Describes the outcome of checked access to the top value of a bounded LIFO Stack.
    enum class StackAccessResult : std::uint8_t {
        Succeeded = 0,
        Empty = 1
    };

    /// Provides a bounded LIFO abstraction backed by fully preallocated inline sequence storage.
    template<class TValue, std::size_t TCapacity>
    class Stack final {

    private:

        // Underlying bounded storage.

        /// Stores Stack values while keeping arbitrary sequence operations outside the public Stack surface.
        Vector<TValue, TCapacity> _values{};

    public:

        static_assert(
            IsMemoryBoundedValue<TValue>,
            "Bounded::Stack TValue must be a certified memory-bounded value."
        );

        static_assert(
            std::is_nothrow_destructible_v<TValue>,
            "Bounded::Stack TValue must be nothrow-destructible."
        );

        /// Creates an empty bounded Stack.
        constexpr Stack() noexcept = default;

        /// Copies a bounded Stack when TValue supports the Vector copy requirements.
        Stack(
            const Stack& source
        ) noexcept = default;

        /// Moves a bounded Stack and leaves the source empty.
        Stack(
            Stack&& source
        ) noexcept = default;

        /// Copies a bounded Stack.
        Stack& operator=(
            const Stack& source
        ) noexcept = default;

        /// Moves a bounded Stack and leaves the source empty.
        Stack& operator=(
            Stack&& source
        ) noexcept = default;

        // Capacity and state inspection.

        /// Returns the maximum number of values this Stack can retain.
        static constexpr std::size_t Capacity() noexcept {
            return TCapacity;
        }

        /// Returns the current number of stacked values.
        constexpr std::size_t Size() const noexcept {
            return _values.Size();
        }

        /// Reports whether this Stack currently contains no values.
        constexpr bool IsEmpty() const noexcept {
            return _values.IsEmpty();
        }

        /// Reports whether this Stack currently occupies every available slot.
        constexpr bool IsFull() const noexcept {
            return _values.IsFull();
        }

        // Top access.

        /// Returns a mutable reference to the top value; the Stack must already be non-empty.
        TValue& Top() noexcept {
            return _values[Size() - 1U];
        }

        /// Returns a read-only reference to the top value; the Stack must already be non-empty.
        const TValue& Top() const noexcept {
            return _values[Size() - 1U];
        }

        /// Retrieves a mutable top-value pointer through a checked access path.
        StackAccessResult GetTop(
            TValue*& value
        ) noexcept {
            if (IsEmpty()) {
                value = nullptr;
                return StackAccessResult::Empty;
            }

            value = std::addressof(
                Top()
            );
            return StackAccessResult::Succeeded;
        }

        /// Retrieves a read-only top-value pointer through a checked access path.
        StackAccessResult GetTop(
            const TValue*& value
        ) const noexcept {
            if (IsEmpty()) {
                value = nullptr;
                return StackAccessResult::Empty;
            }

            value = std::addressof(
                Top()
            );
            return StackAccessResult::Succeeded;
        }

        // Push operations.

        /// Copies one value onto the Stack when capacity remains available.
        StackPushResult Push(
            const TValue& value
        ) noexcept {
            if (_values.PushBack(value) != VectorPushBackResult::Succeeded) { return StackPushResult::CapacityExceeded; }

            return StackPushResult::Succeeded;
        }

        /// Moves one value onto the Stack when capacity remains available.
        StackPushResult Push(
            TValue&& value
        ) noexcept {
            const auto result = _values.PushBack(
                std::move(
                    value
                )
            );

            if (result != VectorPushBackResult::Succeeded) { return StackPushResult::CapacityExceeded; }

            return StackPushResult::Succeeded;
        }

        /// Constructs one value directly in the Stack's top slot.
        template<class... TArgumentTypes>
        StackEmplaceResult Emplace(
            TArgumentTypes&&... arguments
        ) noexcept {
            const auto result = _values.EmplaceBack(
                std::forward<TArgumentTypes>(arguments)...
            );

            if (result != VectorEmplaceBackResult::Succeeded) { return StackEmplaceResult::CapacityExceeded; }

            return StackEmplaceResult::Succeeded;
        }

        // Pop operations.

        /// Removes the top value when the Stack is non-empty.
        StackPopResult Pop() noexcept {
            if (_values.PopBack() != VectorPopBackResult::Succeeded) { return StackPopResult::Empty; }

            return StackPopResult::Succeeded;
        }

        /// Moves the top value into caller-owned output and then removes it from the Stack.
        StackPopResult Pop(
            TValue& output
        ) noexcept {
            static_assert(
                std::is_nothrow_move_assignable_v<TValue>,
                "Stack::Pop(output) requires TValue to be nothrow move-assignable."
            );

            if (IsEmpty()) { return StackPopResult::Empty; }

            output = std::move(
                Top()
            );
            _values.PopBack();

            return StackPopResult::Succeeded;
        }

        /// Destroys all stacked values and returns the Stack to its empty state.
        void Clear() noexcept {
            _values.Clear();
        }

    };

    /// Certifies a bounded Stack when its recursively owned element Type is memory-bounded.
    template<class TValue, std::size_t TCapacity>
    struct MemoryBoundedTraits<Stack<TValue, TCapacity>> : MemoryBoundedValueDeclaration<
        false,
        TValue
    > {
    };

    /// Exposes the compile-time element capacity of a bounded Stack.
    template<class TValue, std::size_t TCapacity>
    struct CapacityTraits<Stack<TValue, TCapacity>> {

        /// Bounded Stack capacity is known at compile time.
        static constexpr bool HasStaticCapacity = true;

        /// Bounded Stack logical capacity is measured in elements.
        static constexpr CapacityUnit Unit = CapacityUnit::Elements;

        /// Reports the maximum number of stacked values.
        static constexpr std::size_t Capacity = TCapacity;

    };

} // ESPressio::Bounded
