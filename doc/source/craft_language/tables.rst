Tables
======

The ``<Table>`` element renders tabular data with titles, rows, and cells.
Tables support horizontal and vertical orientations, column/row weights,
and JSON model binding for data-driven content.

.. note::

   Inside tables, ``<Title>`` is not valid.
   Use ``<HTitle>`` for horizontal table headers and ``<VTitle>`` for vertical table titles.

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

.. code-block:: xml

   <Table>
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
     - Template variable containing JSON data (matrix or object array).
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
