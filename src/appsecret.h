#ifndef _APPSECRET_H_
#define _APPSECRET_H_

#define APPSECRET_BINLEN 32
#define APPSECRET_STRLEN 73

class appSecret_t {
  public:
    typedef uint8_t appSecret_bin_t[32];
    appSecret_t();
    appSecret_t(const char* appSecret);
    appSecret_t(const String &appSecret) { appSecret_t(appSecret.c_str()); }
    appSecret_t(const appSecret_t &other);

    appSecret_t operator=(const appSecret_t &other);
    
    String toString();
    bool isValid();
  private:
    appSecret_bin_t _appSecret_bin;
};

appSecret_t::appSecret_t() : _appSecret_bin{} {}

appSecret_t::appSecret_t(const char* appSecret) {
  char tmp;
  bool _isValid = (sscanf(appSecret, "%2hhx%2hhx%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%c",
    &_appSecret_bin[31], &_appSecret_bin[30], &_appSecret_bin[29], &_appSecret_bin[28],
    &_appSecret_bin[27], &_appSecret_bin[26], &_appSecret_bin[25], &_appSecret_bin[24],
    &_appSecret_bin[23], &_appSecret_bin[22], &_appSecret_bin[21], &_appSecret_bin[20],
    &_appSecret_bin[19], &_appSecret_bin[18], &_appSecret_bin[17], &_appSecret_bin[16],
    &_appSecret_bin[15], &_appSecret_bin[14], &_appSecret_bin[13], &_appSecret_bin[12],
    &_appSecret_bin[11], &_appSecret_bin[10], &_appSecret_bin[ 9], &_appSecret_bin[ 8],
    &_appSecret_bin[ 7], &_appSecret_bin[ 6], &_appSecret_bin[ 5], &_appSecret_bin[ 4],
    &_appSecret_bin[ 3], &_appSecret_bin[ 2], &_appSecret_bin[ 1], &_appSecret_bin[ 0],
    &tmp
  ) == sizeof(appSecret_bin_t)) && (strlen(appSecret) == APPSECRET_STRLEN);
  if (!_isValid) memset((void*) &_appSecret_bin, 0, sizeof(appSecret_bin_t));
}

appSecret_t::appSecret_t(const appSecret_t &other) {
  memcpy((void*) _appSecret_bin, (void*) other._appSecret_bin, sizeof(appSecret_bin_t));
}

String appSecret_t::toString() {
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

appSecret_t appSecret_t::operator=(const appSecret_t &other) {
  memcpy((void*) _appSecret_bin, (void*) other._appSecret_bin, sizeof(appSecret_bin_t));
  return *this;
}

bool appSecret_t::isValid() {
  for (size_t i=0; i<sizeof(appSecret_bin_t); i++) {
    if (_appSecret_bin[i] != 0) return true;
  }
  return false;
}



#endif