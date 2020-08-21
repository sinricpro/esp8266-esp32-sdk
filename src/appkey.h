#ifndef _APPKEY_H_
#define _APPKEY_H_

#define APPKEY_BINLEN 16
#define APPKEY_STRLEN 36

class appKey_t {
  public:
    typedef uint8_t appKey_bin_t[APPKEY_BINLEN];
    appKey_t();
    appKey_t(const char* appKey);
    appKey_t(const String &appKey) { appKey_t(appKey.c_str()); }
    appKey_t(const appKey_t &other);

    appKey_t operator=(const appKey_t &other);
    
    String toString();
    bool isValid();
  private:
    appKey_bin_t _appKey_bin;
};

appKey_t::appKey_t() : _appKey_bin{} {}

appKey_t::appKey_t(const char* appKey) {
  char tmp;
  bool _isValid = (sscanf(appKey,"%2hhx%2hhx%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%c",
    &_appKey_bin[15], &_appKey_bin[14], &_appKey_bin[13], &_appKey_bin[12],
    &_appKey_bin[11], &_appKey_bin[10], &_appKey_bin[9],  &_appKey_bin[8],
    &_appKey_bin[7],  &_appKey_bin[6],  &_appKey_bin[5],  &_appKey_bin[4],
    &_appKey_bin[3],  &_appKey_bin[2],  &_appKey_bin[1],  &_appKey_bin[0],
    &tmp
  ) == sizeof(appKey_bin_t)) && (strlen(appKey) == APPKEY_STRLEN);
  if (!_isValid) memset((void*) &_appKey_bin, 0, sizeof(appKey_bin_t));
}

appKey_t::appKey_t(const appKey_t &other) {
  memcpy((void*) _appKey_bin, (void*) other._appKey_bin, sizeof(appKey_bin_t));
}

String appKey_t::toString() {
  char temp[APPKEY_STRLEN+1];
  sprintf(temp, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
    _appKey_bin[15], _appKey_bin[14], _appKey_bin[13], _appKey_bin[12],
    _appKey_bin[11], _appKey_bin[10], _appKey_bin[9],  _appKey_bin[8],
    _appKey_bin[7],  _appKey_bin[6],  _appKey_bin[5],  _appKey_bin[4],
    _appKey_bin[3],  _appKey_bin[2],  _appKey_bin[1],  _appKey_bin[0]
  );
  return String(temp);
}

appKey_t appKey_t::operator=(const appKey_t &other) {
  memcpy((void*) _appKey_bin, (void*) other._appKey_bin, sizeof(appKey_bin_t));
  return *this;
}

bool appKey_t::isValid() {
  for (size_t i=0; i<sizeof(appKey_bin_t); i++) {
    if (_appKey_bin[i] != 0) return true;
  }
  return false;
}



#endif