/*
 *  Copyright (c) 2019 Sinric. All rights reserved.
 *  Licensed under Creative Commons Attribution-Share Alike (CC BY-SA)
 *
 *  This file is part of the Sinric Pro (https://github.com/sinricpro/)
 */

#ifndef __SINRICPRO_CONFIG_H__
#define __SINRICPRO_CONFIG_H__

#define SDK_VERSION "2.2.2"

#define SINRICPRO_SERVER_URL "ws.sinric.pro"
#define SINRICPRO_SERVER_PORT 80

#define UDP_MULTICAST_IP IPAddress(224,9,9,9)
#define UDP_MULTICAST_PORT 3333

// websocket sends every WEBSOCKET_PING_INTERVAL milliseconds a ping to Server
// if there is no pong received after WEBSOCKET_PING_TIMEOUT milliseconds, retry count is incremented by one
// if retry count reaches WEBSOCKET_RETRY_COUNT websocket connection is closed and try to reconnect server
#define WEBSOCKET_PING_INTERVAL 300000
#define WEBSOCKET_PING_TIMEOUT 10000
#define WEBSOCKET_RETRY_COUNT 2

#endif
