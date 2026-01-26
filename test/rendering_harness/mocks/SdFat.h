// Mock SdFat.h for desktop builds
// Provides minimal FsFile interface for compilation
#pragma once

#include <cstdint>
#include <cstdio>
#include <cstring>

// Minimal FsFile mock - implements just enough for Bitmap to compile
class FsFile {
 public:
  FsFile() : fp(nullptr), pos(0), fileSize(0) {}
  ~FsFile() { close(); }

  bool open(const char* path, int mode = 0) {
    (void)mode;
    fp = fopen(path, "rb");
    if (fp) {
      fseek(fp, 0, SEEK_END);
      fileSize = ftell(fp);
      fseek(fp, 0, SEEK_SET);
      pos = 0;
      return true;
    }
    return false;
  }

  void close() {
    if (fp) {
      fclose(fp);
      fp = nullptr;
    }
    pos = 0;
    fileSize = 0;
  }

  bool isOpen() const { return fp != nullptr; }

  int read() {
    if (!fp) return -1;
    int c = fgetc(fp);
    if (c != EOF) pos++;
    return c;
  }

  size_t read(void* buf, size_t len) {
    if (!fp) return 0;
    size_t bytesRead = fread(buf, 1, len, fp);
    pos += bytesRead;
    return bytesRead;
  }

  bool seek(uint32_t offset) {
    if (!fp) return false;
    if (fseek(fp, offset, SEEK_SET) == 0) {
      pos = offset;
      return true;
    }
    return false;
  }

  bool seekSet(uint32_t offset) { return seek(offset); }

  bool seekCur(int32_t offset) {
    if (!fp) return false;
    if (fseek(fp, offset, SEEK_CUR) == 0) {
      pos = ftell(fp);
      return true;
    }
    return false;
  }

  uint32_t position() const { return pos; }
  uint32_t size() const { return fileSize; }

  operator bool() const { return isOpen(); }

 private:
  FILE* fp;
  uint32_t pos;
  uint32_t fileSize;
};

// Open mode flags
#define O_READ 0x01
#define O_WRITE 0x02
#define O_RDONLY O_READ
#define O_WRONLY O_WRITE
#define O_RDWR (O_READ | O_WRITE)
#define O_CREAT 0x04
#define O_TRUNC 0x08
#define O_APPEND 0x10

// Minimal SdFat class
class SdFat {
 public:
  bool begin() { return true; }
  bool exists(const char* path) {
    FILE* f = fopen(path, "r");
    if (f) {
      fclose(f);
      return true;
    }
    return false;
  }
};
