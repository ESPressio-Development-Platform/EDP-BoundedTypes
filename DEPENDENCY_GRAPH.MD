# EDP-BoundedTypes Dependency Graph

`EDP-BoundedTypes` is a foundational, header-only library. It intentionally has no mandatory or optional dependency on any other ESPressio library, platform SDK, framework, or third-party library.

## Graph

```text
EDP-BoundedTypes
└── C++17 Standard Library
```

## Mandatory Dependencies

- C++17 Standard Library

## Optional Dependencies

None.

## Reverse Integration Dependencies

External libraries may provide `TypeConversionAdapter<TSource, TTarget>` specializations for Types they own. Those integrations create a dependency **from the external library toward `EDP-BoundedTypes`**; they do not create a dependency from `EDP-BoundedTypes` back to the external library.

Current example:

```text
EDP-Platform-Arduino
└── EDP-BoundedTypes [optional]
    condition: Arduino String / Bounded String conversion integration is included or used
```

That reverse optional integration does not alter the dependency graph of this repository.
