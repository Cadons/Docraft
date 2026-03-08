Document Structure
==================

A Craft Language file is an XML document whose root element is ``<Document>``.
Inside the root you define three optional **sections** — Header, Body, and
Footer — that map to the top, middle, and bottom areas of every page.

Minimal Example
---------------

.. code-block:: xml

   <Document path="output/">
     <Body margin_left="20" margin_right="20">
       <Text font_size="24" style="bold">Hello, Docraft!</Text>
     </Body>
   </Document>

Root Element: ``<Document>``
----------------------------

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - Attribute
     - Type
     - Description
   * - ``path``
     - string
     - Output directory for the rendered file.

Sections
--------

``<Header>``, ``<Body>``, ``<Footer>``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Each section is a rectangle with configurable margins. Content nodes are
placed inside a section and flow top-to-bottom.

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - Attribute
     - Type
     - Description
   * - ``margin_top``
     - float
     - Top margin in points.
   * - ``margin_bottom``
     - float
     - Bottom margin in points.
   * - ``margin_left``
     - float
     - Left margin in points (default ``10``).
   * - ``margin_right``
     - float
     - Right margin in points (default ``10``).
   * - ``background_color``
     - color
     - Section background (hex ``#RRGGBB`` or named color).
   * - ``border_color``
     - color
     - Section border color.
   * - ``border_width``
     - float
     - Border width in points.

Common Node Attributes
----------------------

Every content node supports these attributes:

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - Attribute
     - Type
     - Description
   * - ``name``
     - string
     - Logical name for DOM lookups.
   * - ``x``, ``y``
     - float
     - Position in points (used with ``position="absolute"``).
   * - ``width``, ``height``
     - float
     - Explicit size in points.
   * - ``auto_fill_width``
     - bool
     - Auto-fill available width (default ``true``).
   * - ``auto_fill_height``
     - bool
     - Auto-fill available height (default ``true``).
   * - ``padding``
     - float
     - Inner padding in points.
   * - ``weight``
     - float
     - Layout weight for proportional sizing inside ``<Layout>``.
   * - ``position``
     - ``block`` | ``absolute``
     - Positioning mode (default ``block``).
   * - ``z_index``
     - int
     - Stacking order (higher renders on top).
   * - ``visible``
     - bool
     - Whether the node is rendered (default ``true``).

Color Values
------------

Colors can be specified as:

- **Hex**: ``#RRGGBB`` or ``#RRGGBBAA``
- **Named**: ``black``, ``white``, ``red``, ``green``, ``blue``, ``yellow``,
  ``magenta``, ``cyan``, ``purple``
- **Template**: ``${variable}`` or ``${data("field")}``

Page Break
----------

Insert ``<NewPage/>`` to force a manual page break.

.. code-block:: xml

   <Body>
     <Text>Page one content</Text>
     <NewPage/>
     <Text>Page two content</Text>
   </Body>

Blank Line
----------

Insert ``<Blank/>`` to add vertical spacing.

.. code-block:: xml

   <Text>First paragraph</Text>
   <Blank/>
   <Text>Second paragraph</Text>
