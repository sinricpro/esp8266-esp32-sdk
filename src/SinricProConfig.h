/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#pragma once
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

#pragma once

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

// EventLimiter Configuration
#define EVENT_LIMIT_STATE         1000
#define EVENT_LIMIT_SENSOR_STATE  EVENT_LIMIT_STATE
#define EVENT_LIMIT_SENSOR_VALUE  60000
