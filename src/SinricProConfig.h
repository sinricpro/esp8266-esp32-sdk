/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef __SINRICPRO_CONFIG_H__
#define __SINRICPRO_CONFIG_H__
/* 
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * !!                                                 !!
 * !!             WARNING: DON'T TOUCH !              !!
 * !!             ======================              !!
 * !! PLEASE DO NOT MODIFY ANY OF THESE SETTINGS HERE !!
 * !!     THIS IS FOR INTERNAL CONFIGURATION ONLY     !!
 * !!   SINRIC PRO MIGHT NOT WORK IF YOU MODIFY THIS  !!
 * !!                                                 !!
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

// Version Configuration
#define SINRICPRO_VERSION_MAJOR     2
#define SINRICPRO_VERSION_MINOR     9
#define SINRICPRO_VERSION_REVISION  4
#define SINRICPRO_VERSION STR(SINRICPRO_VERSION_MAJOR) "." STR(SINRICPRO_VERSION_MINOR) "." STR(SINRICPRO_VERSION_REVISION)
#define SINRICPRO_VERSION_STR "SinricPro (v" SINRICPRO_VERSION ")"
#define SINRICPRO_VERISON_INT SINRICPRO_VERSION_MAJOR * 1000000 + SINRICPRO_VERSION_MINOR * 1000 + SINRICPRO_VERSION_REVISION

// Server Configuration
#ifndef SINRICPRO_NOSSL
#define WEBSOCKET_SSL
#endif

#define SINRICPRO_SERVER_URL "ws.sinric.pro"
#define SINRICPRO_SERVER_PORT 80
#define SINRICPRO_SERVER_SSL_PORT   443

// UDP Configuration
#define UDP_MULTICAST_IP IPAddress(224,9,9,9)
#define UDP_MULTICAST_PORT 3333

// WebSocket Configuration
#ifdef DEBUG_WIFI_ISSUE
  #define WEBSOCKET_PING_INTERVAL 10000
#else
  #define WEBSOCKET_PING_INTERVAL 300000
#endif
#define WEBSOCKET_PING_TIMEOUT 10000
#define WEBSOCKET_RETRY_COUNT 2

// LeakyBucket Configuration
#define BUCKET_SIZE 10
#define DROP_OUT_TIME 60000
#define DROP_IN_TIME 1000u

#endif
