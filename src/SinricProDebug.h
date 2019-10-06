#ifndef __SINRICPRODEBUG_H__
#define __SINRICPRODEBUG_H__

#ifndef NODEBUG_SINRIC
#ifdef DEBUG_ESP_PORT
#define DEBUG_SINRIC(...) DEBUG_ESP_PORT.printf( __VA_ARGS__ )
#else
//#define DEBUG_WEBSOCKETS(...) os_printf( __VA_ARGS__ )
#endif
#endif


#ifndef DEBUG_SINRIC
#define DEBUG_SINRIC(...)
#define NODEBUG_SINRIC
#endif


#endif
