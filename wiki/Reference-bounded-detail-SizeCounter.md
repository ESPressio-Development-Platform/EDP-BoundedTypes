# src/bounded/detail/SizeCounter.hpp

**Primary classification:** PRIVATE IMPLEMENTATION

**Source baseline:** `fa3ab3cbe1652ff74930ed6fc209658dd54551b5`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-BoundedTypes/blob/fa3ab3cbe1652ff74930ed6fc209658dd54551b5/src/bounded/detail/SizeCounter.hpp)

## Direct includes

- `cstddef`
- `cstdint`
- `limits`
- `type_traits`

## Documented declarations

### `SizeCounterSelector`

**Classification:** PRIVATE IMPLEMENTATION

Selects the smallest unsigned integer Type capable of representing values from zero through the supplied capacity.

```cpp
template<std::size_t TCapacity>
    struct SizeCounterSelector
```

### `Type`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

The smallest unsigned integer Type able to represent the complete logical size range.

```cpp
using Type = std::conditional_t<
            (TCapacity <= static_cast<std::size_t>(std::numeric_limits<std::uint8_t>::max())),
```

### `SizeCounter`

**Classification:** PRIVATE IMPLEMENTATION

Provides the compact runtime counter Type used internally by a bounded value with the supplied static capacity.

```cpp
template<std::size_t TCapacity>
    using SizeCounter = typename SizeCounterSelector<TCapacity>::Type;
```

