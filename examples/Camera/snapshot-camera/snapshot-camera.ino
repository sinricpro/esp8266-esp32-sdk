/*
 * Example for how to use SinricPro Camera device:
 * - Create a ESP32 Camera device from portal.
 * - Copy the secrets below.
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
//#define ENABLE_DEBUG

#include <Arduino.h>
#include <WiFi.h>
#include <SinricPro.h>
#include <SinricProCamera.h>

#include <esp_camera.h>

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

#include "camera_pins.h" 

#define WIFI_SSID         "YOUR-WIFI-SSID"    
#define WIFI_PASS         "YOUR-WIFI-PASSWORD"
#define APP_KEY           "YOUR-APP-KEY"      // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
#define APP_SECRET        "YOUR-APP-SECRET"   // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"
#define CAMERA_ID         "YOUR-ESP32-CAMERA-ID" // Should look like "5dc1564130xxxxxxxxxxxxxx"
#define BAUD_RATE         115200                 // Change baudrate to your need

bool onSnapshot(const String& deviceId) {
    camera_fb_t* fb = esp_camera_fb_get();
    
    if (!fb) {
        Serial.println("Failed to grab image");
        return false;
    }
     
    SinricProCamera& myCamera = SinricPro[deviceId];
    int result = myCamera.sendSnapshot(fb->buf, fb->len);
    esp_camera_fb_return(fb);

    return result == 200;
}

bool onPowerState(const String& deviceId, bool& state) {
    return true;
}

// setup function for WiFi connection
void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");

  WiFi.setSleep(false); 
  WiFi.setAutoReconnect(true);

  WiFi.begin(WIFI_SSID, WIFI_PASS); 

  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(250);
  }
  Serial.printf("connected!\r\n[WiFi]: IP-Address is %s\r\n", WiFi.localIP().toString().c_str());
}

void setupSinricPro() {
    SinricProCamera& myCamera = SinricPro[CAMERA_ID];
    myCamera.onPowerState(onPowerState);
    myCamera.onSnapshot(onSnapshot);
    SinricPro.onConnected([]() { Serial.printf("Connected to SinricPro\r\n"); });
    SinricPro.onDisconnected([]() { Serial.printf("Disconnected from SinricPro\r\n"); });

    SinricPro.begin(APP_KEY, APP_SECRET);
}

esp_err_t setupCamera() {
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer   = LEDC_TIMER_0;
    config.pin_d0       = Y2_GPIO_NUM;
    config.pin_d1       = Y3_GPIO_NUM;
    config.pin_d2       = Y4_GPIO_NUM;
    config.pin_d3       = Y5_GPIO_NUM;
    config.pin_d4       = Y6_GPIO_NUM;
    config.pin_d5       = Y7_GPIO_NUM;
    config.pin_d6       = Y8_GPIO_NUM;
    config.pin_d7       = Y9_GPIO_NUM;
    config.pin_xclk     = XCLK_GPIO_NUM;
    config.pin_pclk     = PCLK_GPIO_NUM;
    config.pin_vsync    = VSYNC_GPIO_NUM;
    config.pin_href     = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn     = PWDN_GPIO_NUM;
    config.pin_reset    = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.frame_size   = FRAMESIZE_XGA;
    config.pixel_format = PIXFORMAT_JPEG;
    config.grab_mode    = CAMERA_GRAB_WHEN_EMPTY;
    config.fb_location  = CAMERA_FB_IN_PSRAM;
    config.jpeg_quality = 12;
    config.fb_count     = 1;

    // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
    // for larger pre-allocated frame buffer.
    if (psramFound()) {
        config.jpeg_quality = 10;
        config.fb_count     = 2;
        config.grab_mode    = CAMERA_GRAB_LATEST;
    } else {
        // Limit the frame size when PSRAM is not available
        config.frame_size  = FRAMESIZE_SVGA;
        config.fb_location = CAMERA_FB_IN_DRAM;
    }

    // camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        return err;
    }

    sensor_t* s = esp_camera_sensor_get();
    // initial sensors are flipped vertically and colors are a bit saturated
    s->set_vflip(s, 1);       // flip it back
    s->set_brightness(s, 1);  // up the brightness just a bit
    s->set_saturation(s, 0);  // lower the saturation

    return ESP_OK;
}

void setup() {
    Serial.begin(BAUD_RATE); Serial.printf("\r\n\r\n");
    setupWiFi();
    setupSinricPro();
    setupCamera();
}

void loop() {
    SinricPro.handle();
}


