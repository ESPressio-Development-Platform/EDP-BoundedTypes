# Resources, Lifecycle and Concurrency

All primary bounded values retain their maximum owned storage inline and perform no heap allocation. Capacity is a compile-time property. Complexity is intentionally simple and predictable; Set/Map use flat insertion-order storage and O(N) lookup. External lifetime dependencies, where supported, are separately identified from owned memory boundedness.
