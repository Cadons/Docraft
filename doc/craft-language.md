# DocCraft `.craft` Language Specification

This document describes the `.craft` XML grammar implemented by the DocCraft parser (current codebase). It is intentionally technical and mirrors parser behavior and validation rules.

## 1. Conformance and Parsing Model

- `.craft` is XML (parsed with `pugixml`).
- Tag and attribute names are **case-sensitive**. Use exact spellings.
- Unknown tags or invalid attribute values throw parsing errors.
- The parser constructs a document tree of model nodes (text, layout, table, shapes, etc.).

## 2. Required Structure

A valid `.craft` file must contain:

- Exactly one root `<Document>` element.
- A `<Body>` element inside `<Document>`.

Optional elements inside `<Document>`:

- `<Settings>`
- `<Header>`
- `<Footer>`

### Skeleton

```xml
<Document>
  <Settings>...</Settings>
  <Header>...</Header>
  <Body>...</Body>
  <Footer>...</Footer>
</Document>
```

## 3. Global Node Attributes (All Elements)

These attributes are recognized on most nodes (including sections, text, layout, tables, shapes, etc.).

| Attribute            | Type   | Values / Notes |
|---------------------|--------|----------------|
| `x`                 | float  | Top-left X position (points). |
| `y`                 | float  | Top-left Y position (points). |
| `width`             | float  | Width in points. |
| `height`            | float  | Height in points. |
| `auto_fill_width`   | bool   | `true` or `false`. |
| `auto_fill_height`  | bool   | `true` or `false`. |
| `padding`           | float  | Padding in points. |
| `weight`            | float  | Used by `Layout` to distribute space. |
| `position`          | string | `block` or `absolute`. |

### Positioning

- `position="block"` (default): participates in layout flow.
- `position="absolute"`: uses explicit `x`/`y` without flow.

## 4. Colors

Color attributes accept either:

- Named colors: `black`, `white`, `red`, `green`, `blue`, `yellow`, `magenta`, `cyan`, `purple`
- Hex: `#RRGGBB` or `#RRGGBBAA`

Invalid color names or hex strings raise an error.

## 5. Sections

### `<Header>`, `<Body>`, `<Footer>`

Sections are rectangle-like nodes with optional margins and borders.

#### Section Attributes

| Attribute       | Type  | Notes |
|----------------|-------|-------|
| `margin_top`    | float | Margin above section. |
| `margin_bottom` | float | Margin below section. |
| `margin_left`   | float | Left margin. |
| `margin_right`  | float | Right margin. |

#### Rectangle Attributes (also valid on sections)

| Attribute           | Type  | Notes |
|--------------------|-------|-------|
| `background_color` | color | Fill color. |
| `border_color`     | color | Border color. |
| `border_width`     | float | Border width. |

#### Example

```xml
<Body margin_left="16" margin_right="16" margin_top="12" margin_bottom="10"
      background_color="#FFFFFF" border_color="#DDDDDD" border_width="1">
  <Text>Body content</Text>
</Body>
```

## 6. Layout Containers

### `<Layout>`

Arranges child elements along one axis.

| Attribute     | Type   | Values |
|--------------|--------|--------|
| `orientation`| string | `horizontal` or `vertical` |

Children can include any supported node. `weight` on child nodes controls proportional sizing.

```xml
<Layout orientation="horizontal">
  <Layout orientation="vertical" weight="0.7">
    <Text>Left</Text>
  </Layout>
  <Layout orientation="vertical" weight="0.3">
    <Text alignment="right">Right</Text>
  </Layout>
</Layout>
```

## 7. Text

### `<Text>`

Text nodes render inline content with style and alignment.

| Attribute   | Type   | Values |
|------------|--------|--------|
| `font_size`| float  | Font size in points. |
| `font_name`| string | Font family name. |
| `color`    | color  | Text color. |
| `style`    | string | `normal`, `bold`, `italic`, `bold_italic` |
| `alignment`| string | `left`, `center`, `right`, `justified` |
| `underline`| bool   | `true` or `false` |

```xml
<Text font_name="OpenSans" font_size="12" color="black" style="bold" alignment="left">
  Hello DocCraft
</Text>
```

## 8. Images

### `<Image>`

| Attribute | Type   | Notes |
|----------|--------|-------|
| `src`    | string | Path to image file. |

```xml
<Image src="images/logo.png" width="120" height="40" />
```

## 9. Blank Spacers

### `<Blank>`

A vertical spacer. Typically used with `height`.

```xml
<Blank height="8" />
```

## 10. Lists

Lists are containers whose children **must be `<Text>`**. Other child tags cause errors.

### `<List>` (Ordered)

| Attribute | Type   | Values |
|----------|--------|--------|
| `style`  | string | `number` or `roman` |

### `<UList>` (Unordered)

| Attribute | Type   | Values |
|----------|--------|--------|
| `dot`    | string | `-`, `*`, `circle`, `box` |

```xml
<List style="roman">
  <Text>First item</Text>
  <Text>Second item</Text>
</List>

<UList dot="*">
  <Text>Alpha</Text>
  <Text>Beta</Text>
</UList>
```

## 11. Tables

### `<Table>`

| Attribute        | Type  | Values / Notes |
|-----------------|-------|----------------|
| `model`         | string| `horizontal` (default) or `vertical` |
| `baseline_offset` | float | Baseline tweak. |
| `TableTile`     | color | Default cell background. |

Tables consist of `<THead>` and `<TBody>`.

### 11.1 Horizontal Table

Requirements:

- `<THead>` is **required**.
- `<THead>` must contain one or more `<Title>` elements.

```xml
<Table TableTile="#EEEEEE">
  <THead>
    <Title background_color="#DDDDDD">Name</Title>
    <Title background_color="#DDDDDD">Value</Title>
  </THead>
  <TBody>
    <Row>
      <Cell><Text>Alpha</Text></Cell>
      <Cell><Text>42</Text></Cell>
    </Row>
  </TBody>
</Table>
```

### 11.2 Vertical Table

Requirements:

- `model="vertical"`
- Each `<Row>` must contain **exactly one** `<VTitle>`.
- `<THead>` (with `<HTitle>`) is optional.

```xml
<Table model="vertical">
  <THead>
    <HTitle background_color="#EEEEEE">Q1</HTitle>
    <HTitle background_color="#EEEEEE">Q2</HTitle>
  </THead>
  <TBody>
    <Row>
      <VTitle background_color="#DDDDDD">Revenue</VTitle>
      <Cell><Text>100</Text></Cell>
      <Cell><Text>120</Text></Cell>
    </Row>
  </TBody>
</Table>
```

### 11.3 Table Child Rules

- `<Row>` may specify `background_color`.
- `<Cell>` may specify `background_color` or `TableTile`.
- A `<Cell>` may contain a **single** child of type `<Text>` or `<Image>`.
- `<Title>`, `<HTitle>`, `<VTitle>` text supports:
  - `alignment`: `left`, `center`, `right`, `justified`
  - `style`: `normal`, `bold`, `italic`, `bold_italic`
  - `color`
  - `background_color` (varies by title type)

## 12. Shapes

### `<Rectangle>`

| Attribute           | Type  | Notes |
|--------------------|-------|-------|
| `background_color` | color | Fill color. |
| `border_color`     | color | Border color. |
| `border_width`     | float | Border width. |

### `<Circle>`

Same attributes as `<Rectangle>`.

### `<Triangle>`

| Attribute           | Type  | Notes |
|--------------------|-------|-------|
| `points`           | string| Format: `x1,y1 x2,y2 x3,y3` (exactly 3 points). |
| `background_color` | color | Fill color. |
| `border_color`     | color | Border color. |
| `border_width`     | float | Border width. |

### `<Polygon>`

| Attribute           | Type  | Notes |
|--------------------|-------|-------|
| `points`           | string| Format: `x1,y1 x2,y2 x3,y3 ...` |
| `background_color` | color | Fill color. |
| `border_color`     | color | Border color. |
| `border_width`     | float | Border width. |

### `<Line>`

| Attribute       | Type  | Notes |
|----------------|-------|-------|
| `x1`, `y1`      | float | Start point. |
| `x2`, `y2`      | float | End point. |
| `border_color`  | color | Line color. |
| `border_width`  | float | Line width. |

#### Example

```xml
<Rectangle x="10" y="20" width="100" height="50"
           background_color="#F0F0F0" border_color="#333333" border_width="1" />

<Line x1="0" y1="0" x2="100" y2="0" border_color="black" border_width="2" />

<Triangle points="0,0 40,0 20,30" background_color="red" />

<Polygon points="0,0 40,0 40,40 0,40" background_color="#FFFFFF" border_color="#000000" border_width="0.5" />
```

## 13. Settings and Fonts

### `<Settings>`

Used to register external fonts.

```xml
<Settings>
  <Fonts>
    <Font name="OpenSans">
      <FontNormal src="OpenSans.ttf" />
      <FontBold src="OpenSans-Bold.ttf" />
      <FontItalic src="OpenSans-Italic.ttf" />
      <FontBoldItalic src="OpenSans-BoldItalic.ttf" />
    </Font>
  </Fonts>
</Settings>
```

Rules:

- `<Font>` **must** include a `name` attribute.
- `<FontNormal>`, `<FontBold>`, `<FontItalic>`, `<FontBoldItalic>` are optional.
- Each font variant uses `src` to specify the file path.

## 14. Complete Example

```xml
<Document>
  <Settings>
    <Fonts>
      <Font name="OpenSans">
        <FontNormal src="OpenSans.ttf" />
        <FontBold src="OpenSans-Bold.ttf" />
      </Font>
    </Fonts>
  </Settings>

  <Header margin_left="16" margin_right="16" background_color="#F2F4F7" border_color="#2B2B2B" border_width="1">
    <Layout orientation="horizontal">
      <Layout orientation="vertical" weight="0.7">
        <Text font_name="OpenSans" font_size="14" color="#111111">Monthly Report</Text>
        <Text font_name="OpenSans" font_size="10" color="#555555">Summary</Text>
      </Layout>
      <Layout orientation="vertical" weight="0.3">
        <Text font_name="OpenSans" font_size="10" color="#111111" alignment="right">Date: 2026-02-15</Text>
        <Text font_name="OpenSans" font_size="10" color="#111111" alignment="right">ID: REP-2026-0215</Text>
      </Layout>
    </Layout>
    <Blank height="4" />
  </Header>

  <Body margin_left="16" margin_right="16" margin_top="12" margin_bottom="10">
    <Text font_name="OpenSans" font_size="11" color="#111111">Summary Table</Text>
    <Table>
      <THead>
        <Title background_color="#E6E6E6">Field</Title>
        <Title background_color="#E6E6E6">Value</Title>
      </THead>
      <TBody>
        <Row background_color="#F9F9F9">
          <Cell><Text font_name="OpenSans" font_size="10">Alpha</Text></Cell>
          <Cell><Text font_name="OpenSans" font_size="10">12.4</Text></Cell>
        </Row>
        <Row>
          <Cell><Text font_name="OpenSans" font_size="10">Beta</Text></Cell>
          <Cell><Text font_name="OpenSans" font_size="10">7.9</Text></Cell>
        </Row>
      </TBody>
    </Table>

    <Blank height="8" />

    <Text font_name="OpenSans" font_size="11" color="#111111">Items</Text>
    <UList dot="*">
      <Text>First item</Text>
      <Text>Second item</Text>
    </UList>
  </Body>

  <Footer margin_left="16" margin_right="16" background_color="#F2F4F7" border_color="#2B2B2B" border_width="1">
    <Blank height="4" />
    <Text font_name="OpenSans" font_size="9" color="#333333">Footer text</Text>
    <Blank height="4" />
  </Footer>
</Document>
```

## 15. Application Domains

The `.craft` format is designed for structured documents and simple graphics composition. Common applications:

- Generated reports and dashboards
- Invoices, receipts, and billing documents
- Certificates and badges
- Medical and lab reports
- Logistics paperwork (packing slips, manifests)
- Compliance and audit documents
- Educational worksheets
- Light diagramming and annotation (via shapes + layout)

## 16. Validation Summary

The parser enforces the following constraints:

- `<Document>` root is required.
- `<Body>` is required.
- Unknown tags are rejected.
- `<List>` / `<UList>` children must be `<Text>`.
- `<Triangle>` must contain exactly 3 points in `points`.
- Horizontal `<Table>` requires `<THead>` and `<Title>` columns.
- Vertical `<Table>` requires one `<VTitle>` per `<Row>`.
- `<Cell>` supports only a single `<Text>` or `<Image>` child.

Violations result in parsing exceptions.
