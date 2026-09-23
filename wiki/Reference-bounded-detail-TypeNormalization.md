# src/bounded/detail/TypeNormalization.hpp

**Primary classification:** PRIVATE IMPLEMENTATION

**Source baseline:** `f5742d864310dc4fdecb734721205b0e21c38e40`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-BoundedTypes/blob/f5742d864310dc4fdecb734721205b0e21c38e40/src/bounded/detail/TypeNormalization.hpp)

## Direct includes

- `type_traits`

## Documented declarations

### `TValue`

**Classification:** PRIVATE IMPLEMENTATION

Removes reference and cv-qualification from a Type for compile-time adapter and trait matching.

```cpp
template<class TValue>
    using NormalizedType = std::remove_cv_t<std::remove_reference_t<TValue>>;
```

