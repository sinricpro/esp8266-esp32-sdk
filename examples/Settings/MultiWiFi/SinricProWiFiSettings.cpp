#include "SinricProWiFiSettings.h"

SinricProWiFiSettings::SinricProWiFiSettings(const char* defaultPrimarySSID, const char* defaultPrimaryPassword,
                                             const char* defaultSecondarySSID, const char* defaultSecondaryPassword,
                                             const char* configFileName)
  : defaultPrimarySSID(defaultPrimarySSID), defaultPrimaryPassword(defaultPrimaryPassword),
    defaultSecondarySSID(defaultSecondarySSID), defaultSecondaryPassword(defaultSecondaryPassword),
    configFileName(configFileName) {
  memset(&wifiSettings, 0, sizeof(wifiSettings));
}

void SinricProWiFiSettings::begin() {
  if (!loadFromFile()) {
    saveDefaultSettings();
  }
  printSettings();
}

void SinricProWiFiSettings::updatePrimarySettings(const char* newSSID, const char* newPassword) {
  if (isValidSetting(newSSID, newPassword)) {
    strncpy(wifiSettings.primarySSID, newSSID, sizeof(wifiSettings.primarySSID));
    strncpy(wifiSettings.primaryPassword, newPassword, sizeof(wifiSettings.primaryPassword));
    saveToFile();
  } else {
    Serial.println("Invalid Primary SSID or Password");
  }
}

void SinricProWiFiSettings::updateSecondarySettings(const char* newSSID, const char* newPassword) {
  if (isValidSetting(newSSID, newPassword)) {
    strncpy(wifiSettings.secondarySSID, newSSID, sizeof(wifiSettings.secondarySSID));
    strncpy(wifiSettings.secondaryPassword, newPassword, sizeof(wifiSettings.secondaryPassword));
    saveToFile();
  } else {
    Serial.println("Invalid Secondary SSID or Password");
  }
}

void SinricProWiFiSettings::printSettings() const {
  Serial.printf("Primary SSID: %s\n", wifiSettings.primarySSID);
  Serial.printf("Primary Password: %s\n", wifiSettings.primaryPassword);
  Serial.printf("Secondary SSID: %s\n", wifiSettings.secondarySSID);
  Serial.printf("Secondary Password: %s\n", wifiSettings.secondaryPassword);
}

void SinricProWiFiSettings::saveToFile() {
  
  #if defined(ESP8266)
    File file = LittleFS.open(configFileName, "w");
  #elif defined(ESP32)
    File file = LittleFS.open(configFileName, FILE_WRITE);
  #endif

  if (file) {
    file.write(reinterpret_cast<const uint8_t*>(&wifiSettings), sizeof(wifiSettings));
    file.close();
  }
}

bool SinricProWiFiSettings::loadFromFile() {
  #if defined(ESP8266)
    File file = LittleFS.open(configFileName, "r");
  #elif defined(ESP32)
    File file = LittleFS.open(configFileName, FILE_READ);
  #endif
  
  if (file && file.size() == sizeof(wifiSettings)) {
    file.read(reinterpret_cast<uint8_t*>(&wifiSettings), sizeof(wifiSettings));
    file.close();
    return true;
  }
  return false;
}

void SinricProWiFiSettings::saveDefaultSettings() {
  Serial.println("Saving default WiFi login!");

  strncpy(wifiSettings.primarySSID, defaultPrimarySSID, sizeof(wifiSettings.primarySSID));
  strncpy(wifiSettings.primaryPassword, defaultPrimaryPassword, sizeof(wifiSettings.primaryPassword));
  strncpy(wifiSettings.secondarySSID, defaultSecondarySSID, sizeof(wifiSettings.secondarySSID));
  strncpy(wifiSettings.secondaryPassword, defaultSecondaryPassword, sizeof(wifiSettings.secondaryPassword));

  saveToFile();
}

void SinricProWiFiSettings::deleteAllSettings() {
  memset(&wifiSettings, 0, sizeof(wifiSettings));
  if (LittleFS.exists(configFileName)) {
    LittleFS.remove(configFileName);
  }
  Serial.println("All WiFi settings have been deleted.");
}

bool SinricProWiFiSettings::isValidSetting(const char* ssid, const char* password) const {
  return validateSSID(ssid) && validatePassword(password);
}

bool SinricProWiFiSettings::validateSSID(const char* ssid) const {
  return ssid && strlen(ssid) > 0 && strlen(ssid) < sizeof(wifi_settings_t::primarySSID);
}

bool SinricProWiFiSettings::validatePassword(const char* password) const {
  return password && strlen(password) < sizeof(wifi_settings_t::primaryPassword);
}

const SinricProWiFiSettings::wifi_settings_t& SinricProWiFiSettings::getWiFiSettings() const {
  return wifiSettings;
}
