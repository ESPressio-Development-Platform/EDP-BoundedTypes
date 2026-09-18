#include <Arduino.h>
#include <ESPressio_BoundedTypes.hpp>

using ESPressio::Bounded::Bytes;
using ESPressio::Bounded::BytesPushBackResult;
using ESPressio::Bounded::String;
using ESPressio::Bounded::StringAppendResult;
using ESPressio::Bounded::StringAssignmentResult;

/// Initializes a small bounded text and binary example and reports the resulting values.
void setup() {
    Serial.begin(115200);

    String<32U> message;

    if (message.Assign("bounded") != StringAssignmentResult::Succeeded) {
        Serial.println("String assignment failed.");
        return;
    }

    if (message.Append("-text") != StringAppendResult::Succeeded) {
        Serial.println("String append failed.");
        return;
    }

    Bytes<8U> payload;

    for (std::uint8_t value = 1U; value <= 4U; ++value)
        if (payload.PushBack(value) != BytesPushBackResult::Succeeded) {
            Serial.println("Byte append failed.");
            return;
        }

    Serial.print("Text: ");
    Serial.println(message.CStr());
    Serial.print("Payload bytes: ");
    Serial.println(payload.Size());
}

/// Performs no repeated work; this demo is intentionally initialization-only.
void loop() {
}
