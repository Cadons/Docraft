# DocCraft `.craft` Language Specification

This document describes the XML grammar currently implemented by the DocCraft parser in this repository.
It is intentionally implementation-oriented and focuses on what the parser actually accepts and enforces.

## 1. Parsing Model

- `.craft` files are XML parsed with `pugixml`.
- Tag names and attribute names are case-sensitive.
- Unknown tags in parsed subtrees raise an error (`No parser registered for node: ...`).
- Unknown attributes are currently ignored (unless a known attribute value is invalid).
- Attribute values are parsed with `pugixml` conversions (`as_float`, `as_bool`, etc.).

## 2. Required Structure

The parser expects:

- root tag: `<Document>`
- required section: `<Body>`
- optional root attribute: `path` (output directory for rendered file)

Optional top-level sections:

- `<Settings>`
- `<Metadata>`
- `<Header>`
- `<Footer>`

Canonical skeleton:

```xml
<Document path="exports/reports">
  <Settings>...</Settings>
  <Metadata>...</Metadata>
  <Header>...</Header>
  <Body>...</Body>
  <Footer>...</Footer>
</Document>
```

Notes:

- Tag casing must match exactly (`<Document>`, not `<document>`).
- The loader reads the first matching top-level section for each known tag.
- Unknown top-level tags are ignored unless they are inside a parsed subtree.

## 3. Supported Tags (Canonical Names)

- `Document`
- `Settings`
- `Metadata`
- `Header`
- `Body`
- `Footer`
- `Text`
- `Title`
- `Subtitle`
- `PageNumber`
- `Image`
- `Table`
- `THead`
- `TBody`
- `Row`
- `Cell`
- `HTitle`
- `VTitle`
- `Layout`
- `Blank`
- `NewPage`
- `List`
- `UList`
- `Rectangle`
- `Circle`
- `Triangle`
- `Line`
- `Polygon`
- `Foreach`

`Foreach` is parsed with uppercase `F` in XML.

## 4. Common Node Attributes

Most nodes call the shared attribute configurator and accept:

| Attribute | Type | Notes |
| --- | --- | --- |
| `name` | string | Optional logical identifier. |
| `x` | float | Position x. If `x`/`y` is set and `position` is omitted, node becomes absolute. |
| `y` | float | Position y. |
| `width` | float | Width in points. |
| `height` | float | Height in points. |
| `auto_fill_width` | bool | `true` / `false`. |
| `auto_fill_height` | bool | `true` / `false`. |
| `padding` | float | Padding in points. |
| `weight` | float | Must be `> 0` and `<= 1`. |
| `position` | string | `block` or `absolute`. |
| `z_index` | int | Rendering order among siblings. |
| `visible` | bool | Defaults to `true`. |

Notes:

- `id` exists as a token name but is not consumed by current parsers.
- Invalid `position` or invalid `weight` raises an exception.

## 5. Colors

Color attributes accept:

- named colors: `black`, `white`, `red`, `green`, `blue`, `yellow`, `magenta`, `cyan`, `purple`
- hex: `#RRGGBB` or `#RRGGBBAA`

Invalid color strings raise `invalid_argument`.

## 6. Sections: `Header`, `Body`, `Footer`

Sections are rectangle-like containers with margins.

Section-specific attributes:

| Attribute | Type | Notes |
| --- | --- | --- |
| `margin_top` | float | Top margin. |
| `margin_bottom` | float | Bottom margin. |
| `margin_left` | float | Left margin. |
| `margin_right` | float | Right margin. |

Rectangle attributes also apply:

| Attribute | Type | Notes |
| --- | --- | --- |
| `background_color` | color | Fill. |
| `border_color` | color | Stroke color. |
| `border_width` | float | Stroke width. |

Important behavior:

- Header and Footer are optional.
- If Header/Footer is missing or `visible="false"`, their configured ratio is added to Body during layout.

## 6.1 Document Metadata

Top-level `<Metadata>` can be used to set PDF metadata fields.

Supported subtags:

- `DocumentTitle`
- `Author`
- `Creator`
- `Producer`
- `Subject`
- `Keywords`
- `Trapped`
- `GtsPdfx`
- `CreationDate`
- `ModificationDate`
- `AutoKeywords`

Example:

```xml
<Metadata>
  <DocumentTitle>Q1 Report</DocumentTitle>
  <Author>Mario Rossi</Author>
  <Creator>Docraft CLI</Creator>
  <Producer>Docraft Engine</Producer>
  <Subject>Sales summary</Subject>
  <Keywords>report, q1, sales</Keywords>
  <CreationDate year="2026" month="2" day="22" hour="10" minutes="15" seconds="5" ind="+" off_hour="1" off_minutes="0" />
  <AutoKeywords enabled="true" max_keywords="8" min_length="5" />
</Metadata>
```

Notes:

- `DocumentTitle` updates both PDF metadata title and Docraft document title.
- `CreationDate` and `ModificationDate` accept either attributes (`year`, `month`, etc.) or child tags (`Year`, `Month`, etc.).
- Required date fields: `year`, `month`, `day`. Other fields are optional.
- `AutoKeywords` is optional and disabled by default unless the tag is present.
- `AutoKeywords` attributes:
  - `enabled` (bool): `true/false`, `1/0`, `yes/no`, `on/off`
  - `max_keywords` (int > 0, default `10`)
  - `min_length` (int > 0, default `4`)
  - `language` (string list, default `it,en,fr,de,es`): comma/semicolon/space-separated language codes.
    Supported values: `it`, `en`, `fr`, `de`, `es`.
- When enabled, extracted keywords are merged with existing `Keywords`, avoiding duplicates.

## 6.2 Metadata and AutoKeywords from C++ API

You can configure and apply document metadata directly from C++ through `DocraftDocument`.

Metadata API:

- `set_document_metadata(const DocraftDocumentMetadata&)`
- `document_metadata()`

Auto-keywords API:

- `enable_auto_keywords(bool)`
- `set_auto_keywords_config(const utils::DocraftKeywordExtractor::Config&)`
- `auto_keywords_enabled()`
- `auto_keywords_config()`
- `refresh_auto_keywords()`

Behavior notes:

- If auto-keywords is enabled, `DocraftDocument::render()` automatically calls `refresh_auto_keywords()` before applying metadata to the PDF backend.
- Calling `refresh_auto_keywords()` manually is optional and useful when you need metadata keywords before rendering.
- Extracted keywords are merged with existing `Keywords` metadata and duplicates are removed.

Example:

```cpp
#include "docraft_document.h"
#include "model/docraft_text.h"

docraft::DocraftDocument doc("Q1 Report");
doc.add_node(std::make_shared<docraft::model::DocraftText>(
    "il parser parser metadata metadata engine engine engine and parser documento"));

docraft::DocraftDocumentMetadata metadata;
metadata.set_author("Mario Rossi");
metadata.set_keywords("manuale");
doc.set_document_metadata(metadata);

doc.set_auto_keywords_config({
    .max_keywords = 8,
    .min_length = 4,
    .stop_word_languages = {"it", "en", "fr", "de", "es"}
});
doc.enable_auto_keywords(true);

// Optional: force extraction before render.
doc.refresh_auto_keywords();

doc.render();
```

## 7. Text Nodes

### 7.1 `Text`

Attributes:

| Attribute | Type | Values |
| --- | --- | --- |
| `font_size` | float | Font size in points. |
| `font_name` | string | Font family name. |
| `color` | color | Text color. |
| `style` | string | `normal`, `bold`, `italic`, `bold_italic` |
| `alignment` | string | `left`, `center`, `right`, `justified` |
| `underline` | bool | `true` / `false` |

Notes:

- Text content is taken from `child_value()`.
- Leading/trailing whitespace is trimmed by the model.
- Nested element tags inside `Text` are technically parsed but not a supported authoring pattern.

Example:

```xml
<Text font_name="OpenSans" font_size="12" style="bold" color="#111111">
  Hello DocCraft
</Text>
```

### 7.2 `Title`

`Title` is a predefined heading text node (h1-like).

Default style:

- `font_size=24`
- `style=bold`

All standard text attributes are supported and can override these defaults.

```xml
<Title>Main Heading</Title>
```

### 7.3 `Subtitle`

`Subtitle` is a predefined heading text node (h2-like).

Default style:

- `font_size=18`
- `style=bold`

All standard text attributes are supported and can override these defaults.

```xml
<Subtitle>Section Heading</Subtitle>
```

### 7.4 `PageNumber`

`PageNumber` supports the same style/alignment attributes as `Text`.

```xml
<PageNumber font_size="9" alignment="right" />
```

## 8. Layout and Flow Helpers

### 8.1 `Layout`

Attribute:

| Attribute | Type | Values |
| --- | --- | --- |
| `orientation` | string | `horizontal` or `vertical` |

Notes:

- Always set `orientation`. Current model does not define a safe default.
- Child `weight` controls width distribution in horizontal layouts.

### 8.2 `Blank`

A spacer node, typically used with `height`.

```xml
<Blank height="8" />
```

### 8.3 `NewPage`

Manual page break marker used by body pagination logic.

```xml
<NewPage />
```

## 9. Lists

### 9.1 `List` (ordered)

| Attribute | Type | Values |
| --- | --- | --- |
| `style` | string | `number` or `roman` |

### 9.2 `UList` (unordered)

| Attribute | Type | Values |
| --- | --- | --- |
| `dot` | string | `-`, `*`, `circle`, `box` |

Rules:

- Children must be exactly `<Text>` nodes.
- Any other child tag causes a parse error.

Defaults:

- `List`: ordered style `number`
- `UList`: dot style `circle`

## 10. Images

### 10.1 `Image`

| Attribute | Type | Notes |
| --- | --- | --- |
| `src` | string | File path (PNG/JPEG typically). |
| `data` | string | Raw source reference or inline base64 payload. |
| `data_width` | int | Required for base64 payloads. |
| `data_height` | int | Required for base64 payloads. |

Rules:

- `src` and `data` are mutually exclusive.
- Base64 is detected when `data` starts with `base64:` or contains `base64,`.
- For base64 payloads, decoded byte length must be `data_width * data_height * 3` (RGB24).
- If `data` is not base64, it is treated as a raw image id for templating.

Examples:

```xml
<Image src="images/logo.png" width="120" height="40" />
<Image data="base64:AAAA////" data_width="2" data_height="1" width="40" height="20" />
<Image data="hero_image" width="200" height="120" />
```

## 11. Tables

### 11.1 `Table` attributes

| Attribute | Type | Notes |
| --- | --- | --- |
| `model` | string | `horizontal`, `vertical`, JSON matrix, or template placeholder. |
| `header` | string | JSON array, or template placeholder for header titles. |
| `baseline_offset` | float | Text baseline tweak. Default is `0.25`. |
| `TableTile` | color | Default content-cell background. |

`model` behavior:

- `horizontal` / `vertical`: sets orientation
- JSON matrix: loads table body rows (horizontal only)
- `${...}`: deferred JSON matrix resolved by template engine

`header` behavior:

- JSON array of strings or `${...}` deferred value
- not allowed together with explicit `<THead>`

Mutual exclusivity:

- If `model`/`header` is JSON/template driven, `<TBody>` is not allowed.
- JSON/template table data is not allowed with vertical orientation.

### 11.2 Horizontal table (explicit XML mode)

Requirements:

- `<THead>` is mandatory.
- `<THead>` must contain `<HTitle>` children.

Example:

```xml
<Table TableTile="#EEEEEE">
  <THead>
    <HTitle background_color="#DDDDDD">Name</HTitle>
    <HTitle background_color="#DDDDDD">Value</HTitle>
  </THead>
  <TBody>
    <Row>
      <Cell><Text>Alpha</Text></Cell>
      <Cell><Text>42</Text></Cell>
    </Row>
  </TBody>
</Table>
```

### 11.3 Horizontal table (JSON/template mode)

Examples:

```xml
<Table model='[["v1","v2"],["v3","v4"]]' header='["H1","H2"]' />
<Table model='${rows_json}' header='${header_json}' />
```

Rules:

- matrix must be non-empty, rectangular, and string-only.
- header array must be non-empty and string-only.
- header size must match model columns.

### 11.4 Vertical table

Requirements:

- set `model="vertical"`
- each `<Row>` must contain exactly one `<VTitle>`
- optional `<THead>` can contain `<HTitle>` nodes

Example:

```xml
<Table model="vertical">
  <THead>
    <HTitle>Q1</HTitle>
    <HTitle>Q2</HTitle>
  </THead>
  <TBody>
    <Row>
      <VTitle>Revenue</VTitle>
      <Cell><Text>100</Text></Cell>
      <Cell><Text>120</Text></Cell>
    </Row>
  </TBody>
</Table>
```

### 11.5 Table child rules

- `<Row>` may use `background_color`.
- `<Cell>` may use `background_color` or `TableTile`.
- Cell content supports `Text` or `Image` as first child.
- Extra children in the same cell are not validated and should be avoided.

Title styling attributes (`HTitle`, `VTitle`):

- `alignment`: `left`, `center`, `right`, `justified`
- `style`: `normal`, `bold`, `italic`, `bold_italic`
- `color`
- `background_color`

## 12. Shapes

### 12.1 `Rectangle`

Attributes:

- `background_color`
- `border_color`
- `border_width`

`Rectangle` is a container and can have children.

### 12.2 `Circle`

Same visual attributes as `Rectangle`.

### 12.3 `Line`

Attributes:

- `x1`, `y1`, `x2`, `y2`
- `border_color`
- `border_width`

### 12.4 `Triangle`

Attributes:

- `points` format: `x1,y1 x2,y2 x3,y3`
- `background_color`
- `border_color`
- `border_width`

Rule:

- if `points` is provided, it must contain exactly 3 points.

### 12.5 `Polygon`

Attributes:

- `points` format: `x1,y1 x2,y2 x3,y3 ...`
- `background_color`
- `border_color`
- `border_width`

Point parsing notes:

- points are split by spaces, each token must be `x,y`.
- malformed tokens raise parse errors.

## 13. Settings and Fonts

### 13.1 `Settings` attributes

| Attribute | Type | Accepted values |
| --- | --- | --- |
| `page_size` | string | `A3`, `a3`, `A4`, `a4`, `A5`, `a5`, `Letter`, `letter`, `Legal`, `legal` |
| `page_orientation` | string | `portrait`, `landscape` |
| `header_ratio` | float | non-negative |
| `body_ratio` | float | non-negative |
| `footer_ratio` | float | non-negative |

Rules:

- if only `page_orientation` is provided, page size defaults to `A4`.
- if any ratio is provided, missing ones default to `0.06 / 0.88 / 0.06`.
- ratio sum must be `<= 1.0`.

### 13.2 Fonts in settings

`Settings` may contain `Fonts`, with `Font` entries:

```xml
<Settings page_size="A4" page_orientation="portrait">
  <Fonts>
    <Font name="OpenSans">
      <FontNormal src="OpenSans-Regular.ttf" />
      <FontBold src="OpenSans-Bold.ttf" />
      <FontItalic src="OpenSans-Italic.ttf" />
      <FontBoldItalic src="OpenSans-BoldItalic.ttf" />
    </Font>
  </Fonts>
</Settings>
```

Rules:

- `Font` requires `name`.
- `FontNormal`, `FontBold`, `FontItalic`, `FontBoldItalic` are optional.
- each variant uses `src`.

## 14. Templating and `Foreach`

DocCraft templating is handled by `DocraftTemplateEngine` before layout/render.

### 14.1 Variable replacement

General placeholder format:

- `${variable_name}`

Used in:

- `Text` content
- `Image src` / raw image id
- `Table model` and `Table header` when template strings are used
- other string fields processed by the template engine

### 14.2 `Foreach`

`Foreach` requires attribute:

- `model`: JSON array string or placeholder that resolves to a JSON array

Example:

```xml
<Foreach model='${employees}'>
  <Text>${data("name")}</Text>
  <Text>${data("role")}</Text>
</Foreach>
```

Rules:

- `Foreach` without `model` raises an error.
- Child nodes are stored as template nodes and expanded at template phase.
- `data("field")` (or single-quoted variant) reads fields from current foreach item.
- non-string field values are rendered as JSON text.

Single-quoted JSON in `model` is normalized by the parser before evaluation.

## 15. Validation Summary

Guaranteed parser validations:

- `<Document>` root must exist.
- `<Body>` must exist.
- unknown child tags in parsed node trees are rejected.
- invalid enum-like values (`style`, `alignment`, `orientation`, `position`, list options) are rejected.
- invalid colors are rejected.
- list children must be `Text`.
- `Image` cannot use both `src` and `data`.
- base64 image payloads must match RGB byte size.
- table structural constraints (as documented above) are enforced.
- `Foreach` requires `model`.

Important non-validations:

- unknown attributes are generally ignored.
- `Layout` without `orientation` is not rejected by parser; always set it explicitly.

## 16. Full Example

```xml
<Document>
  <Settings page_size="A4" page_orientation="portrait" header_ratio="0.08" body_ratio="0.84" footer_ratio="0.08">
    <Fonts>
      <Font name="OpenSans">
        <FontNormal src="OpenSans-Regular.ttf" />
        <FontBold src="OpenSans-Bold.ttf" />
      </Font>
    </Fonts>
  </Settings>

  <Header margin_left="16" margin_right="16" background_color="#F2F4F7">
    <Layout orientation="horizontal">
      <Layout orientation="vertical" weight="0.7">
        <Text font_name="OpenSans" font_size="14" style="bold">Monthly Report</Text>
        <Text font_name="OpenSans" font_size="10">Summary</Text>
      </Layout>
      <Layout orientation="vertical" weight="0.3">
        <PageNumber font_name="OpenSans" font_size="9" alignment="right" />
      </Layout>
    </Layout>
  </Header>

  <Body margin_left="16" margin_right="16" margin_top="8">
    <Text>Summary Table</Text>
    <Table model='${summary_rows}' header='${summary_header}' />
    <Blank height="10" />

    <UList dot="circle">
      <Text>First item</Text>
      <Text>Second item</Text>
    </UList>

    <NewPage />

    <Foreach model='${employees}'>
      <Text>${data("name")} - ${data("role")}</Text>
    </Foreach>
  </Body>

  <Footer margin_left="16" margin_right="16" background_color="#F2F4F7">
    <Text font_size="9" alignment="center">Confidential</Text>
  </Footer>
</Document>
```
