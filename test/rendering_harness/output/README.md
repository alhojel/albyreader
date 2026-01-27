# Rendering Test Output

These PNG files show exactly what the 800×480 e-ink display will render.

Images are rotated 90° because the device uses portrait mode (480×800 logical) on a landscape panel.

## Test Results

| Test | File | Description |
|------|------|-------------|
| Typography | `01_typography.png` | Bookerly, Noto Sans, Ubuntu fonts at sizes 12-18 |
| Unicode | `02_unicode.png` | Math symbols, equations: O(n log n), f(x)=x², E=mc² |
| Paragraph | `03_paragraph.png` | Word wrapping and multi-line layout |
| Graphics | `04_graphics.png` | Lines, rectangles, checkerboard pattern |
| Paper | `05_paper.png` | Academic paper: title, abstract, sections |
| Reader | `06_reader.png` | Full UI: status bar, content, progress |
| Portrait | `07_portrait.png` | Portrait orientation (480×800) |
| Landscape | `08_landscape.png` | Landscape orientation (800×480) |
| Dense | `09_dense.png` | Maximum text density stress test |

## Regenerate

```bash
cd test/rendering_harness
make run
```
