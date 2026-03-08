Settings
========

The ``<Settings>`` element configures document-level options such as page
format, section ratios, and custom fonts.

Page Format
-----------

.. code-block:: xml

   <Settings page_size="A4" page_orientation="portrait">
   </Settings>

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - Attribute
     - Type
     - Description
   * - ``page_size``
     - string
     - ``A3``, ``A4``, ``A5``, ``Letter``, or ``Legal``.
   * - ``page_orientation``
     - string
     - ``portrait`` or ``landscape``.
   * - ``header_ratio``
     - float
     - Header height as a fraction of the page (default ``0.06``).
   * - ``body_ratio``
     - float
     - Body height fraction (default ``0.88``).
   * - ``footer_ratio``
     - float
     - Footer height fraction (default ``0.06``).

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

