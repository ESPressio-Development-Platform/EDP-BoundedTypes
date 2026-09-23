# src/bounded/Deque.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `fa3ab3cbe1652ff74930ed6fc209658dd54551b5`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-BoundedTypes/blob/fa3ab3cbe1652ff74930ed6fc209658dd54551b5/src/bounded/Deque.hpp)

## Direct includes

- `cstddef`
- `cstdint`
- `memory`
- `type_traits`
- `utility`
- `CapacityTraits.hpp`
- `MemoryBoundedTraits.hpp`
- `detail/IndexedIterator.hpp`
- `detail/RawSlotStorage.hpp`
- `detail/SizeCounter.hpp`

## Documented declarations

### `DequePushFrontResult`

**Classification:** PUBLIC API

Describes the outcome of inserting one value at the front of a bounded Deque.

```cpp
enum class DequePushFrontResult : std::uint8_t
```

### `DequePushBackResult`

**Classification:** PUBLIC API

Describes the outcome of inserting one value at the back of a bounded Deque.

```cpp
enum class DequePushBackResult : std::uint8_t
```

### `DequeEmplaceFrontResult`

**Classification:** PUBLIC API

Describes the outcome of in-place construction at the front of a bounded Deque.

```cpp
enum class DequeEmplaceFrontResult : std::uint8_t
```

### `DequeEmplaceBackResult`

**Classification:** PUBLIC API

Describes the outcome of in-place construction at the back of a bounded Deque.

```cpp
enum class DequeEmplaceBackResult : std::uint8_t
```

### `DequePopFrontResult`

**Classification:** PUBLIC API

Describes the outcome of removing the front value from a bounded Deque.

```cpp
enum class DequePopFrontResult : std::uint8_t
```

### `DequePopBackResult`

**Classification:** PUBLIC API

Describes the outcome of removing the back value from a bounded Deque.

```cpp
enum class DequePopBackResult : std::uint8_t
```

### `DequeAccessResult`

**Classification:** PUBLIC API

Describes the outcome of checked bounded Deque element access.

```cpp
enum class DequeAccessResult : std::uint8_t
```

### `TValue`

**Classification:** PUBLIC API

Stores a variable-length double-ended logical sequence in fully preallocated inline ring storage.

```cpp
template<class TValue, std::size_t TCapacity>
    class Deque final
```

### `Detail`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Allows logical iterators to access already-live elements without exposing ring storage publicly.

```cpp
template<class, class, bool>
        friend class Detail::IndexedIterator;
```

### `TValue`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Reserves the complete maximum raw slot storage without constructing inactive TValue objects.

```cpp
Detail::RawSlotStorage<TValue, TCapacity> _storage{};
```

### `_size`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Stores the current number of active values.

```cpp
Detail::SizeCounter<TCapacity> _size = 0;
```

### `_head`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Stores the physical slot index containing the current logical front value.

```cpp
Detail::SizeCounter<TCapacity> _head = 0;
```

### `PhysicalIndex`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Maps a valid logical index to its current physical ring slot.

```cpp
std::size_t PhysicalIndex(
            std::size_t logicalIndex
        ) const noexcept
```

### `PreviousHeadIndex`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Returns the physical slot immediately preceding the current head with ring wrap.

```cpp
std::size_t PreviousHeadIndex() const noexcept
```

### `NextHeadIndex`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Returns the physical slot immediately following the current head with ring wrap.

```cpp
std::size_t NextHeadIndex() const noexcept
```

### `ElementAtUnchecked`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Returns a mutable reference to an already-live element at a valid logical index.

```cpp
TValue& ElementAtUnchecked(
            std::size_t index
        ) noexcept
```

### `ElementAtUnchecked`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Returns a read-only reference to an already-live element at a valid logical index.

```cpp
const TValue& ElementAtUnchecked(
            std::size_t index
        ) const noexcept
```

### `DestroyAll`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Destroys every active ring element and resets logical ring state.

```cpp
void DestroyAll() noexcept
```

### `CopyFrom`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Copies all source values in logical order into an empty destination ring.

```cpp
void CopyFrom(
            const Deque& source
        ) noexcept
```

### `MoveFrom`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Moves all source values in logical order into an empty destination ring and empties the source.

```cpp
void MoveFrom(
            Deque& source
        ) noexcept
```

### `ValueType`

**Classification:** PUBLIC API · source access: `public`

Identifies the logical element Type retained by this Deque.

```cpp
using ValueType = TValue;
```

### `Iterator`

**Classification:** PUBLIC API · source access: `public`

Provides mutable random-access logical iteration over ring storage.

```cpp
using Iterator = Detail::IndexedIterator<
            Deque<TValue, TCapacity>,
```

### `ConstIterator`

**Classification:** PUBLIC API · source access: `public`

Provides read-only random-access logical iteration over ring storage.

```cpp
using ConstIterator = Detail::IndexedIterator<
            Deque<TValue, TCapacity>,
```

### `Deque`

**Classification:** PUBLIC API · source access: `public`

Creates an empty bounded Deque without constructing any TValue instances.

```cpp
constexpr Deque() noexcept = default;
```

### `Deque`

**Classification:** PUBLIC API · source access: `public`

Copies a bounded Deque when TValue supports nothrow copy construction.

```cpp
Deque(
            const Deque& source
        ) noexcept
```

### `Deque`

**Classification:** PUBLIC API · source access: `public`

Moves a bounded Deque when TValue supports nothrow move construction and leaves the source empty.

```cpp
Deque(
            Deque&& source
        ) noexcept
```

### `Deque`

**Classification:** PUBLIC API · source access: `public`

Destroys every active TValue while releasing no external storage.

```cpp
~Deque() noexcept
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Copies a bounded Deque when TValue supports nothrow copy construction.

```cpp
Deque& operator=(
            const Deque& source
        ) noexcept
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Moves a bounded Deque when TValue supports nothrow move construction and leaves the source empty.

```cpp
Deque& operator=(
            Deque&& source
        ) noexcept
```

### `Capacity`

**Classification:** PUBLIC API · source access: `public`

Returns the maximum number of TValue instances this Deque can contain.

```cpp
static constexpr std::size_t Capacity() noexcept
```

### `Size`

**Classification:** PUBLIC API · source access: `public`

Returns the current number of live logical elements.

```cpp
constexpr std::size_t Size() const noexcept
```

### `IsEmpty`

**Classification:** PUBLIC API · source access: `public`

Reports whether this Deque currently contains no logical elements.

```cpp
constexpr bool IsEmpty() const noexcept
```

### `IsFull`

**Classification:** PUBLIC API · source access: `public`

Reports whether this Deque currently occupies every available logical slot.

```cpp
constexpr bool IsFull() const noexcept
```

### `Front`

**Classification:** PUBLIC API · source access: `public`

Returns a mutable reference to the logical front value; the Deque must already be non-empty.

```cpp
TValue& Front() noexcept
```

### `Front`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only reference to the logical front value; the Deque must already be non-empty.

```cpp
const TValue& Front() const noexcept
```

### `Back`

**Classification:** PUBLIC API · source access: `public`

Returns a mutable reference to the logical back value; the Deque must already be non-empty.

```cpp
TValue& Back() noexcept
```

### `Back`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only reference to the logical back value; the Deque must already be non-empty.

```cpp
const TValue& Back() const noexcept
```

### `GetFront`

**Classification:** PUBLIC API · source access: `public`

Retrieves a mutable front-value pointer through a checked access path.

```cpp
DequeAccessResult GetFront(
            TValue*& value
        ) noexcept
```

### `GetFront`

**Classification:** PUBLIC API · source access: `public`

Retrieves a read-only front-value pointer through a checked access path.

```cpp
DequeAccessResult GetFront(
            const TValue*& value
        ) const noexcept
```

### `GetBack`

**Classification:** PUBLIC API · source access: `public`

Retrieves a mutable back-value pointer through a checked access path.

```cpp
DequeAccessResult GetBack(
            TValue*& value
        ) noexcept
```

### `GetBack`

**Classification:** PUBLIC API · source access: `public`

Retrieves a read-only back-value pointer through a checked access path.

```cpp
DequeAccessResult GetBack(
            const TValue*& value
        ) const noexcept
```

### `TValue& operator[]( std::size_t index ) noexcept`

**Classification:** PUBLIC API · source access: `public`

Returns a mutable reference to an element at an index that must already satisfy index < Size().

```cpp
TValue& operator[](
            std::size_t index
        ) noexcept
```

### `const TValue& operator[]( std::size_t index ) const noexcept`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only reference to an element at an index that must already satisfy index < Size().

```cpp
const TValue& operator[](
            std::size_t index
        ) const noexcept
```

### `GetAt`

**Classification:** PUBLIC API · source access: `public`

Retrieves a mutable element pointer through a checked logical index.

```cpp
DequeAccessResult GetAt(
            std::size_t index,
            TValue*& value
        ) noexcept
```

### `GetAt`

**Classification:** PUBLIC API · source access: `public`

Retrieves a read-only element pointer through a checked logical index.

```cpp
DequeAccessResult GetAt(
            std::size_t index,
            const TValue*& value
        ) const noexcept
```

### `begin`

**Classification:** PUBLIC API · source access: `public`

Returns a mutable logical iterator to the front value.

```cpp
Iterator begin() noexcept
```

### `end`

**Classification:** PUBLIC API · source access: `public`

Returns a mutable logical iterator one position beyond the back value.

```cpp
Iterator end() noexcept
```

### `begin`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only logical iterator to the front value.

```cpp
ConstIterator begin() const noexcept
```

### `end`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only logical iterator one position beyond the back value.

```cpp
ConstIterator end() const noexcept
```

### `cbegin`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only logical iterator to the front value.

```cpp
ConstIterator cbegin() const noexcept
```

### `cend`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only logical iterator one position beyond the back value.

```cpp
ConstIterator cend() const noexcept
```

### `PushFront`

**Classification:** PUBLIC API · source access: `public`

Copies one value into the logical front when capacity remains available.

```cpp
DequePushFrontResult PushFront(
            const TValue& value
        ) noexcept
```

### `PushFront`

**Classification:** PUBLIC API · source access: `public`

Moves one value into the logical front when capacity remains available.

```cpp
DequePushFrontResult PushFront(
            TValue&& value
        ) noexcept
```

### `EmplaceFront`

**Classification:** PUBLIC API · source access: `public`

Constructs one value directly in the next logical front slot.

```cpp
template<class... TArgumentTypes>
        DequeEmplaceFrontResult EmplaceFront(
            TArgumentTypes&&... arguments
        ) noexcept
```

### `PushBack`

**Classification:** PUBLIC API · source access: `public`

Copies one value into the logical back when capacity remains available.

```cpp
DequePushBackResult PushBack(
            const TValue& value
        ) noexcept
```

### `PushBack`

**Classification:** PUBLIC API · source access: `public`

Moves one value into the logical back when capacity remains available.

```cpp
DequePushBackResult PushBack(
            TValue&& value
        ) noexcept
```

### `EmplaceBack`

**Classification:** PUBLIC API · source access: `public`

Constructs one value directly in the next logical back slot.

```cpp
template<class... TArgumentTypes>
        DequeEmplaceBackResult EmplaceBack(
            TArgumentTypes&&... arguments
        ) noexcept
```

### `PopFront`

**Classification:** PUBLIC API · source access: `public`

Removes and destroys the logical front value when the Deque is non-empty.

```cpp
DequePopFrontResult PopFront() noexcept
```

### `PopBack`

**Classification:** PUBLIC API · source access: `public`

Removes and destroys the logical back value when the Deque is non-empty.

```cpp
DequePopBackResult PopBack() noexcept
```

### `Clear`

**Classification:** PUBLIC API · source access: `public`

Destroys all active values and returns the Deque to its empty state.

```cpp
void Clear() noexcept
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Reports whether two bounded Deques contain identical logical sequences.

```cpp
bool operator==(
            const Deque& other
        ) const noexcept
```

### `bool operator!=( const Deque& other ) const noexcept`

**Classification:** PUBLIC API · source access: `public`

Reports whether two bounded Deques contain different logical sequences.

```cpp
bool operator!=(
            const Deque& other
        ) const noexcept
```

### `TValue`

**Classification:** PUBLIC API

Certifies a bounded Deque when its recursively owned element Type is memory-bounded.

```cpp
template<class TValue, std::size_t TCapacity>
    struct MemoryBoundedTraits<Deque<TValue, TCapacity>> : MemoryBoundedValueDeclaration<
        false,
```

### `TValue`

**Classification:** PUBLIC API

Exposes the compile-time element capacity of a bounded Deque.

```cpp
template<class TValue, std::size_t TCapacity>
    struct CapacityTraits<Deque<TValue, TCapacity>>
```

### `HasStaticCapacity`

**Classification:** PUBLIC API · source access: `public`

Bounded Deque capacity is known at compile time.

```cpp
static constexpr bool HasStaticCapacity = true;
```

### `Unit`

**Classification:** PUBLIC API · source access: `public`

Bounded Deque logical capacity is measured in elements.

```cpp
static constexpr CapacityUnit Unit = CapacityUnit::Elements;
```

### `Capacity`

**Classification:** PUBLIC API · source access: `public`

Reports the maximum number of logical elements retained by the Deque.

```cpp
static constexpr std::size_t Capacity = TCapacity;
```

