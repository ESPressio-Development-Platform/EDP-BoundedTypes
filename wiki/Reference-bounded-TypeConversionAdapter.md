# src/bounded/TypeConversionAdapter.hpp

**Primary classification:** PUBLIC EXTENSION / TRAIT API

**Source baseline:** `f5742d864310dc4fdecb734721205b0e21c38e40`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-BoundedTypes/blob/f5742d864310dc4fdecb734721205b0e21c38e40/src/bounded/TypeConversionAdapter.hpp)

## Direct includes

- `type_traits`
- `utility`

## Documented declarations

### `UnsupportedTypeConversionResult`

**Classification:** PUBLIC EXTENSION / TRAIT API

Marker result Type used only when no conversion adapter has been supplied for a source/target Type pair.

```cpp
struct UnsupportedTypeConversionResult final
```

### `TSource`

**Classification:** PUBLIC EXTENSION / TRAIT API

Defines the default unavailable conversion adapter for a source/target Type pair.

```cpp
template<class TSource, class TTarget>
    struct TypeConversionAdapter
```

### `ResultType`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

Result Type returned by an unavailable adapter; successful specializations replace this with an operation-specific enum.

```cpp
using ResultType = UnsupportedTypeConversionResult;
```

### `IsAvailable`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

Indicates whether the source/target conversion has been supplied by the Type-owning integration library.

```cpp
static constexpr bool IsAvailable = false;
```

### `IsNoexcept`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

Indicates whether the supplied conversion is guaranteed not to throw.

```cpp
static constexpr bool IsNoexcept = true;
```

### `Convert`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

Rejects conversion when no source/target specialization has been supplied.

```cpp
static ResultType Convert(
            const TSource& source,
            TTarget& target
        ) = delete;
```

### `TSource`

**Classification:** PUBLIC EXTENSION / TRAIT API

Reports whether a conversion adapter is available for the supplied source/target Type pair.

```cpp
template<class TSource, class TTarget>
    inline constexpr bool IsTypeConversionAvailable = TypeConversionAdapter<
        TSource,
```

