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

External libraries may provide `TypeConversionAdapter<TSource, TTarget>` specializations for Types they own. Those integrations create a dependency **from the external library toward `EDP-BoundedTypes`**; they do not create a dependency from `EDP-BoundedTypes` back to the external library.

Examples include future Arduino, ESP-IDF, FreeRTOS, or third-party Type adapters.
