# EDP-BoundedTypes Demos

Each demo is provided in the three build surfaces required by the active ESPressio demo policy:

- `Arduino_IDE`
- `PlatformIO_Arduino`
- `PlatformIO_ESP-IDF`

## Demos

- `string-and-bytes` — bounded text and binary payload mutation.
- `sequence-containers` — Vector, Deque, Queue, Stack, and CircularBuffer.
- `associative-containers` — Set uniqueness and Map key/value behavior.
- `traits-and-capacity` — compiler-backed memory-boundedness and capacity metadata.

The PlatformIO projects consume the repository root through `lib_extra_dirs = ../../..`, so they are buildable directly from a clone of this repository.
