/*
 * This example needs SinricPro 2.2.2
 * 
 * If you encounter any issues:
 * - enable serial debug (see section below)
 * - open serial monitor and check whats happening
 * - visit https://github.com/sinricpro/esp8266-esp32-sdk/issues and check for existing issues or open a new one
*/

// Uncomment the following line to enable serial debug output
//#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
       #define DEBUG_ESP_PORT Serial
       #define NODEBUG_WEBSOCKETS
       #define NDEBUG
#endif 

#include <Arduino.h>
#ifdef ESP8266 
       #include <ESP8266WiFi.h>
#endif 
#ifdef ESP32   
       #include <WiFi.h>
#endif

#include "SinricPro.h"
#include "SinricProLock.h"

#define WIFI_SSID         "YOUR_WIFI_SSID"    
#define WIFI_PASS         "YOUR_WIFI_PASSWORD"
#define APP_KEY           "YOUR_APP_KEY_HERE"      // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
#define APP_SECRET        "YOUR_APP_SECRET_HERE"   // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"
#define GARAGEDOOR_ID     "YOUR_DEVICE_ID_HERE"    // Should look like "5dc1564130xxxxxxxxxxxxxx"
#define BAUD_RATE         9600                     // Change baudrate to your need

#define RELAY             D5    // PIN where relay is attachted to
#define ENDSTOP_OPEN      D6    // PIN where open position reedswitch is connected to  (high = door is open   | low = door is NOT open)
#define ENDSTOP_CLOSED    D7    // PIN where close position reedswitch is connected to (high = door is closed | low = door is NOT closed)
#define MOVE_TIMEOUT      20000 // Timout (20 seconds) for door movement -> please use a value which fits to your open/close time but not more than 20 seconds
#define RELAY_TIME        4000  // Relay high time (4 seconds) - usually between 2500 and 5000


// Door state definitions
#define DOOR_MOVING       0  // Door is not in close and not in open position...somewere between, might be moving
#define DOOR_OPEN         1  // Door is in open position
#define DOOR_CLOSED       2  // Door is in closed position
#define DOOR_MALFUNCTION  3  // Malfunction! Door can't be open and closed at the same time

/* function getDoorState() 
 * returs door state (see section above)
 */
int getDoorState() {
  if (digitalRead(ENDSTOP_CLOSED)==false && digitalRead(ENDSTOP_OPEN)==false) return DOOR_MOVING;
  if (digitalRead(ENDSTOP_CLOSED)==false && digitalRead(ENDSTOP_OPEN)==true)  return DOOR_OPEN;
  if (digitalRead(ENDSTOP_CLOSED)==true  && digitalRead(ENDSTOP_OPEN)==false) return DOOR_CLOSED;
  return DOOR_MALFUNCTION;
}

int lastDoorState; // last known doorstate

/* turnOnRelayAndWait
 *
 * function: turn on relay for a given time and waits until endstop gets triggered or timeout happened
 *
 * input:    relay_pin   = pin which is RELAY connected to
 *           relay_time  = time in milliseconds to keep RELAY high
 *           endstop_pin = pin which is MAGNET connected to
 *           endstop_timeout = timeout time in milliseconds
 * 
 * return:   true if enstop is triggered within timeout time
 *           false if endstop was not triggered within timeout time
*/
bool turnOnRelayAndWait(int relay_pin, unsigned long relay_time, int endstop_pin, unsigned long endstop_timeout) {
  unsigned long startMillis = millis();

  bool timeout = false; // timeout flag
  bool success = false; // success flag
  Serial.printf("RELAY: HIGH\r\n");

  digitalWrite(relay_pin, HIGH); // turn on relay
  bool relay_High = true; // relay flag

  while (success==false && timeout==false) { // while operation is not completed and within timeout time
    unsigned long actualMillis = millis();  

    success = digitalRead(endstop_pin);                        // check endstop
    timeout = (actualMillis - startMillis >= endstop_timeout); // check for timeout

    // if relay_time is reached, turn off relay
    if (relay_High == true && actualMillis - startMillis >= relay_time){
      Serial.printf("RELAY: LOW\r\n");
      digitalWrite(relay_pin, LOW); 
      relay_High = false;
    }
    yield(); // make sure to keep esp stay alive ;)
  }
  digitalWrite(relay_pin, LOW); // turn off relay, just to be safe!
  lastDoorState = getDoorState(); // update lastDoorState to prevent event sending
  return success; 
}

bool onLockState(String deviceId, bool &lockState) {
  int doorState = getDoorState();
  if (doorState == DOOR_MALFUNCTION){
    Serial.printf("Malfunction! Door is reporting to be open and closed at the same time!\r\n");
    SinricPro.setResponseMessage("Error: malfunction!");
    return false; 
  } 
  bool success = false;
  if (lockState) { // close door
    if (doorState == DOOR_CLOSED) return true; // door is allready closed return true
    Serial.printf("Closing door...\r\n");
    success = turnOnRelayAndWait(RELAY, RELAY_TIME, ENDSTOP_CLOSED, MOVE_TIMEOUT); // close the door
    Serial.printf("%s\r\n", success?"Door is closed now.":"Error! Door did not close properly (timeout)!");
  } else { // open door
    if (doorState == DOOR_OPEN) return true; // door is allready open...return true
    Serial.printf("Opening door...\r\n");
    success = turnOnRelayAndWait(RELAY, RELAY_TIME, ENDSTOP_OPEN, MOVE_TIMEOUT); // open the door
    Serial.printf("%s\r\n", success?"Door is open now.":"Error! Door did not open properly (timeout)!");
  }
  if (!success) SinricPro.setResponseMessage("Error: timeout!");
  return success;
}

void handleManualDoorState() {
  int actualDoorState = getDoorState();
  if (actualDoorState != lastDoorState) {
    SinricProLock& garageDoor = SinricPro[GARAGEDOOR_ID];
    switch (actualDoorState) {
      case DOOR_OPEN   :  Serial.printf("Door has been opened manually.\r\n");
                          garageDoor.sendLockStateEvent(false);
                          break;
      case DOOR_CLOSED :  garageDoor.sendLockStateEvent(true);
                          Serial.printf("Door has been closed manually.\r\n");
                          break;
      case DOOR_MOVING :  Serial.printf("Door is moving.\r\n");
                          break;
      default          :  Serial.printf("WARNING! DOOR HAS A MALFUNCTION!\r\n");
                          break;
    }
    lastDoorState = actualDoorState;    
  }
}


void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(250);
  }
  IPAddress localIP = WiFi.localIP();
  Serial.printf("connected!\r\n[WiFi]: IP-Address is %d.%d.%d.%d\r\n", localIP[0], localIP[1], localIP[2], localIP[3]);
}

void setupSinricPro() {
  SinricProLock &garageDoor = SinricPro[GARAGEDOOR_ID];
  garageDoor.onLockState(onLockState);

  SinricPro.begin(APP_KEY, APP_SECRET);
}

void setup() {
  Serial.begin(BAUD_RATE);
  pinMode(RELAY, OUTPUT); 
  pinMode(ENDSTOP_OPEN, INPUT); 
  pinMode(ENDSTOP_CLOSED, INPUT);
  lastDoorState = getDoorState();
  setupWiFi();
  setupSinricPro();
}

void loop() {
  SinricPro.handle();
  handleManualDoorState();
}
