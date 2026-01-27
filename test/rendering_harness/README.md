# AlbyReader Rendering Test Harness

Visual feedback system for testing e-ink rendering without hardware.

## Quick Start

```bash
cd test/rendering_harness
make run                   # Build and run all tests
./rendering_test --list    # Show available tests
./rendering_test --quiet   # CI mode (exit code only)
```

Output PNG files appear in `output/`.

## CLI Usage

```bash
./rendering_test                      # Run all tests
./rendering_test typography paper     # Run specific tests
./rendering_test --quiet              # Minimal output (for CI)
./rendering_test --output /tmp/out    # Custom output directory
./rendering_test --list               # Show available tests
./rendering_test --help               # Show help
```

### Exit Codes
- `0` - All tests passed
- `1` - One or more tests failed or invalid arguments

## What It Does

This test harness compiles the actual AlbyReader rendering code (GfxRenderer, fonts, etc.) for desktop execution. It mocks the ESP32/Arduino hardware dependencies and outputs PNG screenshots of what the 800x480 e-ink display would show.

## Available Tests

| Name | Output File | Description |
|------|-------------|-------------|
| `typography` | `01_typography.png` | Font families, sizes, and styles |
| `unicode` | `02_unicode.png` | Special characters and symbols |
| `paragraph` | `03_paragraph.png` | Multi-line text layout |
| `graphics` | `04_graphics.png` | Lines, rectangles, shapes |
| `paper` | `05_paper.png` | Academic paper layout |
| `reader` | `06_reader.png` | Reading interface UI |
| `portrait` | `07_portrait.png` | Portrait orientation (480x800) |
| `landscape` | `08_landscape.png` | Landscape orientation (800x480) |
| `dense` | `09_dense.png` | Dense content stress test |

## Build Commands

```bash
make        # Build the test harness
make run    # Build and run all tests
make clean  # Remove build artifacts
make help   # Show all commands
```

## CI Integration

```bash
# In CI script
cd test/rendering_harness
make
./rendering_test --quiet || exit 1
```

## Adding New Tests

1. Add test function to `rendering_tests.cpp`:

```cpp
void testMyFeature(GfxRenderer& renderer, EInkDisplay& display) {
  log("\n[TEST] myfeature - Description here\n");
  renderer.clearScreen();

  // Your rendering code here
  renderer.drawText(BOOKERLY_14_FONT_ID, 20, 20, "Hello, World!");

  display.displayBuffer();
  saveScreen(display, "XX_myfeature");
}
```

2. Register in `g_tests` vector:

```cpp
{"myfeature", "Description here", testMyFeature},
```

3. Run `make run` to verify.

## Architecture

```
rendering_harness/
├── mocks/
│   ├── Arduino.h      # Mock Arduino APIs (Serial, millis, etc.)
│   ├── EInkDisplay.h  # Mock display with PNG output
│   ├── SdFat.h        # Mock file system
│   └── SPI.h          # Mock SPI
├── stb_image_write.h  # PNG encoding (single-header library)
├── rendering_tests.cpp # Test cases with CLI
├── Makefile
└── output/            # Generated PNG files
```

## Display Specs

- **Physical**: 800x480 pixels, 1-bit (black/white)
- **Portrait mode**: 480x800 logical coordinates
- **Landscape mode**: 800x480 logical coordinates
- **Margins**: 9px top, 3px right/bottom/left

## Notes

- PNG output is grayscale (black = 0, white = 255)
- Build uses actual font data from `lib/EpdFont/builtinFonts/`
- GfxRenderer coordinate transforms are tested via orientation modes
- Use `--quiet` flag in CI for minimal output with proper exit codes
