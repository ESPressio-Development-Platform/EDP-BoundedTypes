# Dependency Contracts

EDP-BoundedTypes depends on **EDP-Memory** and **EDP-Platform-Portable**.

## EDP-Memory

String/Bytes byte manipulation is expressed through the EDP-Memory `ByteOperations` provider contract. `ByteOperationsPolicy<T>` validates the provider using `Memory::Detail::ByteOperationsProviderTraits`.

Because bounded scalar byte types retain no provider pointer/object, the selected policy provider must additionally be:

- empty/stateless;
- nothrow default constructible.

These are stronger local requirements than the general Memory ByteOperations provider contract.

## EDP-Platform-Portable

`Platform::Portable::Memory::ByteOperationsProvider` is selected as `DefaultByteOperationsProvider`. This dependency exists to provide the default stateless byte-operation policy; it is not a general runtime dependency on the Portable Platform implementation.

## Extension contract

External type owners may specialize `TypeConversionAdapter<Source,Target>`. Memory boundedness of custom owned types is only recognized when their owning library explicitly specializes/certifies the MemoryBounded traits.

## Resource implication

The core bounded containers own their maximum storage inline. No dependency may introduce hidden heap growth into those value/container contracts.

> Dependency contract audit baseline: `c0e88fd1e797247fe9b21de837c63c24d6fe7a50` (`main`).
