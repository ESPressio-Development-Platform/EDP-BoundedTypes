# EDP-BoundedTypes

`EDP-BoundedTypes` provides strongly memory-bounded, allocation-free value and container Types for the ESPressio Development Platform.

The library is intentionally independent of every other ESPressio library. Its purpose is to make maximum owned memory usage explicit at compile time while keeping normal mutation behavior deterministic and free from hidden heap growth.

## Core Principles

- Maximum owned storage is reserved inline for the lifetime of each bounded value.
- Owning bounded collections accept only recursively certified memory-bounded value Types.
- Unknown user-defined Types are rejected until their owning library explicitly certifies their owned member graph.
- Raw pointers, references, `std::string_view`, and similar handles are non-owning views, not bounded owned values.
- Runtime operations that can fail return operation-specific strongly typed result enums.
- Genuine Boolean predicates remain `bool`.
- Capacity failure never silently truncates input.
- Reported non-success leaves the destination logical value unchanged.
- Platform-specific conversion support is owned by the library exposing the external Type, never by `EDP-BoundedTypes`.
- The library targets C++17 and performs no dynamic allocation internally.

## Types

### `String<N>`

A variable-length text byte sequence with:

- maximum payload capacity of `N` bytes;
- `N + 1` bytes of character storage so the null terminator is implicit;
- permanent `storage[Size()] == '\0'`;
- no embedded null characters in the logical payload;
- encoding-agnostic payload semantics: UTF-8 validity is not a `String<N>` invariant;
- read-only raw character access so callers cannot bypass the embedded-null invariant.

For example, `String<255>` reserves 256 character bytes: 255 payload bytes and one implicit null terminator.

### `Bytes<N>`

A variable-length binary sequence containing between zero and `N` bytes. Every byte value, including zero, is valid. No terminator is stored.

### `Vector<T, N>`

A bounded indexed sequence with storage for `N` values reserved inline. Only active values are constructed.

The C++17 raw-slot lifetime model deliberately does **not** claim `std::vector`-style contiguous `T*` semantics for arbitrary non-trivial `T`. Logical random-access iteration is provided through a custom iterator.

### `Deque<T, N>`

A bounded double-ended sequence using inline ring storage.

### `Queue<T, N>`

A FIFO abstraction over bounded ring storage.

### `Stack<T, N>`

A LIFO abstraction over bounded indexed storage.

### `CircularBuffer<T, N>`

A bounded oldest-to-newest ring buffer. Normal `Push()` rejects a full buffer. `PushOverwrite()` explicitly opts into replacing the oldest value.

### `Set<T, N>`

A unique-value collection using deterministic insertion-order storage and linear lookup. Stored values are exposed read-only so uniqueness cannot be invalidated through mutation.

### `Map<K, V, N>`

A unique-key mapping using deterministic insertion-order storage and linear lookup. Keys remain immutable through the public API while mapped values may be mutated.

`operator[]` is intentionally absent because standard map-style indexing can implicitly insert and therefore hide a fallible bounded-capacity operation.

## Whole-Object Memory Boundedness

A Type is safe for ownership inside `Vector`, `Set`, `Map`, and the other bounded containers only when its complete **owned** runtime memory graph is bounded.

```cpp
static_assert(
    ESPressio::Bounded::IsMemoryBoundedValue<int>
);

static_assert(
    !ESPressio::Bounded::IsMemoryBoundedValue<std::string>
);
```

Examples:

```text
Vector<int, 8>                         valid
Vector<String<32>, 8>                  valid
Map<String<16>, Bytes<64>, 4>          valid
Vector<std::string, 8>                 compile-time error
Vector<int*, 8>                        compile-time error
```

A custom Type opts in through `MemoryBoundedTraits<T>` and should derive from `MemoryBoundedValueDeclaration<...>` while declaring every recursively owned member Type.

A custom bounded value may explicitly carry non-owning lifetime dependencies. `MemoryBoundedTraits<T>::HasExternalLifetimeDependencies` records that fact independently of owned-memory boundedness.

## Capacity Traits

`CapacityTraits<T>` separates a numeric capacity from its semantic unit:

```text
String<32>        32 Bytes
Bytes<32>         32 Bytes
Vector<int, 32>   32 Elements
Map<int,int,32>   32 Entries
```

The available units are `None`, `Bytes`, `Elements`, and `Entries`.

## Result Conventions

Fallible mutations use operation-specific result enums:

```cpp
ESPressio::Bounded::String<16> value;

const auto result = value.Assign("example");

if (result != ESPressio::Bounded::StringAssignmentResult::Succeeded) {
    // Handle the explicit non-success outcome.
}
```

Predicates remain Boolean:

```cpp
value.IsEmpty();
value.IsFull();
```

## Operators

Operators are provided only when their accepted operation is infallible, invariant-preserving, and does not hide a significant resource consequence.

Examples include:

- safe copy/move and widening bounded assignment;
- logical comparisons;
- read-only `String` indexing;
- mutable `Bytes` and `Vector` indexing.

Potentially failing concatenation/append operations use named result-returning methods instead of `operator+=`.

Implicit conversion to dynamic/platform Types is intentionally absent.

## External-Type Conversion

`TypeConversionAdapter<TSource, TTarget>` is the generic compile-time extension seam.

If another library exposes a Type that should convert to or from a bounded Type, **that external Type's owning library owns the adapter specialization and its operation-specific result enum**.

`EDP-BoundedTypes` never acquires an Arduino, ESP-IDF, FreeRTOS, SDK, framework, or third-party dependency merely to support that conversion.

## Standard C++ Interoperability

The v1 surface intentionally keeps dynamic standard-library interoperability narrow:

- `String`: `std::string`, `std::string_view`, fixed character arrays, pointer + explicit length.
- `Bytes`: `std::array<std::uint8_t, N>`, `std::vector<std::uint8_t>`, pointer + explicit length.
- `Vector`: `std::array<T, N>`, `std::vector<T>`.

Dynamic inputs are copied into bounded inline storage only after capacity validation.

## Zero Capacity

Zero-capacity specializations are valid:

```text
String<0>
Bytes<0>
Vector<T, 0>
Map<K, V, 0>
```

They report:

```text
Capacity() == 0
Size() == 0
IsEmpty() == true
IsFull() == true
```

Any attempt to add logical content returns the relevant capacity failure.

## Thread Safety

Bounded Types provide no internal synchronization. Normal C++ data-race rules apply. Synchronization belongs to the owning application/domain.

## Security

`Clear()` destroys/resets logical state but does not claim optimizer-resistant sensitive-memory erasure.

A method named `SecureClear()` is intentionally absent from v1 because such a guarantee requires a separately verified security/platform primitive.

## Dependencies

```text
EDP-BoundedTypes
└── C++17 Standard Library
```

There are no optional dependencies.

External conversion integrations depend toward this library rather than creating reverse dependencies.

See [DEPENDENCY_GRAPH.MD](./DEPENDENCY_GRAPH.MD) for the complete dependency graph.

## Demos

Buildable demos are provided under `./demos` for:

- String and Bytes;
- sequence/ring containers;
- Set and Map;
- compile-time traits and capacity metadata.

Each demo is represented for:

- Arduino IDE;
- PlatformIO + Arduino-ESP32;
- PlatformIO + ESP-IDF.

## Testing

Host tests cover:

- capacity boundaries and zero-capacity behavior;
- String null/terminator invariants and self-aliasing operations;
- Bytes behavior;
- non-trivial object lifetime accounting;
- move-source-empty semantics;
- ring wrap and overwrite behavior;
- Set uniqueness and Map key/value semantics;
- trait qualification;
- compile-fail examples for illegal nested Types.

The library is header-only and requires C++17.
