# AGENTS.md - AlbyReader Development Guide

## Project Overview

**AlbyReader** (CrossPoint Reader) is an open-source e-paper reader firmware for the **Xteink X4** device. Goal: Personal academic paper reader with beautiful typography, fast rendering, and weeks of battery life.

### Hardware Target
- **Device**: Xteink X4
- **Display**: 800x480 e-ink, 1-bit (4-level grayscale for images)
- **MCU**: ESP32-C3 (~380KB usable RAM)
- **Storage**: MicroSD card
- **Framework**: Arduino on FreeRTOS

### Key Constraints
- **RAM is precious** - stream content, cache to SD, never hold large buffers
- **E-ink is slow** - minimize refreshes, batch updates
- **Speed matters** - optimize everything, profile constantly
- **Battery is life** - deep sleep aggressively

---

## Repository Structure

```
albyreader/
├── src/                    # Main application code
│   ├── main.cpp           # Entry point, hardware init
│   ├── activities/        # Screen states (reader, browser, settings)
│   └── fontIds.h          # Font ID constants
├── lib/                    # Core libraries
│   ├── GfxRenderer/       # Graphics abstraction (draws to e-ink)
│   ├── EpdFont/           # Font rendering system
│   ├── Epub/              # EPUB parser and paginator
│   ├── Txt/               # Plain text reader
│   └── Utf8/              # UTF-8 handling
├── open-x4-sdk/           # Hardware drivers (submodule)
├── test/
│   └── rendering_harness/ # Desktop rendering tests (PNG output)
└── platformio.ini         # Build configuration
```

---

## Development Workflow

### Build Firmware
```bash
pio run                    # Build default environment
pio run -t upload          # Build and flash to device
```

### Run Rendering Tests (No Hardware Needed)
```bash
cd test/rendering_harness
make run                   # Build and run all tests
./rendering_test --list    # Show available tests
./rendering_test typography paper  # Run specific tests
./rendering_test --quiet   # CI mode (exit code only)
```

Tests output PNG files to `test/rendering_harness/output/` - view these to see exactly what the e-ink display will show.

### Key Files to Know

| File | Purpose |
|------|---------|
| `lib/GfxRenderer/GfxRenderer.cpp` | All drawing operations (text, shapes, images) |
| `lib/Epub/Epub.cpp` | EPUB loading, caching, pagination |
| `src/activities/reader/EpubReaderActivity.cpp` | Reading screen logic |
| `src/fontIds.h` | Font ID constants used throughout |
| `test/rendering_harness/rendering_tests.cpp` | Visual test cases |

---

## Phase Goals

### Phase 0: Rendering Test System ✅
Build visual feedback loop without hardware.
- [x] Desktop build with mock hardware
- [x] PNG output for all rendering
- [x] CLI automation for CI
- [ ] Add more test cases as features develop

### Phase 1: Beautiful Text Rendering
Perfect typography for academic content.
- [ ] Unicode math symbols (∑, ∫, ∞, α, β, etc.)
- [ ] Subscript/superscript approximations
- [ ] Better hyphenation for dense text
- [ ] Justified text with proper spacing
- [ ] Code blocks with monospace font

### Phase 2: arXiv Paper Fetching
Get papers via WiFi directly on device.
- [ ] arXiv API integration
- [ ] Search by ID (e.g., 2401.12345)
- [ ] Download HTML version (lighter than PDF)
- [ ] Parse arXiv HTML format
- [ ] Queue and background download

### Phase 3: Local Library Management
Browse and manage downloaded papers.
- [ ] List papers on SD card
- [ ] Resume reading position
- [ ] Delete/archive papers
- [ ] Sort by date/title/author
- [ ] Reading progress tracking

### Phase 4: Image Quality
Images must work on 1-bit display.
- [ ] Floyd-Steinberg dithering (already exists)
- [ ] Atkinson dithering option
- [ ] High-quality ASCII art fallback for diagrams
- [ ] Figure captions extraction
- [ ] Smart image scaling

### Phase 5: Polish & Stability
Make it reliable for daily use.
- [ ] Memory leak hunting
- [ ] Crash recovery
- [ ] Cache invalidation
- [ ] Performance profiling
- [ ] Battery optimization

---

## Extended Goals (Future)

- **Dark mode** - invert display for night reading
- **LibGen support** - already works with factory firmware
- **OPDS browsing** - browse online book catalogs
- **PDF parsing** - low priority (complex, slow, memory-heavy)
- **Offline arXiv** - cache arXiv metadata for offline search

---

## Display Specifications

### Physical
- Resolution: 800 x 480 pixels
- Color depth: 1-bit (black/white), 2-bit grayscale for images
- Refresh: Full (~2s), Half (~1.7s), Fast (~0.5s)

### Logical Coordinates
| Orientation | Width | Height | Use Case |
|-------------|-------|--------|----------|
| Portrait | 480 | 800 | Reading text |
| Landscape | 800 | 480 | Wide figures/tables |

### Margins
- Top: 9px
- Right/Bottom/Left: 3px
- Additional configurable margin in settings

---

## Font System

Available font families (all sizes have Regular, Bold, Italic, Bold-Italic):

| Family | Sizes | Notes |
|--------|-------|-------|
| Bookerly | 12, 14, 16, 18 | Primary reading font |
| Noto Sans | 12, 14, 16, 18 | UI and fallback |
| OpenDyslexic | 8, 10, 12, 14 | Accessibility option |
| Ubuntu | 10, 12 | UI elements |

Font IDs are defined in `src/fontIds.h`. Use these constants, not magic numbers.

---

## Rendering Architecture

```
User Input → Activity → GfxRenderer → EInkDisplay → Physical Screen
                             ↓
                        Frame Buffer (48KB)
                             ↓
                     Orientation Transform
                             ↓
                        SPI to Panel
```

Key classes:
- `GfxRenderer` - High-level drawing API (drawText, drawRect, etc.)
- `EInkDisplay` - Low-level display driver
- `Page` / `TextBlock` - EPUB content representation
- `Bitmap` - BMP loading with dithering

---

## Coding Guidelines

### Memory
- Never allocate large buffers on stack
- Use SD card for caching (`.crosspoint/` directory)
- Stream content when possible
- Free resources immediately after use

### Performance
- Minimize display refreshes (batch changes)
- Use FAST_REFRESH for page turns
- Use HALF_REFRESH periodically for quality
- Profile with `millis()` timestamps

### Testing
- Add rendering tests for new UI features
- Run `make run` in `test/rendering_harness/` before committing
- Visual inspection > unit tests for display code

### Style
- C++17 standard
- 2-space indentation
- No Arduino `String` class (use `std::string` or C strings)
- Keep includes minimal

---

## Common Tasks

### Add a New Font Size
1. Generate font data with font converter tool
2. Add to `lib/EpdFont/builtinFonts/`
3. Add ID to `src/fontIds.h`
4. Register in `main.cpp`

### Add a Rendering Test
1. Add test function to `test/rendering_harness/rendering_tests.cpp`
2. Register in `g_tests` vector
3. Call `saveScreen(display, "XX_testname")`
4. Run `make run` to verify

### Debug Display Issues
1. Add test case reproducing the issue
2. Run `make run` in rendering harness
3. Inspect PNG output
4. Fix and verify

---

## Resources

- **Hardware SDK**: `open-x4-sdk/` (git submodule)
- **PlatformIO docs**: https://docs.platformio.org/
- **E-ink basics**: https://www.waveshare.com/wiki/E-Paper_Driver_HAT
- **EPUB spec**: https://www.w3.org/publishing/epub3/

---

## Quick Reference

```bash
# Build and flash
pio run -t upload

# Run rendering tests
cd test/rendering_harness && make run

# List available tests
./rendering_test --list

# Run specific test quietly
./rendering_test --quiet typography

# Clean build
pio run -t clean
```
