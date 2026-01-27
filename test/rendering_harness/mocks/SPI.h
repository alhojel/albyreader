// Mock SPI.h for desktop builds
#pragma once

#include <cstdint>

class SPISettings {
 public:
  SPISettings() {}
  SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode) {
    (void)clock; (void)bitOrder; (void)dataMode;
  }
};

class MockSPI {
 public:
  void begin(int8_t sclk = -1, int8_t miso = -1, int8_t mosi = -1, int8_t ss = -1) {
    (void)sclk; (void)miso; (void)mosi; (void)ss;
  }
  void end() {}
  void beginTransaction(SPISettings settings) { (void)settings; }
  void endTransaction() {}
  uint8_t transfer(uint8_t data) { return data; }
  void transfer(void* buf, size_t count) { (void)buf; (void)count; }
};

extern MockSPI SPI;

#define MSBFIRST 1
#define LSBFIRST 0
#define SPI_MODE0 0
#define SPI_MODE1 1
#define SPI_MODE2 2
#define SPI_MODE3 3
