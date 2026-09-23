# src/bounded/Set.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `f5742d864310dc4fdecb734721205b0e21c38e40`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-BoundedTypes/blob/f5742d864310dc4fdecb734721205b0e21c38e40/src/bounded/Set.hpp)

## Direct includes

- `cstddef`
- `cstdint`
- `memory`
- `type_traits`
- `utility`
- `CapacityTraits.hpp`
- `MemoryBoundedTraits.hpp`
- `Vector.hpp`

## Documented declarations

### `SetInsertResult`

**Classification:** PUBLIC API

Describes the outcome of inserting a unique value into a bounded Set.

```cpp
enum class SetInsertResult : std::uint8_t
```

### `SetEraseResult`

**Classification:** PUBLIC API

Describes the outcome of erasing a value from a bounded Set.

```cpp
enum class SetEraseResult : std::uint8_t
```

### `SetFindResult`

**Classification:** PUBLIC API

Describes the outcome of locating a value in a bounded Set.

```cpp
enum class SetFindResult : std::uint8_t
```

### `TValue`

**Classification:** PUBLIC API

Stores unique values in deterministic insertion order using fully preallocated inline storage and linear lookup.

```cpp
template<class TValue, std::size_t TCapacity>
    class Set final
```

### `TValue`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Stores unique values in insertion order while keeping mutation behind Set invariant checks.

```cpp
Vector<TValue, TCapacity> _values{};
```

### `FindIndex`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Locates a matching value and returns its insertion-order index.

```cpp
bool FindIndex(
            const TValue& value,
            std::size_t& index
        ) const noexcept
```

### `ConstIterator`

**Classification:** PUBLIC API · source access: `public`

Identifies read-only insertion-order iteration over Set values.

```cpp
using ConstIterator = typename Vector<TValue, TCapacity>::ConstIterator;
```

### `Set`

**Classification:** PUBLIC API · source access: `public`

Creates an empty bounded Set.

```cpp
constexpr Set() noexcept = default;
```

### `Set`

**Classification:** PUBLIC API · source access: `public`

Copies a bounded Set.

```cpp
Set(
            const Set& source
        ) noexcept = default;
```

### `Set`

**Classification:** PUBLIC API · source access: `public`

Moves a bounded Set and leaves the source empty.

```cpp
Set(
            Set&& source
        ) noexcept = default;
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Copies a bounded Set.

```cpp
Set& operator=(
            const Set& source
        ) noexcept = default;
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Moves a bounded Set and leaves the source empty.

```cpp
Set& operator=(
            Set&& source
        ) noexcept = default;
```

### `Capacity`

**Classification:** PUBLIC API · source access: `public`

Returns the maximum number of unique entries this Set can retain.

```cpp
static constexpr std::size_t Capacity() noexcept
```

### `Size`

**Classification:** PUBLIC API · source access: `public`

Returns the current number of unique logical entries.

```cpp
constexpr std::size_t Size() const noexcept
```

### `IsEmpty`

**Classification:** PUBLIC API · source access: `public`

Reports whether this Set currently contains no values.

```cpp
constexpr bool IsEmpty() const noexcept
```

### `IsFull`

**Classification:** PUBLIC API · source access: `public`

Reports whether this Set currently occupies every available entry slot.

```cpp
constexpr bool IsFull() const noexcept
```

### `Contains`

**Classification:** PUBLIC API · source access: `public`

Reports whether this Set contains a value equal to the supplied candidate.

```cpp
bool Contains(
            const TValue& value
        ) const noexcept
```

### `Find`

**Classification:** PUBLIC API · source access: `public`

Locates a matching value and returns a read-only pointer to the stored instance.

```cpp
SetFindResult Find(
            const TValue& value,
            const TValue*& result
        ) const noexcept
```

### `Insert`

**Classification:** PUBLIC API · source access: `public`

Copies a unique value into insertion-order storage.

```cpp
SetInsertResult Insert(
            const TValue& value
        ) noexcept
```

### `Insert`

**Classification:** PUBLIC API · source access: `public`

Moves a unique non-aliasing value into insertion-order storage.

```cpp
SetInsertResult Insert(
            TValue&& value
        ) noexcept
```

### `Emplace`

**Classification:** PUBLIC API · source access: `public`

Constructs a candidate value, rejects duplicates, and publishes it only when capacity remains available.

```cpp
template<class... TArgumentTypes>
        SetInsertResult Emplace(
            TArgumentTypes&&... arguments
        ) noexcept
```

### `Erase`

**Classification:** PUBLIC API · source access: `public`

Erases one matching value while preserving the insertion order of all remaining values.

```cpp
SetEraseResult Erase(
            const TValue& value
        ) noexcept
```

### `Clear`

**Classification:** PUBLIC API · source access: `public`

Destroys all retained values and returns the Set to its empty state.

```cpp
void Clear() noexcept
```

### `begin`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only iterator to the earliest inserted retained value.

```cpp
ConstIterator begin() noexcept
```

### `end`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only iterator one position beyond the latest inserted retained value.

```cpp
ConstIterator end() noexcept
```

### `begin`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only iterator to the earliest inserted retained value.

```cpp
ConstIterator begin() const noexcept
```

### `end`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only iterator one position beyond the latest inserted retained value.

```cpp
ConstIterator end() const noexcept
```

### `cbegin`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only iterator to the earliest inserted retained value.

```cpp
ConstIterator cbegin() const noexcept
```

### `cend`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only iterator one position beyond the latest inserted retained value.

```cpp
ConstIterator cend() const noexcept
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Reports whether two Sets contain the same logical values irrespective of insertion order.

```cpp
bool operator==(
            const Set& other
        ) const noexcept
```

### `bool operator!=( const Set& other ) const noexcept`

**Classification:** PUBLIC API · source access: `public`

Reports whether two Sets contain different logical value sets.

```cpp
bool operator!=(
            const Set& other
        ) const noexcept
```

### `TValue`

**Classification:** PUBLIC API

Certifies a bounded Set when its recursively owned element Type is memory-bounded.

```cpp
template<class TValue, std::size_t TCapacity>
    struct MemoryBoundedTraits<Set<TValue, TCapacity>> : MemoryBoundedValueDeclaration<
        false,
```

### `TValue`

**Classification:** PUBLIC API

Exposes the compile-time entry capacity of a bounded Set.

```cpp
template<class TValue, std::size_t TCapacity>
    struct CapacityTraits<Set<TValue, TCapacity>>
```

### `HasStaticCapacity`

**Classification:** PUBLIC API · source access: `public`

Bounded Set capacity is known at compile time.

```cpp
static constexpr bool HasStaticCapacity = true;
```

### `Unit`

**Classification:** PUBLIC API · source access: `public`

Bounded Set logical capacity is measured in unique entries.

```cpp
static constexpr CapacityUnit Unit = CapacityUnit::Entries;
```

### `Capacity`

**Classification:** PUBLIC API · source access: `public`

Reports the maximum number of unique values retained by the Set.

```cpp
static constexpr std::size_t Capacity = TCapacity;
```

