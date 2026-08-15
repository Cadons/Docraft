Tables
======

The ``<Table>`` element renders tabular data with titles, rows, and cells.
Tables support horizontal and vertical orientations, per-cell/row backgrounds,
and JSON model binding for data-driven content.

.. note::

   Inside tables, ``<Title>`` is not valid.
   Use ``<HTitle>`` for horizontal table headers and ``<VTitle>`` for vertical table titles.

.. note::

   Column weighting exists on the underlying ``DocraftLoomTable`` node
   (``set_column_weights()``), but is not yet exposed as a Craft Language XML attribute —
   there is currently no way to set it from a ``.craft`` file.

.. note::

   A ``<Cell>``'s content must be a ``<Text>`` (or ``<Title>``/``<Subtitle>``/
   ``<PageNumber>``) or an ``<Image>`` — anything else is a parse error.
   ``position="absolute"`` on a cell or its content is also rejected
   (``InvalidInputException`` at layout time); table cells only support
   normal flow positioning.

Basic Table (Horizontal)
------------------------

.. code-block:: xml

   <Table>
     <THead>
       <HTitle font_size="11" style="bold">Name</HTitle>
       <HTitle font_size="11" style="bold">Price</HTitle>
     </THead>
     <TBody>
       <Row>
         <Cell><Text>Widget A</Text></Cell>
         <Cell><Text>$9.99</Text></Cell>
       </Row>
       <Row>
         <Cell><Text>Widget B</Text></Cell>
         <Cell><Text>$14.50</Text></Cell>
       </Row>
     </TBody>
   </Table>

Basic Table (Vertical)
----------------------

Vertical orientation is not a separate attribute — it's selected by setting
``model="vertical"`` on ``<Table>`` itself (the ``model`` attribute is
overloaded: ``"vertical"``/``"horizontal"`` pick an orientation directly,
any other value is treated as a JSON/template data model, see
"Data-Driven Tables" below). ``<THead>`` is mandatory
for horizontal tables without a JSON model; a vertical table has no
``<THead>`` at all — every row supplies its own ``<VTitle>`` instead.

.. code-block:: xml

   <Table model="vertical">
     <TBody>
       <Row>
         <VTitle font_size="11" style="bold">Name</VTitle>
         <Cell><Text>Widget A</Text></Cell>
       </Row>
       <Row>
         <VTitle font_size="11" style="bold">Price</VTitle>
         <Cell><Text>$9.99</Text></Cell>
       </Row>
     </TBody>
   </Table>

.. note::

   ``<VTitle>`` is mandatory on every row of a vertical table — a row
   missing it is a parse error. JSON/template ``model`` data binding is not
   supported with vertical orientation; build vertical tables by hand.

Attributes
----------

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - Attribute
     - Type
     - Description
   * - ``model``
     - string
     - Overloaded: literal ``"vertical"``/``"horizontal"`` picks the table's
       orientation explicitly; any other value is a ``${variable}``
       reference to JSON data (matrix or object array, horizontal only).
   * - ``baseline_offset``
     - float
     - Vertical offset for text alignment within cells (default ``0.25``).

Header Title Attributes
^^^^^^^^^^^^^^^^^^^^^^^

Each ``<HTitle>`` in horizontal tables and each ``<VTitle>`` in vertical
tables supports:

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - Attribute
     - Type
     - Description
   * - ``alignment``
     - string
     - ``left`` | ``center`` | ``right`` | ``justified``.
   * - ``style``
     - string
     - ``normal`` | ``bold`` | ``italic`` | ``bold_italic``.
   * - ``font_size``
     - float
     - Title font size in points. Must be ``> 0``. Omitted, the title keeps the
       default text size.
   * - ``color``
     - color
     - Title text color.
   * - ``background_color``
     - color
     - Title cell background.

Row Attributes
^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - Attribute
     - Type
     - Description
   * - ``background_color``
     - color
     - Row background color.

Data-Driven Tables
------------------

Tables can bind to JSON data via a template variable:

.. code-block:: xml

   <Table model="${products}">
     <THead>
       <HTitle style="bold">Name</HTitle>
       <HTitle style="bold">Price</HTitle>
     </THead>
   </Table>

The ``model`` attribute accepts:

- **String matrix**: ``[["A","$10"],["B","$20"]]``
- **Object array**: ``[{"name":"A","price":"$10"}]`` — column order matches
  header order.

The header can also be bound with a template variable using ``model``
on ``<THead>``.

Pagination
----------

A table that doesn't fit in the space remaining on a page splits across
pages automatically — you don't need (and can't use) ``<NewPage/>`` inside a
``<Table>``, splitting is entirely automatic.

- **Whole rows first.** As many complete rows as fit stay on the current
  page; the rest continue as a new table at the top of the next page.
- **Header rows repeat.** A leading run of rows whose cells are *all*
  title cells (``<HTitle>`` in a horizontal table, ``<VTitle>`` in a
  vertical one) is treated as the table's header and is cloned onto every
  continuation page, so readers always see the column/row titles.
- **An oversized row splits by content, not just by row.** If a single row
  — most commonly the first content row right after a repeated header —
  is by itself taller than a whole page (typically a cell with a lot of
  wrapped text), Docraft splits that row's text content: as many wrapped
  lines as fit stay on the current page, and the remaining lines continue
  as the same row on the next page, rather than pushing the whole row
  forward indefinitely.
- **Non-text content sets a hard limit.** Only ``<Text>`` cell content can
  be split line-by-line this way. A row that doesn't fit and contains a
  cell whose content isn't splittable (e.g. an ``<Image>`` too tall for one
  page on its own) is rendered as-is, extending past the page's bottom
  margin, instead of being pushed to an endless sequence of new pages.

.. note::

   Every cell in a row always shares that row's height (the tallest cell in
   the row), including the continuation row created by a content split —
   a column that finished its own text early is simply left blank on the
   continuation rather than repeating already-shown content.
