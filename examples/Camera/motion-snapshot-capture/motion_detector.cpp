#include "motion_detector.h"

MotionDetector::MotionDetector(const Config& config)
  : config_(config),
    prevFrame_(nullptr),
    currentFrame_(nullptr),
    isInitialized_(false) {
}

MotionDetector::~MotionDetector() {
  if (isInitialized_) {
    freeFrameBuffers();
  }
}

bool MotionDetector::init() {
  if (isInitialized_) {
    return true;
  }

  if (!allocateFrameBuffers()) {
    Serial.println("Failed to allocate frame buffers");
    return false;
  }

  isInitialized_ = true;
  return true;
}

void MotionDetector::reset() {
  if (!isInitialized_) return;

  size_t frameSize = config_.width * config_.height * 2;  // RGB565 = 2 bytes per pixel
  if (prevFrame_) {
    memset(prevFrame_, 0, frameSize);
  }
  if (currentFrame_) {
    memset(currentFrame_, 0, frameSize);
  }
}

bool MotionDetector::allocateFrameBuffers() {
  size_t frameSize = config_.width * config_.height * 2;  // RGB565 = 2 bytes per pixel

  // Free any existing buffers first
  freeFrameBuffers();

  // Use MALLOC_CAP_SPIRAM for external RAM if available
  prevFrame_ = (uint8_t*)heap_caps_malloc(frameSize, MALLOC_CAP_8BIT);
  currentFrame_ = (uint8_t*)heap_caps_malloc(frameSize, MALLOC_CAP_8BIT);

  if (!prevFrame_ || !currentFrame_) {
    freeFrameBuffers();
    return false;
  }

  memset(prevFrame_, 0, frameSize);
  memset(currentFrame_, 0, frameSize);

  return true;
}

void MotionDetector::freeFrameBuffers() {
  if (prevFrame_) {
    heap_caps_free(prevFrame_);
    prevFrame_ = nullptr;
  }
  if (currentFrame_) {
    heap_caps_free(currentFrame_);
    currentFrame_ = nullptr;
  }
}

bool MotionDetector::compareFrames() {
  if (!isInitialized_ || !prevFrame_ || !currentFrame_) {
    return false;
  }

  int changedBlocks = 0;
  int bytesPerPixel = 2;  // RGB565 uses 2 bytes per pixel

  for (int y = 0; y < config_.height; y += config_.blockSize) {
    for (int x = 0; x < config_.width; x += config_.blockSize) {
      int diffSum = 0;

      // Compare blocks
      for (int by = 0; by < config_.blockSize && (y + by) < config_.height; by++) {
        for (int bx = 0; bx < config_.blockSize && (x + bx) < config_.width; bx++) {
          int pos = ((y + by) * config_.width + (x + bx)) * bytesPerPixel;

          // Get RGB565 values
          uint16_t current = (currentFrame_[pos + 1] << 8) | currentFrame_[pos];
          uint16_t prev = (prevFrame_[pos + 1] << 8) | prevFrame_[pos];

          // Extract RGB components
          uint8_t currentR = (current >> 11) & 0x1F;
          uint8_t currentG = (current >> 5) & 0x3F;
          uint8_t currentB = current & 0x1F;

          uint8_t prevR = (prev >> 11) & 0x1F;
          uint8_t prevG = (prev >> 5) & 0x3F;
          uint8_t prevB = prev & 0x1F;

          diffSum += abs(currentR - prevR);
          diffSum += abs(currentG - prevG);
          diffSum += abs(currentB - prevB);
        }
      }

      if (diffSum / (config_.blockSize * config_.blockSize) > config_.blockThreshold) {
        changedBlocks++;
      }
    }
  }

  if (changedBlocks > 0) { Serial.print(changedBlocks); Serial.println(" blocks changed!"); }
  return changedBlocks > config_.motionThreshold;
}

bool MotionDetector::jpegToRgb565(const uint8_t* jpegData, size_t jpegLen, uint8_t* outBuffer) {
  return jpg2rgb565(jpegData, jpegLen, outBuffer, JPG_SCALE_NONE);
}

bool MotionDetector::detectMotion(camera_fb_t* frame) {
  if (!isInitialized_ || !frame || frame->format != PIXFORMAT_JPEG) {
    return false;
  }

  // Copy current frame to previous frame
  memcpy(prevFrame_, currentFrame_, config_.width * config_.height * 2);

  // Convert new frame to RGB565
  if (!jpegToRgb565(frame->buf, frame->len, currentFrame_)) {
    Serial.println("Failed to convert JPEG to RGB565");
    return false;
  }

  return compareFrames();
}