# Node reference

Every node also accepts the common attributes documented in `SKILL.md` (`name`, `x`, `y`,
`width`, `height`, `padding`, `margin`/`margin_*`, `position`, `z_index`, `visible`) — only
node-specific attributes are listed per tag below. Unknown attributes are a hard parse error.

## `<Text>`, `<Title>`, `<Subtitle>`

Content is the element's text content (child_value), trimmed, then `${...}`-templated. Leaf
nodes — cannot contain `<Text>`/`<Title>`/`<Subtitle>`/`<PageNumber>` children.

| Attribute | Type | Enum values | Default |
|---|---|---|---|
| `font_size` | float | — | `12.0` (Text); `24.0` (Title); `18.0` (Subtitle) |
| `font_name` | string | any registered/base-14 family | `Helvetica`, or `<Settings><Fonts default="...">` if set and no explicit `font_name` |
| `color` | color | — | opaque black |
| `style` | enum | `normal`, `bold`, `italic`, `bold_italic` | `normal` (Text); Title/Subtitle default to bold via constructor, overridable |
| `alignment` | enum | `left`, `center`, `right`, `justified` | `left` |
| `underline` | bool | `true`/`false` | `false` |
| `strikeout` | bool | `true`/`false` | `false` |
| `width` | float | (common attribute; aliases `set_wrap_width()`) | `0` = no wrapping (single line); `>0` enables word-wrap, and `alignment`/`justified` applies per wrapped line |

Title/Subtitle set `margin = font_size` (1em) in their constructor — even without an explicit
`margin` attribute they read as visually separated from following content. Setting `font_size`
explicitly recomputes this derived margin; an explicit `margin`/`margin_*` attribute still wins.

```xml
<Title font_size="28" color="#003366">Q1 Report</Title>
<Text font_name="OpenSans" style="bold" alignment="justified" underline="true">Body copy.</Text>
```

## `<PageNumber />`

Subclass of Text — same attribute set (`font_size`, `font_name`, `color`, `style`, `alignment`,
`underline`, `strikeout`), plus `format`. At render time its text is `format` with `{page}`
replaced by the current 1-based page number and `{total}` replaced by the document's total page
count; `format` defaults to `"{page}"` (bare current page number, matching the old behavior).
Self-closing, no text content of its own.

```xml
<PageNumber format="Page {page} of {total}" />
```

## `<Paragraph>`

Block container grouping one or more Text children into one vertical block.

| Attribute | Type | Default |
|---|---|---|
| `line_spacing` | float multiplier on each child's height | `1.2` |
| `space_before` | float, points | `0` |
| `space_after` | float, points | `0` |
| `alignment` | enum `left`/`center`/`right`/`justified` | `left` |

Bare text directly inside `<Paragraph>text</Paragraph>` (no `<Text>` wrapper) is captured and
synthesized into an implicit `<Text>` child — both forms work:

```xml
<Paragraph line_spacing="1.5" space_before="4" space_after="6" alignment="center">
  This text has no explicit Text tag and still renders.
</Paragraph>

<Paragraph>
  <Text>First run</Text>
  <Text color="red">Second run</Text>
</Paragraph>
```

## `<Rectangle>`

Block-stacking container (children laid out top-to-bottom) that can also paint a
background/border.

| Attribute | Type | Default |
|---|---|---|
| `background_color` | color | fully transparent |
| `border_color` | color | fully transparent |
| `border_width` | float | `1.0` (invisible unless `border_color` has alpha) |
| `border_style` | enum: `solid`, `dashed` | `solid` |
| `width`, `height` | float | `0` = shrink-to-fit / use incoming width, not literally zero |
| `padding` | float (common) | `10.0`, but collapses to `0` if the rectangle paints neither fill nor stroke |

```xml
<Rectangle name="box" width="100" height="50" background_color="#FF0000" border_width="2">
  <Text>inside</Text>
</Rectangle>
```

## `<Canvas>`

Free-form graphics container (extends Rectangle): children are positioned by their own local
`x`/`y` relative to the canvas's own top-left, not stacked; painting is clipped to canvas bounds.
Same attributes as `<Rectangle>` (`background_color`/`border_color`/`border_width` + common), but:

- `width` and `height` are **required** — omitting either throws `InvalidInputException`.
- `padding` is forced to `0` (meaningless for freely-positioned children).
- Every **direct** child is forced into local-origin absolute positioning by the layout stage
  regardless of its own `position` attribute; a child with no `x`/`y` defaults to `(0,0)` (canvas
  origin) rather than flowing like block content.

```xml
<Canvas name="art" width="200" height="150" background_color="#00FF00">
  <Text x="10" y="20">inside</Text>
  <Circle x="50" y="50" radius="10" background_color="blue" />
</Canvas>
```

## `<Chart>` + `<Series>`

Not a distinct node class — synthesizes a `<Canvas>` full of primitives (Line/Circle/
Rectangle/Text).

`<Chart>` attributes:

| Attribute | Type | Required? | Notes |
|---|---|---|---|
| `style` | enum | **required** | `scatter`, `spline`, `line`, `histogram`, `pie` (exactly these 5) |
| `width`, `height` | float | **both required** | canvas box size |
| `axis_position` | enum | optional | `left` (default), `right`, `top-left`, `top-right`, `bottom-left`, `bottom-right`, `center` |
| `title` | string, templated | optional | chart title |
| `x_label`, `y_label` | string, templated | optional | axis labels |
| `background_color`, `border_color`, `border_width` | color/color/float | optional | applied to the resulting Canvas |
| `show_percentage` | bool | optional | default `true`; only meaningful for `style="pie"` |

`<Chart>` children **must all be `<Series>`** — any other child tag throws.

`<Series>` attributes:

| Attribute | Type | Notes |
|---|---|---|
| `name` | string | legend label, default `""` |
| `color` | color | if omitted, cycles a built-in categorical palette by series index |
| `model` | JSON, `${...}`-templatable | array of points, one of three shapes: `[[x,y],...]`, `[{"x":..,"y":..},...]`, or `[{"label":value},...]` (pie/histogram — single-key object per entry) |

```xml
<Chart name="c1" style="scatter" width="300" height="200" title="My Chart" axis_position="bottom-left">
  <Series name="A" color="blue" model='[[1,2],[3,4]]' />
</Chart>

<Chart style="pie" width="300" height="200" show_percentage="false">
  <Series model='[{"Alpha":10},{"Beta":30}]' />
</Chart>
```

## `<Circle>`

| Attribute | Type | Notes |
|---|---|---|
| `background_color`, `border_color`, `border_width` | color/color/float | shared shape style |
| `border_style` | enum: `solid`, `dashed` | default `solid`; applies to the arc too, when present |
| `radius` | float | true circle; mutually exclusive with `width`/`height` |
| `width`, `height` | float | inscribes an ellipse in that bounding box; both required together |
| `start_x`, `start_y`, `finish_x`, `finish_y` | float | arc endpoints, all four or none |

Validation (all throw `InvalidInputException`): `radius` combined with `width`/`height`; neither
`radius` nor a full `width`+`height` pair given (an unsized Circle is rejected, not silently
blank); only one of `width`/`height` given; `radius <= 0` or `width`/`height <= 0`; 1–3 (not 0 or
4) of the arc attributes given; arc on a non-square width/height; arc combined with
`background_color` (an arc is an open path, can't be filled).

Arc semantics: endpoints are points in the node's own box; only their direction from the center
matters. Angles measured clockwise from 12 o'clock; sweep goes from `start` to `finish` in that
direction — swapping them picks the complementary arc.

```xml
<Circle radius="50" background_color="red" />
<Circle width="60" height="40" border_color="#0000FF" border_width="2" /> <!-- oval -->
<Circle radius="50" start_x="0" start_y="50" finish_x="100" finish_y="50" border_color="red" border_width="2"/>
```

## `<Triangle>`

| Attribute | Type | Notes |
|---|---|---|
| `background_color`, `border_color`, `border_width` | color/color/float | |
| `border_style` | enum: `solid`, `dashed` | default `solid` |
| `points` | string, space-separated `x,y` tokens | **must be exactly 3 points**, else throws |

`points="0,0 50,0 25,50"`. Y-down offsets from the node's own frame origin.

## `<Polygon>`

Same attributes as Triangle, but **any number of points ≥ 3** (no upper-bound validation).
Closed, fillable shape. `smooth` is **not** a valid attribute (rejected as unrecognized) — use
`<CurveLine>` for a smooth open path.

## `<CurveLine>`

Open curve through an arbitrary number of points via a uniform Catmull-Rom spline. **Not** a
shape node — no `background_color` (fill) at all.

| Attribute | Type | Default |
|---|---|---|
| `points` | space-separated `x,y` tokens | **minimum 2 points**, else throws |
| `border_color` | color | black |
| `border_width` | float | `1.0` |
| `border_style` | enum: `solid`, `dashed` | `solid` |

```xml
<CurveLine points="0,0 20,40 60,10" border_color="blue" border_width="2"/>
```

## `<Line>`

| Attribute | Type | Default |
|---|---|---|
| `x1`, `y1` | float | `(0, 0)` |
| `x2`, `y2` | float | `(100, 0)` |
| `border_color` | color | black |
| `border_width` | float | `1.0` |
| `border_style` | enum: `solid`, `dashed` | `solid` |

## `<List>` / `<UList>`

`<List>` = ordered, `<UList>` = unordered. **Only `<Text>` children are accepted** — anything
else throws ("cannot be placed in a list"). `<Foreach>` can still generate a variable number of
`<Text>` items.

| Tag | Attribute | Enum values | Default |
|---|---|---|---|
| `<List>` | `style` | `number`, `roman` | `number` |
| `<UList>` | `dot` | `-`, `*`, `circle`, `box` | `circle` |

```xml
<List style="roman">
  <Text>First</Text>
  <Text>Second</Text>
</List>

<UList dot="*">
  <Text>Alpha</Text>
  <Text>Beta</Text>
</UList>
```

## `<Table>` / `<THead>` / `<TBody>` / `<Row>` / `<Cell>` / `<HTitle>` / `<VTitle>`

`<Table>` is parsed specially and not recursed into generically — `<Foreach>` cannot appear
directly as a Table child. A Table nested inside an *outer* `<Foreach>` still resolves
`${data(...)}` against that outer item.

Table-level attributes:

| Attribute | Type | Notes |
|---|---|---|
| `model` | `"horizontal"` / `"vertical"` (orientation, default horizontal), **or** a JSON literal / `${...}`-templated JSON expression describing content |
| `header` | JSON array of strings, or `${...}` expr, e.g. `'["H1","H2"]'` — mutually exclusive with an explicit `<THead>` |
| `baseline_offset` | float | default `0.25` — vertical text-baseline offset within a cell |
| `TableTile` | color | fallback background painted behind any cell that doesn't set its own `background_color` |

Table does **not** accept `background_color`/`border_color`/`border_width` directly (only
`TableTile` sets cell backgrounds).

**Orientation:**
- Horizontal (default): `<THead>` holds `<HTitle>` column headers; each `<Row>` in `<TBody>`
  holds `<Cell>` children, one per column.
- Vertical (`model="vertical"`): each `<Row>` must contain **exactly one `<VTitle>`** (row label,
  mandatory — its absence throws) plus `<Cell>` children. `<THead>`'s `<HTitle>` list is padded
  with a blank leading title cell to align under the row-label column. A JSON/templated `model`
  **cannot** be combined with `orientation="vertical"`.

**Static markup form** (no `model`): `<THead>` is **mandatory** for horizontal tables without a
JSON/templated model, or it throws ("THead tag not found, it is mandatory").

```xml
<Table>
  <THead>
    <HTitle>ColA</HTitle>
    <HTitle>ColB</HTitle>
  </THead>
  <TBody>
    <Row>
      <Cell><Text>v1</Text></Cell>
      <Cell><Image src="assets/logo.png" width="50" height="50" /></Cell>
    </Row>
  </TBody>
</Table>
```

`<HTitle>`/`<VTitle>` accept: `alignment` (`left`/`right`/`center`/`justified`, default
`center`), `style` (`normal`/`bold`/`italic`/`bold_italic`, default `bold`), `color`,
`background_color` (no `border_*`), `font_size` (must be `> 0`). **`<Title>` is rejected inside a
table header** ("Title is reserved for text headings; use HTitle in table headers").

`<Cell>` accepts **only** `width` (float, `> 0`) and `background_color` — no other attributes,
not even the common ones — and exactly **one child, which must be `<Text>` or `<Image>`**. A
second child, or any other content type, throws `InvalidInputException` at parse time — see
`gotchas.md` #3. `<Row>` accepts only `background_color`, but see `gotchas.md` #2 — it's parsed
and never applied.

**JSON/templated `model` forms:**

Array-of-arrays (raw matrix, no `<TBody>` needed — combining one is rejected):

```xml
<Table model='[["v1","v2"],["v3","v4"]]' header='["H1","H2"]' />
```

Array-of-objects — requires an explicit `<TBody>` used as a **per-object row template**, cloned
once per JSON object with `${data("field")}` resolved against it:

```xml
<Table model='[{"name":"Alice"},{"name":"Bob"}]'>
  <TBody>
    <Row><Cell><Text>${data("name")}</Text></Cell></Row>
  </TBody>
</Table>
```

Omitting `<TBody>` when `model` resolves to objects throws ("requires an explicit <TBody> row
template"). An explicit `<THead>` is still honored alongside a JSON `model` (overrides `header`);
combining `header` attribute and `<THead>` throws. `model`/`header` can be `${variable}`
expressions resolving to these JSON shapes:

```xml
<Table model='${summary_rows}' header='${summary_header}' />
```

Mixed-type arrays (some objects, some arrays) in `model` → error. Ragged rows in an
array-of-arrays model → error. All cells in an array-of-arrays model must be JSON strings →
error otherwise.

There is **no per-column weight on `<Table>`** — column widths come from each
`<Cell width="...">`'s explicit width, else an even split; there's no author-specified per-column
weighting in XML (unlike `<Layout>`'s per-child `weight="..."`).

## `<Layout>`

Becomes `DocraftLoomHStack` or `DocraftLoomVStack`.

| Attribute | Enum values | Default |
|---|---|---|
| `orientation` | `horizontal`, `vertical` | `vertical` |
| `spacing` | float | HStack `0.0`; VStack `12.0` |
| `height` | float | VStack only; unset = shrink-to-fit (see below) |

Each child of a `<Layout>` may carry its own `weight="..."` attribute (a plain float). There is no
`<Layout weights="...">` list attribute — `weight` on the individual child is the only way to
specify it, and it is a parse error anywhere `weight` isn't a direct child of `<Layout>`. A child
with no `weight` defaults to `1.0` once any sibling specifies one.

On `orientation="horizontal"` (HStack), weights always divide the available page/container width.
On `orientation="vertical"` (VStack), weights only take effect when the `<Layout>` itself also has
an explicit `height="..."` — a VStack's height is otherwise derived bottom-up from its children (no
ambient "page height" budget the way HStack always has a page width to divide), so per-child
`weight` alone on a heightless vertical layout is silently dropped.

A weighted child is stretched to fill its resolved column/row share — **except an `<Image>` that
declares its own `width`** (see below), which keeps that declared width undistorted; everything
else (a `<Rectangle>` used as a column background, plain text, etc.) still fills the slot, which is
the point of `weight` for those.

```xml
<Layout orientation="horizontal" spacing="8">
  <Text weight="1">Narrow</Text>
  <Text weight="2">Wide</Text>
  <Text weight="1">Narrow</Text>
</Layout>

<Layout orientation="vertical" height="400">
  <Text weight="1">Short</Text>
  <Text weight="2">Tall</Text>
</Layout>
```

## `<Image>`

| Attribute | Type | Notes |
|---|---|---|
| `src` | string, `${...}`-templatable path | mutually exclusive with `data` |
| `data` | string | `base64:<payload>` (or containing `base64,<payload>`), decoded as raw RGB bytes, requires `data_width`+`data_height`; otherwise a data-binding key resolved as a path |
| `data_width`, `data_height` | int | required with base64 `data`; must be `> 0`; decoded byte count must equal `width * height * 3` (RGB) |
| `width`, `height` | float (common) | requested display size in points — **never auto-derived from the file** |

`src` and `data` together throws. Image format derives from `src`'s extension first letter:
`p`/`P` → PNG, `j`/`J` → JPEG, else raw.

A declared `width` survives even inside a weighted `<Layout orientation="horizontal">` column —
the image is not force-stretched to the column's resolved width the way other weighted children
are (that used to distort it, widening only its width and leaving height untouched). Omit `width`
if you *do* want the image to fill its weighted slot.

```xml
<Image src="assets/logo.png" width="50" height="50" />
<Image src="${logo_path}" />
```

## `<Blank />`

Vertical spacer.

| Attribute | Type | Default |
|---|---|---|
| `height` | float | `12.0` (≈ one line of default body text) |

## `<NewPage />`

Zero-size forced page-break marker, self-closing, no attributes. Only forces a break when a
**direct top-level child of `<Body>`** — see `pagination.md`.
