/**
 * Rendering Test Harness for AlbyReader
 *
 * This test harness renders various content types to PNG files for visual inspection.
 * Run this to see what the e-ink display will look like without needing hardware.
 *
 * Output: test/rendering_harness/output/*.png
 */

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

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

// Test output directory (relative to where the binary runs, which is test/rendering_harness/)
static const char* OUTPUT_DIR = "output";

// Helper to save the current screen
void saveScreen(EInkDisplay& display, const char* testName) {
  char filename[256];
  snprintf(filename, sizeof(filename), "%s/%s.png", OUTPUT_DIR, testName);
  if (display.saveFrameBufferAsPNG(filename)) {
    printf("  [OK] Saved: %s\n", filename);
  } else {
    printf("  [FAIL] Could not save: %s\n", filename);
  }
}

// ============================================================================
// Test Cases
// ============================================================================

/**
 * Test 1: Basic Typography
 * Renders text in different fonts and sizes
 */
void testBasicTypography(GfxRenderer& renderer, EInkDisplay& display) {
  printf("\n[TEST] Basic Typography\n");

  renderer.clearScreen();

  int y = 20;
  const int lineSpacing = 10;

  // Title
  renderer.drawText(UI_12_FONT_ID, 20, y, "Typography Test - Font Comparison", true, EpdFontFamily::BOLD);
  y += renderer.getLineHeight(UI_12_FONT_ID) + lineSpacing * 2;

  // Bookerly fonts
  renderer.drawText(BOOKERLY_12_FONT_ID, 20, y, "Bookerly 12: The quick brown fox jumps over the lazy dog");
  y += renderer.getLineHeight(BOOKERLY_12_FONT_ID) + lineSpacing;

  renderer.drawText(BOOKERLY_14_FONT_ID, 20, y, "Bookerly 14: The quick brown fox jumps over the lazy dog");
  y += renderer.getLineHeight(BOOKERLY_14_FONT_ID) + lineSpacing;

  renderer.drawText(BOOKERLY_16_FONT_ID, 20, y, "Bookerly 16: The quick brown fox");
  y += renderer.getLineHeight(BOOKERLY_16_FONT_ID) + lineSpacing;

  renderer.drawText(BOOKERLY_18_FONT_ID, 20, y, "Bookerly 18: The quick brown fox");
  y += renderer.getLineHeight(BOOKERLY_18_FONT_ID) + lineSpacing * 2;

  // Noto Sans fonts
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

  // UI fonts
  renderer.drawText(UI_10_FONT_ID, 20, y, "UI Font 10: Used for interface elements");
  y += renderer.getLineHeight(UI_10_FONT_ID) + lineSpacing;

  renderer.drawText(UI_12_FONT_ID, 20, y, "UI Font 12: Used for buttons and menus");
  y += renderer.getLineHeight(UI_12_FONT_ID) + lineSpacing;

  renderer.drawText(SMALL_FONT_ID, 20, y, "Small Font: Used for status bars and fine print");

  display.displayBuffer();
  saveScreen(display, "01_typography_basic");
}

/**
 * Test 2: Unicode and Special Characters
 * Tests rendering of extended character sets
 */
void testUnicodeCharacters(GfxRenderer& renderer, EInkDisplay& display) {
  printf("\n[TEST] Unicode Characters\n");

  renderer.clearScreen();

  int y = 20;
  const int lineSpacing = 15;

  renderer.drawText(UI_12_FONT_ID, 20, y, "Unicode & Special Characters Test", true, EpdFontFamily::BOLD);
  y += renderer.getLineHeight(UI_12_FONT_ID) + lineSpacing * 2;

  // Common punctuation and symbols
  renderer.drawText(BOOKERLY_14_FONT_ID, 20, y, "Punctuation: \"Hello, World!\" - What's up? (test) [array]");
  y += renderer.getLineHeight(BOOKERLY_14_FONT_ID) + lineSpacing;

  // Accented characters (Latin Extended)
  renderer.drawText(BOOKERLY_14_FONT_ID, 20, y, "Accents: cafe, naive, resume, Zurn, Grosse");
  y += renderer.getLineHeight(BOOKERLY_14_FONT_ID) + lineSpacing;

  // Mathematical operators that might be in fonts
  renderer.drawText(NOTOSANS_14_FONT_ID, 20, y, "Math: + - * / = < > % # @ & |");
  y += renderer.getLineHeight(NOTOSANS_14_FONT_ID) + lineSpacing;

  // Currency symbols
  renderer.drawText(NOTOSANS_14_FONT_ID, 20, y, "Currency: $ EUR GBP JPY");
  y += renderer.getLineHeight(NOTOSANS_14_FONT_ID) + lineSpacing;

  // Numbers
  renderer.drawText(BOOKERLY_14_FONT_ID, 20, y, "Numbers: 0123456789 3.14159 1,000,000");
  y += renderer.getLineHeight(BOOKERLY_14_FONT_ID) + lineSpacing;

  // Fractions and superscripts (if supported)
  renderer.drawText(NOTOSANS_14_FONT_ID, 20, y, "Fractions: 1/2 1/4 3/4 (ASCII fallback)");
  y += renderer.getLineHeight(NOTOSANS_14_FONT_ID) + lineSpacing * 2;

  // Sample academic text
  renderer.drawText(UI_12_FONT_ID, 20, y, "Sample Academic Text:", true, EpdFontFamily::BOLD);
  y += renderer.getLineHeight(UI_12_FONT_ID) + lineSpacing;

  renderer.drawText(BOOKERLY_14_FONT_ID, 20, y, "The algorithm runs in O(n log n) time complexity.");
  y += renderer.getLineHeight(BOOKERLY_14_FONT_ID) + lineSpacing;

  renderer.drawText(BOOKERLY_14_FONT_ID, 20, y, "Let f(x) = x^2 + 2x + 1, then f'(x) = 2x + 2.");
  y += renderer.getLineHeight(BOOKERLY_14_FONT_ID) + lineSpacing;

  renderer.drawText(BOOKERLY_14_FONT_ID, 20, y, "E = mc^2 (Einstein's mass-energy equivalence)");

  display.displayBuffer();
  saveScreen(display, "02_unicode_characters");
}

/**
 * Test 3: Text Layout - Paragraph Rendering
 * Tests multi-line text and word wrapping
 */
void testParagraphLayout(GfxRenderer& renderer, EInkDisplay& display) {
  printf("\n[TEST] Paragraph Layout\n");

  renderer.clearScreen();

  const int margin = 20;
  const int maxWidth = renderer.getScreenWidth() - 2 * margin;
  int y = 20;

  renderer.drawText(UI_12_FONT_ID, margin, y, "Paragraph Layout Test", true, EpdFontFamily::BOLD);
  y += renderer.getLineHeight(UI_12_FONT_ID) + 20;

  // Sample paragraph - manual word wrapping simulation
  const char* paragraph =
      "This is a sample paragraph that demonstrates text layout capabilities. "
      "Good typography is essential for readability on e-ink displays. "
      "The text should flow naturally across multiple lines with proper spacing. "
      "Academic papers often contain dense paragraphs that require careful rendering.";

  // Simple word-wrap implementation for testing
  std::string text = paragraph;
  std::string line;
  std::string word;
  int fontId = BOOKERLY_14_FONT_ID;
  int lineHeight = renderer.getLineHeight(fontId);

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

  // Add section header
  renderer.drawText(UI_12_FONT_ID, margin, y, "Abstract:", true, EpdFontFamily::BOLD);
  y += renderer.getLineHeight(UI_12_FONT_ID) + 10;

  // Another sample text with different styling
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
  saveScreen(display, "03_paragraph_layout");
}

/**
 * Test 4: Graphics Primitives
 * Tests lines, rectangles, and basic shapes
 */
void testGraphicsPrimitives(GfxRenderer& renderer, EInkDisplay& display) {
  printf("\n[TEST] Graphics Primitives\n");

  renderer.clearScreen();

  int y = 20;

  renderer.drawText(UI_12_FONT_ID, 20, y, "Graphics Primitives Test", true, EpdFontFamily::BOLD);
  y += 40;

  // Horizontal lines
  renderer.drawText(SMALL_FONT_ID, 20, y, "Horizontal lines:");
  y += 15;
  for (int i = 0; i < 5; i++) {
    renderer.drawLine(20, y + i * 10, 200, y + i * 10);
  }
  y += 60;

  // Vertical lines
  renderer.drawText(SMALL_FONT_ID, 20, y, "Vertical lines:");
  y += 15;
  for (int i = 0; i < 5; i++) {
    renderer.drawLine(20 + i * 20, y, 20 + i * 20, y + 50);
  }

  // Rectangles on the right side
  int rectX = 250;
  int rectY = 80;

  renderer.drawText(SMALL_FONT_ID, rectX, rectY - 20, "Rectangles:");
  renderer.drawRect(rectX, rectY, 80, 40);
  renderer.drawRect(rectX + 100, rectY, 80, 40);
  renderer.fillRect(rectX + 100 + 20, rectY + 10, 40, 20);

  // Nested rectangles
  rectY += 60;
  renderer.drawText(SMALL_FONT_ID, rectX, rectY - 10, "Nested:");
  for (int i = 0; i < 4; i++) {
    renderer.drawRect(rectX + i * 5, rectY + i * 5, 80 - i * 10, 60 - i * 10);
  }

  // Grid pattern
  int gridX = 20;
  int gridY = 200;
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

  // Border demo (simulating UI elements)
  int borderX = 250;
  int borderY = 200;
  renderer.drawText(SMALL_FONT_ID, borderX, borderY - 10, "UI Border styles:");

  // Simple border
  renderer.drawRect(borderX, borderY, 150, 50);
  renderer.drawText(SMALL_FONT_ID, borderX + 10, borderY + 15, "Simple border");

  // Double border
  borderY += 70;
  renderer.drawRect(borderX, borderY, 150, 50);
  renderer.drawRect(borderX + 3, borderY + 3, 144, 44);
  renderer.drawText(SMALL_FONT_ID, borderX + 15, borderY + 15, "Double border");

  display.displayBuffer();
  saveScreen(display, "04_graphics_primitives");
}

/**
 * Test 5: Simulated Paper Layout
 * Mimics how an academic paper might look
 */
void testPaperLayout(GfxRenderer& renderer, EInkDisplay& display) {
  printf("\n[TEST] Paper Layout Simulation\n");

  renderer.clearScreen();

  const int margin = 25;
  const int maxWidth = renderer.getScreenWidth() - 2 * margin;
  int y = margin;

  // Title
  renderer.drawCenteredText(BOOKERLY_18_FONT_ID, y, "Self-Forcing: A Novel Approach to", true, EpdFontFamily::BOLD);
  y += renderer.getLineHeight(BOOKERLY_18_FONT_ID);
  renderer.drawCenteredText(BOOKERLY_18_FONT_ID, y, "Speculative Decoding", true, EpdFontFamily::BOLD);
  y += renderer.getLineHeight(BOOKERLY_18_FONT_ID) + 15;

  // Authors
  renderer.drawCenteredText(NOTOSANS_12_FONT_ID, y, "Anonymous Authors");
  y += renderer.getLineHeight(NOTOSANS_12_FONT_ID) + 5;
  renderer.drawCenteredText(SMALL_FONT_ID, y, "Anonymous Institution");
  y += renderer.getLineHeight(SMALL_FONT_ID) + 20;

  // Separator line
  renderer.drawLine(margin, y, renderer.getScreenWidth() - margin, y);
  y += 15;

  // Abstract header
  renderer.drawText(NOTOSANS_14_FONT_ID, margin, y, "Abstract", true, EpdFontFamily::BOLD);
  y += renderer.getLineHeight(NOTOSANS_14_FONT_ID) + 5;

  // Abstract text (simplified layout)
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

  // Section 1
  renderer.drawText(NOTOSANS_14_FONT_ID, margin, y, "1. Introduction", true, EpdFontFamily::BOLD);
  y += renderer.getLineHeight(NOTOSANS_14_FONT_ID) + 5;

  const char* introText =
      "Large language models have shown remarkable capabilities, but their "
      "autoregressive generation process remains a bottleneck. Speculative "
      "decoding offers a promising solution by using a smaller draft model.";

  text = introText;
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
    y += lineHeight;
  }

  y += 15;

  // Equation placeholder
  renderer.drawText(fontId, margin + 50, y, "y = f(x) = argmax P(w|context)    (1)", true, EpdFontFamily::ITALIC);
  y += lineHeight + 15;

  // Continue with more text...
  renderer.drawText(fontId, margin, y, "Our key insight is that the model can serve as its own draft");
  y += lineHeight;
  renderer.drawText(fontId, margin, y, "generator by using early exit layers or cached computations.");

  // Page footer
  y = renderer.getScreenHeight() - 30;
  renderer.drawLine(margin, y, renderer.getScreenWidth() - margin, y);
  y += 10;
  renderer.drawCenteredText(SMALL_FONT_ID, y, "1");

  display.displayBuffer();
  saveScreen(display, "05_paper_layout");
}

/**
 * Test 6: Reading Interface Simulation
 * Shows what the actual reading experience might look like
 */
void testReadingInterface(GfxRenderer& renderer, EInkDisplay& display) {
  printf("\n[TEST] Reading Interface\n");

  renderer.clearScreen();

  // Status bar at top
  renderer.fillRect(0, 0, renderer.getScreenWidth(), 25, false);  // White background
  renderer.drawLine(0, 25, renderer.getScreenWidth(), 25);

  renderer.drawText(SMALL_FONT_ID, 10, 5, "Chapter 3: Methods");
  renderer.drawText(SMALL_FONT_ID, renderer.getScreenWidth() - 80, 5, "Page 42/128");

  // Battery indicator (simple rectangle)
  int battX = renderer.getScreenWidth() - 35;
  renderer.drawRect(battX, 7, 25, 12);
  renderer.fillRect(battX + 2, 9, 18, 8);  // Battery fill
  renderer.fillRect(battX + 25, 10, 3, 6);  // Battery tip

  // Main content area
  const int margin = 20;
  const int topMargin = 40;
  const int maxWidth = renderer.getScreenWidth() - 2 * margin;
  int y = topMargin;

  // Chapter content
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
      "",
      "All experiments are conducted on NVIDIA A100 GPUs with identical",
      "batch sizes and sequence lengths for fair comparison."
  };

  for (const char* line : content) {
    if (strlen(line) == 0) {
      y += lineHeight / 2;  // Paragraph spacing
      continue;
    }

    // Check if it's a section header
    bool isHeader = (line[0] == '3' && line[1] == '.');
    bool isBullet = (line[0] == '-' || (line[0] >= '1' && line[0] <= '9' && line[1] == '.'));

    int xOffset = margin;
    if (isBullet) xOffset += 20;

    if (isHeader) {
      y += 10;  // Extra space before header
      renderer.drawText(fontId, xOffset, y, line, true, EpdFontFamily::BOLD);
    } else {
      renderer.drawText(fontId, xOffset, y, line);
    }
    y += lineHeight;

    if (y > renderer.getScreenHeight() - 50) break;
  }

  // Progress bar at bottom
  int progressY = renderer.getScreenHeight() - 15;
  renderer.drawRect(margin, progressY, renderer.getScreenWidth() - 2 * margin, 8);
  int progressWidth = (int)((renderer.getScreenWidth() - 2 * margin - 4) * 0.33);  // 33% through
  renderer.fillRect(margin + 2, progressY + 2, progressWidth, 4);

  display.displayBuffer();
  saveScreen(display, "06_reading_interface");
}

/**
 * Test 7: Screen Orientations
 * Tests portrait vs landscape rendering
 */
void testOrientations(GfxRenderer& renderer, EInkDisplay& display) {
  printf("\n[TEST] Screen Orientations\n");

  // Test Portrait (default)
  renderer.setOrientation(GfxRenderer::Portrait);
  renderer.clearScreen();

  renderer.drawText(UI_12_FONT_ID, 20, 20, "Portrait Mode (480x800)", true, EpdFontFamily::BOLD);
  renderer.drawText(BOOKERLY_14_FONT_ID, 20, 60, "Width: 480 pixels");
  renderer.drawText(BOOKERLY_14_FONT_ID, 20, 85, "Height: 800 pixels");

  // Draw border to show screen bounds
  renderer.drawRect(5, 5, renderer.getScreenWidth() - 10, renderer.getScreenHeight() - 10);

  // Draw some content to show orientation
  renderer.drawText(BOOKERLY_14_FONT_ID, 20, 150, "This is the natural reading orientation.");
  renderer.drawText(BOOKERLY_14_FONT_ID, 20, 175, "Text flows from top to bottom.");

  renderer.fillRect(20, 220, 100, 200);  // Vertical rectangle
  renderer.drawText(SMALL_FONT_ID, 130, 300, "<- Tall rectangle");

  display.displayBuffer();
  saveScreen(display, "07a_portrait_mode");

  // Test Landscape
  renderer.setOrientation(GfxRenderer::LandscapeClockwise);
  renderer.clearScreen();

  renderer.drawText(UI_12_FONT_ID, 20, 20, "Landscape Mode (800x480)", true, EpdFontFamily::BOLD);
  renderer.drawText(BOOKERLY_14_FONT_ID, 20, 60, "Width: 800 pixels");
  renderer.drawText(BOOKERLY_14_FONT_ID, 20, 85, "Height: 480 pixels");

  // Draw border
  renderer.drawRect(5, 5, renderer.getScreenWidth() - 10, renderer.getScreenHeight() - 10);

  // Wide content
  renderer.drawText(BOOKERLY_14_FONT_ID, 20, 150, "Landscape mode is good for wide tables and figures.");
  renderer.drawText(BOOKERLY_14_FONT_ID, 20, 175, "More horizontal space for equations and code.");

  renderer.fillRect(20, 220, 300, 80);  // Wide rectangle
  renderer.drawText(SMALL_FONT_ID, 340, 250, "<- Wide rectangle");

  display.displayBuffer();
  saveScreen(display, "07b_landscape_mode");

  // Reset to portrait
  renderer.setOrientation(GfxRenderer::Portrait);
}

/**
 * Test 8: Stress Test - Dense Content
 * Tests rendering of very dense text
 */
void testDenseContent(GfxRenderer& renderer, EInkDisplay& display) {
  printf("\n[TEST] Dense Content\n");

  renderer.clearScreen();

  const int margin = 15;
  int y = 10;
  int fontId = SMALL_FONT_ID;  // Smallest font for maximum density
  int lineHeight = renderer.getLineHeight(fontId);

  renderer.drawText(UI_10_FONT_ID, margin, y, "Dense Content Stress Test", true, EpdFontFamily::BOLD);
  y += renderer.getLineHeight(UI_10_FONT_ID) + 5;

  // Fill the screen with as much text as possible
  const char* loremIpsum =
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed do eiusmod "
      "tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, "
      "quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo "
      "consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse "
      "cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat "
      "non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";

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

    y += 5;  // Paragraph spacing
    paragraphCount++;
  }

  display.displayBuffer();
  saveScreen(display, "08_dense_content");
}

// ============================================================================
// Main Test Runner
// ============================================================================

int main() {
  printf("===========================================\n");
  printf("AlbyReader Rendering Test Harness\n");
  printf("===========================================\n");
  printf("Output directory: %s\n", OUTPUT_DIR);

  // Create mock display
  EInkDisplay display;
  display.begin();

  // Create renderer
  GfxRenderer renderer(display);

  // Register fonts
  printf("\nRegistering fonts...\n");

  // Bookerly fonts
  EpdFont bookerly12Regular(&bookerly_12_regular);
  EpdFont bookerly12Bold(&bookerly_12_bold);
  EpdFont bookerly12Italic(&bookerly_12_italic);
  EpdFont bookerly12BoldItalic(&bookerly_12_bolditalic);
  EpdFontFamily bookerly12Family(&bookerly12Regular, &bookerly12Bold, &bookerly12Italic, &bookerly12BoldItalic);

  EpdFont bookerly14Regular(&bookerly_14_regular);
  EpdFont bookerly14Bold(&bookerly_14_bold);
  EpdFont bookerly14Italic(&bookerly_14_italic);
  EpdFont bookerly14BoldItalic(&bookerly_14_bolditalic);
  EpdFontFamily bookerly14Family(&bookerly14Regular, &bookerly14Bold, &bookerly14Italic, &bookerly14BoldItalic);

  EpdFont bookerly16Regular(&bookerly_16_regular);
  EpdFont bookerly16Bold(&bookerly_16_bold);
  EpdFont bookerly16Italic(&bookerly_16_italic);
  EpdFont bookerly16BoldItalic(&bookerly_16_bolditalic);
  EpdFontFamily bookerly16Family(&bookerly16Regular, &bookerly16Bold, &bookerly16Italic, &bookerly16BoldItalic);

  EpdFont bookerly18Regular(&bookerly_18_regular);
  EpdFont bookerly18Bold(&bookerly_18_bold);
  EpdFont bookerly18Italic(&bookerly_18_italic);
  EpdFont bookerly18BoldItalic(&bookerly_18_bolditalic);
  EpdFontFamily bookerly18Family(&bookerly18Regular, &bookerly18Bold, &bookerly18Italic, &bookerly18BoldItalic);

  // Noto Sans fonts
  EpdFont notosans12Regular(&notosans_12_regular);
  EpdFont notosans12Bold(&notosans_12_bold);
  EpdFont notosans12Italic(&notosans_12_italic);
  EpdFont notosans12BoldItalic(&notosans_12_bolditalic);
  EpdFontFamily notosans12Family(&notosans12Regular, &notosans12Bold, &notosans12Italic, &notosans12BoldItalic);

  EpdFont notosans14Regular(&notosans_14_regular);
  EpdFont notosans14Bold(&notosans_14_bold);
  EpdFont notosans14Italic(&notosans_14_italic);
  EpdFont notosans14BoldItalic(&notosans_14_bolditalic);
  EpdFontFamily notosans14Family(&notosans14Regular, &notosans14Bold, &notosans14Italic, &notosans14BoldItalic);

  EpdFont notosans16Regular(&notosans_16_regular);
  EpdFont notosans16Bold(&notosans_16_bold);
  EpdFont notosans16Italic(&notosans_16_italic);
  EpdFont notosans16BoldItalic(&notosans_16_bolditalic);
  EpdFontFamily notosans16Family(&notosans16Regular, &notosans16Bold, &notosans16Italic, &notosans16BoldItalic);

  // UI fonts
  EpdFont ui10Regular(&ubuntu_10_regular);
  EpdFont ui10Bold(&ubuntu_10_bold);
  EpdFontFamily ui10Family(&ui10Regular, &ui10Bold);

  EpdFont ui12Regular(&ubuntu_12_regular);
  EpdFont ui12Bold(&ubuntu_12_bold);
  EpdFontFamily ui12Family(&ui12Regular, &ui12Bold);

  // Small font
  EpdFont smallFont(&notosans_8_regular);
  EpdFontFamily smallFamily(&smallFont);

  // Register all fonts
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

  printf("Fonts registered.\n");

  // Run all tests
  printf("\n--- Running Rendering Tests ---\n");

  testBasicTypography(renderer, display);
  testUnicodeCharacters(renderer, display);
  testParagraphLayout(renderer, display);
  testGraphicsPrimitives(renderer, display);
  testPaperLayout(renderer, display);
  testReadingInterface(renderer, display);
  testOrientations(renderer, display);
  testDenseContent(renderer, display);

  printf("\n===========================================\n");
  printf("All tests completed!\n");
  printf("Check output files in: %s\n", OUTPUT_DIR);
  printf("===========================================\n");

  return 0;
}
