Shapes
======

Geometric primitives. Every shape except ``DocraftLoomLine`` derives from
``DocraftLoomShape``, which composes one ``DocraftLoomShapeStyle`` member —
style is composition, not a shared style base class across the node
hierarchy (see :doc:`../about`).

DocraftLoomShapeStyle
-----------------------

Background color, border color, and border width shared by every shape.

.. doxygenstruct:: docraft::loom::nodes::DocraftLoomShapeStyle
   :project: docraft
   :members:

DocraftLoomShape
-------------------

Base class for Circle/Triangle/Polygon/Rectangle (via
``DocraftLoomLayoutContainer``, see :doc:`model_containers`); composes
``DocraftLoomShapeStyle``.

.. doxygenclass:: docraft::loom::nodes::DocraftLoomShape
   :project: docraft
   :members:

DocraftLoomCircle
--------------------

``<Circle>`` — an ellipse held as two semi-axes: ``radius`` sets both (a
circle), ``width``/``height`` inscribe an oval in that bounding box. The
node's frame position is the box's top-left corner, not the center.

.. doxygenclass:: docraft::loom::nodes::DocraftLoomCircle
   :project: docraft
   :members:

DocraftLoomTriangle
----------------------

``<Triangle>`` — exactly 3 points (Y-down offsets from the node's own
origin); throws ``InvalidInputException`` otherwise.

.. doxygenclass:: docraft::loom::nodes::DocraftLoomTriangle
   :project: docraft
   :members:

DocraftLoomPolygon
---------------------

``<Polygon>`` — an arbitrary-point-count shape (needs ≥3 points to render).

.. doxygenclass:: docraft::loom::nodes::DocraftLoomPolygon
   :project: docraft
   :members:

DocraftLoomLine
------------------

``<Line>`` — a stroke-only primitive with ``start``/``end`` points,
``border_color``, and ``border_width``. Not a ``DocraftLoomShape`` (no
background color). Both points are Y-down offsets from the node's own
origin, like ``DocraftLoomTriangle``'s points.

.. doxygenclass:: docraft::loom::nodes::DocraftLoomLine
   :project: docraft
   :members:
