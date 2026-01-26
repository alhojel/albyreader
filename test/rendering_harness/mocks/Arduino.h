// Mock Arduino.h for desktop builds
#pragma once

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <cmath>
#include <algorithm>

// Arduino type definitions
typedef uint8_t byte;

// Pin modes
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2

// Digital values
#define LOW 0
#define HIGH 1

// Mock timing functions
inline unsigned long millis() {
  static unsigned long ms = 0;
  return ms++;
}

inline void delay(unsigned long ms) {
  // No-op in test builds
  (void)ms;
}

inline void yield() {
  // No-op in test builds
}

// Mock pin functions
inline void pinMode(int pin, int mode) {
  (void)pin;
  (void)mode;
}

inline int digitalRead(int pin) {
  (void)pin;
  return LOW;
}

inline void digitalWrite(int pin, int val) {
  (void)pin;
  (void)val;
}

// Mock Serial class
class MockSerial {
 public:
  void begin(unsigned long baud) { (void)baud; }
  void end() {}

  template<typename T>
  void print(T val) {
    // Optionally print to stdout for debugging
    // std::cout << val;
    (void)val;
  }

  template<typename T>
  void println(T val) {
    // Optionally print to stdout for debugging
    // std::cout << val << std::endl;
    (void)val;
  }

  void printf(const char* format, ...) {
    // Optionally enable for debugging
    // va_list args;
    // va_start(args, format);
    // vprintf(format, args);
    // va_end(args);
    (void)format;
  }

  operator bool() const { return true; }
};

extern MockSerial Serial;

// String class (simplified Arduino String)
class String {
 public:
  String() : _str() {}
  String(const char* s) : _str(s ? s : "") {}
  String(const std::string& s) : _str(s) {}

  const char* c_str() const { return _str.c_str(); }
  size_t length() const { return _str.length(); }
  bool isEmpty() const { return _str.empty(); }

  String& operator+=(const String& other) {
    _str += other._str;
    return *this;
  }

  String& operator+=(const char* s) {
    if (s) _str += s;
    return *this;
  }

  bool operator==(const String& other) const { return _str == other._str; }
  bool operator!=(const String& other) const { return _str != other._str; }

 private:
  std::string _str;
};

// PROGMEM support (no-op on desktop)
#define PROGMEM
#define pgm_read_byte(addr) (*(const uint8_t*)(addr))
#define pgm_read_word(addr) (*(const uint16_t*)(addr))
#define pgm_read_dword(addr) (*(const uint32_t*)(addr))
#define pgm_read_ptr(addr) (*(const void**)(addr))
