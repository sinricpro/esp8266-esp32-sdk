#pragma once

#include <Arduino.h>
#include <FS.h>


/**
 *  @brief Manages SinricPro using primary and secondary SSID configurations.
 */
class SinricProWiFiSettings {
public:
  struct wifi_settings_t {
    char primarySSID[32+1];        ///< Primary SSID of the WiFi network.
    char primaryPassword[64+1];    ///< Primary password of the WiFi network.
    char secondarySSID[32+1];      ///< Secondary SSID of the WiFi network.
    char secondaryPassword[64+1];  ///< Secondary password of the WiFi network.
  };

  /**
   * @brief Construct a new SinricProWiFiSettings object with default WiFi settings.
   * 
   * @param defaultPrimarySSID Default primary SSID.
   * @param defaultPrimaryPassword Default primary password.
   * @param defaultSecondarySSID Default secondary SSID.
   * @param defaultSecondaryPassword Default secondary password.
   * @param configFileName File name for storing WiFi settings.
   */
  SinricProWiFiSettings(fs::FS& fs,
                        const char* defaultPrimarySSID, const char* defaultPrimaryPassword,
                        const char* defaultSecondarySSID, const char* defaultSecondaryPassword,
                        const char* configFileName);

  /**
   * @brief Initializes the WiFi manager, loading settings from file or using defaults if loading fails.
   */
  void begin();

  /**
   * @brief Updates the primary WiFi settings.
   * 
   * @param newSSID New primary SSID.
   * @param newPassword New primary password.
   */
  void updatePrimarySettings(const char* newSSID, const char* newPassword);

  /**
   * @brief Updates the secondary WiFi settings.
   * 
   * @param newSSID New secondary SSID.
   * @param newPassword New secondary password.
   */
  void updateSecondarySettings(const char* newSSID, const char* newPassword);

  /**
   * @brief Prints the current WiFi settings to the Serial console.
   */
  void printSettings() const;

  /**
   * @brief Checks if the provided SSID and password are valid.
   * 
   * @param ssid SSID to check.
   * @param password Password to check.
   * @return true if both SSID and password are valid, false otherwise.
   */
  bool isValidSetting(const char* ssid, const char* password) const;

  /**
   * @brief Returns WiFi settings.
   */
  const wifi_settings_t& getWiFiSettings() const;

private:
  fs::FS& fs;                            // filesystem to be used
  const char* defaultPrimarySSID;        ///< Default primary SSID.
  const char* defaultPrimaryPassword;    ///< Default primary password.
  const char* defaultSecondarySSID;      ///< Default secondary SSID.
  const char* defaultSecondaryPassword;  ///< Default secondary password.
  const char* configFileName;            ///< File name to store WiFi settings.

  wifi_settings_t wifiSettings;

  /**
   * @brief Saves the current WiFi settings to a file.
   */
  void saveToFile();

  /**
   * @brief Loads WiFi settings from a file.
   * 
   * @return true if loaded successfully, false otherwise.
   */
  bool loadFromFile();

  /**
   * @brief Saves the default WiFi settings to a file and updates the current settings.
   */
  void saveDefaultSettings();


  /**
   * @brief Validates the given SSID.
   * 
   * @param ssid SSID to validate.
   * @return true if the SSID is valid, false otherwise.
   */
  bool validateSSID(const char* ssid) const;

  /**
   * @brief Validates the given password.
   * 
   * @param password Password to validate.
   * @return true if the password is valid, false otherwise.
   */
  bool validatePassword(const char* password) const;

  /**
   * @brief Deletes all the stored WiFi settings.
   */
  void deleteAllSettings();
};
