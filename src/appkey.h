#ifndef _APPKEY_H_
#define _APPKEY_H_

#define APPKEY_BINLEN 16
#define APPKEY_STRLEN 36

class AppKey {
  public:
    typedef uint8_t appKey_bin_t[APPKEY_BINLEN];
    AppKey();
    AppKey(const char* other);
    AppKey(const String &other);
    AppKey(const AppKey &other);

    AppKey operator=(const AppKey &other);
    
    bool operator==(const AppKey &other);
    bool operator==(const String &other);
    bool operator==(const char* other);
    bool operator!=(const AppKey &other) { return !operator==(other); }
    operator bool() const { return isValid(); }
    operator String() const { return toString(); }
    
    String toString() const;
    bool isValid() const;
  private:
  void fromString(const char* other);
    appKey_bin_t _appKey_bin;
};

AppKey::AppKey() : _appKey_bin{} {}

AppKey::AppKey(const char* other) {
  fromString(other);
}

AppKey::AppKey(const String &other) {
  fromString(other.c_str());
}

AppKey::AppKey(const AppKey &other) {
  memcpy((void*) _appKey_bin, (void*) other._appKey_bin, sizeof(appKey_bin_t));
}

String AppKey::toString() const {
  char temp[APPKEY_STRLEN+1];
  sprintf(temp, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
    _appKey_bin[15], _appKey_bin[14], _appKey_bin[13], _appKey_bin[12],
    _appKey_bin[11], _appKey_bin[10], _appKey_bin[9],  _appKey_bin[8],
    _appKey_bin[7],  _appKey_bin[6],  _appKey_bin[5],  _appKey_bin[4],
    _appKey_bin[3],  _appKey_bin[2],  _appKey_bin[1],  _appKey_bin[0]
  );
  return String(temp);
}

AppKey AppKey::operator=(const AppKey &other) {
  memcpy((void*) _appKey_bin, (void*) other._appKey_bin, sizeof(appKey_bin_t));
  return *this;
}

bool AppKey::operator==(const AppKey &other) {
  for (size_t i=0; i<APPKEY_BINLEN; i++) {
    if (_appKey_bin[i] != other._appKey_bin[i]) return false;
  }
  return true;
}

bool AppKey::operator==(const String &other) {
  return operator==((AppKey) other);
}

bool AppKey::operator==(const char* other) {
  return operator==((AppKey) other);
}

bool AppKey::isValid() const {
  for (size_t i=0; i<sizeof(appKey_bin_t); i++) {
    if (_appKey_bin[i] != 0) return true;
  }
  return false;
}

void AppKey::fromString(const char* other) {
  char tmp;
  bool _isValid = (sscanf(other,"%2hhx%2hhx%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%c",
    &_appKey_bin[15], &_appKey_bin[14], &_appKey_bin[13], &_appKey_bin[12],
    &_appKey_bin[11], &_appKey_bin[10], &_appKey_bin[9],  &_appKey_bin[8],
    &_appKey_bin[7],  &_appKey_bin[6],  &_appKey_bin[5],  &_appKey_bin[4],
    &_appKey_bin[3],  &_appKey_bin[2],  &_appKey_bin[1],  &_appKey_bin[0],
    &tmp
  ) == sizeof(appKey_bin_t)) && (strlen(other) == APPKEY_STRLEN);
  if (!_isValid) memset((void*) &_appKey_bin, 0, sizeof(appKey_bin_t));
}

#endif