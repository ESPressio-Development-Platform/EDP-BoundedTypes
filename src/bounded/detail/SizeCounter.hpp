#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace ESPressio::Bounded::Detail {

    /// Selects the smallest unsigned integer Type capable of representing values from zero through the supplied capacity.
    template<std::size_t TCapacity>
    struct SizeCounterSelector {

        /// The smallest unsigned integer Type able to represent the complete logical size range.
        using Type = std::conditional_t<
            (TCapacity <= static_cast<std::size_t>(std::numeric_limits<std::uint8_t>::max())),
            std::uint8_t,
            std::conditional_t<
                (TCapacity <= static_cast<std::size_t>(std::numeric_limits<std::uint16_t>::max())),
                std::uint16_t,
                std::conditional_t<
                    (TCapacity <= static_cast<std::size_t>(std::numeric_limits<std::uint32_t>::max())),
                    std::uint32_t,
                    std::uint64_t
                >
            >
        >;

    };

    /// Provides the compact runtime counter Type used internally by a bounded value with the supplied static capacity.
    template<std::size_t TCapacity>
    using SizeCounter = typename SizeCounterSelector<TCapacity>::Type;

} // ESPressio::Bounded::Detail
