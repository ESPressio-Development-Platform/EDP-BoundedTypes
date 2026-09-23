# src/bounded/Map.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `f5742d864310dc4fdecb734721205b0e21c38e40`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-BoundedTypes/blob/f5742d864310dc4fdecb734721205b0e21c38e40/src/bounded/Map.hpp)

## Direct includes

- `cstddef`
- `cstdint`
- `iterator`
- `memory`
- `type_traits`
- `utility`
- `CapacityTraits.hpp`
- `MemoryBoundedTraits.hpp`
- `Vector.hpp`

## Documented declarations

### `MapInsertResult`

**Classification:** PUBLIC API

Describes the outcome of inserting a new unique key/value entry into a bounded Map.

```cpp
enum class MapInsertResult : std::uint8_t
```

### `MapInsertOrAssignResult`

**Classification:** PUBLIC API

Describes the outcome of inserting a new entry or replacing the mapped value for an existing key.

```cpp
enum class MapInsertOrAssignResult : std::uint8_t
```

### `MapEraseResult`

**Classification:** PUBLIC API

Describes the outcome of erasing an entry from a bounded Map.

```cpp
enum class MapEraseResult : std::uint8_t
```

### `MapFindResult`

**Classification:** PUBLIC API

Describes the outcome of locating a mapped value by key.

```cpp
enum class MapFindResult : std::uint8_t
```

### `TKey`

**Classification:** PUBLIC API

Provides a non-owning mutable view over one bounded Map entry without exposing key mutation.

```cpp
template<class TKey, class TValue>
    class MapEntryView final
```

### `_key`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

References the immutable key owned by the Map.

```cpp
const TKey* _key = nullptr;
```

### `_value`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

References the mutable mapped value owned by the Map.

```cpp
TValue* _value = nullptr;
```

### `MapEntryView`

**Classification:** PUBLIC API · source access: `public`

Creates a view over an existing key/value entry.

```cpp
MapEntryView(
            const TKey& key,
            TValue& value
        ) noexcept
            : _key(
                std::addressof(
                    key
                )
            ),
```

### `Key`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only reference to the entry key.

```cpp
const TKey& Key() const noexcept
```

### `Value`

**Classification:** PUBLIC API · source access: `public`

Returns a mutable reference to the mapped value.

```cpp
TValue& Value() const noexcept
```

### `TKey`

**Classification:** PUBLIC API

Provides a non-owning read-only view over one bounded Map entry.

```cpp
template<class TKey, class TValue>
    class MapConstEntryView final
```

### `_key`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

References the immutable key owned by the Map.

```cpp
const TKey* _key = nullptr;
```

### `_value`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

References the immutable mapped value owned by the Map.

```cpp
const TValue* _value = nullptr;
```

### `MapConstEntryView`

**Classification:** PUBLIC API · source access: `public`

Creates a read-only view over an existing key/value entry.

```cpp
MapConstEntryView(
            const TKey& key,
            const TValue& value
        ) noexcept
            : _key(
                std::addressof(
                    key
                )
            ),
```

### `Key`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only reference to the entry key.

```cpp
const TKey& Key() const noexcept
```

### `Value`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only reference to the mapped value.

```cpp
const TValue& Value() const noexcept
```

### `TKey`

**Classification:** PUBLIC API

Stores one internally-owned key/value pair for bounded Map storage.

```cpp
template<class TKey, class TValue>
        class MapStorageEntry final
```

### `_key`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Stores the immutable-by-public-contract key used for Map lookup.

```cpp
TKey _key;
```

### `_value`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Stores the mutable mapped value associated with the key.

```cpp
TValue _value;
```

### `MapStorageEntry`

**Classification:** PUBLIC API · source access: `public`

Constructs an entry by copying both key and mapped value.

```cpp
MapStorageEntry(
                const TKey& key,
                const TValue& value
            ) noexcept(
                std::is_nothrow_copy_constructible_v<TKey> &&
                std::is_nothrow_copy_constructible_v<TValue>
            )
                : _key(key),
```

### `MapStorageEntry`

**Classification:** PUBLIC API · source access: `public`

Constructs an entry by moving both key and mapped value.

```cpp
MapStorageEntry(
                TKey&& key,
                TValue&& value
            ) noexcept(
                std::is_nothrow_move_constructible_v<TKey> &&
                std::is_nothrow_move_constructible_v<TValue>
            )
                : _key(
                    std::move(
                        key
                    )
                ),
```

### `MapStorageEntry`

**Classification:** PUBLIC API · source access: `public`

Constructs an entry by copying its key and constructing its mapped value in place.

```cpp
template<class... TValueArgumentTypes>
            MapStorageEntry(
                const TKey& key,
                std::in_place_t,
                TValueArgumentTypes&&... arguments
            ) noexcept(
                std::is_nothrow_copy_constructible_v<TKey> &&
                std::is_nothrow_constructible_v<TValue, TValueArgumentTypes...>
            )
                : _key(key),
```

### `MapStorageEntry`

**Classification:** PUBLIC API · source access: `public`

Copies an internal Map storage entry.

```cpp
MapStorageEntry(
                const MapStorageEntry& source
            ) noexcept(
                std::is_nothrow_copy_constructible_v<TKey> &&
                std::is_nothrow_copy_constructible_v<TValue>
            ) = default;
```

### `MapStorageEntry`

**Classification:** PUBLIC API · source access: `public`

Moves an internal Map storage entry.

```cpp
MapStorageEntry(
                MapStorageEntry&& source
            ) noexcept(
                std::is_nothrow_move_constructible_v<TKey> &&
                std::is_nothrow_move_constructible_v<TValue>
            ) = default;
```

### `noexcept`

**Classification:** PUBLIC API · source access: `public`

Copies an internal Map storage entry when both owned Types support nothrow copy assignment.

```cpp
MapStorageEntry& operator=(
                const MapStorageEntry& source
            ) noexcept(
                std::is_nothrow_copy_assignable_v<TKey> &&
                std::is_nothrow_copy_assignable_v<TValue>
            ) = default;
```

### `noexcept`

**Classification:** PUBLIC API · source access: `public`

Moves an internal Map storage entry when both owned Types support nothrow move assignment.

```cpp
MapStorageEntry& operator=(
                MapStorageEntry&& source
            ) noexcept(
                std::is_nothrow_move_assignable_v<TKey> &&
                std::is_nothrow_move_assignable_v<TValue>
            ) = default;
```

### `Key`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only reference to the stored key.

```cpp
const TKey& Key() const noexcept
```

### `Value`

**Classification:** PUBLIC API · source access: `public`

Returns a mutable reference to the stored mapped value.

```cpp
TValue& Value() noexcept
```

### `Value`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only reference to the stored mapped value.

```cpp
const TValue& Value() const noexcept
```

### `TOwner`

**Classification:** PUBLIC API

Provides forward iteration over bounded Map entries while exposing only safe key/value views.

```cpp
template<class TOwner, class TKey, class TValue, bool TIsConst>
        class MapIterator final
```

### `MapIterator`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Allows mutable Map iterators to be converted to read-only Map iterators.

```cpp
template<class, class, class, bool>
            friend class MapIterator;
```

### `OwnerPointer`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Selects the owner pointer Type according to iterator constness.

```cpp
using OwnerPointer = std::conditional_t<
                TIsConst,
```

### `_owner`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Identifies the bounded Map being traversed.

```cpp
OwnerPointer _owner = nullptr;
```

### `_index`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Identifies the current insertion-order entry index.

```cpp
std::size_t _index = 0U;
```

### `iterator_category`

**Classification:** PUBLIC API · source access: `public`

Identifies this iterator as a forward iterator under C++17 conventions.

```cpp
using iterator_category = std::forward_iterator_tag;
```

### `value_type`

**Classification:** PUBLIC API · source access: `public`

Selects the entry view Type according to iterator constness.

```cpp
using value_type = std::conditional_t<
                TIsConst,
```

### `difference_type`

**Classification:** PUBLIC API · source access: `public`

Identifies the signed difference Type used by standard iterator machinery.

```cpp
using difference_type = std::ptrdiff_t;
```

### `pointer`

**Classification:** PUBLIC API · source access: `public`

Entry views are proxy values rather than pointers to mutable key/value storage.

```cpp
using pointer = void;
```

### `reference`

**Classification:** PUBLIC API · source access: `public`

Entry views are returned by value so key mutation cannot escape through iteration.

```cpp
using reference = value_type;
```

### `MapIterator`

**Classification:** PUBLIC API · source access: `public`

Creates a default iterator that is not associated with a Map.

```cpp
constexpr MapIterator() noexcept = default;
```

### `MapIterator`

**Classification:** PUBLIC API · source access: `public`

Creates an iterator at an insertion-order index in the supplied Map.

```cpp
constexpr MapIterator(
                OwnerPointer owner,
                std::size_t index
            ) noexcept
                : _owner(owner),
```

### `MapIterator`

**Classification:** PUBLIC API · source access: `public`

Converts a mutable Map iterator to a read-only iterator.

```cpp
template<bool TOtherIsConst, std::enable_if_t<TIsConst && !TOtherIsConst, int> = 0>
            constexpr MapIterator(
                const MapIterator<TOwner, TKey, TValue, TOtherIsConst>& other
            ) noexcept
                : _owner(other._owner),
```

### `reference operator*() const noexcept`

**Classification:** PUBLIC API · source access: `public`

Returns a safe entry view for the current insertion-order position.

```cpp
reference operator*() const noexcept
```

### `MapIterator& operator++() noexcept`

**Classification:** PUBLIC API · source access: `public`

Advances to the next insertion-order entry.

```cpp
MapIterator& operator++() noexcept
```

### `MapIterator operator++( int ) noexcept`

**Classification:** PUBLIC API · source access: `public`

Returns the current iterator and then advances to the next entry.

```cpp
MapIterator operator++(
                int
            ) noexcept
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Reports whether two iterators identify the same Map and insertion-order position.

```cpp
bool operator==(
                const MapIterator& other
            ) const noexcept
```

### `bool operator!=( const MapIterator& other ) const noexcept`

**Classification:** PUBLIC API · source access: `public`

Reports whether two iterators identify different Maps or insertion-order positions.

```cpp
bool operator!=(
                const MapIterator& other
            ) const noexcept
```

### `TKey`

**Classification:** PUBLIC API

Classifies mutable Map entry views as explicitly non-owning views.

```cpp
template<class TKey, class TValue>
    struct MemoryBoundedTraits<MapEntryView<TKey, TValue>>
```

### `Category`

**Classification:** PUBLIC API · source access: `public`

A Map entry view references state owned by its Map rather than owning that state.

```cpp
static constexpr MemoryBoundedCategory Category = MemoryBoundedCategory::NonOwningView;
```

### `HasExternalLifetimeDependencies`

**Classification:** PUBLIC API · source access: `public`

A Map entry view depends on the lifetime of its owning Map.

```cpp
static constexpr bool HasExternalLifetimeDependencies = true;
```

### `TKey`

**Classification:** PUBLIC API

Classifies read-only Map entry views as explicitly non-owning views.

```cpp
template<class TKey, class TValue>
    struct MemoryBoundedTraits<MapConstEntryView<TKey, TValue>>
```

### `Category`

**Classification:** PUBLIC API · source access: `public`

A read-only Map entry view references state owned by its Map rather than owning that state.

```cpp
static constexpr MemoryBoundedCategory Category = MemoryBoundedCategory::NonOwningView;
```

### `HasExternalLifetimeDependencies`

**Classification:** PUBLIC API · source access: `public`

A read-only Map entry view depends on the lifetime of its owning Map.

```cpp
static constexpr bool HasExternalLifetimeDependencies = true;
```

### `TKey`

**Classification:** PUBLIC API

Certifies internal Map storage entries when both owned key and mapped value Types are memory-bounded.

```cpp
template<class TKey, class TValue>
    struct MemoryBoundedTraits<Detail::MapStorageEntry<TKey, TValue>> : MemoryBoundedValueDeclaration<
        false,
```

### `TKey`

**Classification:** PUBLIC API

Stores unique key/value mappings in deterministic insertion order using bounded inline storage and linear lookup.

```cpp
template<class TKey, class TValue, std::size_t TCapacity>
    class Map final
```

### `Detail`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Allows Map iterator specializations to request safe entry views without exposing internal storage entries publicly.

```cpp
template<class, class, class, bool>
        friend class Detail::MapIterator;
```

### `StorageEntry`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Identifies the internal recursively bounded key/value storage Type.

```cpp
using StorageEntry = Detail::MapStorageEntry<TKey, TValue>;
```

### `StorageEntry`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Stores internal key/value entries in insertion order.

```cpp
Vector<StorageEntry, TCapacity> _entries{};
```

### `FindIndex`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Locates a key and returns its insertion-order index.

```cpp
bool FindIndex(
            const TKey& key,
            std::size_t& index
        ) const noexcept
```

### `EntryAtUnchecked`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Returns a mutable safe entry view at a valid insertion-order index.

```cpp
MapEntryView<TKey, TValue> EntryAtUnchecked(
            std::size_t index
        ) noexcept
```

### `EntryAtUnchecked`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Returns a read-only safe entry view at a valid insertion-order index.

```cpp
MapConstEntryView<TKey, TValue> EntryAtUnchecked(
            std::size_t index
        ) const noexcept
```

### `Iterator`

**Classification:** PUBLIC API · source access: `public`

Provides mutable mapped-value iteration while preserving read-only key access.

```cpp
using Iterator = Detail::MapIterator<
            Map<TKey, TValue, TCapacity>,
```

### `ConstIterator`

**Classification:** PUBLIC API · source access: `public`

Provides read-only key/value iteration.

```cpp
using ConstIterator = Detail::MapIterator<
            Map<TKey, TValue, TCapacity>,
```

### `Map`

**Classification:** PUBLIC API · source access: `public`

Creates an empty bounded Map.

```cpp
constexpr Map() noexcept = default;
```

### `Map`

**Classification:** PUBLIC API · source access: `public`

Copies a bounded Map.

```cpp
Map(
            const Map& source
        ) noexcept = default;
```

### `Map`

**Classification:** PUBLIC API · source access: `public`

Moves a bounded Map and leaves the source empty.

```cpp
Map(
            Map&& source
        ) noexcept = default;
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Copies a bounded Map.

```cpp
Map& operator=(
            const Map& source
        ) noexcept = default;
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Moves a bounded Map and leaves the source empty.

```cpp
Map& operator=(
            Map&& source
        ) noexcept = default;
```

### `Capacity`

**Classification:** PUBLIC API · source access: `public`

Returns the maximum number of unique key/value entries this Map can retain.

```cpp
static constexpr std::size_t Capacity() noexcept
```

### `Size`

**Classification:** PUBLIC API · source access: `public`

Returns the current number of logical mappings.

```cpp
constexpr std::size_t Size() const noexcept
```

### `IsEmpty`

**Classification:** PUBLIC API · source access: `public`

Reports whether this Map currently contains no mappings.

```cpp
constexpr bool IsEmpty() const noexcept
```

### `IsFull`

**Classification:** PUBLIC API · source access: `public`

Reports whether this Map currently occupies every available entry slot.

```cpp
constexpr bool IsFull() const noexcept
```

### `ContainsKey`

**Classification:** PUBLIC API · source access: `public`

Reports whether this Map contains the supplied key.

```cpp
bool ContainsKey(
            const TKey& key
        ) const noexcept
```

### `Find`

**Classification:** PUBLIC API · source access: `public`

Locates a mapped value and returns a mutable pointer to it.

```cpp
MapFindResult Find(
            const TKey& key,
            TValue*& value
        ) noexcept
```

### `Find`

**Classification:** PUBLIC API · source access: `public`

Locates a mapped value and returns a read-only pointer to it.

```cpp
MapFindResult Find(
            const TKey& key,
            const TValue*& value
        ) const noexcept
```

### `Insert`

**Classification:** PUBLIC API · source access: `public`

Copies a new unique key/value entry into insertion-order storage.

```cpp
MapInsertResult Insert(
            const TKey& key,
            const TValue& value
        ) noexcept
```

### `Insert`

**Classification:** PUBLIC API · source access: `public`

Moves a new unique key/value entry into insertion-order storage.

```cpp
MapInsertResult Insert(
            TKey&& key,
            TValue&& value
        ) noexcept
```

### `Emplace`

**Classification:** PUBLIC API · source access: `public`

Copies a unique key and constructs its mapped value directly from supplied arguments.

```cpp
template<class... TValueArgumentTypes>
        MapInsertResult Emplace(
            const TKey& key,
            TValueArgumentTypes&&... arguments
        ) noexcept
```

### `InsertOrAssign`

**Classification:** PUBLIC API · source access: `public`

Copies a mapped value onto an existing key or inserts a new copied entry when the key is absent.

```cpp
MapInsertOrAssignResult InsertOrAssign(
            const TKey& key,
            const TValue& value
        ) noexcept
```

### `InsertOrAssign`

**Classification:** PUBLIC API · source access: `public`

Moves a mapped value onto an existing key or inserts a new entry when the key is absent.

```cpp
MapInsertOrAssignResult InsertOrAssign(
            const TKey& key,
            TValue&& value
        ) noexcept
```

### `Erase`

**Classification:** PUBLIC API · source access: `public`

Erases a mapping by key while preserving insertion order for all remaining entries.

```cpp
MapEraseResult Erase(
            const TKey& key
        ) noexcept
```

### `Clear`

**Classification:** PUBLIC API · source access: `public`

Destroys all mappings and returns the Map to its empty state.

```cpp
void Clear() noexcept
```

### `begin`

**Classification:** PUBLIC API · source access: `public`

Returns a mutable mapped-value iterator to the earliest inserted retained entry.

```cpp
Iterator begin() noexcept
```

### `end`

**Classification:** PUBLIC API · source access: `public`

Returns a mutable mapped-value iterator one position beyond the latest retained entry.

```cpp
Iterator end() noexcept
```

### `begin`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only iterator to the earliest inserted retained entry.

```cpp
ConstIterator begin() const noexcept
```

### `end`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only iterator one position beyond the latest retained entry.

```cpp
ConstIterator end() const noexcept
```

### `cbegin`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only iterator to the earliest inserted retained entry.

```cpp
ConstIterator cbegin() const noexcept
```

### `cend`

**Classification:** PUBLIC API · source access: `public`

Returns a read-only iterator one position beyond the latest retained entry.

```cpp
ConstIterator cend() const noexcept
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Reports whether two Maps contain identical key/value mappings irrespective of insertion order.

```cpp
template<std::size_t TOtherCapacity>
        bool operator==(
            const Map<TKey, TValue, TOtherCapacity>& other
        ) const noexcept
```

### `TKey`

**Classification:** PUBLIC API · source access: `public`

Reports whether two Maps contain different logical mappings.

```cpp
template<std::size_t TOtherCapacity>
        bool operator!=(
            const Map<TKey, TValue, TOtherCapacity>& other
        ) const noexcept
```

### `TKey`

**Classification:** PUBLIC API

Certifies a bounded Map when both recursively owned key and mapped value Types are memory-bounded.

```cpp
template<class TKey, class TValue, std::size_t TCapacity>
    struct MemoryBoundedTraits<Map<TKey, TValue, TCapacity>> : MemoryBoundedValueDeclaration<
        false,
```

### `TKey`

**Classification:** PUBLIC API

Exposes the compile-time entry capacity of a bounded Map.

```cpp
template<class TKey, class TValue, std::size_t TCapacity>
    struct CapacityTraits<Map<TKey, TValue, TCapacity>>
```

### `HasStaticCapacity`

**Classification:** PUBLIC API · source access: `public`

Bounded Map capacity is known at compile time.

```cpp
static constexpr bool HasStaticCapacity = true;
```

### `Unit`

**Classification:** PUBLIC API · source access: `public`

Bounded Map logical capacity is measured in unique key/value entries.

```cpp
static constexpr CapacityUnit Unit = CapacityUnit::Entries;
```

### `Capacity`

**Classification:** PUBLIC API · source access: `public`

Reports the maximum number of logical mappings retained by the Map.

```cpp
static constexpr std::size_t Capacity = TCapacity;
```

