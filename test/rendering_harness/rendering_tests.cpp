/**
 * Rendering Test Harness for AlbyReader
 *
 * CLI tool for rendering e-ink display content to PNG files.
 * Runs without hardware for visual testing and CI integration.
 *
 * Usage:
 *   ./rendering_test                    # Run all tests
 *   ./rendering_test --list             # List available tests
 *   ./rendering_test typography paper   # Run specific tests
 *   ./rendering_test --quiet            # Minimal output (for CI)
 *   ./rendering_test --output /tmp/out  # Custom output directory
 */

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstring>
#include <string>
#include <vector>
#include <functional>
#include <sys/stat.h>

// Include mocks first
#include "mocks/Arduino.h"
#include "mocks/SPI.h"
#include "mocks/EInkDisplay.h"

// Include the real rendering code
#include "../../lib/GfxRenderer/GfxRenderer.h"
#include "../../lib/EpdFont/EpdFont.h"
#include "../../lib/EpdFont/EpdFontFamily.h"
#include "../../lib/EpdFont/builtinFonts/all.h"
#include "../../src/fontIds.h"

// ============================================================================
// Global State
// ============================================================================

static std::string g_outputDir = "output";
static bool g_quiet = false;
static int g_testsRun = 0;
static int g_testsPassed = 0;
static int g_testsFailed = 0;

// ============================================================================
// Helpers
// ============================================================================

void log(const char* fmt, ...) {
  if (g_quiet) return;
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}

bool saveScreen(EInkDisplay& display, const char* testName) {
  char filename[512];
  snprintf(filename, sizeof(filename), "%s/%s.png", g_outputDir.c_str(), testName);
  bool ok = display.saveFrameBufferAsPNG(filename);
  if (ok) {
    log("  [OK] %s\n", filename);
    g_testsPassed++;
  } else {
    printf("  [FAIL] %s\n", filename);  // Always print failures
    g_testsFailed++;
  }
  g_testsRun++;
  return ok;
}

bool ensureDirectory(const std::string& path) {
  struct stat st;
  if (stat(path.c_str(), &st) == 0) {
    return S_ISDIR(st.st_mode);
  }
  return mkdir(path.c_str(), 0755) == 0;
}

// ============================================================================
// Test Cases
// ============================================================================

void testTypography(GfxRenderer& renderer, EInkDisplay& display) {
  log("\n[TEST] typography - Font comparison\n");
  renderer.clearScreen();

  int y = 20;
  const int lineSpacing = 10;

  renderer.drawText(UI_12_FONT_ID, 20, y, "Typography Test - Font Comparison", true, EpdFontFamily::BOLD);
  y += renderer.getLineHeight(UI_12_FONT_ID) + lineSpacing * 2;

  renderer.drawText(BOOKERLY_12_FONT_ID, 20, y, "Bookerly 12: The quick brown fox jumps over the lazy dog");
  y += renderer.getLineHeight(BOOKERLY_12_FONT_ID) + lineSpacing;

  renderer.drawText(BOOKERLY_14_FONT_ID, 20, y, "Bookerly 14: The quick brown fox jumps over the lazy dog");
  y += renderer.getLineHeight(BOOKERLY_14_FONT_ID) + lineSpacing;

  renderer.drawText(BOOKERLY_16_FONT_ID, 20, y, "Bookerly 16: The quick brown fox");
  y += renderer.getLineHeight(BOOKERLY_16_FONT_ID) + lineSpacing;

  renderer.drawText(BOOKERLY_18_FONT_ID, 20, y, "Bookerly 18: The quick brown fox");
  y += renderer.getLineHeight(BOOKERLY_18_FONT_ID) + lineSpacing * 2;

  renderer.drawText(NOTOSANS_14_FONT_ID, 20, y, "Noto Sans 14: The quick brown fox jumps over the lazy dog");
  y += renderer.getLineHeight(NOTOSANS_14_FONT_ID) + lineSpacing;

  renderer.drawText(NOTOSANS_16_FONT_ID, 20, y, "Noto Sans 16: The quick brown fox");
  y += renderer.getLineHeight(NOTOSANS_16_FONT_ID) + lineSpacing * 2;

  // Font styles
  renderer.drawText(BOOKERLY_14_FONT_ID, 20, y, "Regular text, ", true, EpdFontFamily::REGULAR);
  int x = 20 + renderer.getTextWidth(BOOKERLY_14_FONT_ID, "Regular text, ", EpdFontFamily::REGULAR);
  renderer.drawText(BOOKERLY_14_FONT_ID, x, y, "Bold text, ", true, EpdFontFamily::BOLD);
  x += renderer.getTextWidth(BOOKERLY_14_FONT_ID, "Bold text, ", EpdFontFamily::BOLD);
  renderer.drawText(BOOKERLY_14_FONT_ID, x, y, "Italic text, ", true, EpdFontFamily::ITALIC);
  x += renderer.getTextWidth(BOOKERLY_14_FONT_ID, "Italic text, ", EpdFontFamily::ITALIC);
  renderer.drawText(BOOKERLY_14_FONT_ID, x, y, "Bold-Italic", true, EpdFontFamily::BOLD_ITALIC);
  y += renderer.getLineHeight(BOOKERLY_14_FONT_ID) + lineSpacing * 2;

  renderer.drawText(UI_10_FONT_ID, 20, y, "UI Font 10: Used for interface elements");
  y += renderer.getLineHeight(UI_10_FONT_ID) + lineSpacing;

  renderer.drawText(UI_12_FONT_ID, 20, y, "UI Font 12: Used for buttons and menus");
  y += renderer.getLineHeight(UI_12_FONT_ID) + lineSpacing;

  renderer.drawText(SMALL_FONT_ID, 20, y, "Small Font: Used for status bars and fine print");

  display.displayBuffer();
  saveScreen(display, "01_typography");
}

void testUnicode(GfxRenderer& renderer, EInkDisplay& display) {
  log("\n[TEST] unicode - Special characters\n");
  renderer.clearScreen();

  int y = 20;
  const int lineSpacing = 15;

  renderer.drawText(UI_12_FONT_ID, 20, y, "Unicode & Special Characters Test", true, EpdFontFamily::BOLD);
  y += renderer.getLineHeight(UI_12_FONT_ID) + lineSpacing * 2;

  renderer.drawText(BOOKERLY_14_FONT_ID, 20, y, "Punctuation: \"Hello, World!\" - What's up? (test) [array]");
  y += renderer.getLineHeight(BOOKERLY_14_FONT_ID) + lineSpacing;

  renderer.drawText(BOOKERLY_14_FONT_ID, 20, y, "Accents: cafe, naive, resume, Zurn, Grosse");
  y += renderer.getLineHeight(BOOKERLY_14_FONT_ID) + lineSpacing;

  renderer.drawText(NOTOSANS_14_FONT_ID, 20, y, "Math: + - * / = < > % # @ & |");
  y += renderer.getLineHeight(NOTOSANS_14_FONT_ID) + lineSpacing;

  renderer.drawText(NOTOSANS_14_FONT_ID, 20, y, "Currency: $ EUR GBP JPY");
  y += renderer.getLineHeight(NOTOSANS_14_FONT_ID) + lineSpacing;

  renderer.drawText(BOOKERLY_14_FONT_ID, 20, y, "Numbers: 0123456789 3.14159 1,000,000");
  y += renderer.getLineHeight(BOOKERLY_14_FONT_ID) + lineSpacing * 2;

  renderer.drawText(UI_12_FONT_ID, 20, y, "Sample Academic Text:", true, EpdFontFamily::BOLD);
  y += renderer.getLineHeight(UI_12_FONT_ID) + lineSpacing;

  renderer.drawText(BOOKERLY_14_FONT_ID, 20, y, "The algorithm runs in O(n log n) time complexity.");
  y += renderer.getLineHeight(BOOKERLY_14_FONT_ID) + lineSpacing;

  renderer.drawText(BOOKERLY_14_FONT_ID, 20, y, "Let f(x) = x^2 + 2x + 1, then f'(x) = 2x + 2.");
  y += renderer.getLineHeight(BOOKERLY_14_FONT_ID) + lineSpacing;

  renderer.drawText(BOOKERLY_14_FONT_ID, 20, y, "E = mc^2 (Einstein's mass-energy equivalence)");

  display.displayBuffer();
  saveScreen(display, "02_unicode");
}

void testParagraph(GfxRenderer& renderer, EInkDisplay& display) {
  log("\n[TEST] paragraph - Text layout\n");
  renderer.clearScreen();

  const int margin = 20;
  const int maxWidth = renderer.getScreenWidth() - 2 * margin;
  int y = 20;
  int fontId = BOOKERLY_14_FONT_ID;
  int lineHeight = renderer.getLineHeight(fontId);

  renderer.drawText(UI_12_FONT_ID, margin, y, "Paragraph Layout Test", true, EpdFontFamily::BOLD);
  y += renderer.getLineHeight(UI_12_FONT_ID) + 20;

  const char* paragraph =
      "This is a sample paragraph that demonstrates text layout capabilities. "
      "Good typography is essential for readability on e-ink displays. "
      "The text should flow naturally across multiple lines with proper spacing. "
      "Academic papers often contain dense paragraphs that require careful rendering.";

  std::string text = paragraph;
  std::string line, word;

  for (size_t i = 0; i <= text.length(); i++) {
    char c = (i < text.length()) ? text[i] : ' ';
    if (c == ' ' || c == '\0') {
      std::string testLine = line.empty() ? word : line + " " + word;
      if (renderer.getTextWidth(fontId, testLine.c_str()) > maxWidth && !line.empty()) {
        renderer.drawText(fontId, margin, y, line.c_str());
        y += lineHeight;
        line = word;
      } else {
        line = testLine;
      }
      word.clear();
    } else {
      word += c;
    }
  }
  if (!line.empty()) {
    renderer.drawText(fontId, margin, y, line.c_str());
    y += lineHeight;
  }

  y += 30;
  renderer.drawText(UI_12_FONT_ID, margin, y, "Abstract:", true, EpdFontFamily::BOLD);
  y += renderer.getLineHeight(UI_12_FONT_ID) + 10;

  const char* abstract =
      "We present a novel approach to self-forcing in language model inference. "
      "Our method achieves significant speedups while maintaining generation quality. "
      "Experiments on standard benchmarks demonstrate the effectiveness of our technique.";

  text = abstract;
  line.clear();

  for (size_t i = 0; i <= text.length(); i++) {
    char c = (i < text.length()) ? text[i] : ' ';
    if (c == ' ' || c == '\0') {
      std::string testLine = line.empty() ? word : line + " " + word;
      if (renderer.getTextWidth(fontId, testLine.c_str()) > maxWidth && !line.empty()) {
        renderer.drawText(fontId, margin, y, line.c_str());
        y += lineHeight;
        line = word;
      } else {
        line = testLine;
      }
      word.clear();
    } else {
      word += c;
    }
  }
  if (!line.empty()) {
    renderer.drawText(fontId, margin, y, line.c_str());
  }

  display.displayBuffer();
  saveScreen(display, "03_paragraph");
}

void testGraphics(GfxRenderer& renderer, EInkDisplay& display) {
  log("\n[TEST] graphics - Primitives\n");
  renderer.clearScreen();

  int y = 20;

  renderer.drawText(UI_12_FONT_ID, 20, y, "Graphics Primitives Test", true, EpdFontFamily::BOLD);
  y += 40;

  renderer.drawText(SMALL_FONT_ID, 20, y, "Horizontal lines:");
  y += 15;
  for (int i = 0; i < 5; i++) {
    renderer.drawLine(20, y + i * 10, 200, y + i * 10);
  }
  y += 60;

  renderer.drawText(SMALL_FONT_ID, 20, y, "Vertical lines:");
  y += 15;
  for (int i = 0; i < 5; i++) {
    renderer.drawLine(20 + i * 20, y, 20 + i * 20, y + 50);
  }

  int rectX = 250, rectY = 80;
  renderer.drawText(SMALL_FONT_ID, rectX, rectY - 20, "Rectangles:");
  renderer.drawRect(rectX, rectY, 80, 40);
  renderer.drawRect(rectX + 100, rectY, 80, 40);
  renderer.fillRect(rectX + 100 + 20, rectY + 10, 40, 20);

  rectY += 60;
  renderer.drawText(SMALL_FONT_ID, rectX, rectY - 10, "Nested:");
  for (int i = 0; i < 4; i++) {
    renderer.drawRect(rectX + i * 5, rectY + i * 5, 80 - i * 10, 60 - i * 10);
  }

  int gridX = 20, gridY = 200;
  renderer.drawText(SMALL_FONT_ID, gridX, gridY - 10, "Grid pattern:");
  for (int row = 0; row < 5; row++) {
    for (int col = 0; col < 8; col++) {
      if ((row + col) % 2 == 0) {
        renderer.fillRect(gridX + col * 20, gridY + row * 20, 20, 20);
      } else {
        renderer.drawRect(gridX + col * 20, gridY + row * 20, 20, 20);
      }
    }
  }

  display.displayBuffer();
  saveScreen(display, "04_graphics");
}

void testPaper(GfxRenderer& renderer, EInkDisplay& display) {
  log("\n[TEST] paper - Academic layout\n");
  renderer.clearScreen();

  const int margin = 25;
  const int maxWidth = renderer.getScreenWidth() - 2 * margin;
  int y = margin;

  renderer.drawCenteredText(BOOKERLY_18_FONT_ID, y, "Self-Forcing: A Novel Approach to", true, EpdFontFamily::BOLD);
  y += renderer.getLineHeight(BOOKERLY_18_FONT_ID);
  renderer.drawCenteredText(BOOKERLY_18_FONT_ID, y, "Speculative Decoding", true, EpdFontFamily::BOLD);
  y += renderer.getLineHeight(BOOKERLY_18_FONT_ID) + 15;

  renderer.drawCenteredText(NOTOSANS_12_FONT_ID, y, "Anonymous Authors");
  y += renderer.getLineHeight(NOTOSANS_12_FONT_ID) + 5;
  renderer.drawCenteredText(SMALL_FONT_ID, y, "Anonymous Institution");
  y += renderer.getLineHeight(SMALL_FONT_ID) + 20;

  renderer.drawLine(margin, y, renderer.getScreenWidth() - margin, y);
  y += 15;

  renderer.drawText(NOTOSANS_14_FONT_ID, margin, y, "Abstract", true, EpdFontFamily::BOLD);
  y += renderer.getLineHeight(NOTOSANS_14_FONT_ID) + 5;

  const char* abstractText =
      "We introduce self-forcing, a technique that enables language models to "
      "generate their own draft tokens for speculative decoding. This eliminates "
      "the need for a separate draft model while maintaining speedup benefits.";

  int fontId = BOOKERLY_12_FONT_ID;
  int lineHeight = renderer.getLineHeight(fontId);
  std::string text = abstractText;
  std::string line, word;

  for (size_t i = 0; i <= text.length(); i++) {
    char c = (i < text.length()) ? text[i] : ' ';
    if (c == ' ' || c == '\0') {
      std::string testLine = line.empty() ? word : line + " " + word;
      if (renderer.getTextWidth(fontId, testLine.c_str()) > maxWidth && !line.empty()) {
        renderer.drawText(fontId, margin, y, line.c_str());
        y += lineHeight;
        line = word;
      } else {
        line = testLine;
      }
      word.clear();
    } else {
      word += c;
    }
  }
  if (!line.empty()) {
    renderer.drawText(fontId, margin, y, line.c_str());
    y += lineHeight;
  }

  y += 20;
  renderer.drawText(NOTOSANS_14_FONT_ID, margin, y, "1. Introduction", true, EpdFontFamily::BOLD);
  y += renderer.getLineHeight(NOTOSANS_14_FONT_ID) + 5;

  renderer.drawText(fontId, margin, y, "Large language models have shown remarkable capabilities...");
  y += lineHeight;

  y += 15;
  renderer.drawText(fontId, margin + 50, y, "y = f(x) = argmax P(w|context)    (1)", true, EpdFontFamily::ITALIC);

  y = renderer.getScreenHeight() - 30;
  renderer.drawLine(margin, y, renderer.getScreenWidth() - margin, y);
  y += 10;
  renderer.drawCenteredText(SMALL_FONT_ID, y, "1");

  display.displayBuffer();
  saveScreen(display, "05_paper");
}

void testReader(GfxRenderer& renderer, EInkDisplay& display) {
  log("\n[TEST] reader - Reading interface\n");
  renderer.clearScreen();

  // Status bar
  renderer.fillRect(0, 0, renderer.getScreenWidth(), 25, false);
  renderer.drawLine(0, 25, renderer.getScreenWidth(), 25);
  renderer.drawText(SMALL_FONT_ID, 10, 5, "Chapter 3: Methods");
  renderer.drawText(SMALL_FONT_ID, renderer.getScreenWidth() - 80, 5, "Page 42/128");

  int battX = renderer.getScreenWidth() - 35;
  renderer.drawRect(battX, 7, 25, 12);
  renderer.fillRect(battX + 2, 9, 18, 8);
  renderer.fillRect(battX + 25, 10, 3, 6);

  const int margin = 20;
  const int topMargin = 40;
  int y = topMargin;
  int fontId = BOOKERLY_14_FONT_ID;
  int lineHeight = renderer.getLineHeight(fontId);

  const char* content[] = {
      "3.1 Experimental Setup",
      "",
      "We evaluate our self-forcing approach on a diverse set of benchmarks",
      "including text generation, code completion, and mathematical reasoning",
      "tasks. Our experiments use the following configuration:",
      "",
      "- Base model: 7B parameter transformer",
      "- Draft tokens: 4-8 tokens per speculation round",
      "- Temperature: 0.7 for sampling",
      "- Evaluation metrics: tokens/second, acceptance rate",
      "",
      "3.2 Baselines",
      "",
      "We compare against three baseline approaches:",
      "",
      "1. Standard autoregressive decoding (no speculation)",
      "2. Speculative decoding with a separate 125M draft model",
      "3. Medusa-style parallel decoding heads",
  };

  for (const char* line : content) {
    if (strlen(line) == 0) {
      y += lineHeight / 2;
      continue;
    }

    bool isHeader = (line[0] == '3' && line[1] == '.');
    bool isBullet = (line[0] == '-' || (line[0] >= '1' && line[0] <= '9' && line[1] == '.'));
    int xOffset = margin + (isBullet ? 20 : 0);

    if (isHeader) {
      y += 10;
      renderer.drawText(fontId, xOffset, y, line, true, EpdFontFamily::BOLD);
    } else {
      renderer.drawText(fontId, xOffset, y, line);
    }
    y += lineHeight;
    if (y > renderer.getScreenHeight() - 50) break;
  }

  int progressY = renderer.getScreenHeight() - 15;
  renderer.drawRect(margin, progressY, renderer.getScreenWidth() - 2 * margin, 8);
  int progressWidth = (int)((renderer.getScreenWidth() - 2 * margin - 4) * 0.33);
  renderer.fillRect(margin + 2, progressY + 2, progressWidth, 4);

  display.displayBuffer();
  saveScreen(display, "06_reader");
}

void testPortrait(GfxRenderer& renderer, EInkDisplay& display) {
  log("\n[TEST] portrait - Portrait orientation\n");
  renderer.setOrientation(GfxRenderer::Portrait);
  renderer.clearScreen();

  renderer.drawText(UI_12_FONT_ID, 20, 20, "Portrait Mode (480x800)", true, EpdFontFamily::BOLD);
  renderer.drawText(BOOKERLY_14_FONT_ID, 20, 60, "Width: 480 pixels");
  renderer.drawText(BOOKERLY_14_FONT_ID, 20, 85, "Height: 800 pixels");
  renderer.drawRect(5, 5, renderer.getScreenWidth() - 10, renderer.getScreenHeight() - 10);
  renderer.drawText(BOOKERLY_14_FONT_ID, 20, 150, "This is the natural reading orientation.");
  renderer.fillRect(20, 220, 100, 200);
  renderer.drawText(SMALL_FONT_ID, 130, 300, "<- Tall rectangle");

  display.displayBuffer();
  saveScreen(display, "07_portrait");
}

void testLandscape(GfxRenderer& renderer, EInkDisplay& display) {
  log("\n[TEST] landscape - Landscape orientation\n");
  renderer.setOrientation(GfxRenderer::LandscapeClockwise);
  renderer.clearScreen();

  renderer.drawText(UI_12_FONT_ID, 20, 20, "Landscape Mode (800x480)", true, EpdFontFamily::BOLD);
  renderer.drawText(BOOKERLY_14_FONT_ID, 20, 60, "Width: 800 pixels");
  renderer.drawText(BOOKERLY_14_FONT_ID, 20, 85, "Height: 480 pixels");
  renderer.drawRect(5, 5, renderer.getScreenWidth() - 10, renderer.getScreenHeight() - 10);
  renderer.drawText(BOOKERLY_14_FONT_ID, 20, 150, "Landscape mode is good for wide tables and figures.");
  renderer.fillRect(20, 220, 300, 80);
  renderer.drawText(SMALL_FONT_ID, 340, 250, "<- Wide rectangle");

  display.displayBuffer();
  saveScreen(display, "08_landscape");
  renderer.setOrientation(GfxRenderer::Portrait);
}

void testDense(GfxRenderer& renderer, EInkDisplay& display) {
  log("\n[TEST] dense - Dense content stress test\n");
  renderer.setOrientation(GfxRenderer::Portrait);
  renderer.clearScreen();

  const int margin = 15;
  int y = 10;
  int fontId = SMALL_FONT_ID;
  int lineHeight = renderer.getLineHeight(fontId);

  renderer.drawText(UI_10_FONT_ID, margin, y, "Dense Content Stress Test", true, EpdFontFamily::BOLD);
  y += renderer.getLineHeight(UI_10_FONT_ID) + 5;

  const char* loremIpsum =
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed do eiusmod "
      "tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, "
      "quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo "
      "consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse "
      "cillum dolore eu fugiat nulla pariatur.";

  const int maxWidth = renderer.getScreenWidth() - 2 * margin;
  int paragraphCount = 0;

  while (y < renderer.getScreenHeight() - 30 && paragraphCount < 10) {
    std::string text = loremIpsum;
    std::string line, word;

    for (size_t i = 0; i <= text.length() && y < renderer.getScreenHeight() - 20; i++) {
      char c = (i < text.length()) ? text[i] : ' ';
      if (c == ' ' || c == '\0') {
        std::string testLine = line.empty() ? word : line + " " + word;
        if (renderer.getTextWidth(fontId, testLine.c_str()) > maxWidth && !line.empty()) {
          renderer.drawText(fontId, margin, y, line.c_str());
          y += lineHeight;
          line = word;
        } else {
          line = testLine;
        }
        word.clear();
      } else {
        word += c;
      }
    }
    if (!line.empty() && y < renderer.getScreenHeight() - 20) {
      renderer.drawText(fontId, margin, y, line.c_str());
      y += lineHeight;
    }
    y += 5;
    paragraphCount++;
  }

  display.displayBuffer();
  saveScreen(display, "09_dense");
}

// ============================================================================
// Test Registry
// ============================================================================

struct TestCase {
  const char* name;
  const char* description;
  std::function<void(GfxRenderer&, EInkDisplay&)> fn;
};

static std::vector<TestCase> g_tests = {
    {"typography", "Font families, sizes, and styles", testTypography},
    {"unicode", "Special characters and symbols", testUnicode},
    {"paragraph", "Multi-line text layout", testParagraph},
    {"graphics", "Lines, rectangles, shapes", testGraphics},
    {"paper", "Academic paper layout", testPaper},
    {"reader", "Reading interface UI", testReader},
    {"portrait", "Portrait orientation (480x800)", testPortrait},
    {"landscape", "Landscape orientation (800x480)", testLandscape},
    {"dense", "Dense content stress test", testDense},
};

// ============================================================================
// Font Setup
// ============================================================================

void setupFonts(GfxRenderer& renderer) {
  static EpdFont bookerly12Regular(&bookerly_12_regular);
  static EpdFont bookerly12Bold(&bookerly_12_bold);
  static EpdFont bookerly12Italic(&bookerly_12_italic);
  static EpdFont bookerly12BoldItalic(&bookerly_12_bolditalic);
  static EpdFontFamily bookerly12Family(&bookerly12Regular, &bookerly12Bold, &bookerly12Italic, &bookerly12BoldItalic);

  static EpdFont bookerly14Regular(&bookerly_14_regular);
  static EpdFont bookerly14Bold(&bookerly_14_bold);
  static EpdFont bookerly14Italic(&bookerly_14_italic);
  static EpdFont bookerly14BoldItalic(&bookerly_14_bolditalic);
  static EpdFontFamily bookerly14Family(&bookerly14Regular, &bookerly14Bold, &bookerly14Italic, &bookerly14BoldItalic);

  static EpdFont bookerly16Regular(&bookerly_16_regular);
  static EpdFont bookerly16Bold(&bookerly_16_bold);
  static EpdFont bookerly16Italic(&bookerly_16_italic);
  static EpdFont bookerly16BoldItalic(&bookerly_16_bolditalic);
  static EpdFontFamily bookerly16Family(&bookerly16Regular, &bookerly16Bold, &bookerly16Italic, &bookerly16BoldItalic);

  static EpdFont bookerly18Regular(&bookerly_18_regular);
  static EpdFont bookerly18Bold(&bookerly_18_bold);
  static EpdFont bookerly18Italic(&bookerly_18_italic);
  static EpdFont bookerly18BoldItalic(&bookerly_18_bolditalic);
  static EpdFontFamily bookerly18Family(&bookerly18Regular, &bookerly18Bold, &bookerly18Italic, &bookerly18BoldItalic);

  static EpdFont notosans12Regular(&notosans_12_regular);
  static EpdFont notosans12Bold(&notosans_12_bold);
  static EpdFont notosans12Italic(&notosans_12_italic);
  static EpdFont notosans12BoldItalic(&notosans_12_bolditalic);
  static EpdFontFamily notosans12Family(&notosans12Regular, &notosans12Bold, &notosans12Italic, &notosans12BoldItalic);

  static EpdFont notosans14Regular(&notosans_14_regular);
  static EpdFont notosans14Bold(&notosans_14_bold);
  static EpdFont notosans14Italic(&notosans_14_italic);
  static EpdFont notosans14BoldItalic(&notosans_14_bolditalic);
  static EpdFontFamily notosans14Family(&notosans14Regular, &notosans14Bold, &notosans14Italic, &notosans14BoldItalic);

  static EpdFont notosans16Regular(&notosans_16_regular);
  static EpdFont notosans16Bold(&notosans_16_bold);
  static EpdFont notosans16Italic(&notosans_16_italic);
  static EpdFont notosans16BoldItalic(&notosans_16_bolditalic);
  static EpdFontFamily notosans16Family(&notosans16Regular, &notosans16Bold, &notosans16Italic, &notosans16BoldItalic);

  static EpdFont ui10Regular(&ubuntu_10_regular);
  static EpdFont ui10Bold(&ubuntu_10_bold);
  static EpdFontFamily ui10Family(&ui10Regular, &ui10Bold);

  static EpdFont ui12Regular(&ubuntu_12_regular);
  static EpdFont ui12Bold(&ubuntu_12_bold);
  static EpdFontFamily ui12Family(&ui12Regular, &ui12Bold);

  static EpdFont smallFont(&notosans_8_regular);
  static EpdFontFamily smallFamily(&smallFont);

  renderer.insertFont(BOOKERLY_12_FONT_ID, bookerly12Family);
  renderer.insertFont(BOOKERLY_14_FONT_ID, bookerly14Family);
  renderer.insertFont(BOOKERLY_16_FONT_ID, bookerly16Family);
  renderer.insertFont(BOOKERLY_18_FONT_ID, bookerly18Family);
  renderer.insertFont(NOTOSANS_12_FONT_ID, notosans12Family);
  renderer.insertFont(NOTOSANS_14_FONT_ID, notosans14Family);
  renderer.insertFont(NOTOSANS_16_FONT_ID, notosans16Family);
  renderer.insertFont(UI_10_FONT_ID, ui10Family);
  renderer.insertFont(UI_12_FONT_ID, ui12Family);
  renderer.insertFont(SMALL_FONT_ID, smallFamily);
}

// ============================================================================
// CLI
// ============================================================================

void printUsage(const char* prog) {
  printf("AlbyReader Rendering Test Harness\n\n");
  printf("Usage: %s [options] [test names...]\n\n", prog);
  printf("Options:\n");
  printf("  --help, -h        Show this help\n");
  printf("  --list, -l        List available tests\n");
  printf("  --quiet, -q       Minimal output (for CI)\n");
  printf("  --output DIR      Output directory (default: output)\n");
  printf("\nExamples:\n");
  printf("  %s                   Run all tests\n", prog);
  printf("  %s typography paper  Run specific tests\n", prog);
  printf("  %s --quiet           Run all tests quietly\n", prog);
  printf("  %s --list            Show available tests\n", prog);
}

void printTests() {
  printf("Available tests:\n\n");
  for (const auto& test : g_tests) {
    printf("  %-12s  %s\n", test.name, test.description);
  }
  printf("\n");
}

int main(int argc, char* argv[]) {
  std::vector<std::string> testNames;
  bool showHelp = false;
  bool showList = false;

  // Parse arguments
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    if (arg == "--help" || arg == "-h") {
      showHelp = true;
    } else if (arg == "--list" || arg == "-l") {
      showList = true;
    } else if (arg == "--quiet" || arg == "-q") {
      g_quiet = true;
    } else if (arg == "--output" || arg == "-o") {
      if (i + 1 < argc) {
        g_outputDir = argv[++i];
      } else {
        fprintf(stderr, "Error: --output requires a directory argument\n");
        return 1;
      }
    } else if (arg[0] == '-') {
      fprintf(stderr, "Unknown option: %s\n", arg.c_str());
      return 1;
    } else {
      testNames.push_back(arg);
    }
  }

  if (showHelp) {
    printUsage(argv[0]);
    return 0;
  }

  if (showList) {
    printTests();
    return 0;
  }

  // Ensure output directory exists
  if (!ensureDirectory(g_outputDir)) {
    fprintf(stderr, "Error: Cannot create output directory: %s\n", g_outputDir.c_str());
    return 1;
  }

  log("AlbyReader Rendering Test Harness\n");
  log("Output: %s/\n", g_outputDir.c_str());

  // Setup display and renderer
  EInkDisplay display;
  display.begin();
  GfxRenderer renderer(display);
  setupFonts(renderer);

  // Determine which tests to run
  std::vector<TestCase*> testsToRun;
  if (testNames.empty()) {
    // Run all tests
    for (auto& test : g_tests) {
      testsToRun.push_back(&test);
    }
  } else {
    // Run specified tests
    for (const auto& name : testNames) {
      bool found = false;
      for (auto& test : g_tests) {
        if (name == test.name) {
          testsToRun.push_back(&test);
          found = true;
          break;
        }
      }
      if (!found) {
        fprintf(stderr, "Unknown test: %s\n", name.c_str());
        fprintf(stderr, "Use --list to see available tests\n");
        return 1;
      }
    }
  }

  // Run tests
  for (auto* test : testsToRun) {
    test->fn(renderer, display);
  }

  // Summary
  log("\n");
  if (g_testsFailed == 0) {
    log("All %d tests passed.\n", g_testsRun);
  } else {
    printf("%d/%d tests failed.\n", g_testsFailed, g_testsRun);
  }

  return g_testsFailed > 0 ? 1 : 0;
}
