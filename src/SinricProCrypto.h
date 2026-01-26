#pragma once

/**
 * @file SinricProCrypto.h
 * @brief Portable SHA256 and HMAC-SHA256 implementation for boards without native crypto support
 *
 * This implementation is used for:
 * - Arduino UNO R4 WiFi (Renesas RA4M1)
 * - Arduino Nano 33 IoT (SAMD21)
 * - Arduino MKR WiFi 1010 (SAMD21)
 *
 * ESP8266/RP2040 use BearSSL, ESP32 uses mbedtls (both built into their cores)
 */

#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_UNOWIFIR4) || defined(ARDUINO_MINIMA)

#include <Arduino.h>
#include <stdint.h>
#include <string.h>

namespace SINRICPRO_CRYPTO {

// SHA256 constants
static const uint32_t SHA256_K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

// SHA256 helper macros
#define ROTR(x, n) (((x) >> (n)) | ((x) << (32 - (n))))
#define CH(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define EP1(x) (ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define SIG0(x) (ROTR(x, 7) ^ ROTR(x, 18) ^ ((x) >> 3))
#define SIG1(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ ((x) >> 10))

class SHA256 {
public:
    static const size_t BLOCK_SIZE = 64;
    static const size_t HASH_SIZE = 32;

    SHA256() { reset(); }

    void reset() {
        state[0] = 0x6a09e667;
        state[1] = 0xbb67ae85;
        state[2] = 0x3c6ef372;
        state[3] = 0xa54ff53a;
        state[4] = 0x510e527f;
        state[5] = 0x9b05688c;
        state[6] = 0x1f83d9ab;
        state[7] = 0x5be0cd19;
        count = 0;
        bufferOffset = 0;
    }

    void update(const uint8_t* data, size_t len) {
        while (len--) {
            buffer[bufferOffset++] = *data++;
            count += 8;
            if (bufferOffset == BLOCK_SIZE) {
                processBlock();
                bufferOffset = 0;
            }
        }
    }

    void update(const char* data, size_t len) {
        update((const uint8_t*)data, len);
    }

    void finalize(uint8_t* hash) {
        // Pad message
        buffer[bufferOffset++] = 0x80;
        if (bufferOffset > 56) {
            while (bufferOffset < BLOCK_SIZE) buffer[bufferOffset++] = 0;
            processBlock();
            bufferOffset = 0;
        }
        while (bufferOffset < 56) buffer[bufferOffset++] = 0;

        // Append length
        for (int i = 7; i >= 0; i--) {
            buffer[bufferOffset++] = (count >> (i * 8)) & 0xff;
        }
        processBlock();

        // Output hash
        for (int i = 0; i < 8; i++) {
            hash[i * 4] = (state[i] >> 24) & 0xff;
            hash[i * 4 + 1] = (state[i] >> 16) & 0xff;
            hash[i * 4 + 2] = (state[i] >> 8) & 0xff;
            hash[i * 4 + 3] = state[i] & 0xff;
        }
    }

    static void hash(const uint8_t* data, size_t len, uint8_t* output) {
        SHA256 ctx;
        ctx.update(data, len);
        ctx.finalize(output);
    }

private:
    uint32_t state[8];
    uint64_t count;
    uint8_t buffer[BLOCK_SIZE];
    size_t bufferOffset;

    void processBlock() {
        uint32_t w[64];
        uint32_t a, b, c, d, e, f, g, h, t1, t2;

        // Prepare message schedule
        for (int i = 0; i < 16; i++) {
            w[i] = ((uint32_t)buffer[i * 4] << 24) |
                   ((uint32_t)buffer[i * 4 + 1] << 16) |
                   ((uint32_t)buffer[i * 4 + 2] << 8) |
                   ((uint32_t)buffer[i * 4 + 3]);
        }
        for (int i = 16; i < 64; i++) {
            w[i] = SIG1(w[i - 2]) + w[i - 7] + SIG0(w[i - 15]) + w[i - 16];
        }

        // Initialize working variables
        a = state[0]; b = state[1]; c = state[2]; d = state[3];
        e = state[4]; f = state[5]; g = state[6]; h = state[7];

        // Main loop
        for (int i = 0; i < 64; i++) {
            t1 = h + EP1(e) + CH(e, f, g) + SHA256_K[i] + w[i];
            t2 = EP0(a) + MAJ(a, b, c);
            h = g; g = f; f = e; e = d + t1;
            d = c; c = b; b = a; a = t1 + t2;
        }

        // Update state
        state[0] += a; state[1] += b; state[2] += c; state[3] += d;
        state[4] += e; state[5] += f; state[6] += g; state[7] += h;
    }
};

#undef ROTR
#undef CH
#undef MAJ
#undef EP0
#undef EP1
#undef SIG0
#undef SIG1

class HMAC_SHA256 {
public:
    static const size_t HASH_SIZE = 32;
    static const size_t BLOCK_SIZE = 64;

    void init(const uint8_t* key, size_t keyLen) {
        uint8_t keyBlock[BLOCK_SIZE];
        memset(keyBlock, 0, BLOCK_SIZE);

        if (keyLen > BLOCK_SIZE) {
            SHA256::hash(key, keyLen, keyBlock);
        } else {
            memcpy(keyBlock, key, keyLen);
        }

        // Create inner and outer padded keys
        for (size_t i = 0; i < BLOCK_SIZE; i++) {
            ipad[i] = keyBlock[i] ^ 0x36;
            opad[i] = keyBlock[i] ^ 0x5c;
        }

        // Start inner hash
        innerCtx.reset();
        innerCtx.update(ipad, BLOCK_SIZE);
    }

    void update(const uint8_t* data, size_t len) {
        innerCtx.update(data, len);
    }

    void update(const char* data, size_t len) {
        update((const uint8_t*)data, len);
    }

    void finalize(uint8_t* output) {
        uint8_t innerHash[HASH_SIZE];
        innerCtx.finalize(innerHash);

        // Outer hash
        SHA256 outerCtx;
        outerCtx.update(opad, BLOCK_SIZE);
        outerCtx.update(innerHash, HASH_SIZE);
        outerCtx.finalize(output);
    }

    static void compute(const uint8_t* key, size_t keyLen,
                        const uint8_t* data, size_t dataLen,
                        uint8_t* output) {
        HMAC_SHA256 ctx;
        ctx.init(key, keyLen);
        ctx.update(data, dataLen);
        ctx.finalize(output);
    }

private:
    SHA256 innerCtx;
    uint8_t ipad[BLOCK_SIZE];
    uint8_t opad[BLOCK_SIZE];
};

// Base64 encoding for platforms without libb64
class Base64 {
public:
    static size_t encodedLength(size_t inputLen) {
        return ((inputLen + 2) / 3) * 4 + 1;
    }

    static size_t encode(const uint8_t* input, size_t inputLen, char* output) {
        static const char base64Chars[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

        size_t outputLen = 0;
        size_t i = 0;

        while (i < inputLen) {
            uint32_t octet_a = i < inputLen ? input[i++] : 0;
            uint32_t octet_b = i < inputLen ? input[i++] : 0;
            uint32_t octet_c = i < inputLen ? input[i++] : 0;

            uint32_t triple = (octet_a << 16) + (octet_b << 8) + octet_c;

            output[outputLen++] = base64Chars[(triple >> 18) & 0x3F];
            output[outputLen++] = base64Chars[(triple >> 12) & 0x3F];
            output[outputLen++] = base64Chars[(triple >> 6) & 0x3F];
            output[outputLen++] = base64Chars[triple & 0x3F];
        }

        // Add padding
        size_t mod = inputLen % 3;
        if (mod == 1) {
            output[outputLen - 1] = '=';
            output[outputLen - 2] = '=';
        } else if (mod == 2) {
            output[outputLen - 1] = '=';
        }

        output[outputLen] = '\0';
        return outputLen;
    }
};

} // namespace SINRICPRO_CRYPTO

#endif // ARDUINO_SAMD_* || ARDUINO_UNOWIFIR4 || ARDUINO_MINIMA
