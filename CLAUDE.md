# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

Docraft is a C++23 library for generating PDF documents declaratively from an XML-based
"Craft Language" markup. It runs entirely in-process (no shelling out to LaTeX/wkhtmltopdf/etc.),
using **libharu** as the PDF backend, **pugixml** for XML parsing, and **nlohmann-json** for
template data binding.

## Build, test, run

Standard (non-vcpkg) flow, assuming deps installed via Homebrew/apt (see README.md for exact
packages: libharu, pugixml, nlohmann-json, gtest, fmt):

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
cmake --build build --config Release -j
ctest --test-dir build -C Release --output-on-failure
```

vcpkg flow (Windows, or if you prefer manifest mode) uses `CMakePresets.json` /
`CMakeUserPresets.json` and requires `VCPKG_ROOT` set; toolchain file is
`$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake`.

Run a single test (GoogleTest, via ctest name or direct binary + filter):

```bash
ctest --test-dir build -C Release -R DocraftLayoutEngineTest.SomeCase --output-on-failure
./build/artifacts/bin/docraft_test --gtest_filter=DocraftLayoutEngineTest.SomeCase
```

Stress tests are a separate target/binary and labeled, excluded from normal runs unless asked for:

```bash
ctest --test-dir build -C Release -L stress
```

CLI tools (built alongside the library):

```bash
./build/artifacts/bin/docraft_tool <file.craft> <output.pdf> [--data data.json]
```

`BUILD_SHARED_LIBS=ON` builds docraft as a shared lib instead of static (default OFF).

Fonts under `docraft/resources/fonts/` are embedded into the binary at configure time via
`generate_docraft_fonts()` (`.cmake/generate_fonts.cmake`), driven by `docraft/fonts.json`. If you
add a font file, register it in `fonts.json` — the CMake step generates the embedding header.

Docs (Sphinx + Doxygen) live in `doc/source/`; build with `cd doc && make html`.

## Two parallel architectures — know which one you're in

The codebase currently contains **two layout/rendering pipelines** side by side. This is
intentional, active migration work (see branch names like
`find-the-best-pattern-for-layout-engine`) — do not assume one is dead code without checking
recent commits/CMakeLists first.

### 1. Legacy pipeline (`docraft/{include,src}/docraft/{model,layout,renderer,backend,services}`)

```
Craft XML → CraftLanguageParser (pugixml) → DocraftNode AST
  → Template Engine (${var} substitution, <Foreach> expansion)
  → Layout Engine: Chain-of-Responsibility handlers (layout/handler/*) compute x/y/width/height + pagination
  → Renderer: Visitor pattern walks the AST, dispatching to Painters (renderer/painter/*)
  → Painters call capability-provider Backend interfaces (backend/docraft_*_rendering_backend.h)
  → DocraftHaruBackend (backend/pdf/*) implements those interfaces on libharu
```

- `DocraftDocument` (include/docraft/docraft_document.h) is the entry point; orchestrates
  `configure_settings → template_document → layout → render → save_to_file`.
- `DocraftDocumentContext` holds shared render state; note it's a known Service-Locator-style
  god object (20+ getters) — treat as legacy, avoid extending its surface.
- Backend interfaces are split by capability domain (Text/Line/Shape/Image/Page/Output/Font/Metadata
  rendering backends), not one aggregated facade — this split is deliberate (a prior aggregated
  facade was removed for SRP reasons).
- Node types (Text, Table, List, Image, Shape, Layout, Paragraph, ...) each have: a parser
  registration, a layout handler, and a paint/render path. Adding a node type touches all three
  plus tests.
- Known architectural pain points are catalogued in `.local/ARCHITETTURA_CRITICITA.md` (circular
  deps between PageHaruBackend/DocraftHaruBackend, scattered ownership of cursor/color state, no
  fallback strategy for unsupported backend capabilities, etc.) — useful context before touching
  `backend/` or `docraft_document_context`.

### 2. New "loom" pipeline (`docraft/{include,src}/docraft/loom/`)

A from-scratch redesign of the layout/render path, built around a **pipeline-of-visitors** over a
plain node tree rather than chain-of-responsibility handlers + a document-context god object:

```
DocraftLoomNode tree (nodes/: Text, Paragraph, Rectangle, VStack, HStack, ...)
  → MeasureProcessor   (visitor: computes LayoutBox.measured_size per node)
  → LayoutProcessor    (visitor: walks tree with a DocraftLoomCursor, fills LayoutBox.frame)
  → PaginationProcessor(visitor: fills LayoutBox.page_index)
  → RenderingProcessor (visitor: paints using the final frame/page_index)
```

- Each processor implements `interfaces::DocraftLoomIVisitor` (double-dispatch visitor, one
  `visit(NodeType*)` overload per node type) — adding a node type means adding a `visit` overload
  to every processor, not editing a big switch.
- `DocraftLoomNode::layout_box()` accumulates the results of each pipeline stage
  (`measured_size` → `page_index` → `frame`) — later stages read fields earlier stages wrote, so
  pipeline order matters and stages must not be skipped for a node type.
- `DocraftLoomCursor` tracks the "pen" position during layout (origin top-left, y grows downward),
  separate from any node — this replaces the old handler-local cursor bookkeeping.
- `DocraftLoomPdfCreator` is the loom-side equivalent of `DocraftDocument` (top-level orchestrator);
  `docraft_tool` (`docraft/src/docraft/main.cpp`) exercises it end to end via
  `DocraftLoomCraftLanguageParser`.
- Tests live in `docraft/test/docraft/loom/`; `docraft/test/CMakeLists.txt` is not glob-based — if
  you add a new loom test file, you must add it to TEST_SOURCES there or it silently won't run.

When asked to work on layout/rendering, check whether the task is about the legacy pipeline or
`loom` — they don't share types, and "the layout engine" is ambiguous in this repo right now.

## Coding conventions (enforced by `.clang-tidy`, warnings are errors)

- Namespaces: `lower_case` (e.g. `docraft::loom::pipeline`)
- Classes/structs: `CamelCase`, prefixed `Docraft...` for public types (e.g. `DocraftLoomNode`)
- Variables/members: `lower_case`; private/protected members get a trailing underscore (`name_`)
- Constants (static/member/global/constexpr) and enum constants: `kCamelCase`
- Public headers under `docraft/include/docraft/...`, implementations under
  `docraft/src/docraft/...`, mirroring the same subpath; tests under `docraft/test/docraft/...`
- Every new source/header file must be added explicitly to `docraft/CMakeLists.txt`
  (`DOCRAFT_SOURCES`) and, for tests, to `docraft/test/CMakeLists.txt` (`TEST_SOURCES`) — CMake
  here does not glob.

## Commit messages

Format enforced by convention (`.github/git-commit-instructions.md`): first line
`<type>(<scope>): <short summary>`, imperative mood, ~74 chars. Types: `feat`, `fix`, `docs`,
`style`, `refactor`, `perf`, `test`, `chore`, `build`, `ci`, `revert`.

## Craft Language quick reference

- `<Document>` and `<Body>` are required; unknown tags or invalid enum attribute values are parse
  errors (not warnings).
- Templating: `${variable}` substitution and `<Foreach model="${items}">` expansion happen before
  layout, driven by JSON data (`data("key")` inside a `Foreach`).
- Common attributes across nodes: `name`, `x`, `y`, `width`, `height`, `padding`,
  `z_index`, `position`, `visible`. Colors are `#RRGGBB` or named (`red`, `blue`, ...).

Loom architecture is the newer architecture, follow it as main architecture, and use the old one only to understand
which items must be port into the new architecture