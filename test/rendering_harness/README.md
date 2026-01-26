# AlbyReader Rendering Test Harness

Visual feedback system for testing e-ink rendering without hardware.

## Quick Start

```bash
cd test/rendering_harness
make run
```

Output PNG files appear in `output/`.

## What It Does

This test harness compiles the actual AlbyReader rendering code (GfxRenderer, fonts, etc.) for desktop execution. It mocks the ESP32/Arduino hardware dependencies and outputs PNG screenshots of what the 800x480 e-ink display would show.

## Test Cases

| File | Description |
|------|-------------|
| `01_typography_basic.png` | Font comparison - Bookerly, Noto Sans, OpenDyslexic at various sizes |
| `02_unicode_characters.png` | Punctuation, accents, math symbols, currency |
| `03_paragraph_layout.png` | Word wrapping and multi-line text |
| `04_graphics_primitives.png` | Lines, rectangles, grids, UI borders |
| `05_paper_layout.png` | Simulated academic paper with title, abstract, sections |
| `06_reading_interface.png` | Full reading UI with status bar, progress, content |
| `07a_portrait_mode.png` | Portrait orientation (480x800 logical) |
| `07b_landscape_mode.png` | Landscape orientation (800x480 logical) |
| `08_dense_content.png` | Stress test with maximum text density |

## Build Commands

```bash
make        # Build the test harness
make run    # Build and run, generate PNG output
make clean  # Remove build artifacts
make help   # Show all commands
```

## Adding New Tests

Edit `rendering_tests.cpp` and add a new test function:

```cpp
void testMyFeature(GfxRenderer& renderer, EInkDisplay& display) {
  printf("\n[TEST] My Feature\n");

  renderer.clearScreen();

  // Your rendering code here
  renderer.drawText(BOOKERLY_14_FONT_ID, 20, 20, "Hello, World!");

  display.displayBuffer();
  saveScreen(display, "my_feature");  // Outputs: output/my_feature.png
}
```

Then call it from `main()`.

## Architecture

```
rendering_harness/
├── mocks/
│   ├── Arduino.h      # Mock Arduino APIs (Serial, millis, etc.)
│   ├── EInkDisplay.h  # Mock display with PNG output
│   ├── SdFat.h        # Mock file system
│   └── SPI.h          # Mock SPI
├── stb_image_write.h  # PNG encoding (single-header library)
├── rendering_tests.cpp # Test cases
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
