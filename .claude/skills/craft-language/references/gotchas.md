# Known gaps and quirks

Don't over-promise these behaviors when generating markup or explaining Docraft to a user — each
one is a case where a document parses fine but doesn't do what a reasonable author would expect.

1. **`z_index` only reorders paint order within one container's direct children — never layout.**
   It's a sibling-scoped, stable sort (`DocraftLoomNode::paint_order_indices()`): higher
   `z_index` paints later (visually on top); equal values keep declaration order; it never looks
   into grandchildren or any other container's children, and it never changes where a node sits
   in flow/measurement — only which sibling's ink lands on top of which. If you want a node to
   overlap a sibling, both must be `position="absolute"` (or otherwise occupy overlapping space)
   — `z_index` alone does not cause overlap, it only decides paint order *given* overlap.

2. **`<Row background_color="...">` is parsed but never applied.** Per-row background painting
   silently doesn't work in loom — use the table's `TableTile` attribute for a table-wide
   default, or set `background_color` on each `<Cell>` individually.

3. **`<Cell>` content must be exactly one `<Text>` or `<Image>` child.** A third tag as a cell's
   only child hits an internal `bad_any_cast` rather than a clean validation error. When
   generating table markup, only ever put `<Text>` or `<Image>` directly inside `<Cell>`.

4. **No `<Table weights="...">`.** Unlike `<Layout weights="...">` (HStack), table column widths
   come only from each `<Cell width="...">`'s explicit width or an even split — there's no
   author-specified per-column weighting in XML.

5. **Table cell text-wrap width is a best-effort estimate**, computed during the Measure pass
   before Layout has authoritatively resolved final column widths. It's not guaranteed to
   exactly match Layout's final numbers once row-splitting across pages or nested-container width
   constraints interact. Don't promise pixel-perfect no-overflow guarantees for long cell text —
   it wraps, but the ceiling it wraps to is an estimate.

6. **The generic `weight` common attribute only does anything via `<Layout orientation=
   "horizontal">`.** No concrete node type defines a generic `set_weight()` — `weight="..."` on
   any other tag (or on a vertical `<Layout>`'s children) is parsed but has zero effect.

7. **`width`/`height` silently do nothing on several tags** even though they parse without error:
   `<List>`, `<Paragraph>`, `<Line>`, `<CurveLine>`, `<Table>` have no setter wired for them.
   (`<Circle>` is a special case — its `width`/`height` bounding-box sizing *is* wired, just via a
   different code path than the generic common-attribute setter.)

8. **`<Settings>`/`<Metadata>`/any unrecognized top-level `<Document>` child is silently
   skipped**, not an error — only `Header`/`Body`/`Footer`/`Settings`/`Metadata` are recognized
   directly under `<Document>`.

9. **`fonts.json`'s `OpenSans-Italic` entry actually points at the bold-italic TTF.** Requesting
   `font_name="OpenSans" style="italic"` currently renders bold-italic glyphs. Don't present that
   combination as a clean italic example; use `Roboto` or a custom font if a plain-italic example
   is needed.

10. **No conditional/branching templating directive exists.** Only `<Foreach>` (repetition) and
    `visible` (boolean show/hide of one element) provide control flow — there is no `<If>`, no
    comparison operators, no ternary syntax anywhere in the template engine.

11. **`${data(...)}` only reads one JSON nesting level.** For nested data, nest `<Foreach>`
    elements (see `templating.md`) — dotted paths (`${data("a.b")}`) don't work for `data(...)`;
    that syntax only exists for the separate `${variable}` form.
