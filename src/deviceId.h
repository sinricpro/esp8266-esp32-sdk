#ifndef _DEVICEID_H_
#define _DEVICEID_H_

#define DEVICEID_BINLEN 12 // 12 bytes long
#define DEVICEID_STRLEN 24 // string needs to hold 24 characters

class DeviceId {
  public:
    typedef uint8_t deviceId_bin_t[DEVICEID_BINLEN];
    DeviceId();
    DeviceId(const char* deviceId);
    DeviceId(const String &deviceId);
    DeviceId(const DeviceId &other);

    DeviceId operator=(const DeviceId &other);

    bool operator==(const DeviceId &other) const;
    bool operator==(const String &other) const;
    bool operator==(const char* other) const;

    bool operator!=(const DeviceId &other) const { return !operator==(other); }
    operator bool() const { return isValid(); }
    operator String() const { return toString(); }
    
    String toString() const;
    bool isValid() const;
  private:
    void fromString(const char * other);
    deviceId_bin_t _deviceId_bin;
};

DeviceId::DeviceId() : _deviceId_bin{} {}

DeviceId::DeviceId(const char* deviceId) {
  fromString(deviceId);
}

DeviceId::DeviceId(const String &other) {
  fromString(other.c_str());
}

DeviceId::DeviceId(const DeviceId &other) {
  memcpy((void*) _deviceId_bin, (void*) other._deviceId_bin, sizeof(deviceId_bin_t));
}

String DeviceId::toString() const {
  char temp[DEVICEID_STRLEN+1];
  sprintf(temp, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
    _deviceId_bin[11], _deviceId_bin[10], _deviceId_bin[9], _deviceId_bin[8],
    _deviceId_bin[7],  _deviceId_bin[6],  _deviceId_bin[5], _deviceId_bin[4],
    _deviceId_bin[3],  _deviceId_bin[2],  _deviceId_bin[1], _deviceId_bin[0]
  );
  return String(temp);
}

DeviceId DeviceId::operator=(const DeviceId &other) {
  memcpy((void*) _deviceId_bin, (void*) other._deviceId_bin, sizeof(deviceId_bin_t));
  return *this;
}

bool DeviceId::operator==(const DeviceId &other) const {
  for (size_t i=0; i<DEVICEID_BINLEN; i++) {
    if (_deviceId_bin[i] != other._deviceId_bin[i]) return false;
  }
  return true;
}

bool DeviceId::operator==(const String &other) const {
  return operator==((DeviceId) other);
}

bool DeviceId::operator==(const char* other) const {
  return operator==((DeviceId) other);
}


bool DeviceId::isValid() const {
  for (size_t i=0; i<sizeof(deviceId_bin_t); i++) {
    if (_deviceId_bin[i] != 0) return true;
  }
  return false;
}

void DeviceId::fromString(const char* other) {
  char tmp;
  bool _isValid = (sscanf(other,"%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%c",
    &_deviceId_bin[11], &_deviceId_bin[10], &_deviceId_bin[9], &_deviceId_bin[8],
    &_deviceId_bin[7],  &_deviceId_bin[6],  &_deviceId_bin[5], &_deviceId_bin[4],
    &_deviceId_bin[3],  &_deviceId_bin[2],  &_deviceId_bin[1], &_deviceId_bin[0],
    &tmp
  ) == sizeof(deviceId_bin_t)) && (strlen(other) == DEVICEID_STRLEN);
  if (!_isValid) memset((void*) &_deviceId_bin, 0, sizeof(deviceId_bin_t));
}
#endif