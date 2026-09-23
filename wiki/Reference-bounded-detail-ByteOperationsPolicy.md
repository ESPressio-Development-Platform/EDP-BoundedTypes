# src/bounded/detail/ByteOperationsPolicy.hpp

**Primary classification:** PRIVATE IMPLEMENTATION

**Source baseline:** `fa3ab3cbe1652ff74930ed6fc209658dd54551b5`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-BoundedTypes/blob/fa3ab3cbe1652ff74930ed6fc209658dd54551b5/src/bounded/detail/ByteOperationsPolicy.hpp)

## Direct includes

- `cstddef`
- `cstdint`
- `type_traits`
- `memory/ByteOperationsProvider.hpp`

## Documented declarations

### `DefaultByteOperationsProvider`

**Classification:** PRIVATE IMPLEMENTATION

Default stateless ByteOperations provider used by bounded scalar byte sequences.

```cpp
using DefaultByteOperationsProvider = ESPressio::Platform::Portable::Memory::ByteOperationsProvider;
```

### `TByteOperationsProvider`

**Classification:** PRIVATE IMPLEMENTATION

Adapts one stateless EDP-Memory ByteOperations provider Type into zero-state compile-time operations.

Bounded String/Bytes deliberately retain no provider pointer or provider object. The selected provider must
therefore be a stateless, nothrow-default-constructible policy Type.

- **Template parameter `TByteOperationsProvider`:** Concrete provider satisfying the EDP-Memory ByteOperations capability.

```cpp
template<class TByteOperationsProvider>
    struct ByteOperationsPolicy final
```

### `MoveBytes`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Moves one byte range using the selected overlap-safe provider operation.

```cpp
static void MoveBytes(
            void* destination,
            const void* source,
            std::size_t byteCount
        ) noexcept
```

### `FillBytes`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Fills one writable byte range using the selected provider operation.

```cpp
static void FillBytes(
            void* destination,
            std::uint8_t value,
            std::size_t byteCount
        ) noexcept
```

### `CompareBytes`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Compares two byte ranges using the selected unsigned-byte lexicographical provider operation.

```cpp
static ESPressio::Memory::ByteComparison CompareBytes(
            const void* left,
            const void* right,
            std::size_t byteCount
        ) noexcept
```

