# Makefile for AlbyReader Rendering Test Harness
# Builds and runs desktop rendering tests that output PNG files

# Compiler settings
CXX := g++
# -include flags ensure standard headers are always available (needed because library code
# doesn't always include them but Arduino environment provides them automatically)
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -g -include cstdint -include cstring -include cmath

# Directories
ROOT_DIR := ../..
LIB_DIR := $(ROOT_DIR)/lib
SRC_DIR := $(ROOT_DIR)/src
MOCK_DIR := mocks
OUTPUT_DIR := output

# Include paths
# -isystem for mocks ensures they take precedence over system headers (angle bracket includes)
INCLUDES := \
	-isystem $(MOCK_DIR) \
	-I$(MOCK_DIR) \
	-I$(LIB_DIR)/GfxRenderer \
	-I$(LIB_DIR)/EpdFont \
	-I$(LIB_DIR)/Utf8 \
	-I$(SRC_DIR) \
	-I.

# Source files from the project
PROJECT_SOURCES := \
	$(LIB_DIR)/GfxRenderer/GfxRenderer.cpp \
	$(LIB_DIR)/GfxRenderer/Bitmap.cpp \
	$(LIB_DIR)/GfxRenderer/BitmapHelpers.cpp \
	$(LIB_DIR)/EpdFont/EpdFont.cpp \
	$(LIB_DIR)/EpdFont/EpdFontFamily.cpp \
	$(LIB_DIR)/Utf8/Utf8.cpp

# Test harness sources
HARNESS_SOURCES := \
	$(MOCK_DIR)/mock_impl.cpp \
	rendering_tests.cpp

# All sources
SOURCES := $(PROJECT_SOURCES) $(HARNESS_SOURCES)

# Object files
OBJECTS := $(SOURCES:.cpp=.o)

# Target executable
TARGET := rendering_test

# Default target
.PHONY: all
all: $(OUTPUT_DIR) $(TARGET)

# Create output directory
$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

# Link
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

# Run tests
.PHONY: run
run: all
	./$(TARGET)
	@echo ""
	@echo "=== Test images generated in $(OUTPUT_DIR)/ ==="
	@ls -la $(OUTPUT_DIR)/*.png 2>/dev/null || echo "No PNG files generated"

# Clean
.PHONY: clean
clean:
	rm -f $(TARGET)
	rm -f $(OBJECTS)
	rm -f $(OUTPUT_DIR)/*.png
	rm -f $(OUTPUT_DIR)/*.pbm

# Deep clean (including output)
.PHONY: distclean
distclean: clean
	rm -rf $(OUTPUT_DIR)

# View output (requires feh or similar)
.PHONY: view
view: run
	@if command -v feh >/dev/null 2>&1; then \
		feh -Z -F $(OUTPUT_DIR)/*.png; \
	elif command -v eog >/dev/null 2>&1; then \
		eog $(OUTPUT_DIR)/*.png; \
	elif command -v open >/dev/null 2>&1; then \
		open $(OUTPUT_DIR)/*.png; \
	else \
		echo "No image viewer found. Please open $(OUTPUT_DIR)/*.png manually."; \
	fi

# Help
.PHONY: help
help:
	@echo "AlbyReader Rendering Test Harness"
	@echo ""
	@echo "Usage:"
	@echo "  make        - Build the test harness"
	@echo "  make run    - Build and run tests, generate PNG output"
	@echo "  make view   - Run tests and open images in viewer"
	@echo "  make clean  - Remove build artifacts"
	@echo ""
	@echo "Output images are saved to: $(OUTPUT_DIR)/"
