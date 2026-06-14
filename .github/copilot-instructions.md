## Quick Facts

- **Type**: C++ Library (header-only or compiled)
- **Main API**: `DocraftDocument`, `DocraftCraftLanguageParser`
- **Backend**: libharu (PDF generation)
- **Language**: XML-based markup (Craft Language)
- **Templating**: Variable substitution + Foreach loops via JSON
- **Output**: Single-file PDF with metadata support

---

## Architecture Overview

```
XML (Craft Language)
    ↓
Parser (pugixml)
    ↓
DOM AST (DocraftNode tree)
    ↓
Template Engine (substitute ${var}, expand Foreach)
    ↓
Layout Engine (compute positions, pagination)
    ↓
Renderer (visitor pattern → painter calls)
    ↓
Capability Provider Interfaces
    ↓
PDF Implementation (Haru)
    ↓
PDF Output
```

**Key insight**: Each layer is loosely coupled via interfaces. Only Parser and Document are tightly bound.

---

## Folder Structure

```
docraft/
├── include/docraft/
│   ├── docraft_document.h                      ← Main API
│   ├── docraft_document_context.h              ← Shared render state
│   ├── backend/
│   │   ├── docraft_rendering_backend.h         ← Capability provider interfaces
│   │   ├── docraft_*_rendering_backend.h       ← Capability interfaces
│   │   └── pdf/
│   │       ├── docraft_haru_backend.h          ← PDF impl
│   │       └── docraft_haru_*.h                ← Inner classes
│   ├── layout/
│   │   ├── docraft_layout_engine.h
│   │   └── handler/                            ← Chain of Responsibility
│   ├── renderer/                               ← Visitor pattern
│   ├── craft/                                  ← Parser
│   ├── model/                                  ← Node types
│   ├── templating/                             ← Variable + Foreach
│   └── utils/                                  ← Font, keyword, logger
├── src/docraft/                                ← Implementations
└── test/docraft/                               ← Unit tests
```

---

## Core Components

### 1. Document & Context

- **DocraftDocument**: Entry point, orchestrates pipeline
- **DocraftDocumentContext**: Shared render state
- ⚠️ Service Locator anti-pattern (20+ getters)

### 2. Parser

- **DocraftCraftLanguageParser**: XML → typed AST
- Uses **pugixml**
- Case-sensitive tags
- Unknown tags = parse error

### 3. Backend Interface

Capability provider contracts are split by domain:

- Line, Text, Shape, Image, Page rendering
- Output, Font, Metadata backends
- Implementation: `DocraftHaruBackend` (libharu)

### 4. Layout Engine

- Computes x, y, width, height
- Handles automatic page breaking
- **Chain of Responsibility** pattern
- Specialized handlers per node type

### 5. Renderer

- **Visitor pattern** on AST
- **Painters** call backend primitives
- Node knows WHAT, Painter knows HOW

### 6. Model / DOM

- **DocraftNode**: Base class
- **50+ node types**: Text, Table, List, Shape, etc.
- Each has: parser, layout handler, draw method

### 7. Template Engine

- `${variable}` substitution
- `<Foreach>` expansion
- Runs before layout
- No-op if not set

---

## Data Flow

```
Parser → DocraftDocument → render()
  → configure_settings()
  → template_document()
  → layout()
  → render()
  → save_to_file()
```

---

## Common Tasks

### Add a node type

1. Model: `model/docraft_my_node.h`
2. Parser: Register tag
3. Handler: Layout logic
4. Renderer: Painter calls
5. Tests: All subsystems

### Fix a bug

1. Locate node type
2. Breakpoint in layout handler
3. Breakpoint in painter
4. Breakpoint in backend

### Add backend capability

1. Interface: `backend/docraft_*.h`
2. Extend: capability provider interfaces (Rendering/Resource/Lifecycle)
3. Implement: Haru backend
4. Add to: `DocraftHaruBackend`
5. Use: In painters

---

## Critical Issues

**10 architectural issues in `.local/ARCHITETTURA_CRITICITA.md`:**

1. **SRP** - historical aggregated facade removed; capability providers are split by domain
2. **Circular deps** - PageHaruBackend ↔ DocraftHaruBackend
3. **Ownership** - State scattered (pdf_, cursor, colors)
4. **Interfaces** - Missing coordinate, color space abstractions
5. **Mock** - Doesn't reflect reality
6. **Accessors** - nullptr not compile-time safe
7. **Service Locator** - 20+ getters in Context
8. **Fallback** - No unsupported capability strategy
9. **Conceptual** - Font/Metadata/Output mixed with rendering
10. **Versioning** - No interface evolution support

Severity: 🔴 High (1,2,5,7) | 🟠 Medium (3,4,8,9) | 🟡 Low (6,10)

---

## Templating

```xml
<!-- Variable -->
<Text>Invoice ${invoice_number}</Text>

        <!-- Foreach -->
<Foreach model="${items}">
<Text>${data("name")} x ${data("qty")}</Text>
</Foreach>

        <!-- Table from JSON -->
<Table model='${table_data}' header='${headers}'/>
```

---

## Node Attributes

**Common**: `name`, `x`, `y`, `width`, `height`, `padding`, `weight`, `z_index`, `position`, `visible`

**Text**: `font_name`, `font_size`, `style`, `color`, `alignment`, `underline`

**Colors**: `#RRGGBB`, named colors (`red`, `blue`, etc.)

---

## File Organization

- Headers: `include/docraft/` (parallel `src/`)
- Implementations: `src/docraft/` (`.cpp`)
- Tests: `test/docraft/` (by subsystem)
- No circular includes
- Forward declarations for pointer types

---

## Testing

- Unit tests by subsystem
- Mock backend: `test/docraft/utils/`
- Parser: schema validation
- Layout: positioning
- Renderer: painter calls
- Backend: Haru-specific

---

## Craft Language

**Tags**: Document, Settings, Metadata, Header, Body, Footer, Text, Table, List, Image, Shape, Layout, Foreach

**Rules**:

- `<Document>` and `<Body>` required
- Unknown tags = error
- Invalid enums = error
- Text can't contain Text

---

## Dependencies

| Library       | Purpose    | License |
|---------------|------------|---------|
| libharu       | PDF gen    | ZLIB    |
| pugixml       | XML parse  | MIT     |
| nlohmann-json | JSON parse | MIT     |
| GoogleTest    | Testing    | BSD     |

**Build**: CMake ≥ 3.16, C++17, macOS/Linux/Windows

---

## Commands

```bash
# Build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j

# Test
ctest --test-dir build -C Release --output-on-failure

# Render
./build/artifacts/bin/docraft_tool input.craft output.pdf

# Docs
cd doc && make html
```

---

## Next Steps

1. **Read `.local/ARCHITETTURA_CRITICITA.md`** - Architectural analysis
2. **Read `.local/PROGETTO_CONTESTO.md`** - Deep context
3. **Pick a task** from Common Tasks above
4. **Write tests** for features
5. **Run full build + tests** before PR
