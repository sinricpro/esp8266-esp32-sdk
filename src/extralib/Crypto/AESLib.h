#ifndef AESLib_h
#define AESLib_h

#include "Arduino.h"
#include "AES.h"
#include "Base64.h"

class AESLib
{
  public:    
    void gen_iv(byte  *iv);
    String encrypt(String msg, byte key[], byte my_iv[]);
    //String decrypt(String msg, byte key[], byte my_iv[]);
        
  private:
    uint8_t getrnd();
    AES aes;
};


#endif
