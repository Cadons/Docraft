# Docraft v1.0.0 Release Notes

**Release Date:** August 31, 2026  
**Status:** Stable Release

---

## Overview

Docraft v1.0.0 is the **stable release milestone** for the project: a production-ready C++23, in-process PDF generation library built around the XML-based Craft Language. This release marks the graduation from release candidates to a fully supported, feature-complete version.

## What's New in v1.0.0

### Stable Production Build

- **Zero breaking changes** from RC10—all features are production-hardened and tested
- Comprehensive test coverage across document models, layout, rendering, and templating
- Full documentation and example gallery ready for enterprise adoption

### Core Features (from Beta → Stable)

#### Document Markup & Layout
- **Craft Language** — Declarative XML markup for document authoring (no coordinate-level drawing)
- **Automatic pagination** — Flow-based layout engine with intelligent page breaking
- **Header/Body/Footer sections** — Define once; automatically repeated and managed
- **Page numbering** — Built-in `<PageNumber/>` element with automatic calculation

#### Text & Typography
- **Rich text styling** — Font family, size, weight (bold/regular/italic), underline, alignment (left/center/right/justified), and color
- **Custom font support** — Register TTF fonts (all variants: regular, bold, italic, bold-italic) for full typographic control
- **Default fonts** — Helvetica, Times, Courier pre-bundled; additional fonts available via registration

#### Layout & Composition
- **Flexible layouts** — Horizontal and vertical `<Layout>` elements with weight-based sizing
- **Absolute positioning** — Place elements at exact (x, y) coordinates when flow layout is inappropriate
- **Z-index stacking** — Control rendering order with `z_index` attribute
- **Nested composition** — Arbitrarily deep nesting of layout containers

#### Rich Document Elements
- **Tables** — Column headers, row/column weights, per-cell background colors, borders, cell-level formatting, and JSON model binding for data-driven rendering
- **Lists** — Ordered (numeric, roman numerals) and unordered (dash, star, circle, box) with customizable markers
- **Shapes** — Rectangle, Circle/Oval, Triangle, Line, Polygon with background/border styling
- **Canvas** — Free-form drawing surface for coordinate-based shapes and paths
- **Charts** — Native rendering of Pie, Bar/Histogram, Line, Spline, and Scatter charts with data binding
- **Images** — PNG, JPEG from file, and raw RGB pixel data (including base64-encoded runtime injection)

#### Data Binding & Templating
- **Template variables** — `${variable}` syntax for runtime value substitution
- **Nested data** — Dot-notation support (e.g., `${customer.name}`)
- **Array iteration** — `<Foreach>` loop blocks for data-driven lists and tables
- **JSON data sources** — Bind external JSON files to document templates

#### Document Metadata
- **Author, title, subject, keywords** — Full PDF metadata support
- **Creation date** — Automatic or custom-specified
- **Keyword extraction** — Automatic content analysis for searchable PDFs

#### Page Configuration
- **Multiple page formats** — A3, A4, A5, Letter, Legal
- **Orientation support** — Portrait and landscape
- **Configurable margins** — Header/body/footer ratios and custom spacing

### Recent Stabilizations (RC9 → 1.0.0)

- **Vcpkg integration fixes** — Full Windows/VCPKG build validation and testing
- **Memory safety** — Resolved unique_ptr map lifecycle issues in copy operations
- **Documentation updates** — Expanded examples and API reference

### Build & Integration

- **CMake 3.16+** — Modern build configuration with clear dependency resolution
- **C++23 standard** — Leveraging latest language features for efficiency and safety
- **Pluggable rendering** — Backend abstraction supports custom implementations; libharu currently integrated
- **Static and shared builds** — Both library modes fully supported

### Platforms

- ✅ **macOS** — AppleClang/LLVM, via Homebrew
- ✅ **Linux** — GCC 14+, Clang 16+, Ubuntu/Debian tested
- ✅ **Windows** — MSVC 2022 and LLVM/Clang with vcpkg
- ✅ **Docker** — Pre-built image includes `docraft_tool` CLI

### CLI Tool

- **docraft_tool** executable for rendering `.craft` files to PDF
- **Command-line usage:**
  ```bash
  docraft_tool <input.craft> <output.pdf>
  docraft_tool <input.craft> <output.pdf> --data <data.json>
  ```
- **Data binding** — Pass JSON or key-value data for runtime template substitution

---

## Dependencies

| Dependency | Purpose | License |
|------------|---------|---------|
| libharu    | PDF generation backend | Zlib |
| pugixml    | XML parsing (Craft Language) | MIT |
| nlohmann-json | JSON data binding | MIT |
| GoogleTest | Testing framework (optional) | BSD-3-Clause |

---

## Known Limitations & Future Work

### Limitations
- Backend support is currently centered on libharu (no alternate PDF backends shipping yet)
- Performance tuning for very large documents (1000+ pages) is an ongoing optimization area
- Some advanced PDF features (e.g., annotations, forms, encryption) not yet exposed

### Planned for Future Releases
- Additional PDF rendering backends
- Performance improvements for document generation
- Extended accessibility features (improved tag export)
- Advanced typography (kerning, ligatures, OpenType features)

---

## Breaking Changes

**None.** This is the stable release of 1.0.0—all public APIs are frozen and will remain backward-compatible through the 1.x series.

---

## Migration Guide

If you are upgrading from a beta or RC release:

1. **No API changes** — Existing integrations will work without modification
2. **CMake integration** — If using `find_package(Docraft)`, dependencies are now correctly declared; no additional `find_package(fmt)` workaround needed
3. **New users** — Start with the examples in `assets/readme/` and [the documentation site](https://cadons.github.io/docraft/)

---

## Installation & Getting Started

### Quick Start (Linux/macOS)

```bash
git clone https://github.com/Cadons/Docraft.git
cd Docraft
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
./build/artifacts/bin/docraft_tool assets/readme/showcase.craft showcase.pdf
```

### With Package Managers

**macOS (Homebrew):**
```bash
brew install libharu pugixml nlohmann-json
```

**Ubuntu/Debian:**
```bash
sudo apt-get install libhpdf-dev libpugixml-dev nlohmann-json3-dev
```

**Windows (vcpkg):**
```powershell
vcpkg install libharu pugixml nlohmann-json --triplet x64-windows
```

### Docker

```bash
docker build -f docker/Dockerfile -t docraft_tool:latest .
docker run --rm -v "$PWD:/work" -w /work docraft_tool:latest sample.craft output.pdf
```

---

## Example Documents

The [example gallery](https://cadons.github.io/docraft/examples/index.html) includes:
- Business invoices
- Shipping labels
- Sales reports
- QC reports
- Showcase documents with charts, shapes, tables, and typography

All source files are in `assets/readme/` and `doc/examples/`.

---

## Contributors

Thanks to all contributors, maintainers, and beta testers who helped bring Docraft to stable release. Special thanks to:
- The open-source C++ community for libharu, pugixml, and nlohmann-json
- GitHub Copilot for architectural and refactoring support
- Early adopters for validation and feedback

---

## Support & Documentation

- **API Documentation:** [Doxygen docs](https://cadons.github.io/docraft/api/)
- **Getting Started:** [Quick Start Guide](README.md#quick-start-command-line)
- **Examples:** [Example Gallery](https://cadons.github.io/docraft/examples/index.html)
- **Contributing:** [CONTRIBUTING.md](CONTRIBUTING.md)
- **Issues & Discussions:** [GitHub Issues](https://github.com/Cadons/Docraft/issues)

---

## Full Changelog

From v1.0.0-RC10:
- Minor internal improvements and test enhancements
- No public API changes

For detailed history, see the [comparison from RC1 to stable](https://github.com/Cadons/Docraft/compare/v1.0.0-RC1...v1.0.0).

---

## License

Docraft is licensed under the **Apache License 2.0**.  
See [LICENSE](LICENSE) for full details.
