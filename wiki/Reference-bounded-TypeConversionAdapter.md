# src/bounded/TypeConversionAdapter.hpp

**Primary classification:** PUBLIC EXTENSION / TRAIT API

**Source baseline:** current branch

[Open source](https://github.com/ESPressio-Development-Platform/EDP-BoundedTypes/blob/main/src/bounded/TypeConversionAdapter.hpp)

## Purpose

This header owns the dependency-inversion seam used when another library needs an explicit conversion involving an EDP bounded Type or another Type pair while keeping ownership of the external representation outside `EDP-BoundedTypes`.

Conversion availability and generic success interpretation are deliberately separate capabilities. Existing adapters remain conversion-available without being forced to adopt a shared result enum. An adapter that must be interpreted by generic code opts into the stronger capability by exposing `static constexpr bool IsSuccessful(ResultType) noexcept`.

## `UnsupportedTypeConversionResult`

**Classification:** PUBLIC EXTENSION / TRAIT API

Marker result Type used only by the unavailable primary adapter. It carries no runtime state and is never a shared conversion-failure vocabulary.

## `TypeConversionAdapter<TSource,TTarget>`

**Classification:** PUBLIC EXTENSION / TRAIT API

Primary unavailable conversion contract for one ordered source/target Type pair. The library owning an external/specific Type supplies the relevant specialization so dependency direction remains toward `EDP-BoundedTypes` rather than from it.

### Template parameters

- `TSource` — source semantic/representation Type presented for conversion.
- `TTarget` — destination Type to be populated by the conversion.

### `ResultType`

Operation-specific result Type. The primary unavailable adapter uses `UnsupportedTypeConversionResult`; each real integration owns its own strongly typed result vocabulary.

### `IsAvailable`

Compile-time fact stating whether conversion for the exact ordered Type pair is supplied. Generic success interpretation does not alter this fact, preserving source compatibility for existing adapters.

### `IsNoexcept`

Compile-time fact stating whether `Convert` is guaranteed not to throw. Higher-level contracts may require non-throwing conversion independently from ordinary conversion availability.

### `Convert(source,target)`

Performs the integration-owned conversion. Real adapters must preserve their documented destination-on-failure semantics; EDP bounded adapters use the platform-wide strong failure-preservation rule. The primary adapter deletes this operation.

### Optional stronger contract: `IsSuccessful(result)`

Adapters intended for generic success-aware consumers expose:

```cpp
static constexpr bool IsSuccessful(ResultType result) noexcept;
```

The adapter itself interprets its result vocabulary. No caller may infer success from enum names, integer values, Boolean conversion, or a library-wide result Type.

## `IsTypeConversionAvailable<TSource,TTarget>`

**Classification:** PUBLIC TRAIT API

Boolean variable template exposing only `TypeConversionAdapter<TSource,TTarget>::IsAvailable`. It intentionally remains independent of the stronger success-predicate capability.

## `HasTypeConversionSuccessPredicate<TSource,TTarget>`

**Classification:** PUBLIC TRAIT API

Boolean variable template reporting whether the exact ordered adapter is available and exposes a `bool`-returning, `noexcept` `IsSuccessful(ResultType)` operation. This is the qualification surface for generic consumers such as Serialisation strong-Type adaptation.

The trait does not attempt to infer whether an arbitrary result enum's named value means success. It also does not make a legacy adapter unavailable merely because that adapter lacks the stronger capability.

## `IsTypeConversionSuccessful<TSource,TTarget>(result)`

**Classification:** PUBLIC API

Delegates result interpretation to the adapter-owned `IsSuccessful` predicate. It is `constexpr`/`noexcept` when the stronger contract is present and produces a focused compile-time diagnostic when a caller attempts generic interpretation for an adapter that has not opted in.

The function retains no state, allocates no memory, and does not mutate either conversion operand.

## Ownership and dependency invariants

- The Type-owning integration library owns specializations involving its external Type.
- `EDP-BoundedTypes` does not depend on Arduino, ESP-IDF, FreeRTOS or third-party representations to support their conversions.
- Result vocabulary remains operation/integration specific.
- Generic success interpretation is opt-in and does not redefine ordinary conversion availability.
- This header introduces no allocation, runtime registry, synchronization, or lifetime ownership.
