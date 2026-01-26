/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#pragma once

#include <Arduino.h>
#include <stdarg.h>

// Check if Serial.printf is available
// Arduino UNO R4 WiFi, Nano 33 IoT, MKR WiFi 1010 don't have Serial.printf
#if defined(ARDUINO_UNOWIFIR4) || defined(ARDUINO_MINIMA) || defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT)
  #define SINRICPRO_NO_SERIAL_PRINTF
#endif

/**
 * @brief Portable printf function that works on all Arduino boards
 *
 * Use this instead of Serial.printf() for cross-platform compatibility.
 * Works on ESP8266, ESP32, RP2040, Arduino UNO R4 WiFi, Nano 33 IoT, MKR WiFi 1010
 *
 * @param format printf-style format string
 * @param ... variable arguments
 */
inline void SINRICPRO_PRINTF(const char* format, ...) {
  char buf[256];
  va_list args;
  va_start(args, format);
  vsnprintf(buf, sizeof(buf), format, args);
  va_end(args);
  Serial.print(buf);
}

#ifndef NODEBUG_SINRIC
#ifdef DEBUG_ESP_PORT
  #ifdef SINRICPRO_NO_SERIAL_PRINTF
    // For boards without printf, use portable version
    #define DEBUG_SINRIC(...) do { \
      char _dbg_buf[256]; \
      snprintf(_dbg_buf, sizeof(_dbg_buf), __VA_ARGS__); \
      DEBUG_ESP_PORT.print(_dbg_buf); \
    } while(0)
  #else
    #define DEBUG_SINRIC(...) DEBUG_ESP_PORT.printf( __VA_ARGS__ )
  #endif
#else
//#define DEBUG_WEBSOCKETS(...) os_printf( __VA_ARGS__ )
#endif
#endif


#ifndef DEBUG_SINRIC
#define DEBUG_SINRIC(...)
#define NODEBUG_SINRIC
#endif