# src/bounded/Bytes.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `fa3ab3cbe1652ff74930ed6fc209658dd54551b5`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-BoundedTypes/blob/fa3ab3cbe1652ff74930ed6fc209658dd54551b5/src/bounded/Bytes.hpp)

## Direct includes

- `array`
- `cstddef`
- `cstdint`
- `type_traits`
- `utility`
- `vector`
- `CapacityTraits.hpp`
- `MemoryBoundedTraits.hpp`
- `TypeConversionAdapter.hpp`
- `detail/ByteOperationsPolicy.hpp`
- `detail/CapacityValidation.hpp`
- `detail/SizeCounter.hpp`
- `detail/TypeNormalization.hpp`

## Documented declarations

### `BytesAssignmentResult`

**Classification:** PUBLIC API

Describes the outcome of assigning a complete candidate byte sequence to bounded Bytes.

```cpp
enum class BytesAssignmentResult : std::uint8_t
```

### `BytesAppendResult`

**Classification:** PUBLIC API

Describes the outcome of appending a candidate byte sequence to bounded Bytes.

```cpp
enum class BytesAppendResult : std::uint8_t
```

### `BytesInsertResult`

**Classification:** PUBLIC API

Describes the outcome of inserting a candidate byte sequence into bounded Bytes.

```cpp
enum class BytesInsertResult : std::uint8_t
```

### `BytesEraseResult`

**Classification:** PUBLIC API

Describes the outcome of erasing a logical byte range from bounded Bytes.

```cpp
enum class BytesEraseResult : std::uint8_t
```

### `BytesPushBackResult`

**Classification:** PUBLIC API

Describes the outcome of appending one byte to bounded Bytes.

```cpp
enum class BytesPushBackResult : std::uint8_t
```

### `BytesPopBackResult`

**Classification:** PUBLIC API

Describes the outcome of removing the final logical byte from bounded Bytes.

```cpp
enum class BytesPopBackResult : std::uint8_t
```

### `BytesResizeResult`

**Classification:** PUBLIC API

Describes the outcome of changing the logical length of bounded Bytes.

```cpp
enum class BytesResizeResult : std::uint8_t
```

### `BytesAccessResult`

**Classification:** PUBLIC API

Describes the outcome of checked byte access.

```cpp
enum class BytesAccessResult : std::uint8_t
```

### `TCapacity`

**Classification:** PUBLIC API

Stores a variable-length byte sequence with compile-time capacity, fully inline storage, and no dynamic allocation.
- **Template parameter `TCapacity`:** Maximum logical capacity in bytes.
- **Template parameter `TByteOperationsProvider`:** Stateless EDP-Memory ByteOperations provider selected at compile time.

```cpp
template<
        std::size_t TCapacity,
```

### `ByteOperations`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Compile-time zero-state raw byte-operation policy used by these bounded Bytes.

```cpp
using ByteOperations = Detail::ByteOperationsPolicy<TByteOperationsProvider>;
```

### `uint8_t`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Stores the complete maximum byte payload inline.

```cpp
std::array<std::uint8_t, TCapacity> _storage{};
```

### `_size`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Stores the current logical byte length using the smallest suitable unsigned integer Type.

```cpp
Detail::SizeCounter<TCapacity> _size = 0;
```

### `IsSourceInsideActiveStorage`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Reports whether a complete source range resides inside this Bytes instance's current logical payload.

```cpp
bool IsSourceInsideActiveStorage(
            const std::uint8_t* source,
            std::size_t length,
            std::size_t& sourceOffset
        ) const noexcept
```

### `TSourceByteOperationsProvider`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Copies a known-valid bounded Bytes payload without performing fallible validation.
- **Template parameter `TSourceCapacity`:** Compile-time capacity of the source bounded value.
- **Template parameter `TSourceByteOperationsProvider`:** ByteOperations provider selected by the source bounded value.

```cpp
template<std::size_t TSourceCapacity, class TSourceByteOperationsProvider>
        void CopyFromKnownValid(
            const Bytes<TSourceCapacity, TSourceByteOperationsProvider>& source
        ) noexcept
```

### `AssignBytes`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Assigns an explicitly sized byte sequence after validating pointer safety and capacity.

```cpp
BytesAssignmentResult AssignBytes(
            const std::uint8_t* source,
            std::size_t length
        ) noexcept
```

### `AppendBytes`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Appends an explicitly sized byte sequence after validating pointer safety and resulting capacity.

```cpp
BytesAppendResult AppendBytes(
            const std::uint8_t* source,
            std::size_t length
        ) noexcept
```

### `InsertBytes`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Inserts an explicitly sized byte sequence while preserving a source range that aliases the current payload.

```cpp
BytesInsertResult InsertBytes(
            std::size_t index,
            const std::uint8_t* source,
            std::size_t length
        ) noexcept
```

### `Bytes`

**Classification:** PUBLIC API · source access: `public`

Creates an empty bounded byte sequence with all maximum storage already reserved inline.

```cpp
constexpr Bytes() noexcept = default;
```

### `Bytes`

**Classification:** PUBLIC API · source access: `public`

Copies a bounded byte sequence of identical capacity.

```cpp
Bytes(
            const Bytes& source
        ) noexcept
```

### `Bytes`

**Classification:** PUBLIC API · source access: `public`

Moves a bounded byte sequence of identical capacity and leaves the source empty.

```cpp
Bytes(
            Bytes&& source
        ) noexcept
```

### `TSourceByteOperationsProvider`

**Classification:** PUBLIC API · source access: `public`

Copies bounded Bytes whose compile-time capacity cannot exceed this destination's capacity.
- **Template parameter `TSourceCapacity`:** Compile-time capacity of the source bounded value.
- **Template parameter `TSourceByteOperationsProvider`:** ByteOperations provider selected by the source bounded value.

```cpp
template<std::size_t TSourceCapacity, class TSourceByteOperationsProvider, std::enable_if_t<(TSourceCapacity <= TCapacity), int> = 0>
        Bytes(
            const Bytes<TSourceCapacity, TSourceByteOperationsProvider>& source
        ) noexcept
```

### `TSourceByteOperationsProvider`

**Classification:** PUBLIC API · source access: `public`

Moves bounded Bytes whose compile-time capacity cannot exceed this destination's capacity and leaves the source empty.
- **Template parameter `TSourceCapacity`:** Compile-time capacity of the source bounded value.
- **Template parameter `TSourceByteOperationsProvider`:** ByteOperations provider selected by the source bounded value.

```cpp
template<std::size_t TSourceCapacity, class TSourceByteOperationsProvider, std::enable_if_t<(TSourceCapacity <= TCapacity), int> = 0>
        Bytes(
            Bytes<TSourceCapacity, TSourceByteOperationsProvider>&& source
        ) noexcept
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Copies bounded Bytes of identical capacity.

```cpp
Bytes& operator=(
            const Bytes& source
        ) noexcept
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Moves bounded Bytes of identical capacity and leaves the source empty.

```cpp
Bytes& operator=(
            Bytes&& source
        ) noexcept
```

### `TSourceByteOperationsProvider`

**Classification:** PUBLIC API · source access: `public`

Copies bounded Bytes whose compile-time capacity cannot exceed this destination's capacity.
- **Template parameter `TSourceCapacity`:** Compile-time capacity of the source bounded value.
- **Template parameter `TSourceByteOperationsProvider`:** ByteOperations provider selected by the source bounded value.

```cpp
template<std::size_t TSourceCapacity, class TSourceByteOperationsProvider, std::enable_if_t<(TSourceCapacity <= TCapacity), int> = 0>
        Bytes& operator=(
            const Bytes<TSourceCapacity, TSourceByteOperationsProvider>& source
        ) noexcept
```

### `TSourceByteOperationsProvider`

**Classification:** PUBLIC API · source access: `public`

Moves bounded Bytes whose compile-time capacity cannot exceed this destination's capacity and leaves the source empty.
- **Template parameter `TSourceCapacity`:** Compile-time capacity of the source bounded value.
- **Template parameter `TSourceByteOperationsProvider`:** ByteOperations provider selected by the source bounded value.

```cpp
template<std::size_t TSourceCapacity, class TSourceByteOperationsProvider, std::enable_if_t<(TSourceCapacity <= TCapacity), int> = 0>
        Bytes& operator=(
            Bytes<TSourceCapacity, TSourceByteOperationsProvider>&& source
        ) noexcept
```

### `Capacity`

**Classification:** PUBLIC API · source access: `public`

Returns the maximum number of logical bytes this container can retain.

```cpp
static constexpr std::size_t Capacity() noexcept
```

### `Size`

**Classification:** PUBLIC API · source access: `public`

Returns the current logical byte count.

```cpp
constexpr std::size_t Size() const noexcept
```

### `IsEmpty`

**Classification:** PUBLIC API · source access: `public`

Reports whether the logical byte sequence is empty.

```cpp
constexpr bool IsEmpty() const noexcept
```

### `IsFull`

**Classification:** PUBLIC API · source access: `public`

Reports whether the logical byte sequence occupies its complete capacity.

```cpp
constexpr bool IsFull() const noexcept
```

### `Data`

**Classification:** PUBLIC API · source access: `public`

Returns a mutable pointer to the inline byte storage.

```cpp
std::uint8_t* Data() noexcept
```

### `Data`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only pointer to the inline byte storage.

```cpp
const std::uint8_t* Data() const noexcept
```

### `std::uint8_t& operator[]( std::size_t index ) noexcept`

**Classification:** PUBLIC API · source access: `public`

Returns a mutable reference to a logical byte at an index that must already satisfy index < Size().

```cpp
std::uint8_t& operator[](
            std::size_t index
        ) noexcept
```

### `const std::uint8_t& operator[]( std::size_t index ) const noexcept`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only reference to a logical byte at an index that must already satisfy index < Size().

```cpp
const std::uint8_t& operator[](
            std::size_t index
        ) const noexcept
```

### `GetAt`

**Classification:** PUBLIC API · source access: `public`

Retrieves a mutable pointer to one logical byte through a checked access path.

```cpp
BytesAccessResult GetAt(
            std::size_t index,
            std::uint8_t*& value
        ) noexcept
```

### `GetAt`

**Classification:** PUBLIC API · source access: `public`

Retrieves a read-only pointer to one logical byte through a checked access path.

```cpp
BytesAccessResult GetAt(
            std::size_t index,
            const std::uint8_t*& value
        ) const noexcept
```

### `begin`

**Classification:** PUBLIC API · source access: `public`

Returns a mutable iterator to the first logical byte.

```cpp
std::uint8_t* begin() noexcept
```

### `end`

**Classification:** PUBLIC API · source access: `public`

Returns a mutable iterator one position beyond the final logical byte.

```cpp
std::uint8_t* end() noexcept
```

### `begin`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only iterator to the first logical byte.

```cpp
const std::uint8_t* begin() const noexcept
```

### `end`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only iterator one position beyond the final logical byte.

```cpp
const std::uint8_t* end() const noexcept
```

### `cbegin`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only iterator to the first logical byte.

```cpp
const std::uint8_t* cbegin() const noexcept
```

### `cend`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only iterator one position beyond the final logical byte.

```cpp
const std::uint8_t* cend() const noexcept
```

### `TSourceByteOperationsProvider`

**Classification:** PUBLIC API · source access: `public`

Assigns another bounded byte sequence after validating this destination's runtime capacity.
- **Template parameter `TSourceCapacity`:** Compile-time capacity of the source bounded value.
- **Template parameter `TSourceByteOperationsProvider`:** ByteOperations provider selected by the source bounded value.

```cpp
template<std::size_t TSourceCapacity, class TSourceByteOperationsProvider>
        BytesAssignmentResult Assign(
            const Bytes<TSourceCapacity, TSourceByteOperationsProvider>& source
        ) noexcept
```

### `Assign`

**Classification:** PUBLIC API · source access: `public`

Assigns a fixed standard byte array.
- **Template parameter `TArraySize`:** Fixed source array extent.

```cpp
template<std::size_t TArraySize>
        BytesAssignmentResult Assign(
            const std::array<std::uint8_t, TArraySize>& source
        ) noexcept
```

### `Assign`

**Classification:** PUBLIC API · source access: `public`

Assigns a standard dynamic byte vector without retaining its dynamic storage.

```cpp
BytesAssignmentResult Assign(
            const std::vector<std::uint8_t>& source
        ) noexcept
```

### `Assign`

**Classification:** PUBLIC API · source access: `public`

Assigns an explicitly sized byte range.

```cpp
BytesAssignmentResult Assign(
            const std::uint8_t* source,
            std::size_t length
        ) noexcept
```

### `TSourceByteOperationsProvider`

**Classification:** PUBLIC API · source access: `public`

Appends another bounded byte sequence without truncation.
- **Template parameter `TSourceCapacity`:** Compile-time capacity of the source bounded value.
- **Template parameter `TSourceByteOperationsProvider`:** ByteOperations provider selected by the source bounded value.

```cpp
template<std::size_t TSourceCapacity, class TSourceByteOperationsProvider>
        BytesAppendResult Append(
            const Bytes<TSourceCapacity, TSourceByteOperationsProvider>& source
        ) noexcept
```

### `Append`

**Classification:** PUBLIC API · source access: `public`

Appends a fixed standard byte array without truncation.
- **Template parameter `TArraySize`:** Fixed source array extent.

```cpp
template<std::size_t TArraySize>
        BytesAppendResult Append(
            const std::array<std::uint8_t, TArraySize>& source
        ) noexcept
```

### `Append`

**Classification:** PUBLIC API · source access: `public`

Appends a standard dynamic byte vector without retaining its dynamic storage.

```cpp
BytesAppendResult Append(
            const std::vector<std::uint8_t>& source
        ) noexcept
```

### `Append`

**Classification:** PUBLIC API · source access: `public`

Appends an explicitly sized byte range.

```cpp
BytesAppendResult Append(
            const std::uint8_t* source,
            std::size_t length
        ) noexcept
```

### `TSourceByteOperationsProvider`

**Classification:** PUBLIC API · source access: `public`

Inserts another bounded byte sequence at a checked logical index.
- **Template parameter `TSourceCapacity`:** Compile-time capacity of the source bounded value.
- **Template parameter `TSourceByteOperationsProvider`:** ByteOperations provider selected by the source bounded value.

```cpp
template<std::size_t TSourceCapacity, class TSourceByteOperationsProvider>
        BytesInsertResult Insert(
            std::size_t index,
            const Bytes<TSourceCapacity, TSourceByteOperationsProvider>& source
        ) noexcept
```

### `Insert`

**Classification:** PUBLIC API · source access: `public`

Inserts a fixed standard byte array at a checked logical index.
- **Template parameter `TArraySize`:** Fixed source array extent.

```cpp
template<std::size_t TArraySize>
        BytesInsertResult Insert(
            std::size_t index,
            const std::array<std::uint8_t, TArraySize>& source
        ) noexcept
```

### `Insert`

**Classification:** PUBLIC API · source access: `public`

Inserts a standard dynamic byte vector at a checked logical index.

```cpp
BytesInsertResult Insert(
            std::size_t index,
            const std::vector<std::uint8_t>& source
        ) noexcept
```

### `Insert`

**Classification:** PUBLIC API · source access: `public`

Inserts an explicitly sized byte range at a checked logical index.

```cpp
BytesInsertResult Insert(
            std::size_t index,
            const std::uint8_t* source,
            std::size_t length
        ) noexcept
```

### `Erase`

**Classification:** PUBLIC API · source access: `public`

Erases a strict logical byte range without silently clamping an invalid request.

```cpp
BytesEraseResult Erase(
            std::size_t index,
            std::size_t count
        ) noexcept
```

### `PushBack`

**Classification:** PUBLIC API · source access: `public`

Appends one byte when capacity remains available.

```cpp
BytesPushBackResult PushBack(
            std::uint8_t value
        ) noexcept
```

### `PopBack`

**Classification:** PUBLIC API · source access: `public`

Removes the final logical byte when the sequence is non-empty.

```cpp
BytesPopBackResult PopBack() noexcept
```

### `Resize`

**Classification:** PUBLIC API · source access: `public`

Changes logical size, initializing every newly exposed byte to the supplied fill value.

```cpp
BytesResizeResult Resize(
            std::size_t newSize,
            std::uint8_t fillValue = 0U
        ) noexcept
```

### `Clear`

**Classification:** PUBLIC API · source access: `public`

Clears the logical byte sequence while retaining all preallocated storage.

```cpp
void Clear() noexcept
```

### `TTarget`

**Classification:** PUBLIC API · source access: `public`

Converts this bounded byte sequence to a target Type through the target Type owner's compile-time adapter specialization.
- **Template parameter `TTarget`:** Explicit conversion target Type.

```cpp
template<class TTarget>
        auto CastTo(
            TTarget& target
        ) const noexcept(
            TypeConversionAdapter<
                Bytes<TCapacity, TByteOperationsProvider>,
                Detail::NormalizedType<TTarget>
            >::IsNoexcept
        ) -> typename TypeConversionAdapter<
            Bytes<TCapacity, TByteOperationsProvider>,
```

### `TSource`

**Classification:** PUBLIC API · source access: `public`

Converts a source Type into this bounded byte sequence through the source Type owner's compile-time adapter specialization.
- **Template parameter `TSource`:** Explicit conversion source Type.

```cpp
template<class TSource>
        auto CastFrom(
            const TSource& source
        ) noexcept(
            TypeConversionAdapter<
                Detail::NormalizedType<TSource>,
                Bytes<TCapacity, TByteOperationsProvider>
            >::IsNoexcept
        ) -> typename TypeConversionAdapter<
            Detail::NormalizedType<TSource>,
```

### `TOtherByteOperationsProvider`

**Classification:** PUBLIC API · source access: `public`

Reports whether two bounded byte sequences contain identical logical bytes.
- **Template parameter `TOtherCapacity`:** Compile-time capacity of the compared bounded value.
- **Template parameter `TOtherByteOperationsProvider`:** ByteOperations provider selected by the compared bounded value.

```cpp
template<std::size_t TOtherCapacity, class TOtherByteOperationsProvider>
        bool operator==(
            const Bytes<TOtherCapacity, TOtherByteOperationsProvider>& other
        ) const noexcept
```

### `TOtherByteOperationsProvider`

**Classification:** PUBLIC API · source access: `public`

Reports whether two bounded byte sequences contain different logical bytes.
- **Template parameter `TOtherCapacity`:** Compile-time capacity of the compared bounded value.
- **Template parameter `TOtherByteOperationsProvider`:** ByteOperations provider selected by the compared bounded value.

```cpp
template<std::size_t TOtherCapacity, class TOtherByteOperationsProvider>
        bool operator!=(
            const Bytes<TOtherCapacity, TOtherByteOperationsProvider>& other
        ) const noexcept
```

### `TOtherByteOperationsProvider`

**Classification:** PUBLIC API · source access: `public`

Reports whether this byte sequence sorts lexicographically before another sequence.
- **Template parameter `TOtherCapacity`:** Compile-time capacity of the compared bounded value.
- **Template parameter `TOtherByteOperationsProvider`:** ByteOperations provider selected by the compared bounded value.

```cpp
template<std::size_t TOtherCapacity, class TOtherByteOperationsProvider>
        bool operator<(
            const Bytes<TOtherCapacity, TOtherByteOperationsProvider>& other
        ) const noexcept
```

### `TOtherByteOperationsProvider`

**Classification:** PUBLIC API · source access: `public`

Reports whether this byte sequence is lexicographically before or equal to another sequence.
- **Template parameter `TOtherCapacity`:** Compile-time capacity of the compared bounded value.
- **Template parameter `TOtherByteOperationsProvider`:** ByteOperations provider selected by the compared bounded value.

```cpp
template<std::size_t TOtherCapacity, class TOtherByteOperationsProvider>
        bool operator<=(
            const Bytes<TOtherCapacity, TOtherByteOperationsProvider>& other
        ) const noexcept
```

### `TOtherByteOperationsProvider`

**Classification:** PUBLIC API · source access: `public`

Reports whether this byte sequence sorts lexicographically after another sequence.
- **Template parameter `TOtherCapacity`:** Compile-time capacity of the compared bounded value.
- **Template parameter `TOtherByteOperationsProvider`:** ByteOperations provider selected by the compared bounded value.

```cpp
template<std::size_t TOtherCapacity, class TOtherByteOperationsProvider>
        bool operator>(
            const Bytes<TOtherCapacity, TOtherByteOperationsProvider>& other
        ) const noexcept
```

### `TOtherByteOperationsProvider`

**Classification:** PUBLIC API · source access: `public`

Reports whether this byte sequence is lexicographically after or equal to another sequence.
- **Template parameter `TOtherCapacity`:** Compile-time capacity of the compared bounded value.
- **Template parameter `TOtherByteOperationsProvider`:** ByteOperations provider selected by the compared bounded value.

```cpp
template<std::size_t TOtherCapacity, class TOtherByteOperationsProvider>
        bool operator>=(
            const Bytes<TOtherCapacity, TOtherByteOperationsProvider>& other
        ) const noexcept
```

### `TByteOperationsProvider`

**Classification:** PUBLIC API

Certifies every bounded Bytes specialization as a self-contained memory-bounded value.
- **Template parameter `TCapacity`:** Compile-time capacity of the bounded value.
- **Template parameter `TByteOperationsProvider`:** Stateless ByteOperations provider selected by the bounded value.

```cpp
template<std::size_t TCapacity, class TByteOperationsProvider>
    struct MemoryBoundedTraits<Bytes<TCapacity, TByteOperationsProvider>> : MemoryBoundedValueDeclaration<false>
```

### `TByteOperationsProvider`

**Classification:** PUBLIC API

Exposes the compile-time byte capacity of bounded Bytes.
- **Template parameter `TCapacity`:** Compile-time capacity of the bounded value.
- **Template parameter `TByteOperationsProvider`:** Stateless ByteOperations provider selected by the bounded value.

```cpp
template<std::size_t TCapacity, class TByteOperationsProvider>
    struct CapacityTraits<Bytes<TCapacity, TByteOperationsProvider>>
```

### `HasStaticCapacity`

**Classification:** PUBLIC API · source access: `public`

Bounded Bytes capacity is known at compile time.

```cpp
static constexpr bool HasStaticCapacity = true;
```

### `Unit`

**Classification:** PUBLIC API · source access: `public`

Bounded Bytes logical capacity is measured in bytes.

```cpp
static constexpr CapacityUnit Unit = CapacityUnit::Bytes;
```

### `Capacity`

**Classification:** PUBLIC API · source access: `public`

Reports the maximum number of logical bytes retained by the container.

```cpp
static constexpr std::size_t Capacity = TCapacity;
```

