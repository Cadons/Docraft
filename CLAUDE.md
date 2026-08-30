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

## Architecture: the loom pipeline

There is a single layout/render pipeline **loom**, built around a pipeline-of-visitors over a plain node tree:

```
Craft XML (pugixml)
  → docraft::craft front-end (craft/, craft/parser/): tokenizes tags and validates attributes into
    typed "parsed element" data (ParsedElement, Parsed*Data, shared line-style/shape-style parsing)
  → docraft::craft::DocraftLoomCraftLanguageParser: top-level driver for <Document>/<Settings>/
    <Metadata>/<Header>/<Body>/<Footer>/<Foreach>/<NewPage>
  → DocraftLoomTreeBuilder + per-tag handlers (loom/craft/handlers/) build the DocraftLoomNode
    tree (nodes/: Text, Paragraph, Rectangle, Canvas, VStack, HStack, Table, ...)
  → MeasureProcessor   (visitor: computes LayoutBox.measured_size per node)
  → LayoutProcessor    (visitor: walks tree with a DocraftLoomCursor, fills LayoutBox.frame)
  → PaginationProcessor(visitor: fills LayoutBox.page_index)
  → RenderingProcessor (visitor: paints using the final frame/page_index)
  → docraft::backend::pdf (DocraftHaruBackend + capability interfaces) writes to libharu
```

- `docraft::craft` (`docraft/{include,src}/docraft/craft/`) is the shared XML front-end: it parses
  Craft-language tags into generic, typed "parsed element" data — attribute validation, enum
  parsing (e.g. `border_style="solid"/"dashed"`), no knowledge of loom's node types or layout.
  Despite the name, `DocraftCraftLanguageParser` here is this low-level tag/attribute parser, not
  a document-level entry point.
- `docraft::craft::DocraftLoomCraftLanguageParser` (`docraft_loom_craft_language_parser.h`) is the
  actual top-level orchestrator — the one that knows about `<Document>` structure and walks it
  into a fully wired `DocraftLoomPdfCreator`. `docraft_tool` (`docraft/src/docraft/main.cpp`)
  exercises it end to end.
- `docraft::loom::craft` (`docraft/{include,src}/docraft/loom/craft/`) holds the per-tag handlers
  (`handlers/docraft_loom_*_handler.h`) and `DocraftLoomTreeBuilder` that turn parsed elements into
  the actual `DocraftLoomNode` tree.
- Each pipeline processor implements `interfaces::DocraftLoomIVisitor` (double-dispatch visitor,
  one `visit(NodeType*)` overload per node type) — adding a node type means adding a `visit`
  overload to every processor, not editing a big switch.
- `DocraftLoomNode::layout_box()` accumulates the results of each pipeline stage
  (`measured_size` → `page_index` → `frame`) — later stages read fields earlier stages wrote, so
  pipeline order matters and stages must not be skipped for a node type.
- `DocraftLoomCursor` tracks the "pen" position during layout (origin top-left, y grows downward),
  separate from any node.
- `docraft::backend` (`docraft/{include,src}/docraft/backend/`) is the one piece of the old
  architecture that survived: capability-split rendering interfaces
  (Text/Line/Shape/Image/Page/Output/Font/Metadata), implemented on libharu under `backend/pdf/`
  (`DocraftHaruBackend` and friends). This split by capability domain, not one aggregated facade,
  is deliberate (SRP). It's low-level PDF-writing infrastructure used directly by loom's
  `RenderingProcessor`/`DocraftLoomPdfCreator` — not itself legacy, just the layer that talks to
  libharu.
- Charts (`<Chart>`/`<Series>`) are native to loom, not a legacy leftover: `docraft/{include,src}/
  docraft/loom/charts/` has builders for scatter/spline/line/histogram/pie, synthesized as a
  `<Canvas>` full of primitives (Line/Circle/Rectangle/Text) via `docraft_loom_chart_handler`.
- Tests live in `docraft/test/docraft/loom/`; `docraft/test/CMakeLists.txt` is not glob-based — if
  you add a new loom test file, you must add it to TEST_SOURCES there or it silently won't run.

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