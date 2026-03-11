# Backend Integration Guide (Internal and External)

This guide describes backend integration exactly as supported in current Docraft code.

## 1. Runtime behavior today

- `DocraftDocument` exposes `set_backend(...)`.
- `DocraftDocument::render()` always reapplies backend selection to context at the beginning.
- If a non-null backend was set on document, that backend is used.
- If `nullptr` is set (or no override exists), context restores the default Haru backend.
- `render()` currently wires `DocraftPDFRenderer` as renderer implementation.

Important implication:

- Backend swapping is supported without changing app pipeline.
- The custom backend must satisfy the primitives expected by existing renderer/painters.

## 2. Interface contract you must implement

A backend must implement `IDocraftRenderingBackend`, which aggregates:

- text primitives,
- line/shape primitives,
- image primitives,
- page management,
- save/output extension,
- metadata application,
- font registration and font selection hooks.

In practice you implement one concrete class inheriting `IDocraftRenderingBackend`.

## 3. External integration (recommended path)

For libraries/projects outside the Docraft repo, the supported high-level path is:

1. Parse or build a `DocraftDocument`.
2. Create your backend implementation.
3. Call `document->set_backend(my_backend)`.
4. Call `document->render()`.

Example:

```cpp
#include "docraft/craft/docraft_craft_language_parser.h"
#include "my_backend/my_rendering_backend.h"

void render_with_external_backend(const std::string &craft_path) {
    docraft::craft::DocraftCraftLanguageParser parser;
    parser.load_from_file(craft_path);
    auto document = parser.get_document();

    auto backend = std::make_shared<my_backend::MyRenderingBackend>();
    document->set_backend(backend);
    document->set_document_title("output");
    document->render();

    // Optional: restore default backend for subsequent renders.
    document->set_backend(nullptr);
}
```

## 4. Internal integration (inside Docraft repo)

Typical steps for a backend added directly in this repository:

1. Add backend class, for example `docraft::backend::svg::DocraftSvgBackend`.
2. Implement all methods of `IDocraftRenderingBackend`.
3. Add new source/header files to `docraft/CMakeLists.txt`.
4. Add tests in `docraft/test/backend/` and/or rendering smoke tests.
5. Choose selection strategy:
   - keep default Haru and let apps opt-in using `DocraftDocument::set_backend(...)`;
   - or change default backend in `DocraftDocumentContext` constructor.

## 5. Advanced path: custom orchestration and custom renderer

`DocraftDocumentContext::set_backend(...)` can still be used directly in advanced flows.

However, note the boundary of current public API:

- `DocraftDocument::render()` does not expose a renderer injection hook.
- If you need a backend-specific renderer (not `DocraftPDFRenderer`), you must run your own orchestration pipeline (context + layout + node draw loop), or extend Docraft with a renderer factory hook.

When doing custom orchestration, you are responsible for:

- applying settings (page format, section ratios, external fonts),
- running templating and optional keyword refresh,
- invoking layout,
- iterating nodes and page ownership logic,
- applying metadata and saving via backend.

## 6. Compatibility checklist for new backends

Before considering integration complete, verify:

- text measurement behavior is consistent with rendered output,
- page navigation semantics support pagination logic,
- all painter-required primitives are implemented,
- metadata support is explicit (document unsupported fields),
- file extension and save semantics are documented,
- backend passes relevant render/layout tests.
