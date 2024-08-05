#include "SinricProWiFiSettings.h"

SinricProWiFiSettings::SinricProWiFiSettings(fs::FS& fs,
                                             const char* defaultPrimarySSID, const char* defaultPrimaryPassword,
                                             const char* defaultSecondarySSID, const char* defaultSecondaryPassword,
                                             const char* configFileName)
  : fs(fs)
  , defaultPrimarySSID(defaultPrimarySSID), defaultPrimaryPassword(defaultPrimaryPassword),
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
    strlcpy(wifiSettings.primarySSID, newSSID, sizeof(wifiSettings.primarySSID));
    strlcpy(wifiSettings.primaryPassword, newPassword, sizeof(wifiSettings.primaryPassword));
    saveToFile();
  } else {
    Serial.println("Invalid Primary SSID or Password");
  }
}

void SinricProWiFiSettings::updateSecondarySettings(const char* newSSID, const char* newPassword) {
  if (isValidSetting(newSSID, newPassword)) {
    strlcpy(wifiSettings.secondarySSID, newSSID, sizeof(wifiSettings.secondarySSID));
    strlcpy(wifiSettings.secondaryPassword, newPassword, sizeof(wifiSettings.secondaryPassword));
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
  File file = fs.open(configFileName, "w");

  if (file) {
    file.write(reinterpret_cast<const uint8_t*>(&wifiSettings), sizeof(wifiSettings));
    file.close();
  }
}

bool SinricProWiFiSettings::loadFromFile() {
  File file = fs.open(configFileName, "r");
  
  if (file && file.size() == sizeof(wifiSettings)) {
    file.read(reinterpret_cast<uint8_t*>(&wifiSettings), sizeof(wifiSettings));
    file.close();
    return true;
  }
  return false;
}

void SinricProWiFiSettings::saveDefaultSettings() {
  Serial.println("Saving default WiFi login!");

  strlcpy(wifiSettings.primarySSID, defaultPrimarySSID, sizeof(wifiSettings.primarySSID));
  strlcpy(wifiSettings.primaryPassword, defaultPrimaryPassword, sizeof(wifiSettings.primaryPassword));
  strlcpy(wifiSettings.secondarySSID, defaultSecondarySSID, sizeof(wifiSettings.secondarySSID));
  strlcpy(wifiSettings.secondaryPassword, defaultSecondaryPassword, sizeof(wifiSettings.secondaryPassword));

  saveToFile();
}

void SinricProWiFiSettings::deleteAllSettings() {
  memset(&wifiSettings, 0, sizeof(wifiSettings));
  if (fs.exists(configFileName)) {
    fs.remove(configFileName);
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
