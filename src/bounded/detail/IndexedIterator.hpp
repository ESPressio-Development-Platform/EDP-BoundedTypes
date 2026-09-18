#pragma once

#include <cstddef>
#include <iterator>
#include <memory>
#include <type_traits>

namespace ESPressio::Bounded::Detail {

    /// Provides random-access logical iteration over an owner that exposes ElementAtUnchecked(index).
    template<class TOwner, class TValue, bool TIsConst>
    class IndexedIterator final {

    private:

        /// Allows mutable and const iterator specializations to construct from one another where safe.
        template<class, class, bool>
        friend class IndexedIterator;

        /// Selects the owner pointer Type according to iterator constness.
        using OwnerPointer = std::conditional_t<
            TIsConst,
            const TOwner*,
            TOwner*
        >;

        // Iterator state.

        /// Identifies the logical container instance whose elements are traversed.
        OwnerPointer _owner = nullptr;

        /// Identifies the current logical element index inside the owner.
        std::size_t _index = 0U;

    public:

        /// Identifies this iterator as a random-access iterator under C++17 iterator conventions.
        using iterator_category = std::random_access_iterator_tag;

        /// Identifies the logical element Type returned by dereference.
        using value_type = TValue;

        /// Identifies the signed difference Type used for iterator arithmetic.
        using difference_type = std::ptrdiff_t;

        /// Selects a mutable or read-only element pointer according to iterator constness.
        using pointer = std::conditional_t<
            TIsConst,
            const TValue*,
            TValue*
        >;

        /// Selects a mutable or read-only element reference according to iterator constness.
        using reference = std::conditional_t<
            TIsConst,
            const TValue&,
            TValue&
        >;

        /// Creates a default iterator that is not associated with an owner.
        constexpr IndexedIterator() noexcept = default;

        /// Creates an iterator at a logical index in the supplied owner.
        constexpr IndexedIterator(
            OwnerPointer owner,
            std::size_t index
        ) noexcept
            : _owner(owner),
              _index(index) {
        }

        /// Converts a mutable iterator to a read-only iterator for the same owner and element Type.
        template<bool TOtherIsConst, std::enable_if_t<TIsConst && !TOtherIsConst, int> = 0>
        constexpr IndexedIterator(
            const IndexedIterator<TOwner, TValue, TOtherIsConst>& other
        ) noexcept
            : _owner(other._owner),
              _index(other._index) {
        }

        /// Returns the logical element referenced by this iterator.
        reference operator*() const noexcept {
            return _owner->ElementAtUnchecked(_index);
        }

        /// Returns a pointer to the logical element referenced by this iterator.
        pointer operator->() const noexcept {
            return std::addressof(
                operator*()
            );
        }

        /// Returns the logical element at an offset from this iterator.
        reference operator[](
            difference_type offset
        ) const noexcept {
            return *(
                *this +
                offset
            );
        }

        /// Advances this iterator to the next logical element.
        IndexedIterator& operator++() noexcept {
            ++_index;
            return *this;
        }

        /// Returns the current iterator and then advances it to the next logical element.
        IndexedIterator operator++(
            int
        ) noexcept {
            auto previous = *this;
            ++(
                *this
            );
            return previous;
        }

        /// Moves this iterator to the previous logical element.
        IndexedIterator& operator--() noexcept {
            --_index;
            return *this;
        }

        /// Returns the current iterator and then moves it to the previous logical element.
        IndexedIterator operator--(
            int
        ) noexcept {
            auto previous = *this;
            --(
                *this
            );
            return previous;
        }

        /// Advances this iterator by a signed logical offset.
        IndexedIterator& operator+=(
            difference_type offset
        ) noexcept {
            _index = static_cast<std::size_t>(
                static_cast<difference_type>(_index) +
                offset
            );
            return *this;
        }

        /// Moves this iterator backward by a signed logical offset.
        IndexedIterator& operator-=(
            difference_type offset
        ) noexcept {
            return operator+=(
                -offset
            );
        }

        /// Returns an iterator advanced by a signed logical offset.
        IndexedIterator operator+(
            difference_type offset
        ) const noexcept {
            auto result = *this;
            result += offset;
            return result;
        }

        /// Returns an iterator moved backward by a signed logical offset.
        IndexedIterator operator-(
            difference_type offset
        ) const noexcept {
            auto result = *this;
            result -= offset;
            return result;
        }

        /// Returns the signed logical distance from another iterator over the same owner.
        difference_type operator-(
            const IndexedIterator& other
        ) const noexcept {
            return static_cast<difference_type>(_index) -
                static_cast<difference_type>(other._index);
        }

        /// Reports whether two iterators identify the same owner and logical position.
        bool operator==(
            const IndexedIterator& other
        ) const noexcept {
            return _owner == other._owner &&
                _index == other._index;
        }

        /// Reports whether two iterators identify different owners or logical positions.
        bool operator!=(
            const IndexedIterator& other
        ) const noexcept {
            return !(
                *this ==
                other
            );
        }

        /// Reports whether this iterator precedes another iterator over the same owner.
        bool operator<(
            const IndexedIterator& other
        ) const noexcept {
            return _index < other._index;
        }

        /// Reports whether this iterator does not follow another iterator over the same owner.
        bool operator<=(
            const IndexedIterator& other
        ) const noexcept {
            return _index <= other._index;
        }

        /// Reports whether this iterator follows another iterator over the same owner.
        bool operator>(
            const IndexedIterator& other
        ) const noexcept {
            return _index > other._index;
        }

        /// Reports whether this iterator does not precede another iterator over the same owner.
        bool operator>=(
            const IndexedIterator& other
        ) const noexcept {
            return _index >= other._index;
        }

        /// Returns a copy of this iterator advanced by a signed logical offset.
        friend IndexedIterator operator+(
            difference_type offset,
            const IndexedIterator& iterator
        ) noexcept {
            return iterator + offset;
        }

    };

} // ESPressio::Bounded::Detail
