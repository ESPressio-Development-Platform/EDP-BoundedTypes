# Private Implementation

Operations that require construction or relocation are only enabled when the required noexcept guarantees are available. Failed operations preserve destination logical state. Moved-from bounded values are deterministic empty states. Zero-capacity instantiations are valid and must remain well-defined.

Vector does not promise a contiguous `T[]`/Data() abstraction because raw slot representation and explicit object lifetime are part of the design.
