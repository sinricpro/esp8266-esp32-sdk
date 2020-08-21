#ifndef _DEVICEID_H_
#define _DEVICEID_H_

#define DEVICEID_BINLEN 12 // 12 bytes long
#define DEVICEID_STRLEN 24 // string needs to hold 24 characters

class deviceId_t {
  public:
    typedef uint8_t deviceId_bin_t[DEVICEID_BINLEN];
    deviceId_t();
    deviceId_t(const char* deviceId);
    deviceId_t(const String &deviceId) { deviceId_t(deviceId.c_str()); }
    deviceId_t(const deviceId_t &other);

    deviceId_t operator=(const deviceId_t &other);
    
    String toString();
    bool isValid();
  private:
    deviceId_bin_t _deviceId_bin;
};

deviceId_t::deviceId_t() : _deviceId_bin{} {}

deviceId_t::deviceId_t(const char* deviceId) {
  char tmp;
  bool _isValid = (sscanf(deviceId,"%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%c",
    &_deviceId_bin[11], &_deviceId_bin[10], &_deviceId_bin[9], &_deviceId_bin[8],
    &_deviceId_bin[7],  &_deviceId_bin[6],  &_deviceId_bin[5], &_deviceId_bin[4],
    &_deviceId_bin[3],  &_deviceId_bin[2],  &_deviceId_bin[1], &_deviceId_bin[0],
    &tmp
  ) == sizeof(deviceId_bin_t)) && (strlen(deviceId) == DEVICEID_STRLEN);
  if (!_isValid) memset((void*) &_deviceId_bin, 0, sizeof(deviceId_bin_t));
}

deviceId_t::deviceId_t(const deviceId_t &other) {
  memcpy((void*) _deviceId_bin, (void*) other._deviceId_bin, sizeof(deviceId_bin_t));
}

String deviceId_t::toString() {
  char temp[DEVICEID_STRLEN+1];
  sprintf(temp, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
    _deviceId_bin[11], _deviceId_bin[10], _deviceId_bin[9], _deviceId_bin[8],
    _deviceId_bin[7],  _deviceId_bin[6],  _deviceId_bin[5], _deviceId_bin[4],
    _deviceId_bin[3],  _deviceId_bin[2],  _deviceId_bin[1], _deviceId_bin[0]
  );
  return String(temp);
}

deviceId_t deviceId_t::operator=(const deviceId_t &other) {
  memcpy((void*) _deviceId_bin, (void*) other._deviceId_bin, sizeof(deviceId_bin_t));
  return *this;
}

bool deviceId_t::isValid() {
  for (size_t i=0; i<sizeof(deviceId_bin_t); i++) {
    if (_deviceId_bin[i] != 0) return true;
  }
  return false;
}

#endif