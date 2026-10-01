#include <cstdint>
#include <cstdio>

#include <ESPressio_BoundedTypes.hpp>


namespace Demo {

    /// Represents one external value owned by this demonstration integration.
    struct ExternalValue final {
        /// Numeric payload copied by the demonstration adapter.
        std::uint8_t Value = 0U;
    };

    /// Describes conversion outcomes without exposing their meaning to generic consumers.
    enum class ConversionResult : std::uint8_t {
        Succeeded = 0,
        Rejected = 1
    };

} // Demo

namespace ESPressio::Bounded {

    /// Demonstrates an integration-owned adapter with generic success interpretation.
    template<>
    struct TypeConversionAdapter<Demo::ExternalValue, std::uint8_t> final {

        /// Operation-specific result Type owned by the demonstration integration.
        using ResultType = Demo::ConversionResult;

        /// Indicates that the exact conversion is provided.
        static constexpr bool IsAvailable = true;

        /// Indicates that conversion and result interpretation cannot throw.
        static constexpr bool IsNoexcept = true;

        /// Interprets the integration-specific result for generic callers.
        static constexpr bool IsSuccessful(
            const ResultType result
        ) noexcept {
            return result == ResultType::Succeeded;
        }

        /// Copies the demonstration's integer representation into the destination.
        static ResultType Convert(
            const Demo::ExternalValue& source,
            std::uint8_t& target
        ) noexcept {
            target = source.Value;
            return ResultType::Succeeded;
        }

    };

} // ESPressio::Bounded

static_assert(
    ESPressio::Bounded::HasTypeConversionSuccessPredicate<Demo::ExternalValue, std::uint8_t>,
    "The demonstration adapter must expose generic success interpretation."
);

static_assert(
    ESPressio::Bounded::IsTypeConversionSuccessful<Demo::ExternalValue, std::uint8_t>(
        Demo::ConversionResult::Succeeded
    ),
    "The adapter itself must define the meaning of its success result."
);

using ESPressio::Bounded::CapacityTraits;
using ESPressio::Bounded::CapacityUnit;
using ESPressio::Bounded::IsMemoryBoundedValue;
using ESPressio::Bounded::String;
using ESPressio::Bounded::Vector;

static_assert(
    IsMemoryBoundedValue<String<16U>>,
    "Bounded String must qualify as a memory-bounded value."
);

static_assert(
    CapacityTraits<Vector<int, 8U>>::Unit == CapacityUnit::Elements,
    "Vector capacity must be measured in elements."
);

/// Runs the compile-time trait and capacity demonstration under ESP-IDF.
extern "C" void app_main() {
    std::printf(
        "String capacity=%u bytes, Vector capacity=%u elements\n",
        static_cast<unsigned>(CapacityTraits<String<16U>>::Capacity),
        static_cast<unsigned>(CapacityTraits<Vector<int, 8U>>::Capacity)
    );
}
