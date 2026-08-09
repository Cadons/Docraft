# Colors and fonts

## Colors

Two forms only, resolved after `${...}` templating has already run:

1. **Hex**: `#RRGGBB` or `#RRGGBBAA` (7 or 9 chars, `#` + hex digits only). Anything else
   starting with `#` throws `InvalidInputException` ("Invalid hex color: ...").
2. **Named** (exactly these 9, case-sensitive, lowercase only):
   ```
   black, white, red, green, blue, yellow, cyan, magenta, purple
   ```
   Any other bare word throws `InvalidInputException` listing the accepted set verbatim. **Do
   not invent additional named colors** (no `orange`, `grey`, `pink`, etc.) — the palette is
   deliberately small by design.

Defaults: shape fill/border defaults to fully transparent black; text color defaults to opaque
black; `border_width` defaults to `1.0` (invisible while the default border color's alpha is 0,
so a shape with only `border_width` set and no `border_color` still renders no visible border).

## Fonts

Base-14 (always available, no registration needed):

```
Courier, Courier-Bold, Courier-Oblique, Courier-BoldOblique,
Helvetica, Helvetica-Bold, Helvetica-Oblique, Helvetica-BoldOblique,
Times-Roman, Times-Bold, Times-Italic, Times-BoldItalic,
Symbol, ZapfDingbats
```

Bundled embedded font families (registered via `docraft/fonts.json`, embedded at build time):

- `Roboto` (Regular, Bold, Italic, BoldItalic variants)
- `OpenSans` (Regular, Bold, Italic, BoldItalic variants) — **note:** `fonts.json`'s
  `OpenSans-Italic` entry currently points at the `OpenSans-BoldItalic.ttf` file (a data bug), so
  requesting `font_name="OpenSans" style="italic"` currently renders bold-italic glyphs instead
  of plain italic. Don't present this combination as a clean italic example.

### How style resolves to an actual glyph

Set `font_name="FamilyName"` (the plain family — e.g. `"Roboto"`, `"Helvetica"`, or a custom
`<Font name="...">` family) plus `style="bold"` / `"italic"` / `"bold_italic"` on the
Text/Title/Subtitle/PageNumber node. The resolver looks up the closest registered variant for
that family (e.g. `"Roboto"` + bold → `"Roboto-Bold"`), falling back to plain regular, or to the
literal requested name unresolved if the family isn't registered at all.

**Don't** write the `-Bold`/`-Italic` suffix directly into `font_name` — set `font_name="Roboto"`
and `style="bold"` instead. (Suffix parsing does exist and tolerates both hyphen- and
space-separated suffixes case-insensitively, mainly so base-14 PDF font names like
`Helvetica-BoldOblique` and custom registered variant names both work uniformly — but the
attribute-pair form is the intended usage.)

### Custom fonts

Register in `<Settings>` (see `document-structure.md`), then reference by family name:

```xml
<Settings>
  <Fonts default="Brand">
    <Font name="Brand">
      <FontNormal src="fonts/Brand/Brand-Regular.ttf" />
      <FontBold src="fonts/Brand/Brand-Bold.ttf" />
    </Font>
  </Fonts>
</Settings>
<Body>
  <Text>Uses the document default (Brand)</Text>
  <Text font_name="Times-Roman">Uses its own font instead</Text>
</Body>
```
