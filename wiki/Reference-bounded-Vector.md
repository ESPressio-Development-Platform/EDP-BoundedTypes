# src/bounded/Vector.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `f5742d864310dc4fdecb734721205b0e21c38e40`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-BoundedTypes/blob/f5742d864310dc4fdecb734721205b0e21c38e40/src/bounded/Vector.hpp)

## Direct includes

- `array`
- `cstddef`
- `cstdint`
- `memory`
- `type_traits`
- `utility`
- `vector`
- `CapacityTraits.hpp`
- `MemoryBoundedTraits.hpp`
- `detail/IndexedIterator.hpp`
- `detail/RawSlotStorage.hpp`
- `detail/SizeCounter.hpp`

## Documented declarations

### `VectorAssignmentResult`

**Classification:** PUBLIC API

Describes the outcome of assigning a complete logical sequence to a bounded Vector.

```cpp
enum class VectorAssignmentResult : std::uint8_t
```

### `VectorPushBackResult`

**Classification:** PUBLIC API

Describes the outcome of appending one value to a bounded Vector.

```cpp
enum class VectorPushBackResult : std::uint8_t
```

### `VectorEmplaceBackResult`

**Classification:** PUBLIC API

Describes the outcome of in-place construction at the end of a bounded Vector.

```cpp
enum class VectorEmplaceBackResult : std::uint8_t
```

### `VectorInsertResult`

**Classification:** PUBLIC API

Describes the outcome of inserting one value into a bounded Vector.

```cpp
enum class VectorInsertResult : std::uint8_t
```

### `VectorEraseResult`

**Classification:** PUBLIC API

Describes the outcome of erasing a logical range from a bounded Vector.

```cpp
enum class VectorEraseResult : std::uint8_t
```

### `VectorPopBackResult`

**Classification:** PUBLIC API

Describes the outcome of removing the final logical value from a bounded Vector.

```cpp
enum class VectorPopBackResult : std::uint8_t
```

### `VectorResizeResult`

**Classification:** PUBLIC API

Describes the outcome of changing the logical size of a bounded Vector.

```cpp
enum class VectorResizeResult : std::uint8_t
```

### `VectorAccessResult`

**Classification:** PUBLIC API

Describes the outcome of checked bounded Vector element access.

```cpp
enum class VectorAccessResult : std::uint8_t
```

### `TValue`

**Classification:** PUBLIC API

Stores a variable-length indexed sequence in fully preallocated inline slots without dynamic allocation.

```cpp
template<class TValue, std::size_t TCapacity>
    class Vector final
```

### `Detail`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Allows logical iterators to access already-live elements without exposing raw slot storage publicly.

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

Stores the current number of live logical elements using the smallest suitable unsigned integer Type.

```cpp
Detail::SizeCounter<TCapacity> _size = 0;
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

Destroys every active element without changing the preallocated raw slot capacity.

```cpp
void DestroyAll() noexcept
```

### `CopyFromKnownFit`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Copies all elements from a source whose logical size is already known to fit.

```cpp
template<std::size_t TSourceCapacity>
        void CopyFromKnownFit(
            const Vector<TValue, TSourceCapacity>& source
        ) noexcept
```

### `MoveFromKnownFit`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Moves all elements from a source whose logical size is already known to fit and empties that source.

```cpp
template<std::size_t TSourceCapacity>
        void MoveFromKnownFit(
            Vector<TValue, TSourceCapacity>& source
        ) noexcept
```

### `FindOwnedElementIndex`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Finds whether a source reference identifies an element already owned by this Vector.

```cpp
bool FindOwnedElementIndex(
            const TValue* value,
            std::size_t& index
        ) const noexcept
```

### `ShiftRightForInsertion`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Opens one raw slot at a logical insertion point by nothrow move-constructing existing values backward.

```cpp
void ShiftRightForInsertion(
            std::size_t index
        ) noexcept
```

### `ShiftLeftAfterErase`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Closes a logical gap by nothrow move-constructing trailing values forward into destroyed slots.

```cpp
void ShiftLeftAfterErase(
            std::size_t index,
            std::size_t count
        ) noexcept
```

### `ValueType`

**Classification:** PUBLIC API · source access: `public`

Identifies the logical element Type retained by this Vector.

```cpp
using ValueType = TValue;
```

### `Iterator`

**Classification:** PUBLIC API · source access: `public`

Provides mutable random-access logical iteration without claiming contiguous TValue pointer semantics.

```cpp
using Iterator = Detail::IndexedIterator<
            Vector<TValue, TCapacity>,
```

### `ConstIterator`

**Classification:** PUBLIC API · source access: `public`

Provides read-only random-access logical iteration without claiming contiguous TValue pointer semantics.

```cpp
using ConstIterator = Detail::IndexedIterator<
            Vector<TValue, TCapacity>,
```

### `Vector`

**Classification:** PUBLIC API · source access: `public`

Creates an empty bounded Vector without constructing any TValue instances.

```cpp
constexpr Vector() noexcept = default;
```

### `Vector`

**Classification:** PUBLIC API · source access: `public`

Copies a bounded Vector of identical capacity when TValue supports nothrow copy construction.

```cpp
Vector(
            const Vector& source
        ) noexcept
```

### `Vector`

**Classification:** PUBLIC API · source access: `public`

Moves a bounded Vector of identical capacity when TValue supports nothrow move construction and leaves the source empty.

```cpp
Vector(
            Vector&& source
        ) noexcept
```

### `Vector`

**Classification:** PUBLIC API · source access: `public`

Copies a bounded Vector whose compile-time capacity cannot exceed this destination's capacity.

```cpp
template<std::size_t TSourceCapacity, std::enable_if_t<(TSourceCapacity <= TCapacity && TSourceCapacity != TCapacity), int> = 0>
        Vector(
            const Vector<TValue, TSourceCapacity>& source
        ) noexcept
```

### `Vector`

**Classification:** PUBLIC API · source access: `public`

Moves a bounded Vector whose compile-time capacity cannot exceed this destination's capacity and leaves the source empty.

```cpp
template<std::size_t TSourceCapacity, std::enable_if_t<(TSourceCapacity <= TCapacity && TSourceCapacity != TCapacity), int> = 0>
        Vector(
            Vector<TValue, TSourceCapacity>&& source
        ) noexcept
```

### `Vector`

**Classification:** PUBLIC API · source access: `public`

Destroys every active TValue while releasing no external storage.

```cpp
~Vector() noexcept
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Copies a bounded Vector of identical capacity.

```cpp
Vector& operator=(
            const Vector& source
        ) noexcept
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Moves a bounded Vector of identical capacity and leaves the source empty.

```cpp
Vector& operator=(
            Vector&& source
        ) noexcept
```

### `TSourceCapacity`

**Classification:** PUBLIC API · source access: `public`

Copies a bounded Vector whose compile-time capacity cannot exceed this destination's capacity.

```cpp
template<std::size_t TSourceCapacity, std::enable_if_t<(TSourceCapacity <= TCapacity && TSourceCapacity != TCapacity), int> = 0>
        Vector& operator=(
            const Vector<TValue, TSourceCapacity>& source
        ) noexcept
```

### `TSourceCapacity`

**Classification:** PUBLIC API · source access: `public`

Moves a bounded Vector whose compile-time capacity cannot exceed this destination's capacity and leaves the source empty.

```cpp
template<std::size_t TSourceCapacity, std::enable_if_t<(TSourceCapacity <= TCapacity && TSourceCapacity != TCapacity), int> = 0>
        Vector& operator=(
            Vector<TValue, TSourceCapacity>&& source
        ) noexcept
```

### `Capacity`

**Classification:** PUBLIC API · source access: `public`

Returns the maximum number of TValue instances this Vector can contain.

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

Reports whether this Vector currently contains no logical elements.

```cpp
constexpr bool IsEmpty() const noexcept
```

### `IsFull`

**Classification:** PUBLIC API · source access: `public`

Reports whether this Vector currently occupies every available logical slot.

```cpp
constexpr bool IsFull() const noexcept
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
VectorAccessResult GetAt(
            std::size_t index,
            TValue*& value
        ) noexcept
```

### `GetAt`

**Classification:** PUBLIC API · source access: `public`

Retrieves a read-only element pointer through a checked logical index.

```cpp
VectorAccessResult GetAt(
            std::size_t index,
            const TValue*& value
        ) const noexcept
```

### `begin`

**Classification:** PUBLIC API · source access: `public`

Returns a mutable logical iterator to the first element.

```cpp
Iterator begin() noexcept
```

### `end`

**Classification:** PUBLIC API · source access: `public`

Returns a mutable logical iterator one position beyond the final element.

```cpp
Iterator end() noexcept
```

### `begin`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only logical iterator to the first element.

```cpp
ConstIterator begin() const noexcept
```

### `end`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only logical iterator one position beyond the final element.

```cpp
ConstIterator end() const noexcept
```

### `cbegin`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only logical iterator to the first element.

```cpp
ConstIterator cbegin() const noexcept
```

### `cend`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only logical iterator one position beyond the final element.

```cpp
ConstIterator cend() const noexcept
```

### `Assign`

**Classification:** PUBLIC API · source access: `public`

Assigns another bounded Vector after validating this destination's runtime capacity.

```cpp
template<std::size_t TSourceCapacity>
        VectorAssignmentResult Assign(
            const Vector<TValue, TSourceCapacity>& source
        ) noexcept
```

### `Assign`

**Classification:** PUBLIC API · source access: `public`

Assigns a fixed standard array after validating this destination's runtime capacity.

```cpp
template<std::size_t TArraySize>
        VectorAssignmentResult Assign(
            const std::array<TValue, TArraySize>& source
        ) noexcept
```

### `Assign`

**Classification:** PUBLIC API · source access: `public`

Assigns a standard dynamic Vector after validating size before mutating this bounded destination.

```cpp
VectorAssignmentResult Assign(
            const std::vector<TValue>& source
        ) noexcept
```

### `PushBack`

**Classification:** PUBLIC API · source access: `public`

Appends a copied value when capacity remains available.

```cpp
VectorPushBackResult PushBack(
            const TValue& value
        ) noexcept
```

### `PushBack`

**Classification:** PUBLIC API · source access: `public`

Appends a moved value when capacity remains available.

```cpp
VectorPushBackResult PushBack(
            TValue&& value
        ) noexcept
```

### `EmplaceBack`

**Classification:** PUBLIC API · source access: `public`

Constructs a new final value directly in its preallocated raw slot.

```cpp
template<class... TArgumentTypes>
        VectorEmplaceBackResult EmplaceBack(
            TArgumentTypes&&... arguments
        ) noexcept
```

### `Insert`

**Classification:** PUBLIC API · source access: `public`

Inserts a copied value at a checked logical index, including safe copy insertion from this Vector's own elements.

```cpp
VectorInsertResult Insert(
            std::size_t index,
            const TValue& value
        ) noexcept
```

### `Insert`

**Classification:** PUBLIC API · source access: `public`

Inserts a moved non-aliasing value at a checked logical index.

```cpp
VectorInsertResult Insert(
            std::size_t index,
            TValue&& value
        ) noexcept
```

### `Emplace`

**Classification:** PUBLIC API · source access: `public`

Constructs one value at a checked logical index after opening a raw slot.

```cpp
template<class... TArgumentTypes>
        VectorInsertResult Emplace(
            std::size_t index,
            TArgumentTypes&&... arguments
        ) noexcept
```

### `Erase`

**Classification:** PUBLIC API · source access: `public`

Erases a strict logical range while preserving order.

```cpp
VectorEraseResult Erase(
            std::size_t index,
            std::size_t count
        ) noexcept
```

### `PopBack`

**Classification:** PUBLIC API · source access: `public`

Removes the final logical value when the Vector is non-empty.

```cpp
VectorPopBackResult PopBack() noexcept
```

### `Resize`

**Classification:** PUBLIC API · source access: `public`

Changes logical size, default-constructing new elements when growth is requested.

```cpp
VectorResizeResult Resize(
            std::size_t newSize
        ) noexcept
```

### `Resize`

**Classification:** PUBLIC API · source access: `public`

Changes logical size, copy-constructing any new elements from one fill value.

```cpp
VectorResizeResult Resize(
            std::size_t newSize,
            const TValue& fillValue
        ) noexcept
```

### `Clear`

**Classification:** PUBLIC API · source access: `public`

Destroys all active values and returns the Vector to its empty state.

```cpp
void Clear() noexcept
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Reports whether two bounded Vectors contain identical logical sequences.

```cpp
template<std::size_t TOtherCapacity>
        bool operator==(
            const Vector<TValue, TOtherCapacity>& other
        ) const noexcept
```

### `TValue`

**Classification:** PUBLIC API · source access: `public`

Reports whether two bounded Vectors contain different logical sequences.

```cpp
template<std::size_t TOtherCapacity>
        bool operator!=(
            const Vector<TValue, TOtherCapacity>& other
        ) const noexcept
```

### `TValue`

**Classification:** PUBLIC API · source access: `public`

Reports whether this Vector sorts lexicographically before another logical sequence.

```cpp
template<std::size_t TOtherCapacity>
        bool operator<(
            const Vector<TValue, TOtherCapacity>& other
        ) const noexcept
```

### `TValue`

**Classification:** PUBLIC API · source access: `public`

Reports whether this Vector is lexicographically before or equal to another logical sequence.

```cpp
template<std::size_t TOtherCapacity>
        bool operator<=(
            const Vector<TValue, TOtherCapacity>& other
        ) const noexcept
```

### `TValue`

**Classification:** PUBLIC API · source access: `public`

Reports whether this Vector sorts lexicographically after another logical sequence.

```cpp
template<std::size_t TOtherCapacity>
        bool operator>(
            const Vector<TValue, TOtherCapacity>& other
        ) const noexcept
```

### `TValue`

**Classification:** PUBLIC API · source access: `public`

Reports whether this Vector is lexicographically after or equal to another logical sequence.

```cpp
template<std::size_t TOtherCapacity>
        bool operator>=(
            const Vector<TValue, TOtherCapacity>& other
        ) const noexcept
```

### `TValue`

**Classification:** PUBLIC API

Certifies a bounded Vector when its recursively owned element Type is memory-bounded.

```cpp
template<class TValue, std::size_t TCapacity>
    struct MemoryBoundedTraits<Vector<TValue, TCapacity>> : MemoryBoundedValueDeclaration<
        false,
```

### `TValue`

**Classification:** PUBLIC API

Exposes the compile-time element capacity of a bounded Vector.

```cpp
template<class TValue, std::size_t TCapacity>
    struct CapacityTraits<Vector<TValue, TCapacity>>
```

### `HasStaticCapacity`

**Classification:** PUBLIC API · source access: `public`

Bounded Vector capacity is known at compile time.

```cpp
static constexpr bool HasStaticCapacity = true;
```

### `Unit`

**Classification:** PUBLIC API · source access: `public`

Bounded Vector logical capacity is measured in elements.

```cpp
static constexpr CapacityUnit Unit = CapacityUnit::Elements;
```

### `Capacity`

**Classification:** PUBLIC API · source access: `public`

Reports the maximum number of logical elements retained by the Vector.

```cpp
static constexpr std::size_t Capacity = TCapacity;
```

