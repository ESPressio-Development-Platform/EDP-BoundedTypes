# EDP-BoundedTypes

`EDP-BoundedTypes` provides strongly memory-bounded, allocation-free value and container Types for the ESPressio Development Platform.

The library makes maximum owned memory usage explicit at compile time while keeping mutation deterministic and free from hidden heap growth. Raw byte movement/fill/comparison used by `String` and `Bytes` is consumed through the `EDP-Memory::ByteOperations` abstraction; the ordinary default concrete provider is the Standard C/C++ implementation owned by `EDP-Platform-Portable`.

## Contents

The following bounded-memory facilities are currently provided:

- [Memory-Boundedness Model](./docs/MEMORY-BOUNDEDNESS.MD)  
  Defines whole-object owned-memory boundedness, recursive certification, external lifetime dependencies, failure guarantees, zero-capacity behavior, and the distinction between memory boundedness and serialization/schema boundedness.

- [String and Bytes](./docs/STRING-AND-BYTES.MD)  
  Documents `String<N>` and `Bytes<N>`, including capacity semantics, null handling, mutation/result behavior, byte-operation provider substitution, operators, views, assignment, and conversion boundaries.

- [Sequence Containers](./docs/SEQUENCE-CONTAINERS.MD)  
  Documents `Vector`, `Deque`, `Queue`, `Stack`, and `CircularBuffer`, including raw-slot lifetime rules, operation-specific `noexcept` requirements, iterator semantics, invalidation, and explicit overwrite behavior.

- [Associative Containers](./docs/ASSOCIATIVE-CONTAINERS.MD)  
  Documents `Set` and `Map`, including insertion-order storage, linear lookup, uniqueness guarantees, key immutability, semantic equality, and why map-style `operator[]` is deliberately absent.

- [Traits and External Conversion](./docs/TRAITS-AND-CONVERSION.MD)  
  Documents `MemoryBoundedTraits`, `CapacityTraits`, custom bounded-Type certification, `TypeConversionAdapter`, and the ownership rule for conversions to external/platform Types.

## Core Guarantees

- Maximum owned storage is reserved inline for the lifetime of each bounded value.
- Owning bounded collections accept only recursively certified memory-bounded value Types.
- Unknown user-defined Types are rejected until their owning library explicitly certifies their owned member graph.
- Runtime operations that can fail return operation-specific strongly typed result enums.
- Capacity failure never silently truncates input.
- A reported non-success result leaves the destination logical value unchanged.
- Moved-from bounded containers have a deterministic empty state.
- `String`/`Bytes` raw memory operations consume `EDP-Memory::ByteOperations`; Standard C/C++ memory primitives are not owned directly by this repository.
- The default ByteOperations policy stores no provider object or pointer in each bounded value, so provider integration adds no per-object RAM overhead.
- Platform-specific conversion support is owned by the library exposing the external Type, never by `EDP-BoundedTypes`.
- The library targets C++20 and performs no dynamic allocation internally.

## Type Family

```text
String<N>
Bytes<N>
Vector<T,N>
Deque<T,N>
Queue<T,N>
Stack<T,N>
CircularBuffer<T,N>
Set<T,N>
Map<K,V,N>
```

`String` and `Bytes` additionally accept an optional compile-time stateless ByteOperations provider Type. The default remains `EDP-Platform-Portable::Memory::ByteOperationsProvider`, so ordinary one-argument declarations are unchanged.

Every capacity-bearing Type exposes the common vocabulary:

```cpp
Capacity();
Size();
IsEmpty();
IsFull();
Clear();
```

The exact mutation surface remains Type-specific.

## Dependencies

```text
EDP-BoundedTypes
├── EDP-Memory [mandatory]
│   ├── EDP-System [mandatory]
│   └── EDP-Platform [mandatory]
│       └── EDP-System [mandatory, transitive]
└── EDP-Platform-Portable [mandatory]
    ├── EDP-Platform [mandatory]
    │   └── EDP-System [mandatory]
    └── EDP-Memory [mandatory]
        ├── EDP-System [mandatory]
        └── EDP-Platform [mandatory]
            └── EDP-System [mandatory, transitive]
```

`EDP-Memory` defines the ByteOperations abstraction. `EDP-Platform-Portable` supplies the default Standard C/C++ provider selected by ordinary `String<N>` / `Bytes<N>` declarations.

External integration libraries may depend **toward** `EDP-BoundedTypes`. For example, `EDP-Platform-Arduino` optionally supplies Arduino `String` / `Bounded::String<N>` conversion adapters. That reverse integration does not create a dependency from this repository back to Arduino.

See [DEPENDENCY_GRAPH.MD](./DEPENDENCY_GRAPH.MD) for the maintained dependency graph.

## Demos

Buildable demonstrations are provided under [`./demos`](./demos) for:

- String and Bytes;
- sequence/ring containers;
- Set and Map;
- compile-time traits and capacity metadata.

Each logical demo is represented for:

- Arduino IDE;
- PlatformIO + Arduino-ESP32;
- PlatformIO + ESP-IDF.

## Testing

Host tests cover capacity boundaries, zero-capacity behavior, String invariants, binary data, ByteOperations provider substitution, absence of provider-related per-object size overhead, non-trivial object lifetimes, moved-from empty semantics, ring behavior, associative semantics, trait qualification, and compile-fail cases for invalid contained/provider Types.

The library is header-only and requires C++20.

## C++ Language Baseline

ESPressio requires C++20. PlatformIO-compatible ESP32 demos use the maintained PIOArduino platform package so Arduino-ESP32 and ESP-IDF builds share a current C++20-capable toolchain baseline.
