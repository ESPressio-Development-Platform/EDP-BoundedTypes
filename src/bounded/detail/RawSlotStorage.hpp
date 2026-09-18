#pragma once

#include <array>
#include <cstddef>
#include <new>
#include <type_traits>
#include <utility>

namespace ESPressio::Bounded::Detail {

    /// Reserves compile-time aligned inline slots and manages explicit object lifetime without owning container semantics.
    template<class TValue, std::size_t TCapacity>
    class RawSlotStorage final {

    private:

        /// Defines one correctly aligned raw slot capable of hosting a TValue object.
        using Slot = std::aligned_storage_t<
            sizeof(TValue),
            alignof(TValue)
        >;

        // Raw storage.

        /// Reserves exactly TCapacity raw slots without constructing TValue instances.
        std::array<Slot, TCapacity> _slots{};

    public:

        /// Constructs a TValue object in a raw slot using only nothrow construction paths.
        template<class... TArgumentTypes>
        TValue& Construct(
            std::size_t index,
            TArgumentTypes&&... arguments
        ) noexcept {
            static_assert(
                std::is_nothrow_constructible_v<TValue, TArgumentTypes...>,
                "RawSlotStorage construction requires TValue to be nothrow-constructible from the supplied arguments."
            );

            auto* value = ::new (
                static_cast<void*>(
                    &_slots[index]
                )
            ) TValue(
                std::forward<TArgumentTypes>(arguments)...
            );

            return *value;
        }

        /// Explicitly destroys the live TValue object in one slot.
        void Destroy(
            std::size_t index
        ) noexcept {
            static_assert(
                std::is_nothrow_destructible_v<TValue>,
                "RawSlotStorage destruction requires TValue to be nothrow-destructible."
            );

            Get(index).~TValue();
        }

        /// Returns a mutable reference to the already-live TValue object in one slot.
        TValue& Get(
            std::size_t index
        ) noexcept {
            return *std::launder(
                reinterpret_cast<TValue*>(
                    &_slots[index]
                )
            );
        }

        /// Returns a read-only reference to the already-live TValue object in one slot.
        const TValue& Get(
            std::size_t index
        ) const noexcept {
            return *std::launder(
                reinterpret_cast<const TValue*>(
                    &_slots[index]
                )
            );
        }

        /// Returns a mutable pointer to the already-live TValue object in one slot.
        TValue* GetPointer(
            std::size_t index
        ) noexcept {
            return std::addressof(
                Get(index)
            );
        }

        /// Returns a read-only pointer to the already-live TValue object in one slot.
        const TValue* GetPointer(
            std::size_t index
        ) const noexcept {
            return std::addressof(
                Get(index)
            );
        }

    };

} // ESPressio::Bounded::Detail
