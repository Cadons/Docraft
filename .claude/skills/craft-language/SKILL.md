---
name: craft-language
description: Use this skill whenever writing, generating, or editing a `.craft` file, or any XML markup meant to be fed to Docraft's loom pipeline (DocraftLoomCraftLanguageParser / docraft_tool) to produce a PDF. Triggers on "craft language", "write a .craft file", "generate a Docraft document", "Docraft XML", "craft markup", or requests to lay out a PDF (tables, charts, headers/footers, paginated reports) using Docraft. Covers the full document grammar (Document/Settings/Metadata/Header/Body/Footer), every node type and its exact attributes, the color palette, font system, `${variable}`/`Foreach`/`data()` templating, and pagination behavior — so generated markup is syntactically correct and behaviorally accurate on the first try.
version: 1.0.0
---

# Craft Language (Docraft loom pipeline)

Craft Language is the XML markup Docraft parses into a PDF via its **loom** pipeline
(`DocraftLoomCraftLanguageParser` → `DocraftLoomTreeBuilder` → Measure → Layout → Paginate →
Render). This skill documents loom's Craft Language **as currently implemented** — not the
legacy pipeline, and not aspirational/planned syntax. Where a behavior is a known gap or quirk,
it is called out explicitly so you don't generate markup that silently misbehaves.

Reference files (read them when you need the full detail — don't try to hold everything in this
file alone):

- `references/document-structure.md` — `<Document>`, `<Settings>` (page/fonts/ratios),
  `<Metadata>`, `<Header>`/`<Body>`/`<Footer>` — required elements, defaults, validation errors.
- `references/nodes.md` — every node tag, every attribute, every enum value, every default,
  node-by-node (Text/Title/Subtitle/PageNumber, Paragraph, Rectangle, Canvas, Chart/Series,
  Circle, Triangle, Polygon, CurveLine, Line, List/UList, Table, Layout, Image, Blank, NewPage).
- `references/templating.md` — `${variable}`, `${data("field")}`, `<Foreach>`, nesting rules,
  what's *not* supported (no conditionals).
- `references/colors-fonts.md` — the exact 9 named colors + hex syntax; base-14 fonts, bundled
  font families, custom font registration, style resolution.
- `references/pagination.md` — how Body content flows/splits across pages, `<NewPage/>`, table
  row-splitting.
- `references/examples.md` — full worked `.craft` documents (report with header/footer, table
  from JSON data, nested Foreach, chart, mixed layout).
- `references/gotchas.md` — known dead attributes, validation traps, and half-finished behaviors.
  **Read this before finishing any non-trivial document** — several of these produce a working
  parse that silently doesn't do what a reasonable author would expect.

## Minimal complete document

```xml
<Document>
  <Body>
    <Title>Hello, World</Title>
    <Text>This is a Docraft document.</Text>
  </Body>
</Document>
```

`<Document>` and `<Body>` are the only required elements. Everything else — `<Settings>`,
`<Metadata>`, `<Header>`, `<Footer>` — is optional.

## Core concepts

**Coordinates**: all numeric attributes are in **points** (1/72 inch). Origin is top-left,
**Y grows downward**. Non-numeric text in a numeric attribute silently parses as `0` (pugixml is
lenient — there's no parse error for e.g. `width="abc"`).

**Unknown attributes are a hard parse error.** Don't invent attribute names — every attribute
used below is exhaustive for its tag. `InvalidInputException` is thrown at parse time listing the
offending attribute and what the tag actually accepts.

**Common attributes** (available on every node, parsed generically):

| Attribute | Type | Notes |
|---|---|---|
| `name` | string | Debug/lookup label only, no layout effect. |
| `x`, `y` | float | Meaningful only when `position="absolute"` (or implied — see below). |
| `width`, `height` | float | Only has effect on node types that expose a setter for it (Rectangle, Canvas, Image, Text `width`=wrap width, Blank `height`). See `references/gotchas.md` #7 for which tags silently ignore it. |
| `padding` | float | Inset between a container's box and its children. No-op on leaf nodes. |
| `margin` | float | Shorthand for all 4 edges — space a node asks its parent's stack to reserve around it. Adjacent siblings' touching margins are combined via **max()**, not summed (CSS-style collapsing). |
| `margin_top`/`right`/`bottom`/`left` | float | Per-edge override; combines with `margin` (per-edge wins where set). |
| `position` | enum: `block`, `absolute` | `block` (default) = normal flow, advances the parent's cursor. `absolute` = positioned at `x`/`y`, does not advance the cursor. **If `x` and/or `y` is given and `position` is omitted, absolute is implied.** |
| `z_index` | int | Default `0`. Sibling-scoped paint order only — does **not** affect layout/flow, only which sibling paints on top. Higher paints later (on top). Stable-sorted, so equal `z_index` siblings keep declaration order. Scope is one container's direct children only (not recursive into grandchildren, not cross-container). |
| `visible` | bool (`true`/`false`) | If `false`, the whole element and its subtree are dropped at build time — not just hidden, genuinely not built. Not `${...}`-templatable. |

**Absolute positioning outside `<Canvas>`** is relative to the nearest enclosing coordinate
space (typically page space) — it is *not* scoped to "nearest Canvas ancestor." Inside a
`<Canvas>`, every **direct** child is forced into local-origin absolute positioning regardless of
its own `position` attribute (see `references/nodes.md` → Canvas).

## Node quick index

Full attributes/enums/examples for every one of these are in `references/nodes.md`.

| Tag | Purpose |
|---|---|
| `<Text>`, `<Title>`, `<Subtitle>` | Text runs. Title/Subtitle = bold + larger default size + auto margin. |
| `<PageNumber />` | Self-updating current page number; `format="Page {page} of {total}"` for a total-page count. |
| `<Paragraph>` | Groups Text runs (or bare text) into one block with line-spacing/alignment. |
| `<Rectangle>` | Block-stacking container with optional fill/border. |
| `<Canvas>` | Free-form container — children positioned by their own local `x`/`y`, clipped to bounds. `width`/`height` required. |
| `<Chart>` + `<Series>` | Synthesizes a Canvas of primitives: `scatter`, `spline`, `line`, `histogram`, `pie`. |
| `<Circle>` | Circle (`radius`) or ellipse (`width`+`height`) or arc (`start_x/y`+`finish_x/y`). |
| `<Triangle>` | Exactly 3 points. |
| `<Polygon>` | ≥3 points, closed fillable shape. |
| `<CurveLine>` | Open Catmull-Rom spline through ≥2 points, stroke only (no fill). |
| `<Line>` | Straight segment, `x1,y1` → `x2,y2`. |
| `<List>` / `<UList>` | Ordered/unordered list — children must all be `<Text>`. |
| `<Table>` / `<THead>` / `<TBody>` / `<Row>` / `<Cell>` / `<HTitle>` / `<VTitle>` | Static markup or JSON-driven (`model=`) tables, horizontal or vertical orientation. |
| `<Layout>` | Becomes an HStack (`orientation="horizontal"`) or VStack (default) with optional `spacing`/`weights`. |
| `<Image>` | `src="path"` or `data="base64:..."`. |
| `<Blank />` | Vertical spacer, default height 12pt. |
| `<NewPage />` | Forces a page break — only effective as a **direct** `<Body>` child. |

## Colors

Two forms only:
- Hex: `#RRGGBB` or `#RRGGBBAA`.
- Named (exactly these 9, lowercase): `black white red green blue yellow cyan magenta purple`.

No other named colors exist (no `orange`/`grey`/`pink`/etc.) — using one throws
`InvalidInputException`. Full detail: `references/colors-fonts.md`.

## Templating

```xml
<Text>${company_name}</Text>

<Foreach model='[{"name":"Alice"},{"name":"Bob"}]'>
  <Text>${data("name")}</Text>
</Foreach>
```

`${variable}` (dotted paths, from JSON data bound at document-build time) and `${data("field")}`
(top-level-only, bound to the current `<Foreach>`/table-row item) are the only substitution
forms. `<Foreach model=... | n=...>` is the only repetition construct. **There is no
conditional/branching directive** — only `visible` (boolean show/hide of one element) and
`Foreach` exist as control flow. Full rules, nesting, and gotchas: `references/templating.md`.

## Before finishing a document

1. Every attribute you used exists on that tag (check `references/nodes.md` — unrecognized
   attributes are a hard parse error).
2. `<Cell>` contains exactly one `<Text>` or `<Image>` child, nothing else (a second child or any
   other tag throws `InvalidInputException` at parse time — see `references/gotchas.md` #3).
3. If you used `z_index`, remember it only reorders *paint*, not layout position or flow.
4. If you want per-row table backgrounds, don't rely on `<Row background_color="...">` — it's
   parsed but never applied (`references/gotchas.md` #2). Set `background_color` per `<Cell>`, or
   use the table's own `TableTile` for a uniform default.
5. If you need nested data (e.g. a list of teams each with members), nest `<Foreach>` — `${data()}`
   only reads one JSON level deep.
