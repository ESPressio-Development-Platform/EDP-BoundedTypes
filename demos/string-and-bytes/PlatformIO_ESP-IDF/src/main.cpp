#include <cstdio>
#include <cstdint>

#include <ESPressio_BoundedTypes.hpp>
#include <memory/ByteOperationsProvider.hpp>

using ESPressio::Bounded::Bytes;
using ESPressio::Bounded::BytesPushBackResult;
using ESPressio::Bounded::String;
using ESPressio::Bounded::StringAppendResult;
using ESPressio::Bounded::StringAssignmentResult;

using ByteOperationsProvider = ESPressio::Platform::Portable::Memory::ByteOperationsProvider;

/// Runs the bounded text and binary demonstration under ESP-IDF.
extern "C" void app_main() {
    String<32U, ByteOperationsProvider> message;

    if (message.Assign("bounded") != StringAssignmentResult::Succeeded) {
        std::printf("String assignment failed.\n");
        return;
    }

    if (message.Append("-text") != StringAppendResult::Succeeded) {
        std::printf("String append failed.\n");
        return;
    }

    Bytes<8U, ByteOperationsProvider> payload;

    for (std::uint8_t value = 1U; value <= 4U; ++value)
        if (payload.PushBack(value) != BytesPushBackResult::Succeeded) {
            std::printf("Byte append failed.\n");
            return;
        }

    std::printf(
        "Text: %s, payload bytes: %u\n",
        message.CStr(),
        static_cast<unsigned>(payload.Size())
    );
}
