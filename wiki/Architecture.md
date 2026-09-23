# Architecture

The library separates bounded ownership from general container convenience. Strings, byte buffers, vectors, deques, queues, stacks, circular buffers, sets and maps own their complete maximum storage inline.

Memory boundedness is recursively certified through traits. Unknown user types are rejected until their owning library explicitly certifies their member graph. Generic containers use raw slots so only active elements have C++ object lifetime and element types need not be default constructible.
