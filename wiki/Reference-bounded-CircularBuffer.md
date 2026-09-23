# src/bounded/CircularBuffer.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `fa3ab3cbe1652ff74930ed6fc209658dd54551b5`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-BoundedTypes/blob/fa3ab3cbe1652ff74930ed6fc209658dd54551b5/src/bounded/CircularBuffer.hpp)

## Direct includes

- `cstddef`
- `cstdint`
- `type_traits`
- `utility`
- `CapacityTraits.hpp`
- `Deque.hpp`
- `MemoryBoundedTraits.hpp`

## Documented declarations

### `CircularBufferPushResult`

**Classification:** PUBLIC API

Describes the outcome of adding one value to a CircularBuffer without overwrite permission.

```cpp
enum class CircularBufferPushResult : std::uint8_t
```

### `CircularBufferPushOverwriteResult`

**Classification:** PUBLIC API

Describes the outcome of explicitly adding one value with permission to replace the oldest value.

```cpp
enum class CircularBufferPushOverwriteResult : std::uint8_t
```

### `CircularBufferPopResult`

**Classification:** PUBLIC API

Describes the outcome of removing the oldest value from a bounded CircularBuffer.

```cpp
enum class CircularBufferPopResult : std::uint8_t
```

### `CircularBufferAccessResult`

**Classification:** PUBLIC API

Describes the outcome of checked CircularBuffer element access.

```cpp
enum class CircularBufferAccessResult : std::uint8_t
```

### `TValue`

**Classification:** PUBLIC API

Stores oldest-to-newest values in bounded ring storage with explicit reject or overwrite insertion semantics.

```cpp
template<class TValue, std::size_t TCapacity>
    class CircularBuffer final
```

### `TValue`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Stores CircularBuffer values in logical oldest-to-newest order.

```cpp
Deque<TValue, TCapacity> _values{};
```

### `Iterator`

**Classification:** PUBLIC API · source access: `public`

Identifies mutable logical iteration over oldest-to-newest values.

```cpp
using Iterator = typename Deque<TValue, TCapacity>::Iterator;
```

### `ConstIterator`

**Classification:** PUBLIC API · source access: `public`

Identifies read-only logical iteration over oldest-to-newest values.

```cpp
using ConstIterator = typename Deque<TValue, TCapacity>::ConstIterator;
```

### `CircularBuffer`

**Classification:** PUBLIC API · source access: `public`

Creates an empty bounded CircularBuffer.

```cpp
constexpr CircularBuffer() noexcept = default;
```

### `CircularBuffer`

**Classification:** PUBLIC API · source access: `public`

Copies a bounded CircularBuffer.

```cpp
CircularBuffer(
            const CircularBuffer& source
        ) noexcept = default;
```

### `CircularBuffer`

**Classification:** PUBLIC API · source access: `public`

Moves a bounded CircularBuffer and leaves the source empty.

```cpp
CircularBuffer(
            CircularBuffer&& source
        ) noexcept = default;
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Copies a bounded CircularBuffer.

```cpp
CircularBuffer& operator=(
            const CircularBuffer& source
        ) noexcept = default;
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Moves a bounded CircularBuffer and leaves the source empty.

```cpp
CircularBuffer& operator=(
            CircularBuffer&& source
        ) noexcept = default;
```

### `Capacity`

**Classification:** PUBLIC API · source access: `public`

Returns the maximum number of values this CircularBuffer can retain.

```cpp
static constexpr std::size_t Capacity() noexcept
```

### `Size`

**Classification:** PUBLIC API · source access: `public`

Returns the current number of retained values.

```cpp
constexpr std::size_t Size() const noexcept
```

### `IsEmpty`

**Classification:** PUBLIC API · source access: `public`

Reports whether this CircularBuffer currently contains no values.

```cpp
constexpr bool IsEmpty() const noexcept
```

### `IsFull`

**Classification:** PUBLIC API · source access: `public`

Reports whether this CircularBuffer currently occupies every available slot.

```cpp
constexpr bool IsFull() const noexcept
```

### `Front`

**Classification:** PUBLIC API · source access: `public`

Returns a mutable reference to the oldest retained value; the buffer must already be non-empty.

```cpp
TValue& Front() noexcept
```

### `Front`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only reference to the oldest retained value; the buffer must already be non-empty.

```cpp
const TValue& Front() const noexcept
```

### `Back`

**Classification:** PUBLIC API · source access: `public`

Returns a mutable reference to the newest retained value; the buffer must already be non-empty.

```cpp
TValue& Back() noexcept
```

### `Back`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only reference to the newest retained value; the buffer must already be non-empty.

```cpp
const TValue& Back() const noexcept
```

### `TValue& operator[]( std::size_t index ) noexcept`

**Classification:** PUBLIC API · source access: `public`

Returns a mutable reference at an oldest-to-newest index that must already satisfy index < Size().

```cpp
TValue& operator[](
            std::size_t index
        ) noexcept
```

### `const TValue& operator[]( std::size_t index ) const noexcept`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only reference at an oldest-to-newest index that must already satisfy index < Size().

```cpp
const TValue& operator[](
            std::size_t index
        ) const noexcept
```

### `GetAt`

**Classification:** PUBLIC API · source access: `public`

Retrieves a mutable element pointer through a checked oldest-to-newest index.

```cpp
CircularBufferAccessResult GetAt(
            std::size_t index,
            TValue*& value
        ) noexcept
```

### `GetAt`

**Classification:** PUBLIC API · source access: `public`

Retrieves a read-only element pointer through a checked oldest-to-newest index.

```cpp
CircularBufferAccessResult GetAt(
            std::size_t index,
            const TValue*& value
        ) const noexcept
```

### `begin`

**Classification:** PUBLIC API · source access: `public`

Returns a mutable iterator to the oldest retained value.

```cpp
Iterator begin() noexcept
```

### `end`

**Classification:** PUBLIC API · source access: `public`

Returns a mutable iterator one position beyond the newest retained value.

```cpp
Iterator end() noexcept
```

### `begin`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only iterator to the oldest retained value.

```cpp
ConstIterator begin() const noexcept
```

### `end`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only iterator one position beyond the newest retained value.

```cpp
ConstIterator end() const noexcept
```

### `cbegin`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only iterator to the oldest retained value.

```cpp
ConstIterator cbegin() const noexcept
```

### `cend`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only iterator one position beyond the newest retained value.

```cpp
ConstIterator cend() const noexcept
```

### `Push`

**Classification:** PUBLIC API · source access: `public`

Copies one value into the newest position and rejects insertion when full.

```cpp
CircularBufferPushResult Push(
            const TValue& value
        ) noexcept
```

### `Push`

**Classification:** PUBLIC API · source access: `public`

Moves one value into the newest position and rejects insertion when full.

```cpp
CircularBufferPushResult Push(
            TValue&& value
        ) noexcept
```

### `Emplace`

**Classification:** PUBLIC API · source access: `public`

Constructs one newest value directly and rejects insertion when full.

```cpp
template<class... TArgumentTypes>
        CircularBufferPushResult Emplace(
            TArgumentTypes&&... arguments
        ) noexcept
```

### `PushOverwrite`

**Classification:** PUBLIC API · source access: `public`

Copies one value as newest, replacing the oldest value first when the buffer is full.

```cpp
CircularBufferPushOverwriteResult PushOverwrite(
            const TValue& value
        ) noexcept
```

### `PushOverwrite`

**Classification:** PUBLIC API · source access: `public`

Moves one non-aliasing value as newest, replacing the oldest value first when the buffer is full.

```cpp
CircularBufferPushOverwriteResult PushOverwrite(
            TValue&& value
        ) noexcept
```

### `EmplaceOverwrite`

**Classification:** PUBLIC API · source access: `public`

Constructs one newest value after explicitly discarding the oldest value when the buffer is full.

```cpp
template<class... TArgumentTypes>
        CircularBufferPushOverwriteResult EmplaceOverwrite(
            TArgumentTypes&&... arguments
        ) noexcept
```

### `PopFront`

**Classification:** PUBLIC API · source access: `public`

Removes the oldest retained value when the CircularBuffer is non-empty.

```cpp
CircularBufferPopResult PopFront() noexcept
```

### `Clear`

**Classification:** PUBLIC API · source access: `public`

Destroys all retained values and returns the CircularBuffer to its empty state.

```cpp
void Clear() noexcept
```

### `TValue`

**Classification:** PUBLIC API

Certifies a bounded CircularBuffer when its recursively owned element Type is memory-bounded.

```cpp
template<class TValue, std::size_t TCapacity>
    struct MemoryBoundedTraits<CircularBuffer<TValue, TCapacity>> : MemoryBoundedValueDeclaration<
        false,
```

### `TValue`

**Classification:** PUBLIC API

Exposes the compile-time element capacity of a bounded CircularBuffer.

```cpp
template<class TValue, std::size_t TCapacity>
    struct CapacityTraits<CircularBuffer<TValue, TCapacity>>
```

### `HasStaticCapacity`

**Classification:** PUBLIC API · source access: `public`

Bounded CircularBuffer capacity is known at compile time.

```cpp
static constexpr bool HasStaticCapacity = true;
```

### `Unit`

**Classification:** PUBLIC API · source access: `public`

Bounded CircularBuffer logical capacity is measured in elements.

```cpp
static constexpr CapacityUnit Unit = CapacityUnit::Elements;
```

### `Capacity`

**Classification:** PUBLIC API · source access: `public`

Reports the maximum number of retained values.

```cpp
static constexpr std::size_t Capacity = TCapacity;
```

