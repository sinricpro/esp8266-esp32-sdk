#ifndef _APPSECRET_H_
#define _APPSECRET_H_

#define APPSECRET_BINLEN 32
#define APPSECRET_STRLEN 73

class AppSecret {
  public:
    typedef uint8_t appSecret_bin_t[APPSECRET_BINLEN];
    AppSecret();
    AppSecret(const char* other);
    AppSecret(const String &other) { AppSecret(other.c_str()); }
    AppSecret(const AppSecret &other);

    AppSecret operator=(const AppSecret &other);
    bool operator==(const AppSecret &other);
    bool operator==(const String &other);
    bool operator==(const char* other);
    bool operator!=(const AppSecret &other) {return !operator==(other); }
    operator bool() const { return isValid(); }
    operator String() const { return toString(); }
    
    String toString() const;
    bool isValid() const;
  private:
    void fromString(const char* other);
    appSecret_bin_t _appSecret_bin;
};

AppSecret::AppSecret() : _appSecret_bin{} {}

AppSecret::AppSecret(const char* other) {
  fromString(other);
}

AppSecret::AppSecret(const AppSecret &other) {
  memcpy((void*) _appSecret_bin, (void*) other._appSecret_bin, sizeof(appSecret_bin_t));
}

String AppSecret::toString() const {
  char temp[APPSECRET_STRLEN+1];
  sprintf(temp, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x-%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
    _appSecret_bin[31], _appSecret_bin[30], _appSecret_bin[29], _appSecret_bin[28],
    _appSecret_bin[27], _appSecret_bin[26], _appSecret_bin[25], _appSecret_bin[24],
    _appSecret_bin[23], _appSecret_bin[22], _appSecret_bin[21], _appSecret_bin[20],
    _appSecret_bin[19], _appSecret_bin[18], _appSecret_bin[17], _appSecret_bin[16],
    _appSecret_bin[15], _appSecret_bin[14], _appSecret_bin[13], _appSecret_bin[12],
    _appSecret_bin[11], _appSecret_bin[10], _appSecret_bin[ 9], _appSecret_bin[ 8],
    _appSecret_bin[ 7], _appSecret_bin[ 6], _appSecret_bin[ 5], _appSecret_bin[ 4],
    _appSecret_bin[ 3], _appSecret_bin[ 2], _appSecret_bin[ 1], _appSecret_bin[ 0]
  );
  return String(temp);
}

AppSecret AppSecret::operator=(const AppSecret &other) {
  memcpy((void*) _appSecret_bin, (void*) other._appSecret_bin, sizeof(appSecret_bin_t));
  return *this;
}

bool AppSecret::operator==(const AppSecret &other) {
  for (size_t i=0; i<APPSECRET_BINLEN; i++) {
    if (_appSecret_bin[i] != other._appSecret_bin[i]) return false;
  }
  return true;
}

bool AppSecret::operator==(const String &other) {
  return operator==((AppSecret) other);
}

bool AppSecret::operator==(const char* other) {
  return operator==((AppSecret) other);
}

bool AppSecret::isValid() const {
  for (size_t i=0; i<sizeof(appSecret_bin_t); i++) {
    if (_appSecret_bin[i] != 0) return true;
  }
  return false;
}

void AppSecret::fromString(const char* other) {
  char tmp;
  bool _isValid = (sscanf(other, "%2hhx%2hhx%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%c",
    &_appSecret_bin[31], &_appSecret_bin[30], &_appSecret_bin[29], &_appSecret_bin[28],
    &_appSecret_bin[27], &_appSecret_bin[26], &_appSecret_bin[25], &_appSecret_bin[24],
    &_appSecret_bin[23], &_appSecret_bin[22], &_appSecret_bin[21], &_appSecret_bin[20],
    &_appSecret_bin[19], &_appSecret_bin[18], &_appSecret_bin[17], &_appSecret_bin[16],
    &_appSecret_bin[15], &_appSecret_bin[14], &_appSecret_bin[13], &_appSecret_bin[12],
    &_appSecret_bin[11], &_appSecret_bin[10], &_appSecret_bin[ 9], &_appSecret_bin[ 8],
    &_appSecret_bin[ 7], &_appSecret_bin[ 6], &_appSecret_bin[ 5], &_appSecret_bin[ 4],
    &_appSecret_bin[ 3], &_appSecret_bin[ 2], &_appSecret_bin[ 1], &_appSecret_bin[ 0],
    &tmp
  ) == sizeof(appSecret_bin_t)) && (strlen(other) == APPSECRET_STRLEN);
  if (!_isValid) memset((void*) &_appSecret_bin, 0, sizeof(appSecret_bin_t));

}


#endif