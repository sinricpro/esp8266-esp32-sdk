/*
 * Example for ESP32 Camera Motion Detection System:
 * - Create a ESP32 Camera device from portal.
 * - Copy the secrets below.
 * - Update WiFi settings
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

#include <Arduino.h>
#include <WiFi.h>
#include <SinricPro.h>
#include <SinricProCamera.h>
#include <SPIFFS.h>
#include <esp_camera.h>
#include "esp_http_client.h"
#include "HTTPClient.h"
#include <WiFiClientSecure.h>

// ===================
// Select camera model
// ===================
// #define CAMERA_MODEL_WROVER_KIT  // Has PSRAM
 #define CAMERA_MODEL_ESP_EYE  // Has PSRAM
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
// #define CAMERA_MODEL_XIAO_ESP32S3  // Has PSRAM
// ** Espressif Internal Boards **
// #define CAMERA_MODEL_ESP32_CAM_BOARD
// #define CAMERA_MODEL_ESP32S2_CAM_BOARD
// #define CAMERA_MODEL_ESP32S3_CAM_LCD
// #define CAMERA_MODEL_DFRobot_FireBeetle2_ESP32S3 // Has PSRAM
// #define CAMERA_MODEL_DFRobot_Romeo_ESP32S3 // Has PSRAM
#include "camera_pins.h"

#define MOTION_SENSOR_PIN 5          // GPIO pin connected to PIR motion sensor
#define FRAME_CAPTURE_COUNT 10       // Number of frames to capture when motion is detected
#define FRAME_CAPTURE_DELAY_MS 200   // Delay between frame captures (milliseconds)
#define MOTION_DEBOUNCE_DELAY 250    // Debounce delay for motion sensor (milliseconds)
#define TEMP_FILE_PATH "/mjpeg.tmp"  // Temporary file path in SPIFFS for storing frames

#define WIFI_SSID ""   // Your WiFi network SSID
#define WIFI_PASS ""   // Your WiFi network password
#define APP_KEY ""     // SinricPro APP_KEY
#define APP_SECRET ""  // SinricPro APP_SECRET
#define CAMERA_ID ""   // SinricPro CAMERA_ID

bool lastMotionState = false;               // Last recorded state of the motion sensor
unsigned long lastChangeTime = 0;           // Timestamp of last motion state change
unsigned long lastSendTime = 0;             // Timestamp of last successful motion data send
const unsigned long SEND_COOLDOWN = 60000;  // Minimum time between sends (60 seconds)

/**
 * Initializes the SPIFFS file system
 */
bool initFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS initialization failed!");
    return false;
  }
  return true;
}

/**
 * Initializes the camera with the specified configuration
 * 
 * @return bool True if initialization successful, false otherwise
 */
bool initCamera() {
  camera_config_t config;

  // Basic pin configuration
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

  // Camera settings
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_SVGA;
  config.grab_mode = CAMERA_GRAB_LATEST;
  config.jpeg_quality = 12;

  // Adjust settings based on PSRAM availability
  if (psramFound()) {
    config.fb_count = 2;
    config.fb_location = CAMERA_FB_IN_PSRAM;
  } else {
    config.fb_count = 1;
    config.frame_size = FRAMESIZE_SVGA;
    config.fb_location = CAMERA_FB_IN_DRAM;
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return false;
  }

  sensor_t* s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);        // flip it back
    s->set_brightness(s, 1);   // up the brightness just a bit
    s->set_saturation(s, -2);  // lower the saturation
  }

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

#if defined(CAMERA_MODEL_ESP32S3_EYE)
  s->set_vflip(s, 1);
#endif

  return true;
}

/**
 * Sets up WiFi connection 
 */
void setupWiFi() {
  Serial.print("\nConnecting to WiFi");

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  WiFi.setSleep(false);  // Disable WiFi sleep mode for better performance

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(250);
  }

  Serial.printf("\nWiFi connected! IP address: %s\n",
                WiFi.localIP().toString().c_str());
}
 
/**
 * Sets up SinricPro connection and event handlers
 */
void setupSinricPro() {
  SinricProCamera& camera = SinricPro[CAMERA_ID];

  // Setup power state callback
  camera.onPowerState([](const String& deviceId, bool& state) {
    return true;
  });

  camera.onSnapshot([](const String& deviceId) {
    camera_fb_t *fb = esp_camera_fb_get();

    if (!fb) {
      Serial.println("Failed to grab image");
      return false;
    }

    SinricProCamera &myCamera = SinricPro[CAMERA_ID];
    int result = myCamera.sendSnapshot(fb->buf, fb->len);
    esp_camera_fb_return(fb);
    return result == 200;
  });

  // Setup connection event handlers
  SinricPro.onConnected([]() {
    Serial.println("Connected to SinricPro");
  });

  SinricPro.onDisconnected([]() {
    Serial.println("Disconnected from SinricPro");
  });

  // Start SinricPro
  SinricPro.begin(APP_KEY, APP_SECRET);
}

/**
 * Captures multiple frames from the camera and writes them to file
 * 
 * @param file Reference to opened file for writing frames
 * @return size_t Total size of captured frames in bytes
 */
bool captureFrames(File& file) {
  size_t totalSize = 0;
  unsigned long startTime = millis();

  for (int i = 0; i < FRAME_CAPTURE_COUNT; i++) {
    // Get frame from camera
    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      continue;
    }

    // Write frame to file
    if (file.write(fb->buf, fb->len)) {
      totalSize += fb->len;
      Serial.printf("Frame %d saved, size: %d bytes\n", i + 1, fb->len);
    } else {
      Serial.println("Failed to write frame");
    }

    // Return frame buffer to be reused
    esp_camera_fb_return(fb);
    delay(FRAME_CAPTURE_DELAY_MS);
  }

  Serial.printf("Motion capture completed in %lu ms\n", millis() - startTime);
  Serial.printf("Motion file size: %d\n", totalSize);
  return totalSize > 0;
}

/**
 * Sends captured motion data to SinricPro server
 * Handles different response codes from the server
 */
void sendMotionToServer() {
  SinricProCamera& camera = SinricPro[CAMERA_ID];
  int responseCode = camera.sendMotion(SPIFFS, TEMP_FILE_PATH);

  // Handle different response codes
  switch (responseCode) {
    case 200:
      Serial.println("Motion data sent successfully");
      break;
    case 413:
      Serial.println("Error: File exceeds maximum allowed size");
      break;
    case 429:
      Serial.println("Error: Rate limit exceeded");
      break;
    default:
      Serial.printf("Error: Send failed with code %d\n", responseCode);
      break;
  }
}

/**
 * Captures frames when motion is detected and sends them to server
 * Handles temporary file creation and cleanup
 */
void captureAndSendMotion() {
  // Check if enough time has passed since last send
  unsigned long currentTime = millis();
  if (currentTime - lastSendTime < SEND_COOLDOWN) {
    Serial.println("Motion detected but skipping - cooldown period active");
    return;
  }

  // Open temporary file for writing frames
  File mjpegFile = SPIFFS.open(TEMP_FILE_PATH, FILE_WRITE);
  if (!mjpegFile) {
    Serial.println("Failed to open temporary file for writing");
    return;
  }

  // Capture frames and send if successful
  bool captured = captureFrames(mjpegFile);
  mjpegFile.close();

  if (captured) {
    sendMotionToServer();
  }

  // Cleanup temporary file
  SPIFFS.remove(TEMP_FILE_PATH);
}

/**
 * Handles motion sensor state changes
 * Implements debouncing and triggers capture when motion is detected
 */
void handleMotionSensor() {
  unsigned long currentTime = millis();
  // Check debounce period
  if (currentTime - lastChangeTime < MOTION_DEBOUNCE_DELAY) return;

  // Read current motion state
  bool currentMotionState = digitalRead(MOTION_SENSOR_PIN);
  if (currentMotionState != lastMotionState) {
    Serial.printf("Motion %s\n", currentMotionState ? "detected" : "not detected");
    lastMotionState = currentMotionState;
    lastChangeTime = currentTime;

    // Trigger capture on motion detection
    if (currentMotionState) {
      captureAndSendMotion();
    }
  }
}

bool onPowerState(const String& deviceId, bool& state) {
  return true;
}

/**
 * Setup function - initializes all components
 */
void setup() {
  Serial.begin(115200);

  // Initialize systems
  if (!initFS()) {
    Serial.println("File system initialization failed!");
    return;
  }

  pinMode(MOTION_SENSOR_PIN, INPUT);

  if (!initCamera()) {
    Serial.println("Camera initialization failed!");
    return;
  }

  setupWiFi();
  setupSinricPro();
}

void loop() {
  SinricPro.handle();
  handleMotionSensor();
}
