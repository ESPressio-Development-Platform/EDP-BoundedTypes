# Associative Containers

## Purpose

`Set<T,N>` and `Map<K,V,N>` provide bounded associative storage without tree nodes, hash buckets, allocator state, or heap growth.

v1 deliberately uses:

```text
flat inline bounded storage
insertion-order iteration
linear lookup
```

This minimizes metadata and Type requirements for the small bounded capacities expected on constrained embedded targets.

If sorted or hashed bounded containers are required later, they should be separate Types with explicit semantics rather than silently changing Set/Map behavior.

---

## Set<T,N>

Set retains unique logical values.

Its primary operations are:

```text
Insert
Emplace
Erase
Find
Contains
Clear
```

Insertion reports success, capacity exhaustion, or duplicate detection through `SetInsertResult`.

Uniqueness is checked before publication, so a duplicate or capacity failure leaves the Set unchanged.

---

## Set Element Immutability

Set values are exposed read-only.

Allowing a mutable reference to a stored element could transform one key into another already-present value and silently violate uniqueness.

Therefore Set iteration is const-facing even on a mutable Set instance.

Mutation of membership occurs through Set operations rather than through an element reference.

---

## Set Ordering

Iteration preserves insertion order.

Erase preserves the relative order of the remaining values.

Lookup uses equality comparison and is linear in current logical size.

This representation is intentionally optimized for small bounded capacities, predictable memory, locality, minimal metadata, and simple deterministic behavior rather than asymptotic performance for large host-side collections.

---

## Set Equality

Set equality is semantic, not insertion-order equality.

Two Sets compare equal when their logical sizes match and every value in one Set is present in the other.

Therefore insertion order does not affect equality.

Ordering operators are intentionally absent because Set does not define a natural ordering contract.

---

## Map<K,V,N>

Map retains unique keys and mapped values.

Its primary operations are:

```text
Insert
Emplace
InsertOrAssign
Erase
Find
ContainsKey
Clear
```

Both key and mapped value Types must satisfy the bounded-memory and lifetime requirements.

---

## Key Immutability

Map exposes entries through a controlled view.

Conceptually:

```cpp
const K& Key() const;
V& Value();
const V& Value() const;
```

A caller may mutate the mapped value but cannot mutate the key through the public entry surface.

This prevents lookup/uniqueness invariants from being invalidated behind the container's back.

---

## Why Map Has No operator[]

Standard map-style `map[key]` may default-construct and insert a missing mapped value.

In a bounded container that insertion can fail because capacity is finite.

Hiding that failure behind `operator[]` would violate the library-wide operator rule.

Therefore v1 intentionally uses explicit operations:

```text
Find
Insert
InsertOrAssign
```

Each fallible mutation reports an operation-specific result enum.

---

## InsertOrAssign

`InsertOrAssign` has two logical paths.

Existing key:

```text
replace the mapped value using the required nothrow-safe operation
capacity does not change
```

Missing key:

```text
insert when capacity is available
otherwise report CapacityExceeded
```

Every non-success outcome preserves the original Map.

---

## Map Ordering

Iteration preserves insertion order.

Lookup uses linear key equality.

Erase preserves the relative order of remaining entries.

No comparator or hash policy object is retained in v1.

This keeps object storage and template policy surface minimal.

---

## Map Equality

Map equality is semantic rather than insertion-order equality.

Two Maps compare equal when their sizes match, their key sets match, and every matching key has an equal mapped value.

Different insertion orders therefore do not affect equality.

Ordering operators are intentionally absent.

---

## Capacity and Zero-Capacity Behavior

Set and Map report capacity in `Entries` through `CapacityTraits`.

`Set<T,0>` and `Map<K,V,0>` are valid empty Types.

They report:

```text
Capacity() == 0
Size() == 0
IsEmpty() == true
IsFull() == true
```

and insertion reports the normal capacity failure.

---

## Complexity

With insertion-order flat storage:

```text
Find / Contains
    O(N)

Insert duplicate check
    O(N)

Erase lookup + relocation
    O(N)

semantic Set/Map equality
    potentially O(N^2)
```

These costs are explicit tradeoffs for reduced metadata, no dynamic allocation, and small deterministic bounded capacities.
