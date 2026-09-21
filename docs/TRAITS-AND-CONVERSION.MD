# Traits and External Conversion

## Purpose

The trait layer answers two different compile-time questions:

```text
Can this Type be recursively owned as a memory-bounded value?
Does this Type expose a static logical capacity, and in what unit?
```

The conversion layer provides a third mechanism:

```text
Can another Type-owning library define an explicit conversion to or from a bounded Type without introducing a reverse dependency?
```

---

## MemoryBoundedCategory

The memory qualification vocabulary is:

```cpp
enum class MemoryBoundedCategory : std::uint8_t {
    Unqualified = 0,
    BoundedValue = 1,
    NonOwningView = 2
};
```

The default for an unknown Type is `Unqualified`.

Convenience predicates include `IsMemoryBoundedValue<T>` and `IsNonOwningView<T>`.

---

## Intrinsic Bounded Values

Fundamental arithmetic Types, enums, and recursively finite standard value structures qualify as bounded values where their components qualify.

Examples include:

```text
integral Types
floating Types
enum Types
C arrays
std::array
std::optional
std::pair
std::tuple
std::variant
std::monostate
```

Recursive aggregate qualification requires every owned component Type to be memory-bounded.

---

## Non-Owning Views

Raw pointers and common borrowed-view Types are classified separately from owned bounded values.

Examples:

```text
T*
std::string_view
std::reference_wrapper<T>
```

These objects may have a fixed `sizeof`, but the memory they refer to is not owned by the value.

They therefore cannot be used as an escape hatch inside an owning bounded collection.

---

## Dynamically Owning Types

Dynamic owners such as `std::string`, `std::vector`, node-based standard containers, `std::unique_ptr`, and `std::shared_ptr` are not memory-bounded values.

They may still appear as checked input source Types for specific operations. The prohibition concerns recursive ownership inside a bounded value.

---

## Custom Type Certification

A custom Type must be certified by the library that owns it.

The helper:

```cpp
MemoryBoundedValueDeclaration<
    THasExternalLifetimeDependencies,
    TOwnedMemberTypes...
>
```

recursively validates the declared owned member Types and propagates lifetime-dependency metadata.

A custom specialization conceptually follows:

```cpp
template<>
struct MemoryBoundedTraits<MyType>
    : MemoryBoundedValueDeclaration<
          false,
          MyOwnedMemberA,
          MyOwnedMemberB
      > {
};
```

The compiler can validate the member Types supplied to the declaration, but C++20 provides no standard reflection mechanism capable of proving that the author listed every private owned member.

Therefore certification is an architectural responsibility of the Type-owning library.

---

## External Lifetime Dependencies

The certification Boolean distinguishes self-contained bounded values from bounded values that borrow external lifetime-stable collaborators.

A value can therefore be:

```text
MemoryBoundedCategory::BoundedValue
HasExternalLifetimeDependencies == true
```

without classifying the borrowed pointer/reference itself as an owning bounded value.

---

## CapacityTraits

Capacity is separate from memory boundedness.

The unit vocabulary is:

```cpp
enum class CapacityUnit : std::uint8_t {
    None = 0,
    Bytes = 1,
    Elements = 2,
    Entries = 3
};
```

A capacity-bearing Type exposes through `CapacityTraits<T>`:

```text
HasStaticCapacity
Unit
Capacity
```

Examples:

```text
String<32>
    32 Bytes

Bytes<32>
    32 Bytes

Vector<int,32>
    32 Elements

Deque<int,32>
    32 Elements

Set<int,32>
    32 Entries

Map<int,int,32>
    32 Entries
```

A scalar Type does not acquire a synthetic logical capacity merely because `sizeof(T)` is finite.

---

## Why Capacity Carries a Unit

A bare number is ambiguous.

Thirty-two bytes, thirty-two elements, and thirty-two map entries are different resource contracts.

The explicit `CapacityUnit` prevents higher libraries from accidentally treating them as interchangeable.

---

## TypeConversionAdapter

`TypeConversionAdapter<TSource,TTarget>` is the compile-time extension seam for explicit external conversions.

The default adapter is unavailable.

A specialization supplies:

```text
ResultType
IsAvailable
IsNoexcept
Convert(source, target)
```

`IsTypeConversionAvailable<TSource,TTarget>` exposes availability as a Boolean compile-time predicate.

---

## Conversion Ownership Rule

The rule is:

> The library that exposes the external/specific Type owns the adapter specialization involving that Type.

Therefore `EDP-BoundedTypes` never depends on Arduino, ESP-IDF, FreeRTOS, platform SDKs, or third-party frameworks merely to support their Types.

Dependency direction remains:

```text
external Type-owning library
        |
        v
EDP-BoundedTypes
```

---

## CastTo and CastFrom

Bounded values expose explicit named conversion methods:

```cpp
bounded.CastTo(target);
bounded.CastFrom(source);
```

They do not provide implicit platform conversion operators.

The named API keeps allocation consequences, failure, ownership transfer, and platform coupling visible to the caller.

Unsupported conversions fail at compile time with a missing-adapter diagnostic.

---

## Strong Conversion Guarantee

An adapter must preserve the library-wide failure rule:

> a non-success conversion leaves the destination logical value unchanged.

If an external target allocates dynamically, the adapter owns the responsibility for staging and publishing safely.

The result enum also belongs to the external Type-owning library because that library knows the meaningful platform-specific failure causes.

---

## Arduino String Example

`EDP-Platform-Arduino` supplies the current concrete example.

It owns:

```text
Arduino ::String -> Bounded::String<N>
Bounded::String<N> -> Arduino ::String
Arduino-specific result enums
```

The integration is explicitly opt-in through the Platform-Arduino bounded integration header.

This creates an optional dependency from `EDP-Platform-Arduino` to `EDP-BoundedTypes`.

It does not create an Arduino dependency in this repository.
