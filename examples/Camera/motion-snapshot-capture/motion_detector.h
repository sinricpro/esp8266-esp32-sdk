#pragma once

#include <Arduino.h>
#include "esp_camera.h"
#include "img_converters.h"

class MotionDetector {
public:
  struct Config {
    uint16_t width;
    uint16_t height;
    uint8_t blockSize;
    uint8_t blockThreshold;
    uint8_t motionThreshold;
    uint8_t jpegQuality;
  };

  MotionDetector(const Config& config);
  ~MotionDetector();

  bool init();
  bool detectMotion(camera_fb_t* frame);
  void reset();

private:
  Config config_;
  uint8_t* prevFrame_;     // Previous frame in RGB565 format
  uint8_t* currentFrame_;  // Current frame in RGB565 format
  bool isInitialized_;

  bool allocateFrameBuffers();
  void freeFrameBuffers();
  bool compareFrames();
  bool jpegToRgb565(const uint8_t* jpegData, size_t jpegLen, uint8_t* outBuffer);
};
