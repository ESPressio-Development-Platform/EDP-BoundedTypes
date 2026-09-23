# src/bounded/String.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `f5742d864310dc4fdecb734721205b0e21c38e40`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-BoundedTypes/blob/f5742d864310dc4fdecb734721205b0e21c38e40/src/bounded/String.hpp)

## Direct includes

- `array`
- `cstddef`
- `cstdint`
- `string`
- `string_view`
- `type_traits`
- `utility`
- `CapacityTraits.hpp`
- `MemoryBoundedTraits.hpp`
- `TypeConversionAdapter.hpp`
- `detail/ByteOperationsPolicy.hpp`
- `detail/CapacityValidation.hpp`
- `detail/SizeCounter.hpp`
- `detail/TypeNormalization.hpp`

## Documented declarations

### `StringAssignmentResult`

**Classification:** PUBLIC API

Describes the outcome of assigning a complete candidate value to a bounded String.

```cpp
enum class StringAssignmentResult : std::uint8_t
```

### `StringAppendResult`

**Classification:** PUBLIC API

Describes the outcome of appending a candidate value to a bounded String.

```cpp
enum class StringAppendResult : std::uint8_t
```

### `StringInsertResult`

**Classification:** PUBLIC API

Describes the outcome of inserting a candidate value into a bounded String.

```cpp
enum class StringInsertResult : std::uint8_t
```

### `StringEraseResult`

**Classification:** PUBLIC API

Describes the outcome of erasing a logical range from a bounded String.

```cpp
enum class StringEraseResult : std::uint8_t
```

### `StringSetAtResult`

**Classification:** PUBLIC API

Describes the outcome of replacing a logical character in a bounded String.

```cpp
enum class StringSetAtResult : std::uint8_t
```

### `StringPushBackResult`

**Classification:** PUBLIC API

Describes the outcome of appending one character to a bounded String.

```cpp
enum class StringPushBackResult : std::uint8_t
```

### `StringPopBackResult`

**Classification:** PUBLIC API

Describes the outcome of removing the final logical character from a bounded String.

```cpp
enum class StringPopBackResult : std::uint8_t
```

### `StringAccessResult`

**Classification:** PUBLIC API

Describes the outcome of checked character access on a bounded String.

```cpp
enum class StringAccessResult : std::uint8_t
```

### `TCapacity`

**Classification:** PUBLIC API

Stores a variable-length null-terminated text byte sequence with a compile-time payload capacity and no dynamic allocation.
- **Template parameter `TCapacity`:** Maximum logical capacity in bytes.
- **Template parameter `TByteOperationsProvider`:** Stateless EDP-Memory ByteOperations provider selected at compile time.

```cpp
template<
        std::size_t TCapacity,
```

### `ByteOperations`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Compile-time zero-state raw byte-operation policy used by this String.

```cpp
using ByteOperations = Detail::ByteOperationsPolicy<TByteOperationsProvider>;
```

### `char`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Stores the complete maximum payload and one permanently reserved implicit null terminator.

```cpp
std::array<char, TCapacity + 1U> _storage{};
```

### `_size`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Stores the current logical payload length using the smallest suitable unsigned integer Type.

```cpp
Detail::SizeCounter<TCapacity> _size = 0;
```

### `ContainsEmbeddedNull`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Reports whether the candidate text bytes contain an embedded null character.

```cpp
static bool ContainsEmbeddedNull(
            const char* source,
            std::size_t length
        ) noexcept
```

### `IsSourceInsideActiveStorage`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Reports whether a complete source range resides inside this String's current logical payload.

```cpp
bool IsSourceInsideActiveStorage(
            const char* source,
            std::size_t length,
            std::size_t& sourceOffset
        ) const noexcept
```

### `TSourceByteOperationsProvider`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Copies a known-valid bounded String payload without performing fallible validation.
- **Template parameter `TSourceCapacity`:** Compile-time capacity of the source bounded value.
- **Template parameter `TSourceByteOperationsProvider`:** ByteOperations provider selected by the source bounded value.

```cpp
template<std::size_t TSourceCapacity, class TSourceByteOperationsProvider>
        void CopyFromKnownValid(
            const String<TSourceCapacity, TSourceByteOperationsProvider>& source
        ) noexcept
```

### `AssignBytes`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Assigns candidate text bytes after validating capacity, pointer safety, and the embedded-null invariant.

```cpp
StringAssignmentResult AssignBytes(
            const char* source,
            std::size_t length
        ) noexcept
```

### `AppendBytes`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Appends candidate text bytes after validating the complete resulting logical size and text invariant.

```cpp
StringAppendResult AppendBytes(
            const char* source,
            std::size_t length
        ) noexcept
```

### `InsertBytes`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Inserts candidate text bytes while preserving any source range that aliases this String's active payload.

```cpp
StringInsertResult InsertBytes(
            std::size_t index,
            const char* source,
            std::size_t length
        ) noexcept
```

### `TTarget`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Dispatches an explicitly requested conversion from this bounded String to a target Type.
- **Template parameter `TTarget`:** Explicit conversion target Type.

```cpp
template<class TTarget>
        auto CastToImplementation(
            TTarget& target
        ) const noexcept(
            TypeConversionAdapter<
                String<TCapacity, TByteOperationsProvider>,
                Detail::NormalizedType<TTarget>
            >::IsNoexcept
        ) -> typename TypeConversionAdapter<
            String<TCapacity, TByteOperationsProvider>,
```

### `TSource`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Dispatches an explicitly requested conversion from a source Type into this bounded String.
- **Template parameter `TSource`:** Explicit conversion source Type.

```cpp
template<class TSource>
        auto CastFromImplementation(
            const TSource& source
        ) noexcept(
            TypeConversionAdapter<
                Detail::NormalizedType<TSource>,
                String<TCapacity, TByteOperationsProvider>
            >::IsNoexcept
        ) -> typename TypeConversionAdapter<
            Detail::NormalizedType<TSource>,
```

### `String`

**Classification:** PUBLIC API · source access: `public`

Creates an empty bounded String with its implicit terminator already established.

```cpp
constexpr String() noexcept = default;
```

### `String`

**Classification:** PUBLIC API · source access: `public`

Copies a bounded String of identical capacity.

```cpp
String(
            const String& source
        ) noexcept
```

### `String`

**Classification:** PUBLIC API · source access: `public`

Moves a bounded String of identical capacity and leaves the source empty.

```cpp
String(
            String&& source
        ) noexcept
```

### `TSourceByteOperationsProvider`

**Classification:** PUBLIC API · source access: `public`

Copies a bounded String whose compile-time capacity cannot exceed this destination's capacity.
- **Template parameter `TSourceCapacity`:** Compile-time capacity of the source bounded value.
- **Template parameter `TSourceByteOperationsProvider`:** ByteOperations provider selected by the source bounded value.

```cpp
template<std::size_t TSourceCapacity, class TSourceByteOperationsProvider, std::enable_if_t<(TSourceCapacity <= TCapacity), int> = 0>
        String(
            const String<TSourceCapacity, TSourceByteOperationsProvider>& source
        ) noexcept
```

### `TSourceByteOperationsProvider`

**Classification:** PUBLIC API · source access: `public`

Moves a bounded String whose compile-time capacity cannot exceed this destination's capacity and leaves the source empty.
- **Template parameter `TSourceCapacity`:** Compile-time capacity of the source bounded value.
- **Template parameter `TSourceByteOperationsProvider`:** ByteOperations provider selected by the source bounded value.

```cpp
template<std::size_t TSourceCapacity, class TSourceByteOperationsProvider, std::enable_if_t<(TSourceCapacity <= TCapacity), int> = 0>
        String(
            String<TSourceCapacity, TSourceByteOperationsProvider>&& source
        ) noexcept
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Copies a bounded String of identical capacity.

```cpp
String& operator=(
            const String& source
        ) noexcept
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Moves a bounded String of identical capacity and leaves the source empty.

```cpp
String& operator=(
            String&& source
        ) noexcept
```

### `TSourceByteOperationsProvider`

**Classification:** PUBLIC API · source access: `public`

Copies a bounded String whose compile-time capacity cannot exceed this destination's capacity.
- **Template parameter `TSourceCapacity`:** Compile-time capacity of the source bounded value.
- **Template parameter `TSourceByteOperationsProvider`:** ByteOperations provider selected by the source bounded value.

```cpp
template<std::size_t TSourceCapacity, class TSourceByteOperationsProvider, std::enable_if_t<(TSourceCapacity <= TCapacity), int> = 0>
        String& operator=(
            const String<TSourceCapacity, TSourceByteOperationsProvider>& source
        ) noexcept
```

### `TSourceByteOperationsProvider`

**Classification:** PUBLIC API · source access: `public`

Moves a bounded String whose compile-time capacity cannot exceed this destination's capacity and leaves the source empty.
- **Template parameter `TSourceCapacity`:** Compile-time capacity of the source bounded value.
- **Template parameter `TSourceByteOperationsProvider`:** ByteOperations provider selected by the source bounded value.

```cpp
template<std::size_t TSourceCapacity, class TSourceByteOperationsProvider, std::enable_if_t<(TSourceCapacity <= TCapacity), int> = 0>
        String& operator=(
            String<TSourceCapacity, TSourceByteOperationsProvider>&& source
        ) noexcept
```

### `Capacity`

**Classification:** PUBLIC API · source access: `public`

Returns the maximum number of payload bytes this String can retain, excluding the implicit null terminator.

```cpp
static constexpr std::size_t Capacity() noexcept
```

### `Size`

**Classification:** PUBLIC API · source access: `public`

Returns the current number of logical payload bytes, excluding the implicit null terminator.

```cpp
constexpr std::size_t Size() const noexcept
```

### `IsEmpty`

**Classification:** PUBLIC API · source access: `public`

Reports whether the String currently contains no payload bytes.

```cpp
constexpr bool IsEmpty() const noexcept
```

### `IsFull`

**Classification:** PUBLIC API · source access: `public`

Reports whether the String currently occupies its complete logical payload capacity.

```cpp
constexpr bool IsFull() const noexcept
```

### `Data`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only pointer to the first payload byte, followed by the guaranteed implicit null terminator.

```cpp
const char* Data() const noexcept
```

### `CStr`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only C-compatible null-terminated pointer to this String's payload.

```cpp
const char* CStr() const noexcept
```

### `View`

**Classification:** PUBLIC API · source access: `public`

Returns a non-owning standard view over the logical payload bytes, excluding the implicit terminator.

```cpp
std::string_view View() const noexcept
```

### `char operator[]( std::size_t index ) const noexcept`

**Classification:** PUBLIC API · source access: `public`

Returns the logical character at an index that must already satisfy index < Size().

```cpp
char operator[](
            std::size_t index
        ) const noexcept
```

### `GetAt`

**Classification:** PUBLIC API · source access: `public`

Retrieves one logical character through a checked access path.

```cpp
StringAccessResult GetAt(
            std::size_t index,
            char& value
        ) const noexcept
```

### `begin`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only iterator to the first logical payload character.

```cpp
const char* begin() const noexcept
```

### `end`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only iterator one position beyond the final logical payload character.

```cpp
const char* end() const noexcept
```

### `cbegin`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only iterator to the first logical payload character.

```cpp
const char* cbegin() const noexcept
```

### `cend`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only iterator one position beyond the final logical payload character.

```cpp
const char* cend() const noexcept
```

### `TSourceByteOperationsProvider`

**Classification:** PUBLIC API · source access: `public`

Assigns another bounded String after validating this destination's runtime capacity.
- **Template parameter `TSourceCapacity`:** Compile-time capacity of the source bounded value.
- **Template parameter `TSourceByteOperationsProvider`:** ByteOperations provider selected by the source bounded value.

```cpp
template<std::size_t TSourceCapacity, class TSourceByteOperationsProvider>
        StringAssignmentResult Assign(
            const String<TSourceCapacity, TSourceByteOperationsProvider>& source
        ) noexcept
```

### `Assign`

**Classification:** PUBLIC API · source access: `public`

Assigns a standard string view without truncation.

```cpp
StringAssignmentResult Assign(
            std::string_view source
        ) noexcept
```

### `Assign`

**Classification:** PUBLIC API · source access: `public`

Assigns a standard dynamic String without retaining any dynamic ownership.

```cpp
StringAssignmentResult Assign(
            const std::string& source
        ) noexcept
```

### `Assign`

**Classification:** PUBLIC API · source access: `public`

Assigns an explicitly sized character range without performing an unbounded null scan.

```cpp
StringAssignmentResult Assign(
            const char* source,
            std::size_t length
        ) noexcept
```

### `Assign`

**Classification:** PUBLIC API · source access: `public`

Assigns a null-terminated fixed character array after validating its terminator and payload.
- **Template parameter `TArraySize`:** Fixed source array extent.

```cpp
template<std::size_t TArraySize>
        StringAssignmentResult Assign(
            const char (&source)[TArraySize]
        ) noexcept
```

### `TSourceByteOperationsProvider`

**Classification:** PUBLIC API · source access: `public`

Appends another bounded String without truncation.
- **Template parameter `TSourceCapacity`:** Compile-time capacity of the source bounded value.
- **Template parameter `TSourceByteOperationsProvider`:** ByteOperations provider selected by the source bounded value.

```cpp
template<std::size_t TSourceCapacity, class TSourceByteOperationsProvider>
        StringAppendResult Append(
            const String<TSourceCapacity, TSourceByteOperationsProvider>& source
        ) noexcept
```

### `Append`

**Classification:** PUBLIC API · source access: `public`

Appends a standard string view without truncation.

```cpp
StringAppendResult Append(
            std::string_view source
        ) noexcept
```

### `Append`

**Classification:** PUBLIC API · source access: `public`

Appends a standard dynamic String without retaining any dynamic ownership.

```cpp
StringAppendResult Append(
            const std::string& source
        ) noexcept
```

### `Append`

**Classification:** PUBLIC API · source access: `public`

Appends an explicitly sized character range without performing an unbounded null scan.

```cpp
StringAppendResult Append(
            const char* source,
            std::size_t length
        ) noexcept
```

### `Append`

**Classification:** PUBLIC API · source access: `public`

Appends a null-terminated fixed character array after validating its terminator and payload.
- **Template parameter `TArraySize`:** Fixed source array extent.

```cpp
template<std::size_t TArraySize>
        StringAppendResult Append(
            const char (&source)[TArraySize]
        ) noexcept
```

### `TSourceByteOperationsProvider`

**Classification:** PUBLIC API · source access: `public`

Inserts another bounded String at a checked logical index.
- **Template parameter `TSourceCapacity`:** Compile-time capacity of the source bounded value.
- **Template parameter `TSourceByteOperationsProvider`:** ByteOperations provider selected by the source bounded value.

```cpp
template<std::size_t TSourceCapacity, class TSourceByteOperationsProvider>
        StringInsertResult Insert(
            std::size_t index,
            const String<TSourceCapacity, TSourceByteOperationsProvider>& source
        ) noexcept
```

### `Insert`

**Classification:** PUBLIC API · source access: `public`

Inserts a standard string view at a checked logical index.

```cpp
StringInsertResult Insert(
            std::size_t index,
            std::string_view source
        ) noexcept
```

### `Insert`

**Classification:** PUBLIC API · source access: `public`

Inserts a standard dynamic String at a checked logical index.

```cpp
StringInsertResult Insert(
            std::size_t index,
            const std::string& source
        ) noexcept
```

### `Insert`

**Classification:** PUBLIC API · source access: `public`

Inserts an explicitly sized character range at a checked logical index.

```cpp
StringInsertResult Insert(
            std::size_t index,
            const char* source,
            std::size_t length
        ) noexcept
```

### `Insert`

**Classification:** PUBLIC API · source access: `public`

Inserts a null-terminated fixed character array after validating its terminator and payload.
- **Template parameter `TArraySize`:** Fixed source array extent.

```cpp
template<std::size_t TArraySize>
        StringInsertResult Insert(
            std::size_t index,
            const char (&source)[TArraySize]
        ) noexcept
```

### `Erase`

**Classification:** PUBLIC API · source access: `public`

Erases a strict logical range without silently clamping an invalid request.

```cpp
StringEraseResult Erase(
            std::size_t index,
            std::size_t count
        ) noexcept
```

### `SetAt`

**Classification:** PUBLIC API · source access: `public`

Replaces one existing logical character while preserving the no-embedded-null invariant.

```cpp
StringSetAtResult SetAt(
            std::size_t index,
            char value
        ) noexcept
```

### `PushBack`

**Classification:** PUBLIC API · source access: `public`

Appends one non-null character when capacity remains available.

```cpp
StringPushBackResult PushBack(
            char value
        ) noexcept
```

### `PopBack`

**Classification:** PUBLIC API · source access: `public`

Removes the final logical character when the String is non-empty.

```cpp
StringPopBackResult PopBack() noexcept
```

### `Clear`

**Classification:** PUBLIC API · source access: `public`

Clears the logical payload while retaining all preallocated storage.

```cpp
void Clear() noexcept
```

### `TTarget`

**Classification:** PUBLIC API · source access: `public`

Converts this bounded String to a target Type through the target Type owner's compile-time adapter specialization.
- **Template parameter `TTarget`:** Explicit conversion target Type.

```cpp
template<class TTarget>
        auto CastTo(
            TTarget& target
        ) const noexcept(
            TypeConversionAdapter<
                String<TCapacity, TByteOperationsProvider>,
                Detail::NormalizedType<TTarget>
            >::IsNoexcept
        ) -> typename TypeConversionAdapter<
            String<TCapacity, TByteOperationsProvider>,
```

### `TSource`

**Classification:** PUBLIC API · source access: `public`

Converts a source Type into this bounded String through the source Type owner's compile-time adapter specialization.
- **Template parameter `TSource`:** Explicit conversion source Type.

```cpp
template<class TSource>
        auto CastFrom(
            const TSource& source
        ) noexcept(
            TypeConversionAdapter<
                Detail::NormalizedType<TSource>,
                String<TCapacity, TByteOperationsProvider>
            >::IsNoexcept
        ) -> typename TypeConversionAdapter<
            Detail::NormalizedType<TSource>,
```

### `TOtherByteOperationsProvider`

**Classification:** PUBLIC API · source access: `public`

Reports whether two bounded Strings contain identical logical payload bytes.
- **Template parameter `TOtherCapacity`:** Compile-time capacity of the compared bounded value.
- **Template parameter `TOtherByteOperationsProvider`:** ByteOperations provider selected by the compared bounded value.

```cpp
template<std::size_t TOtherCapacity, class TOtherByteOperationsProvider>
        bool operator==(
            const String<TOtherCapacity, TOtherByteOperationsProvider>& other
        ) const noexcept
```

### `TOtherByteOperationsProvider`

**Classification:** PUBLIC API · source access: `public`

Reports whether two bounded Strings contain different logical payload bytes.
- **Template parameter `TOtherCapacity`:** Compile-time capacity of the compared bounded value.
- **Template parameter `TOtherByteOperationsProvider`:** ByteOperations provider selected by the compared bounded value.

```cpp
template<std::size_t TOtherCapacity, class TOtherByteOperationsProvider>
        bool operator!=(
            const String<TOtherCapacity, TOtherByteOperationsProvider>& other
        ) const noexcept
```

### `TOtherByteOperationsProvider`

**Classification:** PUBLIC API · source access: `public`

Reports whether this String sorts lexicographically before another bounded String by payload byte value.
- **Template parameter `TOtherCapacity`:** Compile-time capacity of the compared bounded value.
- **Template parameter `TOtherByteOperationsProvider`:** ByteOperations provider selected by the compared bounded value.

```cpp
template<std::size_t TOtherCapacity, class TOtherByteOperationsProvider>
        bool operator<(
            const String<TOtherCapacity, TOtherByteOperationsProvider>& other
        ) const noexcept
```

### `TOtherByteOperationsProvider`

**Classification:** PUBLIC API · source access: `public`

Reports whether this String is lexicographically before or equal to another bounded String.
- **Template parameter `TOtherCapacity`:** Compile-time capacity of the compared bounded value.
- **Template parameter `TOtherByteOperationsProvider`:** ByteOperations provider selected by the compared bounded value.

```cpp
template<std::size_t TOtherCapacity, class TOtherByteOperationsProvider>
        bool operator<=(
            const String<TOtherCapacity, TOtherByteOperationsProvider>& other
        ) const noexcept
```

### `TOtherByteOperationsProvider`

**Classification:** PUBLIC API · source access: `public`

Reports whether this String sorts lexicographically after another bounded String.
- **Template parameter `TOtherCapacity`:** Compile-time capacity of the compared bounded value.
- **Template parameter `TOtherByteOperationsProvider`:** ByteOperations provider selected by the compared bounded value.

```cpp
template<std::size_t TOtherCapacity, class TOtherByteOperationsProvider>
        bool operator>(
            const String<TOtherCapacity, TOtherByteOperationsProvider>& other
        ) const noexcept
```

### `TOtherByteOperationsProvider`

**Classification:** PUBLIC API · source access: `public`

Reports whether this String is lexicographically after or equal to another bounded String.
- **Template parameter `TOtherCapacity`:** Compile-time capacity of the compared bounded value.
- **Template parameter `TOtherByteOperationsProvider`:** ByteOperations provider selected by the compared bounded value.

```cpp
template<std::size_t TOtherCapacity, class TOtherByteOperationsProvider>
        bool operator>=(
            const String<TOtherCapacity, TOtherByteOperationsProvider>& other
        ) const noexcept
```

### `TByteOperationsProvider`

**Classification:** PUBLIC API

Certifies every bounded String specialization as a self-contained memory-bounded value.
- **Template parameter `TCapacity`:** Compile-time capacity of the bounded value.
- **Template parameter `TByteOperationsProvider`:** Stateless ByteOperations provider selected by the bounded value.

```cpp
template<std::size_t TCapacity, class TByteOperationsProvider>
    struct MemoryBoundedTraits<String<TCapacity, TByteOperationsProvider>> : MemoryBoundedValueDeclaration<false>
```

### `TByteOperationsProvider`

**Classification:** PUBLIC API

Exposes the compile-time payload-byte capacity of a bounded String.
- **Template parameter `TCapacity`:** Compile-time capacity of the bounded value.
- **Template parameter `TByteOperationsProvider`:** Stateless ByteOperations provider selected by the bounded value.

```cpp
template<std::size_t TCapacity, class TByteOperationsProvider>
    struct CapacityTraits<String<TCapacity, TByteOperationsProvider>>
```

### `HasStaticCapacity`

**Classification:** PUBLIC API · source access: `public`

Bounded String capacity is known at compile time.

```cpp
static constexpr bool HasStaticCapacity = true;
```

### `Unit`

**Classification:** PUBLIC API · source access: `public`

Bounded String logical capacity is measured in payload bytes.

```cpp
static constexpr CapacityUnit Unit = CapacityUnit::Bytes;
```

### `Capacity`

**Classification:** PUBLIC API · source access: `public`

Reports the maximum number of payload bytes retained by the String.

```cpp
static constexpr std::size_t Capacity = TCapacity;
```

