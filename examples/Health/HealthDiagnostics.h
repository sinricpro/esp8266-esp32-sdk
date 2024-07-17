#ifndef HEALTH_DIAGNOSTICS_H
#define HEALTH_DIAGNOSTICS_H

#include <Arduino.h>
#include <ArduinoJson.h>
#ifdef ESP32
  #include <esp_wifi.h>
  #include <esp_heap_caps.h>
#else
  extern "C" {
    #include "umm_malloc/umm_heap_select.h"
    #include "umm_malloc/umm_malloc.h"
  }
#endif
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <user_interface.h>
#elif defined(ESP32) 
  #include <WiFi.h>  
  #include "esp_system.h"
#endif

/**
 * @brief Class to handle health diagnostics
 */
class HealthDiagnostics {
public:
  /**
     * @brief Report the health diagnostic information.
     * 
     * @param healthReport A reference to a String to store the health report in JSON format.
     * @return True on success, otherwise false.
     */
  bool reportHealth(String& healthReport);

private:  
  String getChipId();
  void addHeapInfo(JsonObject& doc);
  void addWiFiInfo(JsonObject& doc);
  void addSketchInfo(JsonObject& doc);
  void addResetCause(JsonObject& doc);
  String getResetReasonESP32(esp_reset_reason_t resetReason);
};

#endif  // HEALTH_DIAGNOSTICS_H