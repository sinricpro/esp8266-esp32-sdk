#include "AESLib.h"

uint8_t AESLib::getrnd()
{
   uint8_t really_random = *(volatile uint8_t *)0x3FF20E44;
   return really_random;
}

void AESLib::gen_iv(byte  *iv) {
    for (int i = 0 ; i < N_BLOCK ; i++ ) {
        iv[i]= (byte) getrnd();
    }
}

/*String AESLib::decrypt(String msg, byte key[], byte my_iv[]) {
  
  
  aes.set_key(key, sizeof(key));
  //int base64_decode(char * output, char * input, int inputLen) {
  char decoded[200];
  char temp[200];
  msg.toCharArray(temp, 200);
  
  base64_decode(decoded, temp, msg.length());
  
  byte out[200];
  aes.do_aes_decrypt((byte *)decoded, N_BLOCK, out, key, 128, (byte *)my_iv);
  
  char message[msg.length()];
  base64_decode(message, (char *)out, msg.length());
  printf("Out %s \n", message);
  
  //Serial.println("Decoded: " + String(decoded));  
  
  //N_BLOCK

}*/

/*void decrypt(String b64data, String IV_base64, int size)
{
  char data_decoded[200];
  char iv_decoded[200];
  byte out[200];
  char temp[200];
  b64data.toCharArray(temp, 200);
  base64_decode(data_decoded, temp, b64data.length());
  IV_base64.toCharArray(temp, 200);
  
  base64_decode(iv_decoded, temp, IV_base64.length());
  
  aes.do_aes_decrypt((byte *)data_decoded, size, out, key, 128, (byte *)iv_decoded);
  char message[msg.length()];
  base64_decode(message, (char *)out, b64data.length());
  printf("Out %s \n", message);
}*/



String AESLib::encrypt(String msg, byte key[], byte my_iv[]) {
  char b64data[200];
  byte cipher[1000];
    
  aes.set_key( key , sizeof(key));
  base64_encode(b64data, (char *)my_iv, N_BLOCK);
  int b64len = base64_encode(b64data, (char *)msg.c_str(),msg.length());
  // Encrypt! With AES128, our key and IV, CBC and pkcs7 padding    
  aes.do_aes_encrypt((byte *)b64data, b64len , cipher, key, 128, my_iv);
  base64_encode(b64data, (char *)cipher, aes.get_size() );  
  return String(b64data);  
}