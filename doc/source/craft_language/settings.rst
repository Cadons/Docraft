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
tolerance) — an invalid combination is a parse error. The body's own height
is always ``page_height - header_height - footer_height``, so it never
overlaps the other two regions even if the ratios don't sum to exactly 1.

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

