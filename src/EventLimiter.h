#pragma once

#include "SinricProConfig.h"
#include "SinricProNamespace.h"

namespace SINRICPRO_NAMESPACE {

class EventLimiter {
  public:
    EventLimiter(unsigned long minimum_distance = 1000);
    operator bool();
  private:
    unsigned long minimum_distance;
    unsigned long next_event;
    unsigned long extra_distance;
    unsigned long fail_counter;
};

EventLimiter::EventLimiter(unsigned long minimum_distance) 
: minimum_distance(minimum_distance)
, next_event(0)
, extra_distance(0)
, fail_counter(0) {}

EventLimiter::operator bool() {
  unsigned long current_millis = millis();
  unsigned long fail_threshold = (minimum_distance / 4);

  if ( current_millis >= next_event ) {

    if ( fail_counter > fail_threshold ) {

      extra_distance += minimum_distance;
      fail_counter = 0;

    } else {

      extra_distance = 0;

    }

    next_event = current_millis + minimum_distance + extra_distance;
    return false;

  }

  fail_counter++;
  if (fail_counter == fail_threshold) Serial.printf("WARNING: YOUR CODE SENDS EXCESSIVE EVENTS! EVENTS ARE NOW LIMITED BY AN ADDITIONAL DELAY OF %lu SECONDS. PLEASE CHECK YOUR CODE!\r\n", extra_distance / 1000);

  return true;
}

} // SINRICPRO_NAMESPACE