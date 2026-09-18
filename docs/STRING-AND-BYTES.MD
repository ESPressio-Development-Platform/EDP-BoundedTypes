# String and Bytes

## Purpose

`String<N>` and `Bytes<N>` provide variable-length bounded scalar-sequence values with fully inline storage.

They share the same memory principle:

```text
logical length varies at runtime
maximum owned storage does not
```

They differ in value semantics: String is text-oriented and forbids embedded null bytes, while Bytes accepts every byte value.

---

## String<N>

`String<N>` stores at most `N` logical payload bytes.

Its actual character storage is:

```text
N payload bytes
+
1 permanently reserved null terminator byte
```

Therefore `String<255>` reserves 256 character bytes: 255 payload bytes and one implicit null terminator.

`Capacity()` and `Size()` count payload bytes only. The implicit terminator is excluded.

---

## String Invariants

For every valid `String<N>`:

```text
0 <= Size() <= Capacity()
storage[Size()] == '\0'
no logical payload byte is '\0'
no heap allocation occurs
```

Embedded null characters are rejected because the Type represents text-oriented byte storage.

Arbitrary binary data belongs in `Bytes<N>`.

---

## Encoding

`String<N>` is encoding-agnostic.

It does not guarantee UTF-8 validity and it does not count Unicode code points.

`N` always means maximum payload bytes.

Encoding validity belongs to a higher semantic/schema/domain validator.

---

## String Assignment and Mutation

Core operations include:

```text
Assign
Append
Insert
Erase
SetAt
PushBack
PopBack
Clear
```

Runtime-fallible operations return operation-specific strongly typed result enums such as:

```text
StringAssignmentResult
StringAppendResult
StringInsertResult
StringEraseResult
StringSetAtResult
StringPushBackResult
StringPopBackResult
```

The library never silently truncates.

Example:

```cpp
ESPressio::Bounded::String<16> value;

const auto result = value.Assign("hello");

if (result != ESPressio::Bounded::StringAssignmentResult::Succeeded) {
    // Handle the explicit non-success outcome.
}
```

---

## String Input Forms

The checked input surface includes:

```text
Bounded::String<M>
std::string
std::string_view
fixed character arrays
const char* + explicit length
```

An arbitrary `const char*` overload that performs an unbounded `strlen` scan is deliberately absent.

For pointer + explicit length:

```text
nullptr + length 0
    valid empty input

nullptr + length > 0
    NullSourcePointer
```

Fixed character arrays are validated for a final null terminator, no earlier embedded null, and capacity compatibility.

---

## String Self-Aliasing

String source-based mutation is designed to remain valid when the source aliases the destination's own active payload.

For example:

```cpp
value.Append(
    value.View()
);
```

is supported when capacity permits.

The implementation uses overlap-safe movement rather than allocating a full scratch buffer.

---

## String Access

Read-only raw access is available through:

```cpp
Data()
CStr()
View()
operator[] const
begin() const
end() const
cbegin()
cend()
```

Mutable `char& operator[]` is deliberately absent because a caller could otherwise write `'\0'` into the active payload and violate the Type invariant.

Checked replacement uses `SetAt(index, character)`.

---

## String Operators

String supports logical-value comparison across different capacities:

```text
==
!=
<
<=
>
>=
```

Comparison uses only logical payload length and bytes. Unused capacity and the implicit terminator do not participate.

Same-capacity and compile-time-safe widening assignment are supported.

Potentially narrowing bounded assignment uses `Assign()` because it can fail at runtime.

The following are intentionally absent:

```text
mutable operator[]
operator+=
operator+
implicit const char* conversion
implicit std::string conversion
implicit platform-Type conversion
operator bool
```

---

## Bytes<N>

`Bytes<N>` stores between zero and `N` arbitrary bytes.

Its storage model is:

```text
N bytes inline
no terminator
every byte value valid
no heap allocation
```

For a semantically exact-size binary object, `std::array<std::uint8_t, N>` is usually the clearer Type.

Use `Bytes<N>` when logical binary length varies between `0..N`.

---

## Bytes Mutation

Core operations include:

```text
Assign
Append
Insert
Erase
PushBack
PopBack
Resize
Clear
```

Growing `Resize()` initializes every newly active byte. It never exposes stale inactive storage.

---

## Bytes Access

Because every byte value is valid, Bytes may safely expose mutable active storage through `Data()`, `operator[]`, and mutable iterators.

Writing through these accessors cannot alter logical `Size()`.

---

## Standard C++ Interoperability

The v1 direct source surface is deliberately narrow.

String:

```text
std::string
std::string_view
character array
pointer + explicit length
Bounded::String<M>
```

Bytes:

```text
std::array<std::uint8_t,N>
std::vector<std::uint8_t>
pointer + explicit length
Bounded::Bytes<M>
```

Dynamic source storage is accepted only as input. The bounded destination retains no dynamic ownership.

---

## External Conversions

Platform- or framework-specific conversions are not implemented inside this repository.

Both String and Bytes expose the generic `CastTo<T>()` / `CastFrom<T>()` mechanism backed by `TypeConversionAdapter`.

The external Type's owning library supplies the specialization.

For example, `EDP-Platform-Arduino` owns the Arduino `String` integration.

See [Traits and External Conversion](./TRAITS-AND-CONVERSION.MD).
