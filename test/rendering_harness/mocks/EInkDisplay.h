// Mock EInkDisplay for desktop rendering tests
#pragma once

#include "Arduino.h"  // Must come first to provide Serial, millis, etc.

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>

class EInkDisplay {
 public:
  // Refresh modes
  enum RefreshMode {
    FULL_REFRESH,
    HALF_REFRESH,
    FAST_REFRESH
  };

  // Display dimensions - matching real device
  static constexpr uint16_t DISPLAY_WIDTH = 800;
  static constexpr uint16_t DISPLAY_HEIGHT = 480;
  static constexpr uint16_t DISPLAY_WIDTH_BYTES = DISPLAY_WIDTH / 8;
  static constexpr uint32_t BUFFER_SIZE = DISPLAY_WIDTH_BYTES * DISPLAY_HEIGHT;

  // Constructor (pins ignored in mock)
  EInkDisplay(int8_t sclk = 0, int8_t mosi = 0, int8_t cs = 0, int8_t dc = 0, int8_t rst = 0, int8_t busy = 0) {
    (void)sclk; (void)mosi; (void)cs; (void)dc; (void)rst; (void)busy;
    memset(frameBuffer, 0xFF, BUFFER_SIZE);  // White screen
  }

  ~EInkDisplay() = default;

  void begin() {
    // Initialize to white (all bits set = white in e-ink convention)
    memset(frameBuffer, 0xFF, BUFFER_SIZE);
  }

  void clearScreen(uint8_t color = 0xFF) const {
    memset(frameBuffer, color, BUFFER_SIZE);
  }

  void drawImage(const uint8_t* imageData, uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool fromProgmem = false) const {
    (void)fromProgmem;
    // Copy image data to frame buffer
    for (uint16_t row = 0; row < h && (y + row) < DISPLAY_HEIGHT; row++) {
      for (uint16_t col = 0; col < w && (x + col) < DISPLAY_WIDTH; col++) {
        uint16_t srcByteIdx = row * ((w + 7) / 8) + col / 8;
        uint8_t srcBit = 7 - (col % 8);
        bool pixel = (imageData[srcByteIdx] >> srcBit) & 1;

        uint16_t dstX = x + col;
        uint16_t dstY = y + row;
        uint32_t dstByteIdx = dstY * DISPLAY_WIDTH_BYTES + dstX / 8;
        uint8_t dstBit = 7 - (dstX % 8);

        if (pixel) {
          frameBuffer[dstByteIdx] |= (1 << dstBit);
        } else {
          frameBuffer[dstByteIdx] &= ~(1 << dstBit);
        }
      }
    }
  }

  void setFramebuffer(const uint8_t* bwBuffer) const {
    memcpy(frameBuffer, bwBuffer, BUFFER_SIZE);
  }

  void copyGrayscaleBuffers(const uint8_t* lsbBuffer, const uint8_t* msbBuffer) {
    (void)lsbBuffer; (void)msbBuffer;
  }

  void copyGrayscaleLsbBuffers(const uint8_t* lsbBuffer) {
    (void)lsbBuffer;
  }

  void copyGrayscaleMsbBuffers(const uint8_t* msbBuffer) {
    (void)msbBuffer;
  }

  void cleanupGrayscaleBuffers(const uint8_t* bwBuffer) {
    (void)bwBuffer;
  }

  void displayBuffer(RefreshMode mode = FAST_REFRESH) {
    (void)mode;
    displayCount++;
  }

  void displayWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    (void)x; (void)y; (void)w; (void)h;
    displayCount++;
  }

  void displayGrayBuffer(bool turnOffScreen = false) {
    (void)turnOffScreen;
    displayCount++;
  }

  void refreshDisplay(RefreshMode mode = FAST_REFRESH, bool turnOffScreen = false) {
    (void)mode; (void)turnOffScreen;
    displayCount++;
  }

  void grayscaleRevert() {}

  void setCustomLUT(bool enabled, const unsigned char* lutData = nullptr) {
    (void)enabled; (void)lutData;
  }

  void deepSleep() {}

  uint8_t* getFrameBuffer() const {
    return const_cast<uint8_t*>(frameBuffer);
  }

  // Save frame buffer as PBM (Portable Bitmap) - simple format
  bool saveFrameBufferAsPBM(const char* filename) const {
    FILE* f = fopen(filename, "wb");
    if (!f) return false;

    // PBM header (P4 = binary PBM)
    fprintf(f, "P4\n%d %d\n", DISPLAY_WIDTH, DISPLAY_HEIGHT);

    // In PBM P4 format: 1 = black, 0 = white
    // Our buffer: 1 = white, 0 = black (e-ink convention)
    // So we need to invert
    uint8_t invertedRow[DISPLAY_WIDTH_BYTES];
    for (uint16_t y = 0; y < DISPLAY_HEIGHT; y++) {
      for (uint16_t x = 0; x < DISPLAY_WIDTH_BYTES; x++) {
        invertedRow[x] = ~frameBuffer[y * DISPLAY_WIDTH_BYTES + x];
      }
      fwrite(invertedRow, 1, DISPLAY_WIDTH_BYTES, f);
    }

    fclose(f);
    return true;
  }

  // Save frame buffer as PNG using stb_image_write
  bool saveFrameBufferAsPNG(const char* filename) const;

  // Get display count (for testing)
  int getDisplayCount() const { return displayCount; }
  void resetDisplayCount() { displayCount = 0; }

 private:
  mutable uint8_t frameBuffer[BUFFER_SIZE];
  int displayCount = 0;
};
