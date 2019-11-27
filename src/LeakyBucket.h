/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */


#ifndef _LEAKY_BUCKET_H_
#define _LEAKY_BUCKET_H_

class LeakyBucket_t {
  public:
    LeakyBucket_t() : dropsInBucket(0), lastDrop(-DROP_IN_TIME), once(false) {}
    bool addDrop();
  private:
    void leak();
    int dropsInBucket;
    unsigned long lastDrop;
    bool once;
    unsigned long lastWarning;
};

bool LeakyBucket_t::addDrop() {
  leak();
  unsigned long actualMillis = millis();

  if (dropsInBucket < BUCKET_SIZE && actualMillis-lastDrop > dropsInBucket + DROP_IN_TIME) { // new drop can be placed into bucket?
    dropsInBucket++;                                                                          // place drop in bucket
    lastDrop = actualMillis;                                                                  // store last drop time
    return true;
  }

  if (dropsInBucket >= BUCKET_SIZE) {
    if (actualMillis-lastWarning > 1000) {
      if (once == false) {
        Serial.printf("[SinricPro]: WARNING: YOU SENT TOO MUCH EVENTS IN A SHORT PERIOD OF TIME!\r\n - PLEASE CHECK YOUR CODE AND SEND EVENTS ONLY IF DEVICE STATE HAS CHANGED!\r\n");      // Print a warning when bucket is full
        once = true;
      }
      Serial.printf("[SinricPro]: EVENTS ARE BLOCKED FOR %lu SECONDS!\r\n",(DROP_OUT_TIME-(actualMillis-lastDrop))/1000);
      lastWarning = actualMillis;
    }
  }
  return false;
}

void LeakyBucket_t::leak() {
// leack bucket...
  unsigned long actualMillis = millis();
  int drops_to_leak = (actualMillis - lastDrop) / DROP_OUT_TIME;
  if (drops_to_leak > 0) {
    if (dropsInBucket <= drops_to_leak) {
      dropsInBucket = 0;
    } else {
      dropsInBucket -= drops_to_leak;
    }
  }
}


#endif