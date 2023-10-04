/*
 * 
 * Example for how to use SinricPro Camera device:
 * - update the Sinric Pro ESP SDK to v2.5.1 or newer
 * - update the Sinric Pro App to v2.7.1 or newer
 * - setup a camera device
 * - handle request using callback (turn on/off builtin led indicating device power state)
 * 
 * If you encounter any issues:
 * - check the readme.md at https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md
 * - ensure all dependent libraries are installed
 *   - see https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md#arduinoide
 *   - see https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md#dependencies
 * - open serial monitor and check whats happening
 * - check full user documentation at https://sinricpro.github.io/esp8266-esp32-sdk
 * - visit https://github.com/sinricpro/esp8266-esp32-sdk/issues and check for existing issues or open a new one
 */

// Uncomment the following line to enable serial debug output
#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
  #define DEBUG_ESP_PORT Serial
  #define NODEBUG_WEBSOCKETS
  #define NDEBUG
#endif 

//#define ENABLE_STATIC_IP

#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>

#include "SinricPro.h"
#include "SinricProCamera.h"

// Micro-RTSP from https://github.com/geeksville/Micro-RTSP 
#include "SimStreamer.h"
#include "OV2640Streamer.h"
#include "CRtspSession.h"

// Select camera model

//#define CAMERA_MODEL_WROVER_KIT
//#define CAMERA_MODEL_ESP_EYE
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_V2_PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE
//#define CAMERA_MODEL_M5STACK_ESP32CAM
//#define CAMERA_MODEL_M5STACK_UNITCAM
//#define CAMERA_MODEL_AI_THINKER
//#define CAMERA_MODEL_TTGO_T_JOURNAL
//#define CAMERA_MODEL_ESP32_CAM_BOARD
//#define CAMERA_MODEL_ESP32S3_CAM_LCD
//#define CAMERA_MODEL_ESP32S2_CAM_BOARD
//#define CAMERA_MODEL_ESP32S3_EYE

/*
Notice:

With latest version of ESP32 Core Micro-RTSP does not compile. To fix it

1. Goto C:\Users\<username>\AppData\Local\Arduino15\packages\esp32\hardware\esp32\<version>

2. Make a copy of platform.txt and rename it to platform.local.txt.

3. Open platform.txt. Change

# Arduino Compile Warning Levels
compiler.warning_flags=-w
compiler.warning_flags.none=-w
compiler.warning_flags.default=
compiler.warning_flags.more=-Wall -Werror=all
compiler.warning_flags.all=-Wall -Werror=all -Wextra

To:

# Arduino Compile Warning Levels
compiler.warning_flags=-w
compiler.warning_flags.none=-w
compiler.warning_flags.default=
compiler.warning_flags.more=-Wall
compiler.warning_flags.all=-Wall -Wextra

4. Restart Arduino IDE !!!!

*/


#include "camera_pins.h"
 
#define WIFI_SSID         "YOUR-WIFI-SSID"
#define WIFI_PASSWD       "YOUR-WIFI-PASSWORD"

#define APP_KEY           "YOUR-APP-KEY"      // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx". Get it from https://portal.sinric.pro/ -> Credentials
#define APP_SECRET        "YOUR-APP-SECRET"   // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx" . Get it from https://portal.sinric.pro/ -> Credentials
#define CAMERA_ID         "YOUR-DEVICE-ID"    // Should look like "5dc1564130xxxxxxxxxxxxxx". Get it from https://portal.sinric.pro/ -> Devices
#define BAUD_RATE         115200 // Change baudrate to your need

OV2640 cam;
CStreamer *streamer;

WiFiServer rtspServer(8554);

#ifdef ENABLE_STATIC_IP
  // Optional but important for stability. 
  IPAddress local_IP(192, 168, 1, 124);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress primaryDNS(8, 8, 8, 8);   //optional
  IPAddress secondaryDNS(8, 8, 4, 4); //optional
#endif 

bool onPowerState(const String &deviceId, bool &state) {
  Serial.printf("Device %s turned %s (via SinricPro) \r\n", deviceId.c_str(), state?"on":"off");
  return true; // request handled properly
}

// setup function for SinricPro
void setupSinricPro() {
  // add device to SinricPro
  SinricProCamera& mySwitch = SinricPro[CAMERA_ID];

  // set callback function to device
  mySwitch.onPowerState(onPowerState);

  // setup SinricPro
  SinricPro.onConnected([](){ Serial.printf("Connected to SinricPro\r\n"); }); 
  SinricPro.onDisconnected([](){ Serial.printf("Disconnected from SinricPro\r\n"); });
  SinricPro.begin(APP_KEY, APP_SECRET);
}

void setupWiFi() {
  IPAddress ip;
  
  #ifdef ENABLE_STATIC_IP
    // Configures static IP address
    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
      Serial.println("Failed to configure IP");
    }
  #endif 

  WiFi.begin(WIFI_SSID, WIFI_PASSWD);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setupCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; //PIXFORMAT_YUV422 PIXFORMAT_GRAYSCALE PIXFORMAT_RGB565 PIXFORMAT_JPEG
 

/* 
    FRAMESIZE_96X96,    // 96x96
    FRAMESIZE_QQVGA,    // 160x120
    FRAMESIZE_QCIF,     // 176x144
    FRAMESIZE_HQVGA,    // 240x176
    FRAMESIZE_240X240,  // 240x240
    FRAMESIZE_QVGA,     // 320x240
    FRAMESIZE_CIF,      // 400x296
    FRAMESIZE_HVGA,     // 480x320
    FRAMESIZE_VGA,      // 640x480
    FRAMESIZE_SVGA,     // 800x600
    FRAMESIZE_XGA,      // 1024x768
    FRAMESIZE_HD,       // 1280x720
    FRAMESIZE_SXGA,     // 1280x1024
    FRAMESIZE_UXGA,     // 1600x1200
  */

  if(psramFound()){
    Serial.println("psram found");
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 10; //10-63 lower number means higher quality
    config.fb_count = 2;
  } else {
    Serial.println("psram not found");
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  cam.init(config);
}

void setupStreaming() {
    rtspServer.begin();  
    streamer = new OV2640Streamer(cam);             // our streamer for UDP/TCP based RTP transport
    Serial.print("Use 'rtsp://"); Serial.print(WiFi.localIP()); Serial.println(":8554/mjpeg/1' to stream via VLC");
}

void handleStreaming() {
    uint32_t msecPerFrame = 100;
    static uint32_t lastimage = millis();

    // If we have an active client connection, just service that until gone
    streamer->handleRequests(0); // we don't use a timeout here,
    // instead we send only if we have new enough frames
    uint32_t now = millis();
    if(streamer->anySessions()) {
        if(now > lastimage + msecPerFrame || now < lastimage) { // handle clock rollover
            streamer->streamImage(now);
            lastimage = now;

            // check if we are overrunning our max frame rate
            now = millis();
            if(now > lastimage + msecPerFrame) {
                printf("warning exceeding max frame rate of %d ms\n", now - lastimage);
            }
        }
    }
    
    WiFiClient rtspClient = rtspServer.accept();
    if(rtspClient) {
        Serial.print("client: ");
        Serial.print(rtspClient.remoteIP());
        Serial.println();
        streamer->addSession(rtspClient);
    }
}

void setup()
{
  Serial.begin(BAUD_RATE);
  while (!Serial);            //wait for serial connection.
  
  setupCamera();
  setupWiFi();   
  setupSinricPro();
  setupStreaming();
}

void loop()
{ 
  SinricPro.handle();
  handleStreaming();
}
