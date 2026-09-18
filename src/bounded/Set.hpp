#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <utility>

#include "CapacityTraits.hpp"
#include "MemoryBoundedTraits.hpp"
#include "Vector.hpp"

namespace ESPressio::Bounded {

    /// Describes the outcome of inserting a unique value into a bounded Set.
    enum class SetInsertResult : std::uint8_t {
        Succeeded = 0,
        CapacityExceeded = 1,
        Duplicate = 2
    };

    /// Describes the outcome of erasing a value from a bounded Set.
    enum class SetEraseResult : std::uint8_t {
        Succeeded = 0,
        NotFound = 1
    };

    /// Describes the outcome of locating a value in a bounded Set.
    enum class SetFindResult : std::uint8_t {
        Succeeded = 0,
        NotFound = 1
    };

    /// Stores unique values in deterministic insertion order using fully preallocated inline storage and linear lookup.
    template<class TValue, std::size_t TCapacity>
    class Set final {

    private:

        // Underlying bounded storage.

        /// Stores unique values in insertion order while keeping mutation behind Set invariant checks.
        Vector<TValue, TCapacity> _values{};

        // Lookup helpers.

        /// Locates a matching value and returns its insertion-order index.
        bool FindIndex(
            const TValue& value,
            std::size_t& index
        ) const noexcept {
            static_assert(
                noexcept(
                    std::declval<const TValue&>() ==
                    std::declval<const TValue&>()
                ),
                "Set lookup requires TValue equality comparison to be noexcept."
            );

            for (std::size_t candidate = 0U; candidate < _values.Size(); ++candidate)
                if (_values[candidate] == value) {
                    index = candidate;
                    return true;
                }

            index = 0U;
            return false;
        }

    public:

        static_assert(
            IsMemoryBoundedValue<TValue>,
            "Bounded::Set TValue must be a certified memory-bounded value."
        );

        static_assert(
            std::is_nothrow_destructible_v<TValue>,
            "Bounded::Set TValue must be nothrow-destructible."
        );

        /// Identifies read-only insertion-order iteration over Set values.
        using ConstIterator = typename Vector<TValue, TCapacity>::ConstIterator;

        /// Creates an empty bounded Set.
        constexpr Set() noexcept = default;

        /// Copies a bounded Set.
        Set(
            const Set& source
        ) noexcept = default;

        /// Moves a bounded Set and leaves the source empty.
        Set(
            Set&& source
        ) noexcept = default;

        /// Copies a bounded Set.
        Set& operator=(
            const Set& source
        ) noexcept = default;

        /// Moves a bounded Set and leaves the source empty.
        Set& operator=(
            Set&& source
        ) noexcept = default;

        // Capacity and state inspection.

        /// Returns the maximum number of unique entries this Set can retain.
        static constexpr std::size_t Capacity() noexcept {
            return TCapacity;
        }

        /// Returns the current number of unique logical entries.
        constexpr std::size_t Size() const noexcept {
            return _values.Size();
        }

        /// Reports whether this Set currently contains no values.
        constexpr bool IsEmpty() const noexcept {
            return _values.IsEmpty();
        }

        /// Reports whether this Set currently occupies every available entry slot.
        constexpr bool IsFull() const noexcept {
            return _values.IsFull();
        }

        // Lookup.

        /// Reports whether this Set contains a value equal to the supplied candidate.
        bool Contains(
            const TValue& value
        ) const noexcept {
            std::size_t index = 0U;
            return FindIndex(
                value,
                index
            );
        }

        /// Locates a matching value and returns a read-only pointer to the stored instance.
        SetFindResult Find(
            const TValue& value,
            const TValue*& result
        ) const noexcept {
            std::size_t index = 0U;

            if (!FindIndex(
                value,
                index
            )) {
                result = nullptr;
                return SetFindResult::NotFound;
            }

            result = std::addressof(
                _values[index]
            );
            return SetFindResult::Succeeded;
        }

        // Insertion.

        /// Copies a unique value into insertion-order storage.
        SetInsertResult Insert(
            const TValue& value
        ) noexcept {
            if (Contains(value)) { return SetInsertResult::Duplicate; }

            if (_values.PushBack(value) != VectorPushBackResult::Succeeded) { return SetInsertResult::CapacityExceeded; }

            return SetInsertResult::Succeeded;
        }

        /// Moves a unique non-aliasing value into insertion-order storage.
        SetInsertResult Insert(
            TValue&& value
        ) noexcept {
            if (Contains(value)) { return SetInsertResult::Duplicate; }

            const auto result = _values.PushBack(
                std::move(
                    value
                )
            );

            if (result != VectorPushBackResult::Succeeded) { return SetInsertResult::CapacityExceeded; }

            return SetInsertResult::Succeeded;
        }

        /// Constructs a candidate value, rejects duplicates, and publishes it only when capacity remains available.
        template<class... TArgumentTypes>
        SetInsertResult Emplace(
            TArgumentTypes&&... arguments
        ) noexcept {
            static_assert(
                std::is_nothrow_constructible_v<TValue, TArgumentTypes...>,
                "Set::Emplace requires TValue to be nothrow-constructible from the supplied arguments."
            );

            TValue candidate(
                std::forward<TArgumentTypes>(arguments)...
            );

            if (Contains(candidate)) { return SetInsertResult::Duplicate; }

            if (_values.PushBack(
                std::move(
                    candidate
                )
            ) != VectorPushBackResult::Succeeded) {
                return SetInsertResult::CapacityExceeded;
            }

            return SetInsertResult::Succeeded;
        }

        // Erase and clear.

        /// Erases one matching value while preserving the insertion order of all remaining values.
        SetEraseResult Erase(
            const TValue& value
        ) noexcept {
            std::size_t index = 0U;

            if (!FindIndex(
                value,
                index
            )) {
                return SetEraseResult::NotFound;
            }

            _values.Erase(
                index,
                1U
            );
            return SetEraseResult::Succeeded;
        }

        /// Destroys all retained values and returns the Set to its empty state.
        void Clear() noexcept {
            _values.Clear();
        }

        // Read-only iteration.

        /// Returns a read-only iterator to the earliest inserted retained value.
        ConstIterator begin() noexcept {
            return _values.cbegin();
        }

        /// Returns a read-only iterator one position beyond the latest inserted retained value.
        ConstIterator end() noexcept {
            return _values.cend();
        }

        /// Returns a read-only iterator to the earliest inserted retained value.
        ConstIterator begin() const noexcept {
            return _values.cbegin();
        }

        /// Returns a read-only iterator one position beyond the latest inserted retained value.
        ConstIterator end() const noexcept {
            return _values.cend();
        }

        /// Returns a read-only iterator to the earliest inserted retained value.
        ConstIterator cbegin() const noexcept {
            return _values.cbegin();
        }

        /// Returns a read-only iterator one position beyond the latest inserted retained value.
        ConstIterator cend() const noexcept {
            return _values.cend();
        }

        // Semantic Set equality.

        /// Reports whether two Sets contain the same logical values irrespective of insertion order.
        bool operator==(
            const Set& other
        ) const noexcept {
            if (Size() != other.Size()) { return false; }

            for (const auto& value : _values)
                if (!other.Contains(value)) { return false; }

            return true;
        }

        /// Reports whether two Sets contain different logical value sets.
        bool operator!=(
            const Set& other
        ) const noexcept {
            return !(
                *this ==
                other
            );
        }

    };

    /// Certifies a bounded Set when its recursively owned element Type is memory-bounded.
    template<class TValue, std::size_t TCapacity>
    struct MemoryBoundedTraits<Set<TValue, TCapacity>> : MemoryBoundedValueDeclaration<
        false,
        TValue
    > {
    };

    /// Exposes the compile-time entry capacity of a bounded Set.
    template<class TValue, std::size_t TCapacity>
    struct CapacityTraits<Set<TValue, TCapacity>> {

        /// Bounded Set capacity is known at compile time.
        static constexpr bool HasStaticCapacity = true;

        /// Bounded Set logical capacity is measured in unique entries.
        static constexpr CapacityUnit Unit = CapacityUnit::Entries;

        /// Reports the maximum number of unique values retained by the Set.
        static constexpr std::size_t Capacity = TCapacity;

    };

} // ESPressio::Bounded
