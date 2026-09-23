# EDP-BoundedTypes Dependency Graph

> Revalidated: 2026-09-23 — Composition Framework enhancement migration changes compile-time provider/requirement metadata and validation only; no ESPressio-library dependency edge changed.

## Library Dependency Tree

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

## Mandatory Dependencies

- `EDP-Memory` — defines the `ByteOperations` abstraction and provider contract consumed by bounded String/Bytes raw byte movement, filling, and comparison.
- `EDP-Platform-Portable` — supplies the default Standard C/C++ `ByteOperationsProvider` used by ordinary `String<N>` and `Bytes<N>` declarations.

`EDP-System` and `EDP-Platform` are reached transitively through the Memory/Portable provider stack.

The dependency remains acyclic: neither `EDP-Memory` nor `EDP-Platform-Portable` depends on `EDP-BoundedTypes`.

## Optional Dependencies

`EDP-BoundedTypes` has no optional ESPressio-library dependencies.

## Reverse Integration Dependencies

External libraries may provide `TypeConversionAdapter<TSource, TTarget>` specializations for Types they own. Those integrations create a dependency **from the external library toward `EDP-BoundedTypes`**; they do not create a dependency from `EDP-BoundedTypes` back to the external library.

Current example:

```text
EDP-Platform-Arduino
└── EDP-BoundedTypes [optional]
    condition: Arduino String / Bounded String conversion integration is included or used
```

That reverse optional integration does not alter this repository's dependency direction.
