# Document structure

The root element **must** be `<Document>`.

```xml
<Document>
  <Settings>...</Settings>   <!-- optional, at most once -->
  <Metadata>...</Metadata>   <!-- optional, at most once -->
  <Header>...</Header>       <!-- optional -->
  <Body>...</Body>           <!-- REQUIRED -->
  <Footer>...</Footer>       <!-- optional -->
</Document>
```

- Missing `<Body>` → `DataFormatException` ("Invalid .craft document: missing required <Body>
  element").
- Malformed XML → `DataFormatException`.
- Any top-level tag under `<Document>` other than `Header`/`Body`/`Footer`/`Settings`/`Metadata`
  is **silently ignored** (not an error) — different from an unknown *attribute*, which is always
  a hard error.

## `<Header>` / `<Body>` / `<Footer>`

Each is built into a `DocraftLoomVStack` (there's no dedicated Header/Body/Footer node class —
it's just a vertical stack with region-specific attributes, not the generic common attributes):

| Attribute | Type | Default |
|---|---|---|
| `margin_top` | float | `20.0` |
| `margin_bottom` | float | `20.0` |
| `margin_left` | float | `20.0` |
| `margin_right` | float | `20.0` |
| `background_color` | color | fully transparent |
| `border_color` | color | fully transparent |
| `border_width` | float | `1.0` (invisible by default since border color alpha is 0) |

Body's margins have layout meaning: body content starts `margin.top` below the header, usable
per-page height shrinks by `margin.top + margin.bottom`, and `margin.left/right` insets every
top-level child.

Header/Footer are laid out **once** and stamped identically on every physical page. Body is laid
out as one continuous flow, then split across pages (see `pagination.md`). If a header's real
content is taller than its `<SectionRatios>` share implies, the body is pushed down below its
actual bottom edge rather than overlapping it; a footer taller than its ratio still fits fully on
the page rather than overflowing off it.

```xml
<Header margin_left="30" margin_right="30" border_width="0.5" border_color="black">
  <Text>Header text</Text>
</Header>
<Body margin_left="20" margin_top="15">
  <Title>Hello</Title>
</Body>
<Footer margin_bottom="5">
  <PageNumber />
</Footer>
```

## `<Settings>`

```xml
<Settings>
  <Page size="A4" orientation="portrait" />
  <SectionRatios header_ratio="0.06" body_ratio="0.88" footer_ratio="0.06" />
  <Fonts default="MyFont">
    <Font name="MyFont">
      <FontNormal src="path/to/Regular.ttf" />
      <FontBold src="path/to/Bold.ttf" />
      <FontItalic src="path/to/Italic.ttf" />
      <FontBoldItalic src="path/to/BoldItalic.ttf" />
    </Font>
  </Fonts>
</Settings>
```

- `<Settings>` itself takes **no attributes** (e.g. `<Settings page_size="A4">` throws
  `InvalidInputException`).
- `<Page>`, `<SectionRatios>`, `<Fonts>` may each appear **at most once**; a repeat, or any other
  child tag, throws `InvalidInputException`.
- `<Page size="..." orientation="...">`:
  - `size` (case-insensitive): `A3`, `A5`, `Letter`, `Legal`, `A4`. Default `A4`. Anything else
    throws.
  - `orientation` (case-sensitive, exactly these strings): `landscape`, `portrait`. Default
    `portrait`.
- `<SectionRatios header_ratio="" body_ratio="" footer_ratio="">`: three floats, default
  `0.06 / 0.88 / 0.06`, each must be `>= 0`, and the sum must be `<= 1.0` (+1e-6 tolerance) or
  `InvalidInputException` ("Section ratios must sum to 1.0 or less"). These are only fractions
  used to compute an *initial* region height — a region whose real content needs more room still
  gets it (see Header/Body/Footer note above).
- `<Fonts default="FamilyName">`: sets the font family used by every Text/Title/Subtitle/
  PageNumber/table-cell-text node that doesn't set its own `font_name`. Empty string throws.
  - `<Font name="FamilyName">` registers a family from up to 4 TTF variants: `<FontNormal
    src="...">`, `<FontBold src="...">`, `<FontItalic src="...">`, `<FontBoldItalic src="...">`,
    each requiring a non-empty `src`. `<Font>` requires a non-empty `name` and **at least one**
    variant child, or `InvalidInputException`.
  - Any child of `<Fonts>` other than `<Font>` throws.

```xml
<Settings>
  <Page size="A3" orientation="landscape" />
  <SectionRatios header_ratio="0.1" body_ratio="0.8" footer_ratio="0.1" />
  <Fonts default="TestOpenSans">
    <Font name="TestOpenSans">
      <FontNormal src="fonts/OpenSans/OpenSans.ttf" />
      <FontBold src="fonts/OpenSans/OpenSans-Bold.ttf" />
    </Font>
  </Fonts>
</Settings>
```

## `<Metadata>`

```xml
<Metadata>
  <DocumentTitle>Quarterly Report</DocumentTitle>
  <Author>Mario Rossi</Author>
  <Creator>Docraft</Creator>
  <Producer>Docraft</Producer>
  <Subject>...</Subject>
  <Keywords>report, q1</Keywords>
</Metadata>
```

Only these six plain-text subtags are wired into the PDF info dict:
`DocumentTitle`, `Author`, `Creator`, `Producer`, `Subject`, `Keywords`. Other tag names
(`CreationDate`, `ModificationDate`, `Trapped`, `GtsPdfx`, `AutoKeywords`) are legacy leftovers
still tokenized but **silently ignored** by the loom parser — don't generate them expecting an
effect.
