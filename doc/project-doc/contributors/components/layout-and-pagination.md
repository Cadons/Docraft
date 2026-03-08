# Layout and Pagination

Layout computes physical geometry and page assignment for every visible node.

## 1. Core architecture

Main pieces:

- `DocraftLayoutEngine`: orchestration + section planning.
- `DocraftCursor`: current flow position and direction stack.
- Handler chain for node-specific computations.

Handlers currently include dedicated logic for text, lists, tables, blank lines, layouts, and generic nodes.

## 2. Section-based layout strategy

Documents are laid out as section blocks:

- Header
- Body
- Footer

The engine computes a section plan from configured ratios and visibility.

If header/footer are hidden or absent, their ratio is reassigned to body.

## 3. Cursor and flow model

`DocraftCursor` tracks:

- current `x/y`,
- current flow direction (vertical/horizontal),
- nested direction stack for layouts.

Nodes can be:

- `block`: participate in flow;
- `absolute`: positioned independently from flow cursor.

## 4. Pagination behavior

Body layout includes pagination rules:

- explicit `NewPage` creates a new page,
- overflowing non-absolute nodes are moved/re-laid out on next page,
- tables can be split across pages when partial row ranges fit.

After pagination decisions, `page_owner` is assigned recursively to node subtrees.

## 5. Why handlers are chained

The chain-of-responsibility pattern keeps layout logic modular.

Adding a new node type usually means adding a dedicated handler and placing it at the correct precedence point in the chain.

Handler order matters because first match wins.

## 6. Practical contributor notes

- Preserve layout determinism: avoid random or order-sensitive side effects.
- Keep text measurement backend-driven (`measure_text_width`) for consistency.
- Be explicit with section bounds and cursor resets when adding pagination behavior.
- Cover overflow + page owner edge cases in tests.
