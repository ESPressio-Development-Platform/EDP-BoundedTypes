# Memory-Boundedness Model

## Purpose

`EDP-BoundedTypes` exists to make one property mechanically visible:

> the maximum amount of memory **owned by a value** must be finite and knowable without relying on heap growth.

The library is deliberately concerned with owned runtime memory. It is not a serialization library, allocator, object registry, or ownership framework. Raw String/Bytes byte operations are consumed through the EDP-Memory ByteOperations abstraction rather than implemented as system-memory operations here.

---

## Whole-Object Boundedness

A Type is an owning memory-bounded value only when its complete owned object graph is bounded.

Examples:

```text
int
    bounded

std::array<int, 8>
    bounded

Bounded::String<32>
    bounded

Bounded::Vector<Bounded::String<16>, 8>
    bounded

std::string
    not bounded

std::vector<int>
    not bounded

std::unique_ptr<int>
    not bounded
```

The rule is recursive. A bounded outer collection cannot hide an unbounded inner owner.

Therefore:

```cpp
ESPressio::Bounded::Vector<int, 8> values;
ESPressio::Bounded::Vector<ESPressio::Bounded::String<32>, 8> names;
// ESPressio::Bounded::Vector<std::string, 8> invalid; // compile-time error
```

---

## Bounded Storage Is Preallocated

Capacity-bearing bounded Types reserve their complete maximum storage inline for their entire lifetime.

Logical growth therefore does not:

```text
allocate
reallocate
fragment the heap
change the object's owned memory footprint
```

For generic containers, reserving all storage does **not** mean constructing every possible element.

Instead:

```text
raw aligned slots for N elements exist for the entire container lifetime
only Size() active elements have C++ object lifetime
inactive slots remain raw storage
```

This allows contained Types to be non-default-constructible.

---

## ByteOperations and Object Footprint

`String` and `Bytes` select an EDP-Memory ByteOperations provider at compile time.

The default provider is the stateless Standard C/C++ concrete owned by `EDP-Platform-Portable`. No provider pointer or provider object is retained inside each bounded value.

The BoundedTypes integration accepts only stateless, nothrow-default-constructible ByteOperations providers. This preserves the existing deterministic object footprint while still allowing compile-time substitution.

This provider policy changes how raw bytes are moved/filled/compared; it does not change which memory the bounded value owns.

---

## Memory Boundedness vs Serialization Boundedness

Whole-object memory boundedness and serialization/schema boundedness are different properties.

A future Serializable Type may contain runtime-only members that are excluded from its schema:

```cpp
class Example final {

private:

    ESPressio::Bounded::String<32> _name;
    RuntimePolicy& _policy;

};
```

The schema may include only `_name`. That says nothing about whether the complete C++ object is suitable for ownership inside a bounded collection.

The bounded-memory question is always:

> What memory does this value own transitively at runtime?

Schema membership cannot be used to bypass that rule.

---

## Unknown Types Are Rejected

C++17 cannot inspect every private member of an arbitrary user-defined Type and prove that no member owns dynamic memory.

Therefore the default qualification is conservative:

```text
unknown user-defined Type
    -> Unqualified
```

The Type-owning library must explicitly certify a custom value before it may be recursively owned by a bounded collection.

See [Traits and External Conversion](./TRAITS-AND-CONVERSION.MD) for the certification API.

---

## External Lifetime Dependencies

Owned-memory boundedness is separate from self-contained lifetime.

A custom bounded value may borrow an external object:

```cpp
class ConfiguredValue final {

private:

    ESPressio::Bounded::String<16> _value;
    AssignmentPolicy& _policy;

};
```

If `_policy` is explicitly non-owning, the object can still have a finite owned footprint.

Its certification may therefore report:

```text
Category
    BoundedValue

HasExternalLifetimeDependencies
    true
```

This means the value is memory-bounded but requires the owning application to preserve the lifetime of its borrowed dependency.

Raw pointers, references, `std::string_view`, and `std::reference_wrapper` are not themselves accepted as owning bounded values.

---

## Contained-Type Requirements

Memory boundedness does not imply that every operation on a Type is safe for deterministic container mutation.

Every Type physically owned by a generic bounded container must be:

```text
certified memory-bounded
nothrow destructible
```

Additional requirements are imposed only by operations that need them.

Examples:

```text
PushBack(const T&)
    requires nothrow copy construction

PushBack(T&&)
    requires nothrow move construction

EmplaceBack(args...)
    requires nothrow construction from args

Insert / Erase
    require the nothrow relocation operations used by that container
```

If the required operation can throw, that API is rejected at compile time. It does not become a runtime failure enum.

This allows bounded Types to remain move-only, non-copyable, or non-default-constructible when the operations being used do not require those capabilities.

---

## Strong Failure Guarantee

Every operation that reports a non-success result preserves the destination's observable logical value.

Examples include:

```text
capacity exceeded
duplicate rejected
invalid index
invalid source
embedded null rejected
conversion failure
```

The implementation validates runtime-failable conditions before publication and relies on compile-time `noexcept` requirements for element construction/relocation.

---

## Deterministic Move State

Where move construction or move assignment is available:

```text
destination
    receives the previous logical value

source
    becomes empty
```

The moved-from state is not merely "valid but unspecified".

---

## Common Capacity State

Capacity-bearing Types expose:

```cpp
static constexpr std::size_t Capacity() noexcept;
constexpr std::size_t Size() const noexcept;
constexpr bool IsEmpty() const noexcept;
constexpr bool IsFull() const noexcept;
void Clear() noexcept;
```

Public lengths and indexes use `std::size_t`.

The stored runtime size counter is an implementation detail and uses the smallest unsigned integer capable of representing `0..N`.

---

## Zero Capacity

Zero-capacity Types are valid:

```text
String<0>
Bytes<0>
Vector<T,0>
Map<K,V,0>
```

Their invariant is:

```text
Capacity() == 0
Size() == 0
IsEmpty() == true
IsFull() == true
```

Any operation that attempts to add logical content returns its normal capacity failure.

---

## Allocation and Thread Safety

The library itself performs no dynamic allocation.

String/Bytes byte operations are delegated to a zero-state compile-time ByteOperations provider and do not add dynamic ownership or per-object provider state.

Accepting a dynamic standard-library value as an **input source** does not transfer that dynamic ownership into the bounded destination.

Bounded Types provide no internal synchronization. Normal C++ data-race rules apply. Thread/task ownership and synchronization belong to the consuming application or domain.

---

## Secure Erasure

`Clear()` resets logical state but does not claim optimizer-resistant secure memory wiping.

`SecureClear()` is deliberately absent because a security guarantee of that kind requires a separately verified security/platform primitive rather than an ordinary C++ memory write.
