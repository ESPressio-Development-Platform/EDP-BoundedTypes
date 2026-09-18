#pragma once

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

#include "CapacityTraits.hpp"
#include "MemoryBoundedTraits.hpp"
#include "Vector.hpp"

namespace ESPressio::Bounded {

    /// Describes the outcome of inserting a new unique key/value entry into a bounded Map.
    enum class MapInsertResult : std::uint8_t {
        Succeeded = 0,
        CapacityExceeded = 1,
        DuplicateKey = 2
    };

    /// Describes the outcome of inserting a new entry or replacing the mapped value for an existing key.
    enum class MapInsertOrAssignResult : std::uint8_t {
        Succeeded = 0,
        CapacityExceeded = 1
    };

    /// Describes the outcome of erasing an entry from a bounded Map.
    enum class MapEraseResult : std::uint8_t {
        Succeeded = 0,
        NotFound = 1
    };

    /// Describes the outcome of locating a mapped value by key.
    enum class MapFindResult : std::uint8_t {
        Succeeded = 0,
        NotFound = 1
    };

    /// Provides a non-owning mutable view over one bounded Map entry without exposing key mutation.
    template<class TKey, class TValue>
    class MapEntryView final {

    private:

        // Referenced entry state.

        /// References the immutable key owned by the Map.
        const TKey* _key = nullptr;

        /// References the mutable mapped value owned by the Map.
        TValue* _value = nullptr;

    public:

        /// Creates a view over an existing key/value entry.
        MapEntryView(
            const TKey& key,
            TValue& value
        ) noexcept
            : _key(
                std::addressof(
                    key
                )
            ),
              _value(
                std::addressof(
                    value
                )
            ) {
        }

        /// Returns a read-only reference to the entry key.
        const TKey& Key() const noexcept {
            return *_key;
        }

        /// Returns a mutable reference to the mapped value.
        TValue& Value() const noexcept {
            return *_value;
        }

    };

    /// Provides a non-owning read-only view over one bounded Map entry.
    template<class TKey, class TValue>
    class MapConstEntryView final {

    private:

        // Referenced entry state.

        /// References the immutable key owned by the Map.
        const TKey* _key = nullptr;

        /// References the immutable mapped value owned by the Map.
        const TValue* _value = nullptr;

    public:

        /// Creates a read-only view over an existing key/value entry.
        MapConstEntryView(
            const TKey& key,
            const TValue& value
        ) noexcept
            : _key(
                std::addressof(
                    key
                )
            ),
              _value(
                std::addressof(
                    value
                )
            ) {
        }

        /// Returns a read-only reference to the entry key.
        const TKey& Key() const noexcept {
            return *_key;
        }

        /// Returns a read-only reference to the mapped value.
        const TValue& Value() const noexcept {
            return *_value;
        }

    };

    namespace Detail {

        /// Stores one internally-owned key/value pair for bounded Map storage.
        template<class TKey, class TValue>
        class MapStorageEntry final {

        private:

            // Owned entry state.

            /// Stores the immutable-by-public-contract key used for Map lookup.
            TKey _key;

            /// Stores the mutable mapped value associated with the key.
            TValue _value;

        public:

            /// Constructs an entry by copying both key and mapped value.
            MapStorageEntry(
                const TKey& key,
                const TValue& value
            ) noexcept(
                std::is_nothrow_copy_constructible_v<TKey> &&
                std::is_nothrow_copy_constructible_v<TValue>
            )
                : _key(key),
                  _value(value) {
            }

            /// Constructs an entry by moving both key and mapped value.
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
                  _value(
                    std::move(
                        value
                    )
                ) {
            }

            /// Constructs an entry by copying its key and constructing its mapped value in place.
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
                  _value(
                    std::forward<TValueArgumentTypes>(arguments)...
                ) {
            }

            /// Copies an internal Map storage entry.
            MapStorageEntry(
                const MapStorageEntry& source
            ) noexcept(
                std::is_nothrow_copy_constructible_v<TKey> &&
                std::is_nothrow_copy_constructible_v<TValue>
            ) = default;

            /// Moves an internal Map storage entry.
            MapStorageEntry(
                MapStorageEntry&& source
            ) noexcept(
                std::is_nothrow_move_constructible_v<TKey> &&
                std::is_nothrow_move_constructible_v<TValue>
            ) = default;

            /// Copies an internal Map storage entry when both owned Types support nothrow copy assignment.
            MapStorageEntry& operator=(
                const MapStorageEntry& source
            ) noexcept(
                std::is_nothrow_copy_assignable_v<TKey> &&
                std::is_nothrow_copy_assignable_v<TValue>
            ) = default;

            /// Moves an internal Map storage entry when both owned Types support nothrow move assignment.
            MapStorageEntry& operator=(
                MapStorageEntry&& source
            ) noexcept(
                std::is_nothrow_move_assignable_v<TKey> &&
                std::is_nothrow_move_assignable_v<TValue>
            ) = default;

            /// Returns a read-only reference to the stored key.
            const TKey& Key() const noexcept {
                return _key;
            }

            /// Returns a mutable reference to the stored mapped value.
            TValue& Value() noexcept {
                return _value;
            }

            /// Returns a read-only reference to the stored mapped value.
            const TValue& Value() const noexcept {
                return _value;
            }

        };

        /// Provides forward iteration over bounded Map entries while exposing only safe key/value views.
        template<class TOwner, class TKey, class TValue, bool TIsConst>
        class MapIterator final {

        private:

            /// Allows mutable Map iterators to be converted to read-only Map iterators.
            template<class, class, class, bool>
            friend class MapIterator;

            /// Selects the owner pointer Type according to iterator constness.
            using OwnerPointer = std::conditional_t<
                TIsConst,
                const TOwner*,
                TOwner*
            >;

            // Iterator state.

            /// Identifies the bounded Map being traversed.
            OwnerPointer _owner = nullptr;

            /// Identifies the current insertion-order entry index.
            std::size_t _index = 0U;

        public:

            /// Identifies this iterator as a forward iterator under C++17 conventions.
            using iterator_category = std::forward_iterator_tag;

            /// Selects the entry view Type according to iterator constness.
            using value_type = std::conditional_t<
                TIsConst,
                MapConstEntryView<TKey, TValue>,
                MapEntryView<TKey, TValue>
            >;

            /// Identifies the signed difference Type used by standard iterator machinery.
            using difference_type = std::ptrdiff_t;

            /// Entry views are proxy values rather than pointers to mutable key/value storage.
            using pointer = void;

            /// Entry views are returned by value so key mutation cannot escape through iteration.
            using reference = value_type;

            /// Creates a default iterator that is not associated with a Map.
            constexpr MapIterator() noexcept = default;

            /// Creates an iterator at an insertion-order index in the supplied Map.
            constexpr MapIterator(
                OwnerPointer owner,
                std::size_t index
            ) noexcept
                : _owner(owner),
                  _index(index) {
            }

            /// Converts a mutable Map iterator to a read-only iterator.
            template<bool TOtherIsConst, std::enable_if_t<TIsConst && !TOtherIsConst, int> = 0>
            constexpr MapIterator(
                const MapIterator<TOwner, TKey, TValue, TOtherIsConst>& other
            ) noexcept
                : _owner(other._owner),
                  _index(other._index) {
            }

            /// Returns a safe entry view for the current insertion-order position.
            reference operator*() const noexcept {
                return _owner->EntryAtUnchecked(_index);
            }

            /// Advances to the next insertion-order entry.
            MapIterator& operator++() noexcept {
                ++_index;
                return *this;
            }

            /// Returns the current iterator and then advances to the next entry.
            MapIterator operator++(
                int
            ) noexcept {
                auto previous = *this;
                ++(
                    *this
                );
                return previous;
            }

            /// Reports whether two iterators identify the same Map and insertion-order position.
            bool operator==(
                const MapIterator& other
            ) const noexcept {
                return _owner == other._owner &&
                    _index == other._index;
            }

            /// Reports whether two iterators identify different Maps or insertion-order positions.
            bool operator!=(
                const MapIterator& other
            ) const noexcept {
                return !(
                    *this ==
                    other
                );
            }

        };

    } // ESPressio::Bounded::Detail

    /// Classifies mutable Map entry views as explicitly non-owning views.
    template<class TKey, class TValue>
    struct MemoryBoundedTraits<MapEntryView<TKey, TValue>> {

        /// A Map entry view references state owned by its Map rather than owning that state.
        static constexpr MemoryBoundedCategory Category = MemoryBoundedCategory::NonOwningView;

        /// A Map entry view depends on the lifetime of its owning Map.
        static constexpr bool HasExternalLifetimeDependencies = true;

    };

    /// Classifies read-only Map entry views as explicitly non-owning views.
    template<class TKey, class TValue>
    struct MemoryBoundedTraits<MapConstEntryView<TKey, TValue>> {

        /// A read-only Map entry view references state owned by its Map rather than owning that state.
        static constexpr MemoryBoundedCategory Category = MemoryBoundedCategory::NonOwningView;

        /// A read-only Map entry view depends on the lifetime of its owning Map.
        static constexpr bool HasExternalLifetimeDependencies = true;

    };

    /// Certifies internal Map storage entries when both owned key and mapped value Types are memory-bounded.
    template<class TKey, class TValue>
    struct MemoryBoundedTraits<Detail::MapStorageEntry<TKey, TValue>> : MemoryBoundedValueDeclaration<
        false,
        TKey,
        TValue
    > {
    };

    /// Stores unique key/value mappings in deterministic insertion order using bounded inline storage and linear lookup.
    template<class TKey, class TValue, std::size_t TCapacity>
    class Map final {

    private:

        /// Allows Map iterator specializations to request safe entry views without exposing internal storage entries publicly.
        template<class, class, class, bool>
        friend class Detail::MapIterator;

        /// Identifies the internal recursively bounded key/value storage Type.
        using StorageEntry = Detail::MapStorageEntry<TKey, TValue>;

        // Underlying bounded storage.

        /// Stores internal key/value entries in insertion order.
        Vector<StorageEntry, TCapacity> _entries{};

        // Lookup helpers.

        /// Locates a key and returns its insertion-order index.
        bool FindIndex(
            const TKey& key,
            std::size_t& index
        ) const noexcept {
            static_assert(
                noexcept(
                    std::declval<const TKey&>() ==
                    std::declval<const TKey&>()
                ),
                "Map key lookup requires TKey equality comparison to be noexcept."
            );

            for (std::size_t candidate = 0U; candidate < _entries.Size(); ++candidate)
                if (_entries[candidate].Key() == key) {
                    index = candidate;
                    return true;
                }

            index = 0U;
            return false;
        }

        /// Returns a mutable safe entry view at a valid insertion-order index.
        MapEntryView<TKey, TValue> EntryAtUnchecked(
            std::size_t index
        ) noexcept {
            return MapEntryView<TKey, TValue>(
                _entries[index].Key(),
                _entries[index].Value()
            );
        }

        /// Returns a read-only safe entry view at a valid insertion-order index.
        MapConstEntryView<TKey, TValue> EntryAtUnchecked(
            std::size_t index
        ) const noexcept {
            return MapConstEntryView<TKey, TValue>(
                _entries[index].Key(),
                _entries[index].Value()
            );
        }

    public:

        static_assert(
            IsMemoryBoundedValue<TKey>,
            "Bounded::Map key Type must be a certified memory-bounded value."
        );

        static_assert(
            IsMemoryBoundedValue<TValue>,
            "Bounded::Map mapped Type must be a certified memory-bounded value."
        );

        static_assert(
            std::is_nothrow_destructible_v<TKey>,
            "Bounded::Map key Type must be nothrow-destructible."
        );

        static_assert(
            std::is_nothrow_destructible_v<TValue>,
            "Bounded::Map mapped Type must be nothrow-destructible."
        );

        /// Provides mutable mapped-value iteration while preserving read-only key access.
        using Iterator = Detail::MapIterator<
            Map<TKey, TValue, TCapacity>,
            TKey,
            TValue,
            false
        >;

        /// Provides read-only key/value iteration.
        using ConstIterator = Detail::MapIterator<
            Map<TKey, TValue, TCapacity>,
            TKey,
            TValue,
            true
        >;

        /// Creates an empty bounded Map.
        constexpr Map() noexcept = default;

        /// Copies a bounded Map.
        Map(
            const Map& source
        ) noexcept = default;

        /// Moves a bounded Map and leaves the source empty.
        Map(
            Map&& source
        ) noexcept = default;

        /// Copies a bounded Map.
        Map& operator=(
            const Map& source
        ) noexcept = default;

        /// Moves a bounded Map and leaves the source empty.
        Map& operator=(
            Map&& source
        ) noexcept = default;

        // Capacity and state inspection.

        /// Returns the maximum number of unique key/value entries this Map can retain.
        static constexpr std::size_t Capacity() noexcept {
            return TCapacity;
        }

        /// Returns the current number of logical mappings.
        constexpr std::size_t Size() const noexcept {
            return _entries.Size();
        }

        /// Reports whether this Map currently contains no mappings.
        constexpr bool IsEmpty() const noexcept {
            return _entries.IsEmpty();
        }

        /// Reports whether this Map currently occupies every available entry slot.
        constexpr bool IsFull() const noexcept {
            return _entries.IsFull();
        }

        // Lookup.

        /// Reports whether this Map contains the supplied key.
        bool ContainsKey(
            const TKey& key
        ) const noexcept {
            std::size_t index = 0U;
            return FindIndex(
                key,
                index
            );
        }

        /// Locates a mapped value and returns a mutable pointer to it.
        MapFindResult Find(
            const TKey& key,
            TValue*& value
        ) noexcept {
            std::size_t index = 0U;

            if (!FindIndex(
                key,
                index
            )) {
                value = nullptr;
                return MapFindResult::NotFound;
            }

            value = std::addressof(
                _entries[index].Value()
            );
            return MapFindResult::Succeeded;
        }

        /// Locates a mapped value and returns a read-only pointer to it.
        MapFindResult Find(
            const TKey& key,
            const TValue*& value
        ) const noexcept {
            std::size_t index = 0U;

            if (!FindIndex(
                key,
                index
            )) {
                value = nullptr;
                return MapFindResult::NotFound;
            }

            value = std::addressof(
                _entries[index].Value()
            );
            return MapFindResult::Succeeded;
        }

        // Insertion.

        /// Copies a new unique key/value entry into insertion-order storage.
        MapInsertResult Insert(
            const TKey& key,
            const TValue& value
        ) noexcept {
            static_assert(
                std::is_nothrow_copy_constructible_v<TKey>,
                "Map::Insert copy requires TKey to be nothrow copy-constructible."
            );

            static_assert(
                std::is_nothrow_copy_constructible_v<TValue>,
                "Map::Insert copy requires TValue to be nothrow copy-constructible."
            );

            if (ContainsKey(key)) { return MapInsertResult::DuplicateKey; }

            if (_entries.EmplaceBack(
                key,
                value
            ) != VectorEmplaceBackResult::Succeeded) {
                return MapInsertResult::CapacityExceeded;
            }

            return MapInsertResult::Succeeded;
        }

        /// Moves a new unique key/value entry into insertion-order storage.
        MapInsertResult Insert(
            TKey&& key,
            TValue&& value
        ) noexcept {
            static_assert(
                std::is_nothrow_move_constructible_v<TKey>,
                "Map::Insert move requires TKey to be nothrow move-constructible."
            );

            static_assert(
                std::is_nothrow_move_constructible_v<TValue>,
                "Map::Insert move requires TValue to be nothrow move-constructible."
            );

            if (ContainsKey(key)) { return MapInsertResult::DuplicateKey; }

            if (_entries.EmplaceBack(
                std::move(
                    key
                ),
                std::move(
                    value
                )
            ) != VectorEmplaceBackResult::Succeeded) {
                return MapInsertResult::CapacityExceeded;
            }

            return MapInsertResult::Succeeded;
        }

        /// Copies a unique key and constructs its mapped value directly from supplied arguments.
        template<class... TValueArgumentTypes>
        MapInsertResult Emplace(
            const TKey& key,
            TValueArgumentTypes&&... arguments
        ) noexcept {
            static_assert(
                std::is_nothrow_copy_constructible_v<TKey>,
                "Map::Emplace requires TKey to be nothrow copy-constructible."
            );

            static_assert(
                std::is_nothrow_constructible_v<TValue, TValueArgumentTypes...>,
                "Map::Emplace requires TValue to be nothrow-constructible from the supplied arguments."
            );

            if (ContainsKey(key)) { return MapInsertResult::DuplicateKey; }

            if (_entries.EmplaceBack(
                key,
                std::in_place,
                std::forward<TValueArgumentTypes>(arguments)...
            ) != VectorEmplaceBackResult::Succeeded) {
                return MapInsertResult::CapacityExceeded;
            }

            return MapInsertResult::Succeeded;
        }

        // Insert-or-assign.

        /// Copies a mapped value onto an existing key or inserts a new copied entry when the key is absent.
        MapInsertOrAssignResult InsertOrAssign(
            const TKey& key,
            const TValue& value
        ) noexcept {
            static_assert(
                std::is_nothrow_copy_assignable_v<TValue>,
                "Map::InsertOrAssign copy requires TValue to be nothrow copy-assignable for existing keys."
            );

            TValue* existing = nullptr;

            if (Find(
                key,
                existing
            ) == MapFindResult::Succeeded) {
                *existing = value;
                return MapInsertOrAssignResult::Succeeded;
            }

            const auto insertResult = Insert(
                key,
                value
            );

            if (insertResult == MapInsertResult::CapacityExceeded) { return MapInsertOrAssignResult::CapacityExceeded; }

            return MapInsertOrAssignResult::Succeeded;
        }

        /// Moves a mapped value onto an existing key or inserts a new entry when the key is absent.
        MapInsertOrAssignResult InsertOrAssign(
            const TKey& key,
            TValue&& value
        ) noexcept {
            static_assert(
                std::is_nothrow_move_assignable_v<TValue>,
                "Map::InsertOrAssign move requires TValue to be nothrow move-assignable for existing keys."
            );

            TValue* existing = nullptr;

            if (Find(
                key,
                existing
            ) == MapFindResult::Succeeded) {
                *existing = std::move(
                    value
                );
                return MapInsertOrAssignResult::Succeeded;
            }

            static_assert(
                std::is_nothrow_copy_constructible_v<TKey>,
                "Map::InsertOrAssign insertion requires TKey to be nothrow copy-constructible."
            );

            static_assert(
                std::is_nothrow_move_constructible_v<TValue>,
                "Map::InsertOrAssign insertion requires TValue to be nothrow move-constructible."
            );

            if (_entries.EmplaceBack(
                key,
                std::move(
                    value
                )
            ) != VectorEmplaceBackResult::Succeeded) {
                return MapInsertOrAssignResult::CapacityExceeded;
            }

            return MapInsertOrAssignResult::Succeeded;
        }

        // Erase and clear.

        /// Erases a mapping by key while preserving insertion order for all remaining entries.
        MapEraseResult Erase(
            const TKey& key
        ) noexcept {
            std::size_t index = 0U;

            if (!FindIndex(
                key,
                index
            )) {
                return MapEraseResult::NotFound;
            }

            _entries.Erase(
                index,
                1U
            );
            return MapEraseResult::Succeeded;
        }

        /// Destroys all mappings and returns the Map to its empty state.
        void Clear() noexcept {
            _entries.Clear();
        }

        // Safe entry iteration.

        /// Returns a mutable mapped-value iterator to the earliest inserted retained entry.
        Iterator begin() noexcept {
            return Iterator(
                this,
                0U
            );
        }

        /// Returns a mutable mapped-value iterator one position beyond the latest retained entry.
        Iterator end() noexcept {
            return Iterator(
                this,
                Size()
            );
        }

        /// Returns a read-only iterator to the earliest inserted retained entry.
        ConstIterator begin() const noexcept {
            return ConstIterator(
                this,
                0U
            );
        }

        /// Returns a read-only iterator one position beyond the latest retained entry.
        ConstIterator end() const noexcept {
            return ConstIterator(
                this,
                Size()
            );
        }

        /// Returns a read-only iterator to the earliest inserted retained entry.
        ConstIterator cbegin() const noexcept {
            return begin();
        }

        /// Returns a read-only iterator one position beyond the latest retained entry.
        ConstIterator cend() const noexcept {
            return end();
        }

        // Semantic Map equality.

        /// Reports whether two Maps contain identical key/value mappings irrespective of insertion order.
        template<std::size_t TOtherCapacity>
        bool operator==(
            const Map<TKey, TValue, TOtherCapacity>& other
        ) const noexcept {
            static_assert(
                noexcept(
                    std::declval<const TValue&>() ==
                    std::declval<const TValue&>()
                ),
                "Map equality requires TValue equality comparison to be noexcept."
            );

            if (Size() != other.Size()) { return false; }

            for (std::size_t index = 0U; index < _entries.Size(); ++index) {
                const auto& entry = _entries[index];
                const TValue* otherValue = nullptr;

                if (other.Find(
                    entry.Key(),
                    otherValue
                ) != MapFindResult::Succeeded) {
                    return false;
                }

                if (!(
                    entry.Value() ==
                    *otherValue
                )) {
                    return false;
                }
            }

            return true;
        }

        /// Reports whether two Maps contain different logical mappings.
        template<std::size_t TOtherCapacity>
        bool operator!=(
            const Map<TKey, TValue, TOtherCapacity>& other
        ) const noexcept {
            return !(
                *this ==
                other
            );
        }

    };

    /// Certifies a bounded Map when both recursively owned key and mapped value Types are memory-bounded.
    template<class TKey, class TValue, std::size_t TCapacity>
    struct MemoryBoundedTraits<Map<TKey, TValue, TCapacity>> : MemoryBoundedValueDeclaration<
        false,
        TKey,
        TValue
    > {
    };

    /// Exposes the compile-time entry capacity of a bounded Map.
    template<class TKey, class TValue, std::size_t TCapacity>
    struct CapacityTraits<Map<TKey, TValue, TCapacity>> {

        /// Bounded Map capacity is known at compile time.
        static constexpr bool HasStaticCapacity = true;

        /// Bounded Map logical capacity is measured in unique key/value entries.
        static constexpr CapacityUnit Unit = CapacityUnit::Entries;

        /// Reports the maximum number of logical mappings retained by the Map.
        static constexpr std::size_t Capacity = TCapacity;

    };

} // ESPressio::Bounded
