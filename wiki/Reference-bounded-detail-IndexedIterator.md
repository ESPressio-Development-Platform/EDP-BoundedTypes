# src/bounded/detail/IndexedIterator.hpp

**Primary classification:** PRIVATE IMPLEMENTATION

**Source baseline:** `fa3ab3cbe1652ff74930ed6fc209658dd54551b5`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-BoundedTypes/blob/fa3ab3cbe1652ff74930ed6fc209658dd54551b5/src/bounded/detail/IndexedIterator.hpp)

## Direct includes

- `cstddef`
- `iterator`
- `memory`
- `type_traits`

## Documented declarations

### `TOwner`

**Classification:** PRIVATE IMPLEMENTATION

Provides random-access logical iteration over an owner that exposes ElementAtUnchecked(index).

```cpp
template<class TOwner, class TValue, bool TIsConst>
    class IndexedIterator final
```

### `IndexedIterator`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Allows mutable and const iterator specializations to construct from one another where safe.

```cpp
template<class, class, bool>
        friend class IndexedIterator;
```

### `OwnerPointer`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Selects the owner pointer Type according to iterator constness.

```cpp
using OwnerPointer = std::conditional_t<
            TIsConst,
```

### `_owner`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Identifies the logical container instance whose elements are traversed.

```cpp
OwnerPointer _owner = nullptr;
```

### `_index`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Identifies the current logical element index inside the owner.

```cpp
std::size_t _index = 0U;
```

### `iterator_category`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Identifies this iterator as a random-access iterator under C++17 iterator conventions.

```cpp
using iterator_category = std::random_access_iterator_tag;
```

### `value_type`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Identifies the logical element Type returned by dereference.

```cpp
using value_type = TValue;
```

### `difference_type`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Identifies the signed difference Type used for iterator arithmetic.

```cpp
using difference_type = std::ptrdiff_t;
```

### `pointer`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Selects a mutable or read-only element pointer according to iterator constness.

```cpp
using pointer = std::conditional_t<
            TIsConst,
```

### `reference`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Selects a mutable or read-only element reference according to iterator constness.

```cpp
using reference = std::conditional_t<
            TIsConst,
```

### `IndexedIterator`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Creates a default iterator that is not associated with an owner.

```cpp
constexpr IndexedIterator() noexcept = default;
```

### `IndexedIterator`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Creates an iterator at a logical index in the supplied owner.

```cpp
constexpr IndexedIterator(
            OwnerPointer owner,
            std::size_t index
        ) noexcept
            : _owner(owner),
```

### `IndexedIterator`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Converts a mutable iterator to a read-only iterator for the same owner and element Type.

```cpp
template<bool TOtherIsConst, std::enable_if_t<TIsConst && !TOtherIsConst, int> = 0>
        constexpr IndexedIterator(
            const IndexedIterator<TOwner, TValue, TOtherIsConst>& other
        ) noexcept
            : _owner(other._owner),
```

### `reference operator*() const noexcept`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Returns the logical element referenced by this iterator.

```cpp
reference operator*() const noexcept
```

### `pointer operator->() const noexcept`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Returns a pointer to the logical element referenced by this iterator.

```cpp
pointer operator->() const noexcept
```

### `reference operator[]( difference_type offset ) const noexcept`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Returns the logical element at an offset from this iterator.

```cpp
reference operator[](
            difference_type offset
        ) const noexcept
```

### `IndexedIterator& operator++() noexcept`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Advances this iterator to the next logical element.

```cpp
IndexedIterator& operator++() noexcept
```

### `IndexedIterator operator++( int ) noexcept`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Returns the current iterator and then advances it to the next logical element.

```cpp
IndexedIterator operator++(
            int
        ) noexcept
```

### `IndexedIterator& operator--() noexcept`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Moves this iterator to the previous logical element.

```cpp
IndexedIterator& operator--() noexcept
```

### `IndexedIterator operator--( int ) noexcept`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Returns the current iterator and then moves it to the previous logical element.

```cpp
IndexedIterator operator--(
            int
        ) noexcept
```

### `IndexedIterator& operator+=( difference_type offset ) noexcept`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Advances this iterator by a signed logical offset.

```cpp
IndexedIterator& operator+=(
            difference_type offset
        ) noexcept
```

### `IndexedIterator& operator-=( difference_type offset ) noexcept`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Moves this iterator backward by a signed logical offset.

```cpp
IndexedIterator& operator-=(
            difference_type offset
        ) noexcept
```

### `IndexedIterator operator+( difference_type offset ) const noexcept`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Returns an iterator advanced by a signed logical offset.

```cpp
IndexedIterator operator+(
            difference_type offset
        ) const noexcept
```

### `IndexedIterator operator-( difference_type offset ) const noexcept`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Returns an iterator moved backward by a signed logical offset.

```cpp
IndexedIterator operator-(
            difference_type offset
        ) const noexcept
```

### `difference_type operator-( const IndexedIterator& other ) const noexce`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Returns the signed logical distance from another iterator over the same owner.

```cpp
difference_type operator-(
            const IndexedIterator& other
        ) const noexcept
```

### `operator`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Reports whether two iterators identify the same owner and logical position.

```cpp
bool operator==(
            const IndexedIterator& other
        ) const noexcept
```

### `bool operator!=( const IndexedIterator& other ) const noexcept`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Reports whether two iterators identify different owners or logical positions.

```cpp
bool operator!=(
            const IndexedIterator& other
        ) const noexcept
```

### `bool operator<( const IndexedIterator& other ) const noexcept`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Reports whether this iterator precedes another iterator over the same owner.

```cpp
bool operator<(
            const IndexedIterator& other
        ) const noexcept
```

### `bool operator<=( const IndexedIterator& other ) const noexcept`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Reports whether this iterator does not follow another iterator over the same owner.

```cpp
bool operator<=(
            const IndexedIterator& other
        ) const noexcept
```

### `bool operator>( const IndexedIterator& other ) const noexcept`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Reports whether this iterator follows another iterator over the same owner.

```cpp
bool operator>(
            const IndexedIterator& other
        ) const noexcept
```

### `bool operator>=( const IndexedIterator& other ) const noexcept`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Reports whether this iterator does not precede another iterator over the same owner.

```cpp
bool operator>=(
            const IndexedIterator& other
        ) const noexcept
```

### `offset`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Returns a copy of this iterator advanced by a signed logical offset.

```cpp
friend IndexedIterator operator+(
            difference_type offset,
            const IndexedIterator& iterator
        ) noexcept
```

