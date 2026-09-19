# EDP-BoundedTypes Demos

Each demo is provided in the three build surfaces required by the active ESPressio demo policy:

- `Arduino_IDE`
- `PlatformIO_Arduino`
- `PlatformIO_ESP-IDF`

## Demos

- `string-and-bytes` — bounded text/binary mutation and explicit compile-time ByteOperations provider selection.
- `sequence-containers` — Vector, Deque, Queue, Stack, and CircularBuffer.
- `associative-containers` — Set uniqueness and Map key/value behavior.
- `traits-and-capacity` — compiler-backed memory-boundedness and capacity metadata.

The String/Bytes demo explicitly names the Portable ByteOperations provider so the abstraction-to-concrete binding is visible at the call site. Ordinary `String<N>` / `Bytes<N>` declarations use that same provider by default.

PlatformIO projects consume the repository root through `lib_extra_dirs = ../../..` and declare the ESPressio Memory/Portable dependency stack required by the default ByteOperations provider.
