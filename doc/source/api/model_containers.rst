Containers
==========

Structural nodes that arrange children — stacks, tables, and lists. Note
there is no ``DocraftLoomForeach`` node: ``<Foreach>`` is expanded away
during tree building (see :doc:`craft_parser`), not represented in the node
tree at all.

DocraftLoomLayoutContainer
-----------------------------

Shared base for Rectangle/VStack/HStack — factors out ``spacing()`` and the
CSS-margin-collapsing gap resolution between adjacent children (see
``DocraftLoomNode::margin()`` in :doc:`model_nodes`).

.. doxygenclass:: docraft::loom::nodes::DocraftLoomLayoutContainer
   :project: docraft
   :members:

DocraftLoomRectangle
-----------------------

``<Rectangle>`` — a ``DocraftLoomLayoutContainer`` with explicit
``width``/``height`` that stacks children top-to-bottom, plus
background/border/padding (via ``DocraftLoomShape``, see :doc:`model_shapes`).

.. doxygenclass:: docraft::loom::nodes::DocraftLoomRectangle
   :project: docraft
   :members:

DocraftLoomVStack / DocraftLoomHStack
------------------------------------------

``<Layout orientation="vertical"|"horizontal">`` — stack children top-to-
bottom or left-to-right. Only ``HStack`` supports ``weights`` (proportional
column sizing); a vertical ``Layout``'s ``weights`` attribute is a no-op.

.. doxygenclass:: docraft::loom::nodes::DocraftLoomVStack
   :project: docraft
   :members:

.. doxygenclass:: docraft::loom::nodes::DocraftLoomHStack
   :project: docraft
   :members:

DocraftLoomTable / DocraftLoomTableCell
----------------------------------------------

``<Table>`` — a grid of cells with column weights, optional per-cell
background, title cells (painted last, over dividers), and
``split_after_row()`` support for pagination across pages. Cell content must
be ``DocraftLoomText`` or ``DocraftLoomImage``; absolute positioning inside a
cell throws ``InvalidInputException`` at layout time.

.. doxygenclass:: docraft::loom::nodes::DocraftLoomTable
   :project: docraft
   :members:

.. doxygenclass:: docraft::loom::nodes::DocraftLoomTableCell
   :project: docraft
   :members:

DocraftLoomList
------------------

``<List>``/``<UList>`` — ordered (number/roman) or unordered (dash/star/
circle/box) markers; items are plain ``DocraftLoomText`` children.

.. doxygenclass:: docraft::loom::nodes::DocraftLoomList
   :project: docraft
   :members:
