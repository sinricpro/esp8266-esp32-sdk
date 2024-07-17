#include "HealthDiagnostics.h"

String HealthDiagnostics::getChipId() {
#ifdef ESP32
  return String((uint32_t)ESP.getEfuseMac(), HEX);
#else
  return String(ESP.getChipId(), HEX);
#endif
}

void HealthDiagnostics::addHeapInfo(JsonObject& doc) {
#ifdef ESP32
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

#ifdef CONFIG_SPIRAM_SUPPORT
  heap_caps_get_info(&heap_info, MALLOC_CAP_SPIRAM);

  JsonObject psram = doc["psram"].to<JsonObject>();
  psram["totalFreeBytes"] = heap_info.total_free_bytes;
  psram["totalAllocatedBytes"] = heap_info.total_allocated_bytes;
  psram["largestFreeBlock"] = heap_info.largest_free_block;
  psram["minimumFreeBytes"] = heap_info.minimum_free_bytes;
  psram["allocatedBlocks"] = heap_info.allocated_blocks;
  psram["freeBlocks"] = heap_info.free_blocks;
  psram["totalBlocks"] = heap_info.total_blocks;
#endif
#else
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
#endif
}

void HealthDiagnostics::addWiFiInfo(JsonObject& doc) {
  doc["ssid"] = WiFi.SSID();
  doc["bssid"] = WiFi.BSSIDstr();
  doc["rssi"] = WiFi.RSSI();
  doc["ipAddress"] = WiFi.localIP().toString();
  doc["subnetMask"] = WiFi.subnetMask().toString();
  doc["gateway"] = WiFi.gatewayIP().toString();
  doc["dns"] = WiFi.dnsIP().toString();
  doc["macAddress"] = WiFi.macAddress();
  doc["channel"] = WiFi.channel();
}

void HealthDiagnostics::addSketchInfo(JsonObject& doc) {
  doc["cpuFreq"] = ESP.getCpuFreqMHz();
  doc["sketchSize"] = ESP.getSketchSize();
  doc["freeSketchSpace"] = ESP.getFreeSketchSpace();
  doc["flashChipSize"] = ESP.getFlashChipSize();
  doc["flashChipSpeed"] = ESP.getFlashChipSpeed();
}

String HealthDiagnostics::getResetReasonESP32(esp_reset_reason_t resetReason) {
  switch (resetReason) {
    case ESP_RST_POWERON: return "Power-on event";
    case ESP_RST_EXT: return "External pin reset";
    case ESP_RST_SW: return "Software reset via esp_restart";
    case ESP_RST_PANIC: return "Software reset due to exception/panic";
    case ESP_RST_INT_WDT: return "Reset (software or hardware) due to interrupt watchdog";
    case ESP_RST_TASK_WDT: return "Reset due to task watchdog";
    case ESP_RST_WDT: return "Reset due to other watchdogs";
    case ESP_RST_DEEPSLEEP: return "Deep sleep reset";
    case ESP_RST_BROWNOUT: return "Brownout reset";
    case ESP_RST_SDIO: return "Reset over SDIO";
    default: return "Unknown reset reason";
  }
}

void HealthDiagnostics::addResetCause(JsonObject& doc) { 
  #ifdef ESP32
     doc["reason"] = getResetReasonESP32(esp_reset_reason());
  #else
     doc["reason"] = ESP.getResetReason();
  #endif  
  //doc["crashDetails"] = "" // Use something like https://github.com/krzychb/EspSaveCrash
}

bool HealthDiagnostics::reportHealth(String& healthReport) {
  JsonDocument doc;
  doc["chipId"] = getChipId();
  doc["uptime"] = millis() / 1000; // seconds

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