# src/bounded/Queue.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `f5742d864310dc4fdecb734721205b0e21c38e40`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-BoundedTypes/blob/f5742d864310dc4fdecb734721205b0e21c38e40/src/bounded/Queue.hpp)

## Direct includes

- `cstddef`
- `cstdint`
- `memory`
- `type_traits`
- `utility`
- `CapacityTraits.hpp`
- `Deque.hpp`
- `MemoryBoundedTraits.hpp`

## Documented declarations

### `QueueEnqueueResult`

**Classification:** PUBLIC API

Describes the outcome of adding one value to a bounded FIFO Queue.

```cpp
enum class QueueEnqueueResult : std::uint8_t
```

### `QueueEmplaceResult`

**Classification:** PUBLIC API

Describes the outcome of in-place construction at the back of a bounded FIFO Queue.

```cpp
enum class QueueEmplaceResult : std::uint8_t
```

### `QueueDequeueResult`

**Classification:** PUBLIC API

Describes the outcome of removing the front value from a bounded FIFO Queue.

```cpp
enum class QueueDequeueResult : std::uint8_t
```

### `QueueAccessResult`

**Classification:** PUBLIC API

Describes the outcome of checked access to the front value of a bounded FIFO Queue.

```cpp
enum class QueueAccessResult : std::uint8_t
```

### `TValue`

**Classification:** PUBLIC API

Provides a bounded FIFO abstraction backed by fully preallocated inline ring storage.

```cpp
template<class TValue, std::size_t TCapacity>
    class Queue final
```

### `TValue`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Stores FIFO values in a bounded ring while keeping random-access details outside the public Queue surface.

```cpp
Deque<TValue, TCapacity> _values{};
```

### `Queue`

**Classification:** PUBLIC API · source access: `public`

Creates an empty bounded Queue.

```cpp
constexpr Queue() noexcept = default;
```

### `Queue`

**Classification:** PUBLIC API · source access: `public`

Copies a bounded Queue when TValue supports the Deque copy requirements.

```cpp
Queue(
            const Queue& source
        ) noexcept = default;
```

### `Queue`

**Classification:** PUBLIC API · source access: `public`

Moves a bounded Queue and leaves the source empty.

```cpp
Queue(
            Queue&& source
        ) noexcept = default;
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Copies a bounded Queue.

```cpp
Queue& operator=(
            const Queue& source
        ) noexcept = default;
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Moves a bounded Queue and leaves the source empty.

```cpp
Queue& operator=(
            Queue&& source
        ) noexcept = default;
```

### `Capacity`

**Classification:** PUBLIC API · source access: `public`

Returns the maximum number of values this Queue can retain.

```cpp
static constexpr std::size_t Capacity() noexcept
```

### `Size`

**Classification:** PUBLIC API · source access: `public`

Returns the current number of queued values.

```cpp
constexpr std::size_t Size() const noexcept
```

### `IsEmpty`

**Classification:** PUBLIC API · source access: `public`

Reports whether this Queue currently contains no values.

```cpp
constexpr bool IsEmpty() const noexcept
```

### `IsFull`

**Classification:** PUBLIC API · source access: `public`

Reports whether this Queue currently occupies every available slot.

```cpp
constexpr bool IsFull() const noexcept
```

### `Front`

**Classification:** PUBLIC API · source access: `public`

Returns a mutable reference to the front value; the Queue must already be non-empty.

```cpp
TValue& Front() noexcept
```

### `Front`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only reference to the front value; the Queue must already be non-empty.

```cpp
const TValue& Front() const noexcept
```

### `GetFront`

**Classification:** PUBLIC API · source access: `public`

Retrieves a mutable front-value pointer through a checked access path.

```cpp
QueueAccessResult GetFront(
            TValue*& value
        ) noexcept
```

### `GetFront`

**Classification:** PUBLIC API · source access: `public`

Retrieves a read-only front-value pointer through a checked access path.

```cpp
QueueAccessResult GetFront(
            const TValue*& value
        ) const noexcept
```

### `Enqueue`

**Classification:** PUBLIC API · source access: `public`

Copies one value into the FIFO back when capacity remains available.

```cpp
QueueEnqueueResult Enqueue(
            const TValue& value
        ) noexcept
```

### `Enqueue`

**Classification:** PUBLIC API · source access: `public`

Moves one value into the FIFO back when capacity remains available.

```cpp
QueueEnqueueResult Enqueue(
            TValue&& value
        ) noexcept
```

### `Emplace`

**Classification:** PUBLIC API · source access: `public`

Constructs one value directly in the FIFO back slot.

```cpp
template<class... TArgumentTypes>
        QueueEmplaceResult Emplace(
            TArgumentTypes&&... arguments
        ) noexcept
```

### `Dequeue`

**Classification:** PUBLIC API · source access: `public`

Removes the front value when the Queue is non-empty.

```cpp
QueueDequeueResult Dequeue() noexcept
```

### `Dequeue`

**Classification:** PUBLIC API · source access: `public`

Moves the front value into caller-owned output and then removes it from the Queue.

```cpp
QueueDequeueResult Dequeue(
            TValue& output
        ) noexcept
```

### `Clear`

**Classification:** PUBLIC API · source access: `public`

Destroys all queued values and returns the Queue to its empty state.

```cpp
void Clear() noexcept
```

### `TValue`

**Classification:** PUBLIC API

Certifies a bounded Queue when its recursively owned element Type is memory-bounded.

```cpp
template<class TValue, std::size_t TCapacity>
    struct MemoryBoundedTraits<Queue<TValue, TCapacity>> : MemoryBoundedValueDeclaration<
        false,
```

### `TValue`

**Classification:** PUBLIC API

Exposes the compile-time element capacity of a bounded Queue.

```cpp
template<class TValue, std::size_t TCapacity>
    struct CapacityTraits<Queue<TValue, TCapacity>>
```

### `HasStaticCapacity`

**Classification:** PUBLIC API · source access: `public`

Bounded Queue capacity is known at compile time.

```cpp
static constexpr bool HasStaticCapacity = true;
```

### `Unit`

**Classification:** PUBLIC API · source access: `public`

Bounded Queue logical capacity is measured in elements.

```cpp
static constexpr CapacityUnit Unit = CapacityUnit::Elements;
```

### `Capacity`

**Classification:** PUBLIC API · source access: `public`

Reports the maximum number of queued values.

```cpp
static constexpr std::size_t Capacity = TCapacity;
```

