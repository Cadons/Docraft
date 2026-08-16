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

3. **`<Cell>` content must be exactly one `<Text>` or `<Image>` child.** A second child, or a
   child tag other than `<Text>`/`<Image>`, throws a clean parse-time `InvalidInputException` —
   it no longer silently drops the extra content. Only ever put a single `<Text>` or `<Image>`
   directly inside `<Cell>`.

4. **No per-column weight on `<Table>`.** Unlike `<Layout>`'s per-child `weight="..."`, table
   column widths come only from each `<Cell width="...">`'s explicit width or an even split —
   there's no author-specified per-column weighting in XML.

5. **Table cell text-wrap width is a best-effort estimate**, computed during the Measure pass
   before Layout has authoritatively resolved final column widths. It's not guaranteed to
   exactly match Layout's final numbers once row-splitting across pages or nested-container width
   constraints interact. Don't promise pixel-perfect no-overflow guarantees for long cell text —
   it wraps, but the ceiling it wraps to is an estimate.

6. **Per-child `weight="..."` on a vertical `<Layout>` needs an explicit `height` on the `<Layout>`
   too.** `weight` is only accepted on a direct child of `<Layout>` — used anywhere else it's a
   parse error, not a silent no-op. On `orientation="horizontal"` (HStack), children's weights
   always divide the available page/container width. On `orientation="vertical"` (VStack), weights
   only take effect when the `<Layout>` itself also has an explicit `height="..."` — a VStack's
   height is otherwise derived bottom-up from its children (no ambient "page height" budget the way
   HStack always has a page width to divide), so per-child `weight` alone on a heightless vertical
   layout still has zero effect. If `height="..."` is smaller than what the weighted children's own
   natural heights need (each child is never squeezed shorter than its own content), the `<Layout>`
   grows past the declared `height` instead of clipping or overlapping the next sibling — a
   `[WARNING]` is logged when this happens.

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
