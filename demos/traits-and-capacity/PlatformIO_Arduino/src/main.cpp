#include <Arduino.h>
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

/// Reports compile-time bounded capacity metadata at runtime for inspection.
void setup() {
    Serial.begin(115200);

    Serial.printf(
        "String capacity=%u bytes, Vector capacity=%u elements\n",
        static_cast<unsigned>(CapacityTraits<String<16U>>::Capacity),
        static_cast<unsigned>(CapacityTraits<Vector<int, 8U>>::Capacity)
    );
}

/// Performs no repeated work; this demo is intentionally initialization-only.
void loop() {
}
