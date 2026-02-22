# Document and Context

This component pair is the runtime heart of Docraft.

- `DocraftDocument` is the orchestration façade used by applications.
- `DocraftDocumentContext` is the mutable runtime state shared by layout, renderer, and painters.

## 1. `DocraftDocument`: orchestration role

`DocraftDocument` owns:

- root DOM nodes,
- optional `DocraftSettings`,
- metadata,
- optional template engine,
- auto-keyword configuration.

Its `render()` method currently executes the full pipeline in a fixed order:

1. select backend on context (`set_backend(document_override_or_default)`);
2. assign renderer and font applier;
3. apply document settings;
4. apply templating;
5. refresh auto keywords (if enabled);
6. run layout;
7. draw nodes;
8. push metadata to backend and save file.

Because sequencing is centralized here, contributor changes to pipeline order should be done carefully. Side-effects (for example, templating mutating nodes before layout) are assumed by tests and by current behavior.

## 2. `DocraftDocumentContext`: shared state

Context stores mutable runtime state needed across layers:

- active backend (`IDocraftRenderingBackend`),
- cached interface views (`text_backend`, `shape_backend`, `image_backend`, `page_backend`, `line_backend`),
- active renderer (`DocraftAbstractRenderer`),
- active font applier,
- page width/height,
- layout cursor,
- section references and section ratios.

In practical terms, context is the dependency container passed to most operations during render.

## 3. Why this split exists

Keeping orchestration and mutable state separate gives two benefits:

- easier testing of lower-level parts (`layout`, `painters`) by injecting a mock backend/context;
- possibility to run custom orchestration (for advanced integrations) without rewriting core components.

## 4. Lifecycle notes contributors should know

- Context is created in `DocraftDocument` constructor and reused across renders.
- At the start of each `render()`, document reapplies backend selection to context.
- `DocraftDocumentContext::set_backend(...)` refreshes cached backend views and page dimensions.
- Backends are stateful (page pointer, font state, graphics state), so context lifetime defines render session lifetime.
- Page ownership (`node->page_owner`) is assigned during layout and then used during draw filtering.
- Metadata is applied at the end, just before `save_to_file`.

## 5. Backend selection behavior

`DocraftDocument` supports backend override through `set_backend(...)`.

Render behavior:

- if a backend was set on document, render uses that backend;
- if `nullptr` is set (or no override exists), render uses default Haru backend.
- render currently instantiates `DocraftPDFRenderer`.

`DocraftDocumentContext::set_backend(...)` remains useful for advanced or custom orchestration paths.

## 6. Suggested safe contribution patterns

When changing document/context behavior:

- preserve the pipeline order unless there is a clear migration plan;
- add tests for both happy path and side effects (pagination, metadata, templating);
- avoid introducing hidden state in model nodes that bypasses context;
- keep backend-specific logic out of `DocraftDocument`.
