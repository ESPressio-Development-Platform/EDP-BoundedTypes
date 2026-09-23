# src/bounded/Stack.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `f5742d864310dc4fdecb734721205b0e21c38e40`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-BoundedTypes/blob/f5742d864310dc4fdecb734721205b0e21c38e40/src/bounded/Stack.hpp)

## Direct includes

- `cstddef`
- `cstdint`
- `type_traits`
- `utility`
- `CapacityTraits.hpp`
- `MemoryBoundedTraits.hpp`
- `Vector.hpp`

## Documented declarations

### `StackPushResult`

**Classification:** PUBLIC API

Describes the outcome of adding one value to a bounded LIFO Stack.

```cpp
enum class StackPushResult : std::uint8_t
```

### `StackEmplaceResult`

**Classification:** PUBLIC API

Describes the outcome of in-place construction at the top of a bounded LIFO Stack.

```cpp
enum class StackEmplaceResult : std::uint8_t
```

### `StackPopResult`

**Classification:** PUBLIC API

Describes the outcome of removing the top value from a bounded LIFO Stack.

```cpp
enum class StackPopResult : std::uint8_t
```

### `StackAccessResult`

**Classification:** PUBLIC API

Describes the outcome of checked access to the top value of a bounded LIFO Stack.

```cpp
enum class StackAccessResult : std::uint8_t
```

### `TValue`

**Classification:** PUBLIC API

Provides a bounded LIFO abstraction backed by fully preallocated inline sequence storage.

```cpp
template<class TValue, std::size_t TCapacity>
    class Stack final
```

### `TValue`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Stores Stack values while keeping arbitrary sequence operations outside the public Stack surface.

```cpp
Vector<TValue, TCapacity> _values{};
```

### `Stack`

**Classification:** PUBLIC API · source access: `public`

Creates an empty bounded Stack.

```cpp
constexpr Stack() noexcept = default;
```

### `Stack`

**Classification:** PUBLIC API · source access: `public`

Copies a bounded Stack when TValue supports the Vector copy requirements.

```cpp
Stack(
            const Stack& source
        ) noexcept = default;
```

### `Stack`

**Classification:** PUBLIC API · source access: `public`

Moves a bounded Stack and leaves the source empty.

```cpp
Stack(
            Stack&& source
        ) noexcept = default;
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Copies a bounded Stack.

```cpp
Stack& operator=(
            const Stack& source
        ) noexcept = default;
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Moves a bounded Stack and leaves the source empty.

```cpp
Stack& operator=(
            Stack&& source
        ) noexcept = default;
```

### `Capacity`

**Classification:** PUBLIC API · source access: `public`

Returns the maximum number of values this Stack can retain.

```cpp
static constexpr std::size_t Capacity() noexcept
```

### `Size`

**Classification:** PUBLIC API · source access: `public`

Returns the current number of stacked values.

```cpp
constexpr std::size_t Size() const noexcept
```

### `IsEmpty`

**Classification:** PUBLIC API · source access: `public`

Reports whether this Stack currently contains no values.

```cpp
constexpr bool IsEmpty() const noexcept
```

### `IsFull`

**Classification:** PUBLIC API · source access: `public`

Reports whether this Stack currently occupies every available slot.

```cpp
constexpr bool IsFull() const noexcept
```

### `Top`

**Classification:** PUBLIC API · source access: `public`

Returns a mutable reference to the top value; the Stack must already be non-empty.

```cpp
TValue& Top() noexcept
```

### `Top`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only reference to the top value; the Stack must already be non-empty.

```cpp
const TValue& Top() const noexcept
```

### `GetTop`

**Classification:** PUBLIC API · source access: `public`

Retrieves a mutable top-value pointer through a checked access path.

```cpp
StackAccessResult GetTop(
            TValue*& value
        ) noexcept
```

### `GetTop`

**Classification:** PUBLIC API · source access: `public`

Retrieves a read-only top-value pointer through a checked access path.

```cpp
StackAccessResult GetTop(
            const TValue*& value
        ) const noexcept
```

### `Push`

**Classification:** PUBLIC API · source access: `public`

Copies one value onto the Stack when capacity remains available.

```cpp
StackPushResult Push(
            const TValue& value
        ) noexcept
```

### `Push`

**Classification:** PUBLIC API · source access: `public`

Moves one value onto the Stack when capacity remains available.

```cpp
StackPushResult Push(
            TValue&& value
        ) noexcept
```

### `Emplace`

**Classification:** PUBLIC API · source access: `public`

Constructs one value directly in the Stack's top slot.

```cpp
template<class... TArgumentTypes>
        StackEmplaceResult Emplace(
            TArgumentTypes&&... arguments
        ) noexcept
```

### `Pop`

**Classification:** PUBLIC API · source access: `public`

Removes the top value when the Stack is non-empty.

```cpp
StackPopResult Pop() noexcept
```

### `Pop`

**Classification:** PUBLIC API · source access: `public`

Moves the top value into caller-owned output and then removes it from the Stack.

```cpp
StackPopResult Pop(
            TValue& output
        ) noexcept
```

### `Clear`

**Classification:** PUBLIC API · source access: `public`

Destroys all stacked values and returns the Stack to its empty state.

```cpp
void Clear() noexcept
```

### `TValue`

**Classification:** PUBLIC API

Certifies a bounded Stack when its recursively owned element Type is memory-bounded.

```cpp
template<class TValue, std::size_t TCapacity>
    struct MemoryBoundedTraits<Stack<TValue, TCapacity>> : MemoryBoundedValueDeclaration<
        false,
```

### `TValue`

**Classification:** PUBLIC API

Exposes the compile-time element capacity of a bounded Stack.

```cpp
template<class TValue, std::size_t TCapacity>
    struct CapacityTraits<Stack<TValue, TCapacity>>
```

### `HasStaticCapacity`

**Classification:** PUBLIC API · source access: `public`

Bounded Stack capacity is known at compile time.

```cpp
static constexpr bool HasStaticCapacity = true;
```

### `Unit`

**Classification:** PUBLIC API · source access: `public`

Bounded Stack logical capacity is measured in elements.

```cpp
static constexpr CapacityUnit Unit = CapacityUnit::Elements;
```

### `Capacity`

**Classification:** PUBLIC API · source access: `public`

Reports the maximum number of stacked values.

```cpp
static constexpr std::size_t Capacity = TCapacity;
```

