# Pagination

Only the **Body's top-level children** flow across pages. Header and Footer are laid out once
and stamped on every physical page — they are never paginated.

Body's continuous, single-page layout is walked child-by-child; each top-level child is a whole
unit that either:

1. **Fits** entirely below the running cursor and above the page's bottom margin → stays on the
   current page.
2. **Doesn't fit** → the whole node moves to a fresh page as one unit (not "print what fits") —
   **except `<Table>`**, which can be split (see below).
3. Is an **oversized escape hatch**: a node that already starts at a fresh page's top and still
   doesn't fit (taller than one whole page) is accepted as overflow rather than looping forever
   creating empty pages.

## `<Table>` row-splitting

If a table doesn't fit as a whole, the pagination stage computes how many leading rows *do* fit
and splits the table there. **The remainder table repeats the original table's leading run of
header row(s)** onto the front of the continuation, then is re-stacked starting at the new page's body top. This can
recurse across as many further pages as needed. A table with 0 columns can't be split at all.

When a table is already at a fresh page's top and only the repeated header row (s) fit — no genuine content row does —
the next row is taller than one whole page, so row-level splitting alone would just reproduce the same
`[header, oversized row]` shape on every following page forever. In that case the pagination stage instead splits **that
row's own cell content**: any cell whose text was wrapped into multiple lines (`<Text>` content wider than its column)
has as many leading lines as fit on this page kept in place, with the rest continuing — under a repeated header — on the
next page, recursing across as many further pages as the text needs. A cell whose content can't be split this way (an
`<Image>`, or text that wasn't wrapped into multiple lines)
must already fit on its own within the available space, or the whole row falls through to the oversized escape hatch and
renders as overflow on one page instead of splitting.

## `<NewPage />`

Forces an unconditional page advance whenever encountered among the Body's **top-level**
children, regardless of remaining room on the current page. Draws nothing itself. It must be a
direct child of `<Body>` to have this effect — nested inside another container (e.g. a
`<Rectangle>` or `<Layout>`) it still builds as a valid (invisible) node, but does **not** force
a break, since pagination only special-cases its direct iteration over Body's own children.

```xml
<Body>
  <Title>Section 1</Title>
  <Text>...</Text>
  <NewPage />
  <Title>Section 2</Title>
</Body>
```

## Other notes

- The gap between two adjacent top-level Body children (their collapsed margin, or the
  container's own `spacing`) is captured from the pre-pagination continuous layout and simply
  replayed — pagination does not recompute spacing.
- No mid-node splitting exists for anything other than `<Table>` — an oversized `<Rectangle>`,
  `<Paragraph>`, image, etc. either fits whole, is pushed whole to the next page, or (if still
  too tall for one page) overflows past the page bottom as the oversized-escape case.
