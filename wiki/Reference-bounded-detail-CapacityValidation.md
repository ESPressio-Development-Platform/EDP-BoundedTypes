# src/bounded/detail/CapacityValidation.hpp

**Primary classification:** PRIVATE IMPLEMENTATION

**Source baseline:** `fa3ab3cbe1652ff74930ed6fc209658dd54551b5`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-BoundedTypes/blob/fa3ab3cbe1652ff74930ed6fc209658dd54551b5/src/bounded/detail/CapacityValidation.hpp)

## Direct includes

- `cstddef`
- `cstdint`

## Documented declarations

### `CapacityValidationResult`

**Classification:** PRIVATE IMPLEMENTATION

Describes whether a candidate logical size fits within a compile-time storage capacity.

```cpp
enum class CapacityValidationResult : std::uint8_t
```

### `ValidateCapacity`

**Classification:** PRIVATE IMPLEMENTATION

Validates a candidate logical size against a compile-time maximum capacity.

```cpp
template<std::size_t TCapacity>
    constexpr CapacityValidationResult ValidateCapacity(
        std::size_t candidateSize
    ) noexcept
```

