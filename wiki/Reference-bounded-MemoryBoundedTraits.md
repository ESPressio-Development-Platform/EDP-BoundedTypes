# src/bounded/MemoryBoundedTraits.hpp

**Primary classification:** PUBLIC EXTENSION / TRAIT API

**Source baseline:** `f5742d864310dc4fdecb734721205b0e21c38e40`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-BoundedTypes/blob/f5742d864310dc4fdecb734721205b0e21c38e40/src/bounded/MemoryBoundedTraits.hpp)

## Direct includes

- `array`
- `cstddef`
- `cstdint`
- `functional`
- `optional`
- `string_view`
- `tuple`
- `type_traits`
- `utility`
- `variant`

## Documented declarations

### `MemoryBoundedCategory`

**Classification:** PUBLIC EXTENSION / TRAIT API

Classifies whether a Type is a recursively ownable bounded value, a non-owning view, or has not been qualified.

```cpp
enum class MemoryBoundedCategory : std::uint8_t
```

### `TValue`

**Classification:** PUBLIC EXTENSION / TRAIT API

Describes the bounded-memory qualification of a Type.

```cpp
template<class TValue>
    struct MemoryBoundedTraits
```

### `Category`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

Classifies arithmetic, enumeration, and null pointer scalar Types as intrinsically bounded values; all other unknown Types remain unqualified.

```cpp
static constexpr MemoryBoundedCategory Category =
            (
                std::is_arithmetic_v<std::remove_cv_t<TValue>> ||
                std::is_enum_v<std::remove_cv_t<TValue>> ||
                std::is_same_v<std::remove_cv_t<TValue>, std::nullptr_t>
            )
                ? MemoryBoundedCategory::BoundedValue
                : MemoryBoundedCategory::Unqualified;
```

### `HasExternalLifetimeDependencies`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

Indicates whether correct use of the Type depends on the lifetime of externally-owned state.

```cpp
static constexpr bool HasExternalLifetimeDependencies = false;
```

### `TValue`

**Classification:** PUBLIC EXTENSION / TRAIT API

Reports whether a Type is compiler-qualified as a recursively ownable memory-bounded value.

```cpp
template<class TValue>
    inline constexpr bool IsMemoryBoundedValue =
        !std::is_reference_v<TValue> &&
        (
            MemoryBoundedTraits<std::remove_cv_t<TValue>>::Category ==
            MemoryBoundedCategory::BoundedValue
        );
```

### `TValue`

**Classification:** PUBLIC EXTENSION / TRAIT API

Reports whether a Type is explicitly classified as a non-owning view or handle.

```cpp
template<class TValue>
    inline constexpr bool IsNonOwningView =
        !std::is_reference_v<TValue> &&
        (
            MemoryBoundedTraits<std::remove_cv_t<TValue>>::Category ==
            MemoryBoundedCategory::NonOwningView
        );
```

### `AreAllMemoryBoundedValues`

**Classification:** PUBLIC EXTENSION / TRAIT API

Reports whether every Type in a parameter pack is a compiler-qualified memory-bounded value.

```cpp
template<class... TValueTypes>
        inline constexpr bool AreAllMemoryBoundedValues = (
            true &&
            ... &&
            IsMemoryBoundedValue<TValueTypes>
        );
```

### `HasAnyExternalLifetimeDependency`

**Classification:** PUBLIC EXTENSION / TRAIT API

Reports whether any Type in a parameter pack carries explicit external lifetime dependencies.

```cpp
template<class... TValueTypes>
        inline constexpr bool HasAnyExternalLifetimeDependency = (
            false ||
            ... ||
            MemoryBoundedTraits<std::remove_cv_t<TValueTypes>>::HasExternalLifetimeDependencies
        );
```

### `MemoryBoundedValueDeclaration`

**Classification:** PUBLIC EXTENSION / TRAIT API

Provides a reusable certification base for a user-defined Type whose complete owned member graph is explicitly declared.

```cpp
template<bool THasExternalLifetimeDependencies, class... TOwnedMemberTypes>
    struct MemoryBoundedValueDeclaration
```

### `Category`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

Classifies the certified Type as a recursively ownable bounded value.

```cpp
static constexpr MemoryBoundedCategory Category = MemoryBoundedCategory::BoundedValue;
```

### `HasExternalLifetimeDependencies`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

Indicates whether this Type or any recursively owned member depends on externally-owned lifetime state.

```cpp
static constexpr bool HasExternalLifetimeDependencies =
            THasExternalLifetimeDependencies ||
            Detail::HasAnyExternalLifetimeDependency<TOwnedMemberTypes...>;
```

### `TValue`

**Classification:** PUBLIC EXTENSION / TRAIT API

Classifies raw pointers as non-owning handles rather than recursively ownable bounded values.

```cpp
template<class TValue>
    struct MemoryBoundedTraits<TValue*>
```

### `Category`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

Raw pointers refer to externally-owned storage and therefore do not qualify as owned bounded values.

```cpp
static constexpr MemoryBoundedCategory Category = MemoryBoundedCategory::NonOwningView;
```

### `HasExternalLifetimeDependencies`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

A raw pointer always depends on the externally-managed lifetime of its pointee.

```cpp
static constexpr bool HasExternalLifetimeDependencies = true;
```

### `TCharacter`

**Classification:** PUBLIC EXTENSION / TRAIT API

Classifies string views as non-owning views rather than recursively ownable bounded values.

```cpp
template<class TCharacter, class TTraits>
    struct MemoryBoundedTraits<std::basic_string_view<TCharacter, TTraits>>
```

### `Category`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

A string view does not own the storage it references.

```cpp
static constexpr MemoryBoundedCategory Category = MemoryBoundedCategory::NonOwningView;
```

### `HasExternalLifetimeDependencies`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

A string view depends on the lifetime of externally-owned character storage.

```cpp
static constexpr bool HasExternalLifetimeDependencies = true;
```

### `TValue`

**Classification:** PUBLIC EXTENSION / TRAIT API

Classifies reference wrappers as non-owning handles rather than recursively ownable bounded values.

```cpp
template<class TValue>
    struct MemoryBoundedTraits<std::reference_wrapper<TValue>>
```

### `Category`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

A reference wrapper does not own the object it references.

```cpp
static constexpr MemoryBoundedCategory Category = MemoryBoundedCategory::NonOwningView;
```

### `HasExternalLifetimeDependencies`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

A reference wrapper depends on the lifetime of an externally-owned object.

```cpp
static constexpr bool HasExternalLifetimeDependencies = true;
```

### `TValue`

**Classification:** PUBLIC EXTENSION / TRAIT API

Recursively qualifies a built-in fixed-extent array when its element Type is itself memory-bounded.

```cpp
template<class TValue, std::size_t TExtent>
    struct MemoryBoundedTraits<TValue[TExtent]>
```

### `Category`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

The array qualifies only when every element Type is compiler-qualified as memory-bounded.

```cpp
static constexpr MemoryBoundedCategory Category =
            IsMemoryBoundedValue<TValue>
                ? MemoryBoundedCategory::BoundedValue
                : MemoryBoundedCategory::Unqualified;
```

### `HasExternalLifetimeDependencies`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

Propagates external lifetime dependencies from the element Type.

```cpp
static constexpr bool HasExternalLifetimeDependencies =
            IsMemoryBoundedValue<TValue> &&
            MemoryBoundedTraits<std::remove_cv_t<TValue>>::HasExternalLifetimeDependencies;
```

### `TValue`

**Classification:** PUBLIC EXTENSION / TRAIT API

Recursively qualifies a standard fixed-extent array when its element Type is itself memory-bounded.

```cpp
template<class TValue, std::size_t TExtent>
    struct MemoryBoundedTraits<std::array<TValue, TExtent>>
```

### `Category`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

The array qualifies only when every element Type is compiler-qualified as memory-bounded.

```cpp
static constexpr MemoryBoundedCategory Category =
            IsMemoryBoundedValue<TValue>
                ? MemoryBoundedCategory::BoundedValue
                : MemoryBoundedCategory::Unqualified;
```

### `HasExternalLifetimeDependencies`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

Propagates external lifetime dependencies from the element Type.

```cpp
static constexpr bool HasExternalLifetimeDependencies =
            IsMemoryBoundedValue<TValue> &&
            MemoryBoundedTraits<std::remove_cv_t<TValue>>::HasExternalLifetimeDependencies;
```

### `TValue`

**Classification:** PUBLIC EXTENSION / TRAIT API

Recursively qualifies an optional when its contained value Type is itself memory-bounded.

```cpp
template<class TValue>
    struct MemoryBoundedTraits<std::optional<TValue>>
```

### `Category`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

The optional qualifies only when its contained value Type is memory-bounded.

```cpp
static constexpr MemoryBoundedCategory Category =
            IsMemoryBoundedValue<TValue>
                ? MemoryBoundedCategory::BoundedValue
                : MemoryBoundedCategory::Unqualified;
```

### `HasExternalLifetimeDependencies`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

Propagates external lifetime dependencies from the optional value Type.

```cpp
static constexpr bool HasExternalLifetimeDependencies =
            IsMemoryBoundedValue<TValue> &&
            MemoryBoundedTraits<std::remove_cv_t<TValue>>::HasExternalLifetimeDependencies;
```

### `TFirst`

**Classification:** PUBLIC EXTENSION / TRAIT API

Recursively qualifies a pair when both owned value Types are themselves memory-bounded.

```cpp
template<class TFirst, class TSecond>
    struct MemoryBoundedTraits<std::pair<TFirst, TSecond>>
```

### `Category`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

The pair qualifies only when both value Types are memory-bounded.

```cpp
static constexpr MemoryBoundedCategory Category =
            (
                IsMemoryBoundedValue<TFirst> &&
                IsMemoryBoundedValue<TSecond>
            )
                ? MemoryBoundedCategory::BoundedValue
                : MemoryBoundedCategory::Unqualified;
```

### `HasExternalLifetimeDependencies`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

Propagates external lifetime dependencies from both pair values.

```cpp
static constexpr bool HasExternalLifetimeDependencies =
            (
                IsMemoryBoundedValue<TFirst> &&
                MemoryBoundedTraits<std::remove_cv_t<TFirst>>::HasExternalLifetimeDependencies
            ) ||
            (
                IsMemoryBoundedValue<TSecond> &&
                MemoryBoundedTraits<std::remove_cv_t<TSecond>>::HasExternalLifetimeDependencies
            );
```

### `MemoryBoundedTraits`

**Classification:** PUBLIC EXTENSION / TRAIT API

Recursively qualifies a tuple when every owned element Type is itself memory-bounded.

```cpp
template<class... TValueTypes>
    struct MemoryBoundedTraits<std::tuple<TValueTypes...>>
```

### `Category`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

The tuple qualifies only when every owned element Type is memory-bounded.

```cpp
static constexpr MemoryBoundedCategory Category =
            Detail::AreAllMemoryBoundedValues<TValueTypes...>
                ? MemoryBoundedCategory::BoundedValue
                : MemoryBoundedCategory::Unqualified;
```

### `HasExternalLifetimeDependencies`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

Propagates external lifetime dependencies from all tuple element Types.

```cpp
static constexpr bool HasExternalLifetimeDependencies =
            Detail::AreAllMemoryBoundedValues<TValueTypes...> &&
            Detail::HasAnyExternalLifetimeDependency<TValueTypes...>;
```

### `MemoryBoundedTraits`

**Classification:** PUBLIC EXTENSION / TRAIT API

Recursively qualifies a variant when every possible alternative Type is itself memory-bounded.

```cpp
template<class... TValueTypes>
    struct MemoryBoundedTraits<std::variant<TValueTypes...>>
```

### `Category`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

The variant qualifies only when every possible alternative Type is memory-bounded.

```cpp
static constexpr MemoryBoundedCategory Category =
            Detail::AreAllMemoryBoundedValues<TValueTypes...>
                ? MemoryBoundedCategory::BoundedValue
                : MemoryBoundedCategory::Unqualified;
```

### `HasExternalLifetimeDependencies`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

Propagates external lifetime dependencies from all possible alternative Types.

```cpp
static constexpr bool HasExternalLifetimeDependencies =
            Detail::AreAllMemoryBoundedValues<TValueTypes...> &&
            Detail::HasAnyExternalLifetimeDependency<TValueTypes...>;
```

### `MemoryBoundedTraits`

**Classification:** PUBLIC EXTENSION / TRAIT API

Classifies std::monostate as an intrinsically bounded empty value.

```cpp
template<>
    struct MemoryBoundedTraits<std::monostate>
```

### `Category`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

std::monostate is a self-contained bounded value.

```cpp
static constexpr MemoryBoundedCategory Category = MemoryBoundedCategory::BoundedValue;
```

### `HasExternalLifetimeDependencies`

**Classification:** PUBLIC EXTENSION / TRAIT API · source access: `public`

std::monostate has no external lifetime dependency.

```cpp
static constexpr bool HasExternalLifetimeDependencies = false;
```

