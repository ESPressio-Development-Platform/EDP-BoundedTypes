# src/bounded/detail/RawSlotStorage.hpp

**Primary classification:** PRIVATE IMPLEMENTATION

**Source baseline:** `fa3ab3cbe1652ff74930ed6fc209658dd54551b5`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-BoundedTypes/blob/fa3ab3cbe1652ff74930ed6fc209658dd54551b5/src/bounded/detail/RawSlotStorage.hpp)

## Direct includes

- `array`
- `cstddef`
- `new`
- `type_traits`
- `utility`

## Documented declarations

### `TValue`

**Classification:** PRIVATE IMPLEMENTATION

Reserves compile-time aligned inline slots and manages explicit object lifetime without owning container semantics.

```cpp
template<class TValue, std::size_t TCapacity>
    class RawSlotStorage final
```

### `Slot`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Defines one correctly aligned raw slot capable of hosting a TValue object.

```cpp
using Slot = std::aligned_storage_t<
            sizeof(TValue),
```

### `Slot`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Reserves exactly TCapacity raw slots without constructing TValue instances.

```cpp
std::array<Slot, TCapacity> _slots{};
```

### `Construct`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Constructs a TValue object in a raw slot using only nothrow construction paths.

```cpp
template<class... TArgumentTypes>
        TValue& Construct(
            std::size_t index,
            TArgumentTypes&&... arguments
        ) noexcept
```

### `Destroy`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Explicitly destroys the live TValue object in one slot.

```cpp
void Destroy(
            std::size_t index
        ) noexcept
```

### `Get`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Returns a mutable reference to the already-live TValue object in one slot.

```cpp
TValue& Get(
            std::size_t index
        ) noexcept
```

### `Get`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Returns a read-only reference to the already-live TValue object in one slot.

```cpp
const TValue& Get(
            std::size_t index
        ) const noexcept
```

### `GetPointer`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Returns a mutable pointer to the already-live TValue object in one slot.

```cpp
TValue* GetPointer(
            std::size_t index
        ) noexcept
```

### `GetPointer`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Returns a read-only pointer to the already-live TValue object in one slot.

```cpp
const TValue* GetPointer(
            std::size_t index
        ) const noexcept
```

