#include "Timestamp.h"

uint32_t Timestamp::getTimestamp() {
    update();
    return timestamp_ms / 1000UL;
}

void Timestamp::setTimestamp(uint32_t new_timestamp) {
    timestamp_ms = uint64_t(new_timestamp) * 1000;
    last_update  = millis();
}

void Timestamp::update() {
    if (!timestamp_ms) return;
    uint32_t current_millis = millis();
    uint32_t diff_millis    = current_millis - last_update;
    timestamp_ms += diff_millis;
    last_update = current_millis;
}
