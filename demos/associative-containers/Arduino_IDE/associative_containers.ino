#include <Arduino.h>
#include <ESPressio_BoundedTypes.hpp>

using ESPressio::Bounded::Map;
using ESPressio::Bounded::MapFindResult;
using ESPressio::Bounded::MapInsertResult;
using ESPressio::Bounded::Set;
using ESPressio::Bounded::SetInsertResult;

/// Demonstrates unique Set insertion and explicit Map lookup without hidden insertion operators.
void setup() {
    Serial.begin(115200);

    Set<int, 4U> identifiers;
    identifiers.Insert(10);
    identifiers.Insert(20);

    Map<int, int, 4U> readings;
    readings.Insert(
        10,
        100
    );
    readings.Insert(
        20,
        200
    );

    int* reading = nullptr;

    if (readings.Find(
        20,
        reading
    ) == MapFindResult::Succeeded) {
        *reading = 201;
    }

    Serial.printf(
        "Set entries=%u Map entries=%u Reading20=%d\n",
        static_cast<unsigned>(identifiers.Size()),
        static_cast<unsigned>(readings.Size()),
        reading == nullptr
            ? -1
            : *reading
    );
}

/// Performs no repeated work; this demo is intentionally initialization-only.
void loop() {
}
