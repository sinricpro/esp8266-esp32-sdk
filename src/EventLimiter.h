#pragma once

#include "SinricProNameSpace.h"

namespace SINRICPRO_NAMESPACE {

class EventLimiter {
  public:
    EventLimiter(unsigned long minimum_distance = 1000);
    operator bool();
  private:
    unsigned long minimum_distance;
    unsigned long last_event;
};

EventLimiter::EventLimiter(unsigned long minimum_distance) 
: minimum_distance(minimum_distance)
, last_event(0) {}

EventLimiter::operator bool() {
  unsigned long current_millis = millis();
  if (last_event) {
    if (current_millis - last_event >= minimum_distance) {
      last_event = current_millis;
      return false;
    } 
  } else {
    last_event = current_millis;
    return false;
  }
  return true;
}

} // SINRICPRO_NAMESPACE