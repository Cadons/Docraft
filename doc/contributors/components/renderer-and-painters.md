# Renderer and Painters

Rendering in Docraft is intentionally split into two layers:

- renderer: semantic dispatch (`render_text`, `render_table`, ...),
- painters: concrete drawing logic per node type.

## 1. Why this split exists

Without painters, rendering code quickly becomes monolithic. With painters:

- node draw behavior stays focused,
- rendering logic for each node type stays isolated,
- backend contracts are reused consistently.

## 2. Draw path

Typical path for one node:

1. `node->draw(context)`
2. `context->renderer()->render_xxx(node)`
3. `DocraftPDFRenderer` creates node painter
4. painter emits backend primitives

This keeps model nodes free of backend-specific operations.

## 3. Painter responsibilities

Examples:

- `DocraftTextPainter`: line-level text rendering, justification, underline.
- `DocraftTablePainter`: table backgrounds, borders, cell/title drawing.
- shape painters: path + fill/stroke semantics.
- image painter: image format dispatch (PNG/JPEG/RAW memory).

Painters should not compute layout. They should trust layout results already stored in nodes.

## 4. Contributor guidelines

When adding a renderable node:

1. Add renderer method to abstract/implementation classes.
2. Add dedicated painter.
3. Keep painter focused on draw primitives only.
4. Reuse context backend interfaces instead of direct backend casts.
5. Add smoke tests and behavior tests.

## 5. Common anti-patterns to avoid

- putting backend-specific code in model nodes;
- recomputing layout values inside painters;
- bypassing context interface caches;
- coupling one painter to unrelated node types.
