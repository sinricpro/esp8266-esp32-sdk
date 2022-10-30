#pragma once

#include <Arduino.h>

class Timestamp {
  public:
    uint32_t getTimestamp();
    void     setTimestamp(uint32_t);

  protected:
    void update();

  protected:
    uint64_t timestamp_ms = 0;
    uint32_t last_update  = 0;
};
