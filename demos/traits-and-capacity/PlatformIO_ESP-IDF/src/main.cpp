#include <cstdio>

#include <ESPressio_BoundedTypes.hpp>

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
