Document Structure
==================

A Craft Language file is an XML document whose root element is ``<Document>``.
Inside the root you define three optional **sections** — Header, Body, and
Footer — that map to the top, middle, and bottom areas of every page.

Minimal Example
---------------

.. code-block:: xml

   <Document>
     <Body margin_left="20" margin_right="20">
       <Text font_size="24" style="bold">Hello, Docraft!</Text>
     </Body>
   </Document>

Root Element: ``<Document>``
----------------------------

``<Document>`` carries no attributes of its own — the output path is not
part of the ``.craft`` file at all; it's passed separately, either as the
second CLI argument (``docraft_tool in.craft out.pdf``) or to
``DocraftLoomPdfCreator::render(path)`` from C++ (see
:doc:`../getting_started`). Its recognized children are
``<Header>``/``<Body>``/``<Footer>`` (``<Body>`` required),
``<Settings>`` (:doc:`settings`), and ``<Metadata>`` (:doc:`metadata`); any
other direct child is silently ignored.

Sections
--------

``<Header>``, ``<Body>``, ``<Footer>``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Each section is a rectangle with configurable margins. Content nodes are
placed inside a section and flow top-to-bottom. A ``header_ratio``/
``footer_ratio`` (see :doc:`settings`) only sets a *minimum* reserved
height — if a section's actual content needs more room, it grows instead
of overlapping the body.

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - Attribute
     - Type
     - Description
   * - ``margin_top``
     - float
     - Top margin in points (default ``20``), inset from the page edge (or,
       for the body, from the header).
   * - ``margin_bottom``
     - float
     - Bottom margin in points (default ``20``).
   * - ``margin_left``
     - float
     - Left margin in points (default ``20``).
   * - ``margin_right``
     - float
     - Right margin in points (default ``20``).
   * - ``background_color``
     - color
     - Section background (hex ``#RRGGBB`` or named color).
   * - ``border_color``
     - color
     - Section border color.
   * - ``border_width``
     - float
     - Border width in points.

A section also accepts the common ``padding`` attribute below — unlike
``margin_*`` (space outside the section, from the page/adjacent section),
``padding`` insets the section's own content from *its own* border/background
box, so a bordered ``<Header>`` doesn't render its first line of text flush
against that border.

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
     - Explicit size in points. Only applied to node types that declare a
       setter for it (e.g. ``Rectangle``, ``Image``) — a no-op otherwise
       (e.g. ``Text`` has no ``width``/``height``).
   * - ``padding``
     - float
     - Inner padding in points, between a container's own box and its
       children (default ``10`` for ``Rectangle``, ``<Layout>``/``<Header>``/
       ``<Body>``/``<Footer>`` — anything stacking children vertically or
       horizontally). A no-op for leaf nodes (``Text``, ``Image``,
       ``Circle``, ...), which have no children to inset.
   * - ``margin``
     - float
     - Uniform outer spacing in points, reserved by the parent stack around
       this node. Adjacent margins collapse via ``max()``, not sum (CSS-like).
       Override per edge with ``margin_top``/``margin_right``/
       ``margin_bottom``/``margin_left``.
   * - ``weight``
     - float
     - Proportional sizing share. Only meaningful for a direct child of a
       **horizontal** ``<Layout>`` (see :doc:`layout`) or a table column —
       every other node type ignores it.
   * - ``position``
     - ``block`` | ``absolute``
     - Positioning mode (default ``block``).
   * - ``z_index``
     - int
     - Paint order only (higher paints later/on top); does not affect layout.
   * - ``visible``
     - bool
     - When ``false``, the subtree is never built at all (default ``true``).

.. note::

   ``auto_fill_width``/``auto_fill_height`` and a ``path`` attribute on
   ``<Document>`` are recognized as XML tokens by the parser but are not
   currently read anywhere — they have no effect on the rendered document.

.. _color-values:

Color Values
------------

Colors can be specified as:

- **Hex**: ``#RRGGBB`` or ``#RRGGBBAA``
- **Named**: ``black``, ``white``, ``red``, ``green``, ``blue``, ``yellow``,
  ``magenta``, ``cyan``, ``purple`` — this list is exhaustive; any other name
  (``orange``, ``grey``, ...) is a parse error, use a hex value instead
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
