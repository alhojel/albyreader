// Mock implementations for desktop builds

#include "Arduino.h"
#include "SPI.h"
#include "EInkDisplay.h"

// Define stb_image_write implementation
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image_write.h"

// Global mock instances
MockSerial Serial;
MockSPI SPI;

// EInkDisplay PNG save implementation
bool EInkDisplay::saveFrameBufferAsPNG(const char* filename) const {
  // Convert 1-bit packed buffer to 8-bit grayscale for PNG
  uint8_t* grayscale = new uint8_t[DISPLAY_WIDTH * DISPLAY_HEIGHT];

  for (uint32_t y = 0; y < DISPLAY_HEIGHT; y++) {
    for (uint32_t x = 0; x < DISPLAY_WIDTH; x++) {
      uint32_t byteIdx = y * DISPLAY_WIDTH_BYTES + x / 8;
      uint8_t bit = 7 - (x % 8);
      // In e-ink convention: 1 = white, 0 = black
      bool isWhite = (frameBuffer[byteIdx] >> bit) & 1;
      grayscale[y * DISPLAY_WIDTH + x] = isWhite ? 255 : 0;
    }
  }

  int result = stbi_write_png(filename, DISPLAY_WIDTH, DISPLAY_HEIGHT, 1, grayscale, DISPLAY_WIDTH);

  delete[] grayscale;
  return result != 0;
}
