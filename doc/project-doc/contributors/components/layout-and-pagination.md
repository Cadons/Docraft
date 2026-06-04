# Layout and Pagination

Layout computes physical geometry and page assignment for every visible node.

## 1. Engine file structure (PImpl)

The layout engine is intentionally split across files:

- Public facade: `docraft/src/docraft/layout/docraft_layout_engine.cc`
- Private implementation declaration: `docraft/src/docraft/layout/docraft_layout_engine_impl.h`
- Private implementation definition: `docraft/src/docraft/layout/docraft_layout_engine_impl.cc`
- Public API: `docraft/include/docraft/layout/docraft_layout_engine.h`

The facade only forwards calls to `DocraftLayoutEngine::Impl`.
All layout internals (handlers, section planning, pagination, node traversal) live in `Impl`.

## 2. Core architecture

Main pieces:

- `DocraftLayoutEngine`: stable public entry point.
- `DocraftLayoutEngine::Impl`: full orchestration logic.
- `DocraftCursor`: flow position and direction stack.
- Handler chain for node-specific computations.

Handlers currently include dedicated logic for text, lists, tables, blank lines, layouts, and generic nodes.

## 3. Execution flow

At a high level:

1. `compute_document_layout(...)` splits nodes into Header/Body/Footer.
2. A `SectionPlan` is computed from navigation ratios and section visibility.
3. Header is laid out first (if visible).
4. Body is laid out with pagination rules.
5. Footer is laid out last (if visible).

For single-node layout:

1. `compute_layout(node, cursor)` checks visibility.
2. It selects flow vs absolute positioning mode.
3. It configures local cursor scope for text/list/rect containers.
4. Child nodes are recursively laid out.
5. The handler chain computes the current node box.
6. Cursor advances using horizontal/vertical spacing rules.

## 4. Section strategy and ratios

Documents are laid out as section blocks:

- Header
- Body
- Footer

If header/footer are hidden or absent, their ratio is re-assigned to body.
This keeps total vertical allocation stable for each page.

## 5. Cursor and flow model

`DocraftCursor` tracks:

- current `x/y`,
- current flow direction (vertical/horizontal),
- nested direction stack for layouts.

Nodes can be:

- `block`: participate in cursor flow,
- `absolute`: positioned independently from flow cursor.

Inside horizontal layouts, children receive width slices based on `weight`.
If weight is unspecified (`-1`), equal weights are assigned before layout.

## 6. Pagination behavior

Body layout includes pagination rules:

- explicit `NewPage` creates a new page,
- overflowing non-absolute nodes are moved/re-laid out on next page,
- tables can be split across pages when partial row ranges fit.

Table overflow handling:

1. Count how many rows still fit in body bounds.
2. Split table at that row using `split_after_row(...)`.
3. Reinsert the remainder into body children after the current table.
4. Assign page ownership for both fragments.

After pagination decisions, `page_owner` is assigned recursively to node subtrees.

## 7. Why handlers are chained

The chain-of-responsibility pattern keeps layout logic modular.

Adding a new node type usually means:

1. implement a dedicated layout handler,
2. register it in `Impl::configure_handlers()` with correct priority,
3. verify precedence (first match wins),
4. add tests for layout + pagination interactions.

## 8. Practical contributor notes

- Keep text measurement backend-driven for deterministic widths.
- Preserve stable ordering in recursive layout to avoid visual regressions.
- Be explicit with cursor resets when changing pagination rules.
- Cover overflow and `page_owner` propagation in tests.
