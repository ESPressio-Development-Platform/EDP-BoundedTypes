# Sequence Containers

## Purpose

The sequence family provides bounded, allocation-free runtime collections whose complete maximum owned storage is reserved inline.

The family is:

```text
Vector<T,N>
Deque<T,N>
Queue<T,N>
Stack<T,N>
CircularBuffer<T,N>
```

Every contained Type must be a certified memory-bounded value and nothrow-destructible.

Additional construction/move/copy requirements are imposed only by operations that need them.

---

## Raw Slot Lifetime Model

Generic containers reserve aligned raw storage for `N` possible values.

Only the active logical elements are constructed.

This means:

```text
unused capacity consumes reserved bytes
unused capacity does not create live T objects
T does not need to be default-constructible merely to be stored
```

Construction uses placement construction into a slot and destruction explicitly ends the active object's lifetime.

Arbitrary non-trivial values are never relocated with raw `memcpy` or `memmove`.

---

## Vector<T,N>

`Vector<T,N>` is a bounded indexed variable-length sequence.

It provides operations including:

```text
Assign
PushBack
EmplaceBack
Insert
Emplace
Erase
PopBack
Resize
Clear
```

Unchecked indexed access is available through `operator[]`; checked access uses `GetAt`.

---

## Why Vector Does Not Expose Data()

Individually placement-constructed non-trivial objects in adjacent raw slots are not represented by this implementation as one standard `T[]` array suitable for arbitrary pointer arithmetic.

Therefore generic `Vector<T,N>` deliberately does **not** promise:

```text
std::vector ABI equivalence
contiguous_iterator semantics
generic T* Data()
```

Instead it exposes logical random-access iterators.

This preserves the more important design rule:

```text
reserve all maximum storage
construct only active values
```

---

## Vector Insertion and Erasure

Middle insertion shifts active objects using nothrow move construction into newly available slots, rather than requiring every Type to support move assignment.

Erasure closes the gap using the same lifetime-aware relocation model.

Consequently, operation availability is determined by the exact element operations required.

A Type may be valid for `PushBack` while an unsupported throwing relocation capability makes middle `Insert` unavailable.

---

## Resize

`Resize(newSize)` is available when `T` is nothrow default-constructible.

`Resize(newSize, fillValue)` is available when `T` is nothrow copy-constructible.

Shrinking destroys removed elements.

Growing constructs every newly active element.

If `newSize > Capacity()`, the operation reports `CapacityExceeded` and leaves the Vector unchanged.

---

## Vector Invalidation

Because storage never reallocates, end growth does not invalidate existing element references.

The logical invalidation model is:

```text
PushBack / EmplaceBack
    existing element references remain valid
    previous end() invalidated

PopBack
    removed element invalidated
    earlier elements remain valid

Insert
    elements before insertion point remain valid
    insertion point and later logical positions invalidated

Erase
    elements before erased range remain valid
    erased range and later logical positions invalidated

Clear / assignment / destination move
    all destination element references invalidated
```

---

## Deque<T,N>

`Deque<T,N>` uses bounded inline ring storage.

Its core mutation surface is:

```text
PushFront
PushBack
EmplaceFront
EmplaceBack
PopFront
PopBack
Clear
```

Logical indexing always runs from front at index 0 to back at index `Size() - 1`.

Arbitrary middle insertion/erasure is intentionally not part of v1.

Custom logical iterators traverse front-to-back across physical ring wrap.

Structural mutation conservatively invalidates Deque iterators.

---

## Queue<T,N>

`Queue<T,N>` is a FIFO abstraction over bounded ring storage.

Its public vocabulary is intentionally narrower than Deque:

```text
Enqueue
Emplace
Dequeue
Front
GetFront
Clear
```

It does not expose arbitrary indexing merely because its implementation can reuse ring machinery.

---

## Stack<T,N>

`Stack<T,N>` is a LIFO abstraction.

Its core surface is:

```text
Push
Emplace
Pop
Top
GetTop
Clear
```

It intentionally hides arbitrary Vector-style sequence operations.

---

## CircularBuffer<T,N>

`CircularBuffer<T,N>` uses ring storage but separates reject-on-full and overwrite semantics explicitly.

Normal `Push()` returns a capacity failure when full.

Explicit `PushOverwrite()` replaces the oldest value when full.

The destructive behavior is therefore never hidden behind a normal `Push()`.

For `CircularBuffer<T,0>`, both write forms report non-success and no value is discarded.

---

## Iterators

Vector, Deque, and CircularBuffer use logical iterators that retain an owner and logical index.

They provide standard random-access iterator behavior where appropriate, but they do not claim contiguous-iterator semantics.

Ordering/subtraction of iterators from different owners is outside the valid precondition, matching ordinary container expectations.

---

## Copy and Move

Container copy operations exist only where the required contained-Type copy operations are nothrow.

Move operations exist only where the required move operations are nothrow.

After a successful move, the source is deterministically empty.

---

## Comparisons

Vector supports lexicographical logical-value comparison where the contained Type provides the necessary nothrow comparison operations.

Deque provides logical sequence equality/inequality.

Capacity and inactive slots never participate in logical-value equality.
