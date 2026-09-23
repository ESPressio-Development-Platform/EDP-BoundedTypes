# src/bounded/CapacityTraits.hpp

**Primary classification:** PUBLIC EXTENSION / TRAIT API

**Source baseline:** `fa3ab3cbe1652ff74930ed6fc209658dd54551b5`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-BoundedTypes/blob/fa3ab3cbe1652ff74930ed6fc209658dd54551b5/src/bounded/CapacityTraits.hpp)

## Direct includes

- `array`
- `cstddef`
- `cstdint`
- `type_traits`

## Documented declarations

### `CapacityUnit`

**Classification:** PUBLIC EXTENSION / TRAIT API

Identifies the semantic unit represented by a compile-time capacity value.

```cpp
enum class CapacityUnit : std::uint8_t
```

### `TValue`

**Classification:** PUBLIC EXTENSION / TRAIT API

Describes compile-time logical capacity metadata for a Type.

```cpp
template<class TValue>
    struct CapacityTraits
```

### `HasStaticCapacity`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

Indicates whether the Type exposes a fixed logical capacity at compile time.

```cpp
static constexpr bool HasStaticCapacity = false;
```

### `Unit`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

Identifies the semantic unit of the compile-time capacity.

```cpp
static constexpr CapacityUnit Unit = CapacityUnit::None;
```

### `Capacity`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

Exposes the compile-time logical capacity when one exists.

```cpp
static constexpr std::size_t Capacity = 0;
```

### `TValue`

**Classification:** PUBLIC EXTENSION / TRAIT API

Reports whether a Type exposes a fixed logical capacity at compile time.

```cpp
template<class TValue>
    inline constexpr bool HasStaticCapacity = CapacityTraits<std::remove_cv_t<TValue>>::HasStaticCapacity;
```

### `TValue`

**Classification:** PUBLIC EXTENSION / TRAIT API

Exposes element-count capacity for a built-in fixed-extent array.

```cpp
template<class TValue, std::size_t TExtent>
    struct CapacityTraits<TValue[TExtent]>
```

### `HasStaticCapacity`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

Built-in arrays always have fixed compile-time capacity.

```cpp
static constexpr bool HasStaticCapacity = true;
```

### `Unit`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

Built-in array capacity is measured in elements.

```cpp
static constexpr CapacityUnit Unit = CapacityUnit::Elements;
```

### `Capacity`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

Reports the number of elements in the array.

```cpp
static constexpr std::size_t Capacity = TExtent;
```

### `TValue`

**Classification:** PUBLIC EXTENSION / TRAIT API

Exposes element-count capacity for a standard fixed-extent array.

```cpp
template<class TValue, std::size_t TExtent>
    struct CapacityTraits<std::array<TValue, TExtent>>
```

### `HasStaticCapacity`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

Standard arrays always have fixed compile-time capacity.

```cpp
static constexpr bool HasStaticCapacity = true;
```

### `Unit`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

Standard array capacity is measured in elements.

```cpp
static constexpr CapacityUnit Unit = CapacityUnit::Elements;
```

### `Capacity`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

Reports the number of elements in the array.

```cpp
static constexpr std::size_t Capacity = TExtent;
```

