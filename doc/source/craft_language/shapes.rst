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

``<Circle>`` draws a circle or an oval. It is sized in one of **two mutually exclusive
ways**:

* ``radius`` -- draws a circle of that radius;
* ``width`` **and** ``height`` -- inscribes an oval in that bounding box, so its four
  extreme points touch the middle of each side of the box. ``width == height`` gives a
  circle again.

.. code-block:: xml

   <Circle radius="25" background_color="blue" border_color="black"/>
   <Circle width="120" height="60" background_color="blue"/>   <!-- oval -->

The node's ``x``/``y`` are the **top-left corner of that bounding box**, not the center;
the center is ``(x + width/2, y + height/2)``.

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - Attribute
     - Type
     - Description
   * - ``radius``
     - float
     - Circle radius. Must be ``> 0``. Cannot be combined with ``width``/``height``.
   * - ``width``, ``height``
     - float
     - Bounding box the oval is inscribed in. Both must be given together and be ``> 0``.
       Cannot be combined with ``radius``.
   * - ``background_color``
     - color
     - Fill color.
   * - ``border_color``
     - color
     - Stroke color.
   * - ``border_width``
     - float
     - Stroke width in points.

The two sizing methods are exclusive on purpose, and every violation is a parse error
rather than a silently misdrawn shape:

* ``radius`` together with ``width`` and/or ``height`` -- ambiguous, rejected;
* only one of ``width``/``height`` -- half a bounding box, rejected;
* no sizing attribute at all -- rejected (this used to draw nothing at all).

Arcs
~~~~

Adding ``start_x``/``start_y`` and ``finish_x``/``finish_y`` draws only the **arc**
between those two points instead of the whole outline — a semicircle, a quarter, any
slice.

.. code-block:: xml

   <!-- upper semicircle: 9 o'clock round to 3 o'clock -->
   <Circle radius="50" start_x="0" start_y="50" finish_x="100" finish_y="50"
           border_color="red" border_width="2"/>

The endpoints are points in the circle's own box, like every other coordinate in
Docraft. Two rules make them unambiguous:

* **Only the direction from the centre matters.** The distance comes from the circle's
  own radius, so an endpoint that doesn't land exactly on the outline is projected onto
  it rather than being an error — handy, since a point that lies *precisely* on a
  circle rarely has round coordinates.
* **The arc is always swept clockwise from start to finish.** Two points have two arcs
  between them; swapping ``start`` and ``finish`` gives you the other one.

.. list-table::
   :header-rows: 1
   :widths: 25 15 60

   * - Attribute
     - Type
     - Description
   * - ``start_x``, ``start_y``
     - float
     - Where the arc begins. Required together with the finish pair.
   * - ``finish_x``, ``finish_y``
     - float
     - Where the arc ends, sweeping clockwise from the start.

Rejected combinations:

* only some of the four arc attributes — an arc needs both endpoints;
* an arc together with ``background_color`` — an arc is an open path and cannot be
  filled; use ``<Polygon>`` for a filled sector;
* an arc on an oval (``width`` and ``height`` differing) — arcs are supported on
  circles only.

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

The endpoints are **offsets from the line's own anchor**, exactly like the local points
of a ``<Triangle>``/``<Polygon>``. That anchor is:

* inside a :ref:`Canvas <craft-canvas>`, the canvas's top-left origin plus the line's own
  ``x``/``y`` (both default to ``0``) -- so ``x1``/``y1``/``x2``/``y2`` read directly as
  canvas coordinates;
* in normal block flow, wherever the layout cursor currently is.

Since both endpoints are placed independently, an offset shared by the two (a horizontal
rule partway down a canvas, ``y1="75" y2="75"``) moves the whole segment instead of
cancelling out. Negative coordinates draw above/left of the anchor; inside a ``<Canvas>``
they are clipped at its bounds.

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - Attribute
     - Type
     - Description
   * - ``x1``, ``y1``
     - float
     - Start point, relative to the line's anchor.
   * - ``x2``, ``y2``
     - float
     - End point, relative to the line's anchor.
   * - ``border_color``
     - color
     - Stroke color.
   * - ``border_width``
     - float
     - Stroke width in points.

.. _craft-canvas:

Canvas
------

``<Canvas>`` is a free-form graphics container: unlike every other container node,
its children are not block-stacked -- each positions itself by its own ``x``/``y``,
relative to the canvas's own top-left origin, and painting is clipped to the canvas's
bounds. It's the building block :doc:`charts` are drawn on top of.

The origin is the canvas's **top-left** corner and ``y`` grows **downward**, so the
center of a ``200x150`` canvas is ``(100, 75)``.

.. code-block:: xml

   <Canvas width="200" height="150" background_color="#F5F5F5">
     <!-- circle centered on the canvas: x/y are the bounding box's top-left corner -->
     <Circle x="80" y="55" radius="20" background_color="blue"/>
     <!-- an oval inscribed in a 60x30 box -->
     <Circle x="10" y="10" width="60" height="30" border_color="black"/>
     <!-- horizontal rule halfway down -->
     <Line x1="0" y1="75" x2="200" y2="75" border_color="black"/>
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

