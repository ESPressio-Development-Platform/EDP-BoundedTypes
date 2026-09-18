#include <cstdio>

#include <ESPressio_BoundedTypes.hpp>

using ESPressio::Bounded::Map;
using ESPressio::Bounded::MapFindResult;
using ESPressio::Bounded::Set;

/// Runs the bounded associative-container demonstration under ESP-IDF.
extern "C" void app_main() {
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

    std::printf(
        "Set entries=%u Map entries=%u Reading20=%d\n",
        static_cast<unsigned>(identifiers.Size()),
        static_cast<unsigned>(readings.Size()),
        reading == nullptr
            ? -1
            : *reading
    );
}
