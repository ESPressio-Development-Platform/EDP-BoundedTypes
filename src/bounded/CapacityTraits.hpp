#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace ESPressio::Bounded {

    /// Identifies the semantic unit represented by a compile-time capacity value.
    enum class CapacityUnit : std::uint8_t {
        None = 0,
        Bytes = 1,
        Elements = 2,
        Entries = 3
    };

    /// Describes compile-time logical capacity metadata for a Type.
    template<class TValue>
    struct CapacityTraits {

        /// Indicates whether the Type exposes a fixed logical capacity at compile time.
        static constexpr bool HasStaticCapacity = false;

        /// Identifies the semantic unit of the compile-time capacity.
        static constexpr CapacityUnit Unit = CapacityUnit::None;

        /// Exposes the compile-time logical capacity when one exists.
        static constexpr std::size_t Capacity = 0;

    };

    /// Reports whether a Type exposes a fixed logical capacity at compile time.
    template<class TValue>
    inline constexpr bool HasStaticCapacity = CapacityTraits<std::remove_cv_t<TValue>>::HasStaticCapacity;

    /// Exposes element-count capacity for a built-in fixed-extent array.
    template<class TValue, std::size_t TExtent>
    struct CapacityTraits<TValue[TExtent]> {

        /// Built-in arrays always have fixed compile-time capacity.
        static constexpr bool HasStaticCapacity = true;

        /// Built-in array capacity is measured in elements.
        static constexpr CapacityUnit Unit = CapacityUnit::Elements;

        /// Reports the number of elements in the array.
        static constexpr std::size_t Capacity = TExtent;

    };

    /// Exposes element-count capacity for a standard fixed-extent array.
    template<class TValue, std::size_t TExtent>
    struct CapacityTraits<std::array<TValue, TExtent>> {

        /// Standard arrays always have fixed compile-time capacity.
        static constexpr bool HasStaticCapacity = true;

        /// Standard array capacity is measured in elements.
        static constexpr CapacityUnit Unit = CapacityUnit::Elements;

        /// Reports the number of elements in the array.
        static constexpr std::size_t Capacity = TExtent;

    };

} // ESPressio::Bounded
