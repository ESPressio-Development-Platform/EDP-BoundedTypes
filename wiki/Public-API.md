# Public API

Principal public types are `String<N>`, `Bytes<N>`, `Vector<T,N>`, `Deque<T,N>`, `Queue<T,N>`, `Stack<T,N>`, `CircularBuffer<T,N>`, `Set<T,N>` and `Map<K,V,N>`, together with memory-boundedness/capacity traits and conversion adapters.

`String<N>` reserves N payload bytes plus a permanent terminator and forbids embedded NUL. `Bytes<N>` represents arbitrary bytes. Sequence and associative containers expose explicit capacity-failure results rather than hidden growth.

Map intentionally omits `operator[]` because insertion can fail. Set elements and Map keys are immutable through public views.

Exact declarations remain authoritative in the exported headers.
