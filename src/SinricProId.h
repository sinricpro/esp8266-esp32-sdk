#ifndef _SINRICID_H_
#define _SINRICID_H_

#define DEVICEID_BINLEN 12 // 12 bytes long
#define DEVICEID_STRLEN 24 // string needs to hold 24 characters

struct DeviceId_Bin_t {
  DeviceId_Bin_t() : _data{} {}
  void fromString(const char * other);
  String toString() const;
  uint8_t _data[DEVICEID_BINLEN];
};

void DeviceId_Bin_t::fromString(const char* other) {
  char tmp;
  bool _isValid = (sscanf(other,"%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%c",
    &_data[11], &_data[10], &_data[9], &_data[8], &_data[7],  &_data[6],  &_data[5], &_data[4],
    &_data[3],  &_data[2],  &_data[1], &_data[0],
    &tmp
  ) == sizeof(_data)) && (strlen(other) == DEVICEID_STRLEN);
  if (!_isValid) memset((void*) &_data, 0, sizeof(_data));
}

String DeviceId_Bin_t::toString() const {
  char temp[DEVICEID_STRLEN+1];
  sprintf(temp, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
    _data[11], _data[10], _data[9], _data[8], _data[7],  _data[6],  _data[5], _data[4],
    _data[3],  _data[2],  _data[1], _data[0]
  );
  return String(temp);
}


#define APPKEY_BINLEN 16
#define APPKEY_STRLEN 36

struct AppKey_Bin_t {
  AppKey_Bin_t() : _data{} {}
  void fromString(const char * other);
  String toString() const;
  uint8_t _data[APPKEY_BINLEN];
};

void AppKey_Bin_t::fromString(const char* other) {
  char tmp;
  bool _isValid = (sscanf(other,"%2hhx%2hhx%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%c",
    &_data[15], &_data[14], &_data[13], &_data[12], &_data[11], &_data[10], &_data[9],  &_data[8],
    &_data[7],  &_data[6],  &_data[5],  &_data[4],  &_data[3],  &_data[2],  &_data[1],  &_data[0],
    &tmp
  ) == sizeof(_data)) && (strlen(other) == APPKEY_STRLEN);
  if (!_isValid) memset((void*) &_data, 0, sizeof(_data));
}

String AppKey_Bin_t::toString() const {
  char temp[APPKEY_STRLEN+1];
  sprintf(temp, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
    _data[15], _data[14], _data[13], _data[12], _data[11], _data[10], _data[9],  _data[8],
    _data[7],  _data[6],  _data[5],  _data[4],  _data[3],  _data[2],  _data[1],  _data[0]
  );
  return String(temp);
}

#define APPSECRET_BINLEN 32
#define APPSECRET_STRLEN 73

struct AppSecret_Bin_t {
  AppSecret_Bin_t() : _data{} {}
  void fromString(const char * other);
  String toString() const;
  uint8_t _data[APPSECRET_BINLEN];
};

void AppSecret_Bin_t::fromString(const char* other) {
  char tmp;
  bool _isValid = (sscanf(other, "%2hhx%2hhx%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%c",
    &_data[31], &_data[30], &_data[29], &_data[28], &_data[27], &_data[26], &_data[25], &_data[24],
    &_data[23], &_data[22], &_data[21], &_data[20], &_data[19], &_data[18], &_data[17], &_data[16],
    &_data[15], &_data[14], &_data[13], &_data[12], &_data[11], &_data[10], &_data[ 9], &_data[ 8],
    &_data[ 7], &_data[ 6], &_data[ 5], &_data[ 4], &_data[ 3], &_data[ 2], &_data[ 1], &_data[ 0],
    &tmp
  ) == sizeof(_data)) && (strlen(other) == APPSECRET_STRLEN);
  if (!_isValid) memset((void*) &_data, 0, sizeof(_data));
}

String AppSecret_Bin_t::toString() const {
  char temp[APPSECRET_STRLEN+1];
  sprintf(temp, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x-%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
    _data[31], _data[30], _data[29], _data[28], _data[27], _data[26], _data[25], _data[24],
    _data[23], _data[22], _data[21], _data[20], _data[19], _data[18], _data[17], _data[16],
    _data[15], _data[14], _data[13], _data[12], _data[11], _data[10], _data[ 9], _data[ 8],
    _data[ 7], _data[ 6], _data[ 5], _data[ 4], _data[ 3], _data[ 2], _data[ 1], _data[ 0]
  );
  return String(temp);
}

template <class T>
class SinricProId {
  public:
    SinricProId() : _data() {};
    SinricProId(const char* other) { _data.fromString(other); }
    SinricProId(const String &other) { _data.fromString(other.c_str()); }
    SinricProId(const SinricProId &other) { copy(other); }
    SinricProId(const T &other) { copy(other); }
    SinricProId(const uint8_t other[], size_t size) { copy(other, size); }

    SinricProId operator=(const SinricProId &other) { copy(other); return *this; }
    SinricProId operator=(const char* other) { fromString(other); return *this; }
    SinricProId operator=(const String &other) { fromString(other.c_str()); return *this; }
    SinricProId operator=(const T &other) { copy(other); }

    bool operator==(const SinricProId &other) const { return compare(other); }
    bool operator==(const char* other) const { return compare((SinricProId) other); }
    bool operator==(const String& other) const { return compare((SinricProId) other); }
    bool operator==(const T &other) const { return compare((SinricProId) other); }

    bool operator!=(const SinricProId &other) const { return !compare(other); }
    bool operator!=(const char* other) const { return !compare(other); }
    bool operator!=(const String &other) const { return !compare(other); }
    bool operator!=(const T &other) const { return !compare(other); }
    
    operator bool() const { return isValid(); }
    operator String() const { return _data.toString(); }
    
    String toString() const { return _data.toString(); };
    const char* c_str() const { static String str = _data.toString(); return str.c_str(); }
    bool isValid() const { return !compare(SinricProId<T>()); }
  
  private:
    void fromString(const char * other) { _data.fromString(other); }
    void copy(const SinricProId &other) { memcpy(_data._data, other._data._data, sizeof(_data._data)); }    
    void copy(const T &other) { memcpy(_data, other, sizeof(_data)); }
    void copy(const uint8_t other[], size_t size) { memcpy(_data._data, other, min(sizeof(_data._data), size)); }
    bool compare(const SinricProId &other) const { return memcmp(_data._data, other._data._data, sizeof(_data._data)) == 0;}
    T _data;
};

typedef SinricProId<DeviceId_Bin_t> DeviceId;
typedef SinricProId<AppKey_Bin_t> AppKey;
typedef SinricProId<AppSecret_Bin_t> AppSecret;

#endif