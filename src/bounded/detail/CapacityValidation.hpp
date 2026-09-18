#pragma once

#include <cstddef>
#include <cstdint>

namespace ESPressio::Bounded {

    /// Describes whether a candidate logical size fits within a compile-time storage capacity.
    enum class CapacityValidationResult : std::uint8_t {
        WithinCapacity = 0,
        CapacityExceeded = 1
    };

    /// Validates a candidate logical size against a compile-time maximum capacity.
    template<std::size_t TCapacity>
    constexpr CapacityValidationResult ValidateCapacity(
        std::size_t candidateSize
    ) noexcept {
        if (candidateSize <= TCapacity) { return CapacityValidationResult::WithinCapacity; }

        return CapacityValidationResult::CapacityExceeded;
    }

} // ESPressio::Bounded
