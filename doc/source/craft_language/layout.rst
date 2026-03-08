Layout
======

The ``<Layout>`` element arranges its children either horizontally or
vertically. Children can be weighted to control how available space is
distributed.

.. note::

   The ``weight`` attribute is valid only in the inclusive range ``0`` to ``1``.
   Values greater than ``1`` are invalid.

Example — Horizontal Layout
----------------------------

.. code-block:: xml

   <Layout orientation="horizontal">
     <Text weight="0.25">Left column</Text>
     <Text weight="0.50">Center column (twice as wide)</Text>
     <Text weight="0.25">Right column</Text>
   </Layout>

Example — Vertical Layout
--------------------------

.. code-block:: xml

   <Layout orientation="vertical">
     <Text weight="0.60">Top row</Text>
     <Text weight="0.40">Bottom row</Text>
   </Layout>

Attributes
----------

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - Attribute
     - Type
     - Description
   * - ``orientation``
     - string
     - ``horizontal`` or ``vertical`` (default ``horizontal``).

Child nodes inside a layout can use the ``weight`` attribute to control
proportional sizing. If no weights are set, children share space equally.
