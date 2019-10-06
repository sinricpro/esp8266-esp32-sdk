#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include "NTP.h"
#include "SinricProQueue.h"

extern QueueList<SinricProMessage*> receiveQueue;
extern QueueList<SinricProMessage*> sendQueue;

#endif // _GLOBALS_H_
