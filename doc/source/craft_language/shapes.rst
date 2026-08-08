Shapes
======

Docraft supports several geometric shape elements. All shapes share
``background_color``, ``border_color``, and ``border_width`` attributes.

Rectangle
---------

``<Rectangle>`` draws a box and can contain children rendered on top.

.. code-block:: xml

   <Rectangle width="200" height="100"
              background_color="#E0E0E0"
              border_color="black" border_width="1">
     <Text>Inside the box</Text>
   </Rectangle>

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - Attribute
     - Type
     - Description
   * - ``background_color``
     - color
     - Fill color.
   * - ``border_color``
     - color
     - Stroke color.
   * - ``border_width``
     - float
     - Stroke width in points (default ``1``).

Circle
------

``<Circle>`` draws a circle based on the node's bounding box.

.. code-block:: xml

   <Circle width="50" height="50"
           background_color="blue" border_color="black"/>

Attributes are the same as ``<Rectangle>``.

Triangle
--------

``<Triangle>`` is defined by three points in local coordinates.

.. code-block:: xml

   <Triangle points="0,0 50,100 100,0"
             background_color="red" border_color="black"/>

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - Attribute
     - Type
     - Description
   * - ``points``
     - string
     - Three points as ``x1,y1 x2,y2 x3,y3``.
   * - ``background_color``
     - color
     - Fill color.
   * - ``border_color``
     - color
     - Stroke color.
   * - ``border_width``
     - float
     - Stroke width in points.

Line
----

``<Line>`` draws a line between two points.

.. code-block:: xml

   <Line x1="0" y1="0" x2="200" y2="0"
         border_color="black" border_width="0.5"/>

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - Attribute
     - Type
     - Description
   * - ``x1``, ``y1``
     - float
     - Start point.
   * - ``x2``, ``y2``
     - float
     - End point.
   * - ``border_color``
     - color
     - Stroke color.
   * - ``border_width``
     - float
     - Stroke width in points.

Canvas
------

``<Canvas>`` is a free-form graphics container: unlike every other container node,
its children are not block-stacked -- each positions itself by its own ``x``/``y``,
relative to the canvas's own top-left origin, and painting is clipped to the canvas's
bounds. It's the building block :doc:`charts` are drawn on top of.

.. code-block:: xml

   <Canvas width="200" height="150" background_color="#F5F5F5">
     <Circle x="20" y="20" width="30" height="30" background_color="blue"/>
     <Line x1="0" y1="100" x2="200" y2="100" border_color="black"/>
   </Canvas>

``width``/``height`` are required. Otherwise accepts the same
``background_color``/``border_color``/``border_width`` attributes as ``<Rectangle>``.

.. _craft-curveline:

CurveLine
---------

``<CurveLine>`` draws an **open** curve passing through a series of points — the
curved counterpart of ``<Line>``, and what a spline chart's series line is made of.

.. code-block:: xml

   <CurveLine points="0,60 40,10 80,60 120,10 160,60"
              border_color="green" border_width="1.5"/>

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - Attribute
     - Type
     - Description
   * - ``points``
     - string
     - Space-separated ``x,y`` pairs, as Y-down offsets from the node's own origin.
       At least 2 are required; fewer is a parse error.
   * - ``border_color``
     - color
     - Stroke color.
   * - ``border_width``
     - float
     - Stroke width in points.

The curve **interpolates**: it passes exactly through every point rather than near
them. With exactly 2 points it degenerates to a straight segment, which is why 2 is a
legal count here where a closed ``<Polygon>`` needs 3.

There is no ``background_color``: an open curve has no interior to fill. For a filled
shape use ``<Polygon>``.

.. note::

   On a sharp change of direction the interpolation can bow slightly outside the
   straight path between two points. Where that overshoot would misrepresent the data
   — a quantity that cannot go below zero, say — use ``<Polygon>`` or a series of
   ``<Line>`` segments instead.

Polygon
-------

``<Polygon>`` draws an arbitrary closed polygon. It is always closed and fillable; for
an open curve through the same points see :ref:`CurveLine <craft-curveline>` above.

.. code-block:: xml

   <Polygon points="0,0 100,0 120,60 50,100 -20,60"
            background_color="green" border_color="black"/>

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - Attribute
     - Type
     - Description
   * - ``points``
     - string
     - Space-separated ``x,y`` pairs.
   * - ``background_color``
     - color
     - Fill color.
   * - ``border_color``
     - color
     - Stroke color.
   * - ``border_width``
     - float
     - Stroke width in points.

