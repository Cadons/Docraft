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

Polygon
-------

``<Polygon>`` draws an arbitrary closed polygon.

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

