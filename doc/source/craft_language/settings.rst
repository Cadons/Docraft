Settings
========

The ``<Settings>`` element configures document-level options such as page
format, section ratios, and custom fonts.

Page Format
-----------

``<Settings>`` itself carries no attributes — it must contain only
``<Page>``, ``<SectionRatios>``, and/or ``<Fonts>``, each at most once:

.. code-block:: xml

   <Settings>
     <Page size="A4" orientation="portrait"/>
     <SectionRatios header_ratio="0.1" body_ratio="0.8" footer_ratio="0.1"/>
   </Settings>

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - Element / Attribute
     - Type
     - Description
   * - ``<Page size="...">``
     - string
     - ``A3``, ``A4``, ``A5``, ``Letter``, or ``Legal`` (default ``A4``).
       An unrecognized value is a parse error.
   * - ``<Page orientation="...">``
     - string
     - ``portrait`` or ``landscape`` (default ``portrait``). An unrecognized
       value is a parse error.
   * - ``<SectionRatios header_ratio="...">``
     - float
     - Header height as a fraction of the page (default ``0.06``).
   * - ``<SectionRatios body_ratio="...">``
     - float
     - Body height fraction (default ``0.88``).
   * - ``<SectionRatios footer_ratio="...">``
     - float
     - Footer height fraction (default ``0.06``).

Ratios must each be ≥ 0 and sum to at most ``1.0`` (with a small epsilon
tolerance) — an invalid combination is a parse error. Each ratio sets a
*minimum* reserved height, not a fixed one: if a ``<Header>``/``<Footer>``'s
actual content (including its own padding/margins) needs more room than its
ratio allocates, that region grows to fit it instead of overlapping the body,
and the footer is pushed up so it still fits fully on the page. The body's
own height is always ``page_height - header_extent - footer_extent``, using
each region's actual (ratio-or-larger) extent, so it never overlaps the
other two regions.

Because of this, ``<SectionRatios>`` is **not** the primary lever for page
geometry in most documents: whenever a header/footer's measured content is
taller than its ratio's share of the page — the common case for anything
beyond a one-line header — that measured height wins outright, and changing
the ratio has no visible effect on pagination at all. Reach for it only to
pre-reserve extra blank space above a minimum, e.g. to keep a short header
vertically centered in a taller band; to actually resize a header/footer,
change its content (or content's padding/margins) instead.

Custom Fonts
------------

Register external TTF fonts so they can be used by ``<Text>`` nodes:

.. code-block:: xml

   <Settings>
     <Fonts>
       <Font name="MyFont">
         <FontNormal src="fonts/MyFont-Regular.ttf"/>
         <FontBold src="fonts/MyFont-Bold.ttf"/>
         <FontItalic src="fonts/MyFont-Italic.ttf"/>
         <FontBoldItalic src="fonts/MyFont-BoldItalic.ttf"/>
       </Font>
     </Fonts>
   </Settings>

Each ``<Font>`` declares a family name. Inside, you provide up to four
variant files:

- ``<FontNormal>`` — regular weight
- ``<FontBold>`` — bold weight
- ``<FontItalic>`` — italic style
- ``<FontBoldItalic>`` — bold italic

Each variant requires a ``src`` attribute pointing to a ``.ttf`` file.

.. _builtin-fonts:

Built-in Fonts
--------------

Every ``font_name`` resolves against the PDF standard's 14 base fonts before
falling back to any font registered via ``<Fonts>`` above — no embedding, no
external files, always available:

.. list-table::
   :header-rows: 1
   :widths: 25 75

   * - Family
     - Built-in names
   * - Courier
     - ``Courier``, ``Courier-Bold``, ``Courier-Oblique``, ``Courier-BoldOblique``
   * - Helvetica
     - ``Helvetica``, ``Helvetica-Bold``, ``Helvetica-Oblique``, ``Helvetica-BoldOblique``
   * - Times
     - ``Times-Roman``, ``Times-Bold``, ``Times-Italic``, ``Times-BoldItalic``
   * - Symbol
     - ``Symbol`` (symbol glyphs, no bold/italic variant)
   * - ZapfDingbats
     - ``ZapfDingbats`` (dingbat glyphs, no bold/italic variant)

``Helvetica`` is the document-wide fallback (see :ref:`Default Font
<document-default-font>` below) when nothing else applies.

.. code-block:: xml

   <Text font_name="Helvetica" style="bold">Resolves to Helvetica-Bold</Text>
   <Text font_name="Courier" style="italic">Resolves to Courier-Oblique</Text>

For **Courier** and **Helvetica**, ``style="bold"``/``"italic"``/``"bold_italic"``
composes with the plain family name to pick the matching built-in variant
automatically, so ``font_name="Helvetica"`` is enough regardless of ``style``.

**Times** is the one exception: its regular weight is named ``Times-Roman``,
not ``Times`` — a name ``style`` doesn't strip the way it strips ``-Bold``/
``-Italic`` off the other families, so ``font_name="Times-Roman"`` combined
with ``style="bold"`` does **not** resolve to ``Times-Bold``; it silently
stays regular-weight. Name the variant you want directly instead:

.. code-block:: xml

   <!-- Wrong: style is ignored, renders as plain Times-Roman -->
   <Text font_name="Times-Roman" style="bold">Not actually bold</Text>

   <!-- Right: name the built-in variant explicitly -->
   <Text font_name="Times-Bold">Actually bold</Text>

``Symbol`` and ``ZapfDingbats`` have a single variant each — every character
code maps to a symbol/dingbat glyph instead of Latin text, and ``style`` has
nothing to compose with.

.. _document-default-font:

Default Font
------------

``<Fonts default="...">`` sets the font family applied to any ``<Text>``,
``<Title>``, ``<Subtitle>``, ``<PageNumber>``, or table cell that doesn't
specify its own ``font_name`` attribute. It can name either a custom family
declared alongside it or a built-in PDF font:

.. code-block:: xml

   <Settings>
     <Fonts default="MyFont">
       <Font name="MyFont">
         <FontNormal src="fonts/MyFont-Regular.ttf"/>
         <FontBold src="fonts/MyFont-Bold.ttf"/>
       </Font>
     </Fonts>
   </Settings>
   <Body>
     <Text>Uses MyFont</Text>
     <Text font_name="Times-Roman">Uses Times-Roman instead</Text>
   </Body>

A node's own ``font_name`` attribute always takes precedence over the
document default. When neither is set, nodes fall back to ``Helvetica``.

Text Encoding
-------------

Text content, ``${...}`` template values, and JSON data are all read as
UTF-8. What happens to a non-ASCII character (e.g. accented Western
European letters — à, è, é, ì, ò, ù, ç — or “smart” punctuation) at render
time depends on which font resolves for that text:

- A **custom TTF** registered via ``<Font>`` above supports UTF-8 directly
  — any character the font itself contains renders correctly.
- A **built-in PDF font** (``Helvetica``, ``Times-Roman``, ``Courier``, and
  their bold/italic variants — the default when no ``font_name``/document
  default names a custom family) only supports the single-byte
  Windows-1252 (WinAnsi) code page. Docraft transcodes UTF-8 to
  Windows-1252 automatically for these, so every accented Western European
  letter and common typographic character (curly quotes, en/em dash,
  ellipsis, €, ™, ...) still renders correctly. A character with no
  Windows-1252 representation at all (e.g. CJK, Cyrillic, Greek, emoji)
  renders as ``?`` — use a custom TTF that covers the needed script/range
  instead. The first time a given unmappable character is hit, Docraft logs a
  warning to stderr naming the character's Unicode codepoint, so a document
  silently losing a character during a batch render is discoverable instead
  of only visible on manual inspection of the output PDF.

