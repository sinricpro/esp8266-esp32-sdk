/*
 * Example for SinricPro Camera with WebRTC live view in the SinricPro app and portal:
 * - Create a Camera device from portal. In Camera Stream Configuration select
 *   Board "ESP32" and Streaming Protocol "WebRTC".
 * - Copy the secrets below.
 *
 * On ESP32-S3 the video is H.264 on a WebRTC video track; elsewhere, and for viewers that ask for
 * it, it is JPEG frames over a WebRTC data channel. Either way it goes directly to the viewer (or
 * via the SinricPro TURN relay when a direct path is not possible). Viewers can change resolution
 * and frame rate, toggle flash / flip / mirror, and quality drops automatically on slow links.
 * On XIAO ESP32S3 Sense the onboard microphone is streamed as well.
 * Alexa and Google Home need the H.264 video track at 480p or larger, so they work only on an
 * ESP32-S3 with "H.264 video track" ticked in the portal's Camera Stream Configuration.
 *
 * Requirements:
 * - SinricProWebRTC library 0.3.1 or later (https://github.com/sinricpro/arduino-esp32-webrtc-lib)
 * - ESP32 or ESP32-S3 board with PSRAM, Arduino ESP32 core 3.3.10 or 3.3.11
 * - Tools > PSRAM: Enabled, Tools > Partition Scheme: "Huge APP (3MB No OTA/1MB SPIFFS)"
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
#include <SinricProWebRTC.h> // https://github.com/sinricpro/arduino-esp32-webrtc-lib
#include <SinricProWebRTCSession.h> // https://github.com/sinricpro/arduino-esp32-webrtc-lib

// ===================
// Select camera model
// ===================
// #define CAMERA_MODEL_WROVER_KIT  // Has PSRAM
// #define CAMERA_MODEL_ESP_EYE  // Has PSRAM
// #define CAMERA_MODEL_ESP32S3_EYE // Has PSRAM
// #define CAMERA_MODEL_M5STACK_PSRAM // Has PSRAM
// #define CAMERA_MODEL_M5STACK_V2_PSRAM // M5Camera version B Has PSRAM
// #define CAMERA_MODEL_M5STACK_WIDE // Has PSRAM
// #define CAMERA_MODEL_M5STACK_CAMS3_UNIT  // Has PSRAM
#define CAMERA_MODEL_AI_THINKER // Has PSRAM
// #define CAMERA_MODEL_XIAO_ESP32S3 // Has PSRAM
// #define CAMERA_MODEL_DFRobot_FireBeetle2_ESP32S3 // Has PSRAM
// #define CAMERA_MODEL_DFRobot_Romeo_ESP32S3 // Has PSRAM
// Boards without PSRAM are not supported: WebRTC needs PSRAM for frame buffers.
#include "camera_pins.h"

// Flash LED that viewers can toggle (-1 = none). AI-Thinker ESP32-CAM has its flash on GPIO 4.
#if defined(CAMERA_MODEL_AI_THINKER)
#define FLASH_LED_PIN 4
#else
#define FLASH_LED_PIN -1
#endif

// Onboard PDM microphone of the XIAO ESP32S3 Sense.
#if defined(CAMERA_MODEL_XIAO_ESP32S3)
#define WEBRTC_MIC 1
#include <ESP_I2S.h>
I2SClass microphone;
#else
#define WEBRTC_MIC 0
#endif

// esp_h264 encodes in software and ships a prebuilt library for the S3 only; classic ESP32 keeps
// the JPEG data-channel path.
#if CONFIG_IDF_TARGET_ESP32S3
#define WEBRTC_H264 1
#else
#define WEBRTC_H264 0
#endif

#define WIFI_SSID ""   // Your WiFi network SSID
#define WIFI_PASS ""   // Your WiFi network password

#define APP_KEY    "" // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
#define APP_SECRET "" // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"
#define CAMERA_ID  "" // Should look like "5dc1564130xxxxxxxxxxxxxx"

#define BAUD_RATE 115200  // Change baudrate to your need

SinricProWebRTCSession webrtcSession;
// Kept so the session can re-initialise the camera in YUV422 for an H.264 video track.
camera_config_t cameraConfig;

// Called when a viewer opens the camera in the SinricPro app or portal.
// Blocks until the ESP32 has gathered its ICE candidates (up to ~4 seconds).
bool onWebRTCOffer(const String &deviceId, const String &offerSdp, const std::vector<SinricProIceServer> &iceServers, String &answerSdp) {
  std::vector<WebRTCIceServer> servers;
  for (const SinricProIceServer &server : iceServers) {
    servers.push_back({server.url, server.username, server.credential});
  }

  Serial.printf("[WebRTC]: Offer received (%u ICE server URLs)\r\n", (unsigned)servers.size());
  bool success = webrtcSession.handleOffer(offerSdp, servers, answerSdp);
  if (success) {
    Serial.println("[WebRTC]: Answer sent");
  } else {
    // Shown to the viewer in the SinricPro app and portal.
    Serial.printf("[WebRTC]: %s\r\n", webrtcSession.lastError().c_str());
    SinricPro.setResponseMessage(webrtcSession.lastError());
  }
  return success;
}

#if WEBRTC_MIC
// 20 ms of 16 kHz PDM audio, averaged down to the 8 kHz PCMU used by the WebRTC audio track.
bool readMicrophone(uint8_t *pcmu, size_t size) {
  static int16_t pcm[320];
  static size_t used = 0;
  size_t bytes = 0;

  i2s_channel_read(microphone.rxChan(), reinterpret_cast<uint8_t *>(pcm) + used, sizeof(pcm) - used, &bytes, 0);
  used += bytes;
  if (used < sizeof(pcm)) return false;
  used = 0;

  for (size_t i = 0; i < size && 2 * i + 1 < 320; i++) {
    int16_t sample = (static_cast<int32_t>(pcm[2 * i]) + pcm[2 * i + 1]) / 2;
    pcmu[i] = SinricWebRTC::encodeMuLaw(sample);
  }
  return true;
}

void setupMicrophone() {
  microphone.setPinsPdmRx(42, 41);
  if (!microphone.begin(I2S_MODE_PDM_RX, 16000, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO)) {
    Serial.println("Microphone initialization failed");
    while (true) delay(1000);
  }
}
#endif

bool onSnapshot(const String &deviceId) {
  camera_fb_t *fb = esp_camera_fb_get();

  if (!fb) {
    Serial.println("Failed to grab image");
    return false;
  }

  SinricProCamera &myCamera = SinricPro[deviceId];
  int responseCode = myCamera.sendSnapshot(fb->buf, fb->len);
  esp_camera_fb_return(fb);

  if (responseCode != 200) Serial.printf("Error: Snapshot send failed with code %d\r\n", responseCode);
  return responseCode == 200;
}

bool onPowerState(const String &deviceId, bool &state) {
  if (!state) webrtcSession.stop();  // Turning the camera off disconnects the current viewer
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

void setupWebRTC() {
  SinricProWebRTCSession::Config config;
  config.maxFrameSize = FRAMESIZE_SVGA;  // the size setupCamera() initialized the camera with
  config.flashPin = FLASH_LED_PIN;
  config.cameraConfig = cameraConfig;
#if WEBRTC_H264
  config.h264 = true;
#endif
#if CONFIG_IDF_TARGET_ESP32
  // Once WiFi and the SinricPro TLS socket are up, classic ESP32 has a single ~28 kB contiguous
  // internal block left. Caches large enough to consume it leave the WiFi driver unable to
  // allocate its TX buffers, and the DTLS handshake never completes.
  config.dataChannelSendCache = 6 * 1024;
  config.dataChannelRecvCache = 3 * 1024;
#endif
#if WEBRTC_MIC
  config.audio = true;
  webrtcSession.setAudioSource(readMicrophone);
#endif

  if (!webrtcSession.begin(config)) {
    Serial.println("[WebRTC]: Not enough memory for the WebRTC task");
    while (true) delay(1000);
  }
}

void setupSinricPro() {
  SinricProCamera &myCamera = SinricPro[CAMERA_ID];
  myCamera.onPowerState(onPowerState);
  myCamera.onSnapshot(onSnapshot);
  myCamera.onWebRTCOffer(onWebRTCOffer);
  // Lets viewers offer sound; they request it only when the user turns it on. On an ESP32-S3 an
  // audio track alongside H.264 costs video packets, so live view opens as video only.
  myCamera.enableWebRTCAudio(WEBRTC_MIC);
  myCamera.enableWebRTCVideo(WEBRTC_H264); // likewise for video; without it viewers get JPEG

  SinricPro.onConnected([]() {
    Serial.printf("Connected to SinricPro\r\n");
  });
  SinricPro.onDisconnected([]() {
    Serial.printf("Disconnected from SinricPro\r\n");
  });

  SinricPro.begin(APP_KEY, APP_SECRET);
}

void setupCamera() {
  if (!psramFound()) {
    Serial.println("PSRAM not found. Enable PSRAM in Tools menu; WebRTC requires it.");
    while (true) delay(1000);
  }

  camera_config_t config = {};
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
  config.pixel_format = PIXFORMAT_JPEG;  // do not change!
  // Buffers are sized for the init resolution, so init at the largest size viewers may pick.
  // Each streamed frame must stay under 128 KB; SVGA at quality 14 is well below that.
  config.frame_size = FRAMESIZE_SVGA;
  config.jpeg_quality = 14;
  config.fb_count = 2;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.grab_mode = CAMERA_GRAB_LATEST;

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

#if defined(CAMERA_MODEL_TTGO_T_V05_VERSION)
  // At 20 MHz this board's camera stops the Wi-Fi TX buffers draining once a stream reaches VGA.
  config.xclk_freq_hz = 10000000;
#endif

  cameraConfig = config;

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\r\n", err);
    while (true) delay(1000);
  }

  sensor_t *s = esp_camera_sensor_get();
  // Start small so live view opens quickly; viewers can switch up to SVGA. H.264 on an ESP32-S3
  // takes its size from its own mode table, so this only sets the JPEG starting point.
  s->set_framesize(s, FRAMESIZE_QVGA);

  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);        // flip it back
    s->set_brightness(s, 1);   // up the brightness just a bit
    s->set_saturation(s, -2);  // lower the saturation
  }

#if defined(CAMERA_MODEL_M5STACK_WIDE)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

#if defined(CAMERA_MODEL_ESP32S3_EYE)
  s->set_vflip(s, 1);
#endif
}

void setup() {
  Serial.begin(BAUD_RATE);
  setupCamera();
#if WEBRTC_MIC
  setupMicrophone();
#endif
  setupWiFi();
  setupWebRTC();
  setupSinricPro();
}

void loop() {
  SinricPro.handle();
}
