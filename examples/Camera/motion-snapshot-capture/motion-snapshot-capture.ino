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
#define ENABLE_DEBUG
#define SINRICPRO_CAMERA_URL "testcamera.sinric.pro"

#include <Arduino.h>
#include <WiFi.h>
#include <SinricPro.h>
#include <SinricProCamera.h>
#include <SPIFFS.h>
#include <esp_camera.h>

// ===================
// Select camera model
// ===================
// #define CAMERA_MODEL_WROVER_KIT  // Has PSRAM
// #define CAMERA_MODEL_ESP_EYE  // Has PSRAM
// #define CAMERA_MODEL_ESP32S3_EYE // Has PSRAM
// #define CAMERA_MODEL_M5STACK_PSRAM // Has PSRAM
// #define CAMERA_MODEL_M5STACK_V2_PSRAM // M5Camera version B Has PSRAM
// #define CAMERA_MODEL_M5STACK_WIDE // Has PSRAM
// #define CAMERA_MODEL_M5STACK_ESP32CAM // No PSRAM
// #define CAMERA_MODEL_M5STACK_UNITCAM // No PSRAM
// #define CAMERA_MODEL_M5STACK_CAMS3_UNIT  // Has PSRAM
// #define CAMERA_MODEL_AI_THINKER  // Has PSRAM
// #define CAMERA_MODEL_TTGO_T_JOURNAL  // No PSRAM
// #define CAMERA_MODEL_TTGO_T_V16_VERSION
// #define CAMERA_MODEL_TTGO_T_V05_VERSION
// #define CAMERA_MODEL_TTGO_T_PLUS_VERSION
// #define CAMERA_MODEL_TTGO_T_V162_VERSION
// #define CAMERA_MODEL_TTGO_T_MINI_VERSION
// #define CAMERA_MODEL_TTGO_T_V17_VERSION
 #define CAMERA_MODEL_XIAO_ESP32S3 // Has PSRAM
// ** Espressif Internal Boards **
// #define CAMERA_MODEL_ESP32_CAM_BOARD
// #define CAMERA_MODEL_ESP32S2_CAM_BOARD
// #define CAMERA_MODEL_ESP32S3_CAM_LCD
// #define CAMERA_MODEL_DFRobot_FireBeetle2_ESP32S3 // Has PSRAM
// #define CAMERA_MODEL_DFRobot_Romeo_ESP32S3 // Has PSRAM
#include "camera_pins.h"
#include "motion_detector.h"

#define WIFI_SSID "June"
#define WIFI_PASS "wifipassword"

#define APP_KEY "7c7f0312-502e-4eba-a377-80fd599e79e7"                                          // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
#define APP_SECRET "4bdc2144-f17b-463a-be32-600e6835b922-63bc1698-7c52-41f0-bcb5-a444528ba781"  // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"
#define CAMERA_ID "673bce6823cdb5c141975952"                                                    // Should look like "5dc1564130xxxxxxxxxxxxxx"

// More sensitive to large moving objects
// #define BLOCK_THRESHOLD 40    // Require stronger pixel changes
// #define MOTION_THRESHOLD 15   // Require more blocks to change

// More sensitive to mid moving objects
#define BLOCK_THRESHOLD 30   // Accept subtler pixel changes
#define MOTION_THRESHOLD 10  // Accept mid changed blocks

// More sensitive to small moving objects
//  #define BLOCK_THRESHOLD 20    // Accept subtler pixel changes
//  #define MOTION_THRESHOLD 5    // Accept fewer changed blocks

MotionDetector *motionDetector = nullptr;
bool motionDetected = false;

// File to save the MJPEG
const char* motionFileName = "/motion.mjpeg";
// Number of frames and delay between captures
#define NUM_FRAMES  5
#define FRAME_DELAY 200  // in milliseconds

bool onPowerState(const String &deviceId, bool &state) {
  return true;
}

// setup function for WiFi connection
void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  WiFi.setSleep(false);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(250);
  }
  Serial.printf("connected!\r\n[WiFi]: IP-Address is %s\r\n", WiFi.localIP().toString().c_str());
}

void setupSinricPro() {
  SinricProCamera &myCamera = SinricPro[CAMERA_ID];
  myCamera.onPowerState(onPowerState);

  SinricPro.onConnected([]() {
    Serial.printf("Connected to SinricPro\r\n");
  });
  SinricPro.onDisconnected([]() {
    Serial.printf("Disconnected from SinricPro\r\n");
  });

  SinricPro.begin(APP_KEY, APP_SECRET, "testws.sinric.pro");
}

void setupCamera() {
  Serial.printf("Setup Camera..\r\n");

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
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_VGA;  //  https://github.com/espressif/esp32-camera/blob/master/driver/include/sensor.h#L98C5-L98C18
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  if (psramFound()) {
    config.fb_count = 2;
    config.grab_mode = CAMERA_GRAB_LATEST;
  } else {
    Serial.println("Warnning! PSRAM not found!!!!");
    // Limit the frame size when PSRAM is not available
    config.frame_size = FRAMESIZE_SVGA;
    config.fb_location = CAMERA_FB_IN_DRAM;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  Serial.printf("Camera Setup done!..\r\n");
}

void setupMotionDetection() {
  MotionDetector::Config mdConfig = {
    .width = 640,                         // VGA width
    .height = 480,                        // VGA height
    .blockSize = 16,                      // 16x16 pixel blocks
    .blockThreshold = BLOCK_THRESHOLD,    // Sensitivity to changes within blocks
    .motionThreshold = MOTION_THRESHOLD,  // Number of blocks that must change
    .jpegQuality = 12                     // JPEG quality setting
  };

  motionDetector = new MotionDetector(mdConfig);
  if (!motionDetector->init()) {
    Serial.println("Motion detector initialization failed!");
    return;
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("Failed to mount SPIFFS");
    return;
  }

  setupCamera();
  setupMotionDetection();
  setupWiFi();
  setupSinricPro();
}
 
std::vector<uint8_t> createMJPEG(std::vector<camera_fb_t*>& frames) {
    std::vector<uint8_t> mjpegData;
    
    // MJPEG header
    const char* mjpegHeader = "--boundary\r\nContent-Type: image/jpeg\r\n\r\n";
    
    for (auto frame : frames) {
        // Add boundary and headers
        mjpegData.insert(mjpegData.end(), mjpegHeader, mjpegHeader + strlen(mjpegHeader));
        
        // Add JPEG data
        mjpegData.insert(mjpegData.end(), frame->buf, frame->buf + frame->len);

        Serial.printf("frame->len size: %d\r\n", frame->len);
        
        // Add newline
        mjpegData.push_back('\r');
        mjpegData.push_back('\n');
    }
    
    return mjpegData;
}

void captureAndSendFrames() {
    std::vector<camera_fb_t*> frames;
    
    // Capture 5 frames
    for (int i = 0; i < 5; i++) {
        camera_fb_t* fb = esp_camera_fb_get();
        if (!fb) {
            Serial.println("Camera capture failed");
            continue;
        }
        frames.push_back(fb);
        delay(200); // Small delay between captures
    }
    
    if (frames.size() > 0) {
        // Create MJPEG data
        std::vector<uint8_t> mjpegData = createMJPEG(frames);

        Serial.printf("mjpegData size: %d\r\n", mjpegData.size());

        SinricProCamera &myCamera = SinricPro[CAMERA_ID];
        int result = myCamera.sendMotion(mjpegData.data(), mjpegData.size());
        Serial.printf("Snapshot %s\r\n", result == 200 ? "sent!" : "send failed!");
    }
    
    // Free the frames
    for (auto frame : frames) {
        esp_camera_fb_return(frame);
    }
}

// void sendMotion() {
//   SinricProCamera &myCamera = SinricPro[CAMERA_ID];
//   int result = myCamera.sendMotion(motionFileName);
//   // 200 = Success
//   // 429 = Too many requests
//   // 400 = Format error
//   // 500 = internal error
//   Serial.printf("Motion %s\r\n", result == 200 ? "sent!" : "send failed!");
// }

void sendSnapshot(camera_fb_t *fb) {
  SinricProCamera &myCamera = SinricPro[CAMERA_ID];
  int result = myCamera.sendSnapshot(fb->buf, fb->len);
  // 200 = Success
  // 429 = Too many requests
  // 400 = Format error
  // 500 = internal error
  Serial.printf("Snapshot %s\r\n", result == 200 ? "sent!" : "send failed!");
}

void loop() {
  SinricPro.handle();

  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  if (motionDetector->detectMotion(fb)) {
    if (!motionDetected) {
      Serial.println("Motion detected!");
      motionDetected = true;
    }
  } else {
    motionDetected = false;
  }

  if (motionDetected) {
    // sendSnapshot(fb);

    esp_camera_fb_return(fb);
    captureAndSendFrames(); 
  }

  esp_camera_fb_return(fb);

  delay(100);
}