# EDP-BoundedTypes

`EDP-BoundedTypes` provides strongly memory-bounded, allocation-free value and container Types for the ESPressio Development Platform.

The library is intentionally independent of every other ESPressio library. Its purpose is to make maximum owned memory usage explicit at compile time while keeping mutation deterministic and free from hidden heap growth.

## Contents

The following bounded-memory facilities are currently provided:

- [Memory-Boundedness Model](./docs/MEMORY-BOUNDEDNESS.MD)  
  Defines whole-object owned-memory boundedness, recursive certification, external lifetime dependencies, failure guarantees, zero-capacity behavior, and the distinction between memory boundedness and serialization/schema boundedness.

- [String and Bytes](./docs/STRING-AND-BYTES.MD)  
  Documents `String<N>` and `Bytes<N>`, including capacity semantics, null handling, mutation/result behavior, operators, views, assignment, and conversion boundaries.

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
- Platform-specific conversion support is owned by the library exposing the external Type, never by `EDP-BoundedTypes`.
- The library targets C++17 and performs no dynamic allocation internally.

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
└── C++17 Standard Library
```

There are no mandatory or optional ESPressio-library dependencies.

External integration libraries may depend **toward** `EDP-BoundedTypes`. For example, `EDP-Platform-Arduino` optionally supplies the Arduino `String` / `Bounded::String<N>` conversion adapters. That reverse integration does not create a dependency from this repository back to Arduino.

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

Host tests cover capacity boundaries, zero-capacity behavior, String invariants, binary data, non-trivial object lifetimes, moved-from empty semantics, ring behavior, associative semantics, trait qualification, and compile-fail cases for invalid contained Types.

The library is header-only and requires C++17.
