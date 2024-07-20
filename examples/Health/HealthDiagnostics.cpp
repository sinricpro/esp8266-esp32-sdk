#include "HealthDiagnostics.h"

String HealthDiagnostics::getChipId() {
#if defined(ESP32)
  return String((uint32_t)ESP.getEfuseMac(), HEX);
#elif defined(ESP8266)
  return String(ESP.getChipId(), HEX);
#elif defined(ARDUINO_ARCH_RP2040)
  return String(rp2040.getChipID(), HEX);
#endif
}

void HealthDiagnostics::addHeapInfo(JsonObject& doc) {
#if defined(ESP32)
  doc["freeHeap"] = ESP.getFreeHeap();
  doc["totalHeap"] = ESP.getHeapSize();
  doc["minFreeHeap"] = ESP.getMinFreeHeap();
  doc["maxAllocHeap"] = ESP.getMaxAllocHeap();

  multi_heap_info_t heap_info;
  heap_caps_get_info(&heap_info, MALLOC_CAP_INTERNAL);

  JsonObject internalHeap = doc["internalHeap"].to<JsonObject>();
  internalHeap["totalFreeBytes"] = heap_info.total_free_bytes;
  internalHeap["totalAllocatedBytes"] = heap_info.total_allocated_bytes;
  internalHeap["largestFreeBlock"] = heap_info.largest_free_block;
  internalHeap["minimumFreeBytes"] = heap_info.minimum_free_bytes;
  internalHeap["allocatedBlocks"] = heap_info.allocated_blocks;
  internalHeap["freeBlocks"] = heap_info.free_blocks;
  internalHeap["totalBlocks"] = heap_info.total_blocks;

  heap_caps_get_info(&heap_info, MALLOC_CAP_SPIRAM);
  
  JsonObject psram = doc["psram"].to<JsonObject>();
  psram["totalFreeBytes"] = heap_info.total_free_bytes;
  psram["totalAllocatedBytes"] = heap_info.total_allocated_bytes;
  psram["largestFreeBlock"] = heap_info.largest_free_block;
  psram["minimumFreeBytes"] = heap_info.minimum_free_bytes;
  psram["allocatedBlocks"] = heap_info.allocated_blocks;
  psram["freeBlocks"] = heap_info.free_blocks;
  psram["totalBlocks"] = heap_info.total_blocks;
 
#elif defined(ESP8266)
  doc["freeHeap"] = ESP.getFreeHeap();
  doc["heapFragmentation"] = ESP.getHeapFragmentation();
  doc["maxFreeBlockSize"] = ESP.getMaxFreeBlockSize();

  // Get detailed heap information.
  JsonObject heapInfo = doc["heapInfo"].to<JsonObject>();
  UMM_HEAP_INFO ummHeapInfo;
  umm_info(&ummHeapInfo, 0);
  heapInfo["freeBlocks"] = ummHeapInfo.freeBlocks;
  heapInfo["usedBlocks"] = ummHeapInfo.usedBlocks;
  heapInfo["totalBlocks"] = ummHeapInfo.totalBlocks;
  heapInfo["totalEntries"] = ummHeapInfo.totalEntries;
  heapInfo["usedEntries"] = ummHeapInfo.usedEntries;
  heapInfo["freeEntries"] = ummHeapInfo.freeEntries;
  heapInfo["maxFreeContiguousBytes"] = umm_max_block_size();

#elif defined(ARDUINO_ARCH_RP2040)
  doc["freeHeap"] = rp2040.getFreeHeap();
  doc["totalHeap"] = rp2040.getTotalHeap();
#endif
}

void HealthDiagnostics::addWiFiInfo(JsonObject& doc) {
  doc["ssid"] = WiFi.SSID();

#if defined(ESP32) || defined(ESP8266)
  doc["bssid"] = WiFi.BSSIDstr();
#endif

  doc["rssi"] = WiFi.RSSI();
  doc["ipAddress"] = WiFi.localIP().toString();
  doc["subnetMask"] = WiFi.subnetMask().toString();
  doc["gateway"] = WiFi.gatewayIP().toString();
  doc["dns"] = WiFi.dnsIP().toString();
  doc["macAddress"] = WiFi.macAddress();
  doc["channel"] = WiFi.channel();
}

void HealthDiagnostics::addSketchInfo(JsonObject& doc) {
#if defined(ESP32) || defined(ESP8266)
  doc["cpuFreq"] = ESP.getCpuFreqMHz();
  doc["sketchSize"] = ESP.getSketchSize();
  doc["freeSketchSpace"] = ESP.getFreeSketchSpace();
  doc["flashChipSize"] = ESP.getFlashChipSize();
  doc["flashChipSpeed"] = ESP.getFlashChipSpeed();
#endif
}

void HealthDiagnostics::addResetCause(JsonObject& doc) {
#if defined(ESP32)
  switch (esp_reset_reason()) {
    case ESP_RST_POWERON: doc["reason"] = "Power-on event"; break;
    case ESP_RST_EXT: doc["reason"] = "External pin reset"; break;
    case ESP_RST_SW: doc["reason"] = "Software reset via esp_restart"; break;
    case ESP_RST_PANIC: doc["reason"] = "Software reset due to exception/panic"; break;
    case ESP_RST_INT_WDT: doc["reason"] = "Reset (software or hardware) due to interrupt watchdog"; break;
    case ESP_RST_TASK_WDT: doc["reason"] = "Reset due to task watchdog"; break;
    case ESP_RST_WDT: doc["reason"] = "Reset due to other watchdogs"; break;
    case ESP_RST_DEEPSLEEP: doc["reason"] = "Deep sleep reset"; break;
    case ESP_RST_BROWNOUT: doc["reason"] = "Brownout reset"; break;
    case ESP_RST_SDIO: doc["reason"] = "Reset over SDIO"; break;
    default: doc["reason"] = "Unknown reset reason"; break;
  }
#elif defined(ESP8266)
  doc["reason"] = ESP.getResetReason();
#endif

  //doc["crashDetails"] = "" // Use something like https://github.com/krzychb/EspSaveCrash
}

bool HealthDiagnostics::reportHealth(String& healthReport) {
  JsonDocument doc;
  doc["chipId"] = getChipId();
  doc["uptime"] = millis() / 1000;  // seconds

  // Add detailed heap information.
  JsonObject heap = doc["heap"].to<JsonObject>();
  addHeapInfo(heap);

  // Detailed Sketch information.
  JsonObject sketch = doc["sketch"].to<JsonObject>();
  addSketchInfo(sketch);

  // Detailed WiFi information.
  JsonObject wifi = doc["wifi"].to<JsonObject>();
  addWiFiInfo(wifi);

  // Add last reset reson
  JsonObject resetInfo = doc["reset"].to<JsonObject>();
  addResetCause(resetInfo);

  serializeJson(doc, healthReport);
  return true;
}