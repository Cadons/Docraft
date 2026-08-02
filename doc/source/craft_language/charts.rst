Charts
======

``<Chart>`` draws a data visualization -- scatter, spline, line, histogram, or pie --
directly on the page, without shelling out to an external plotting library. Internally
it's built on top of ``<Canvas>`` (see :doc:`shapes`): each chart style is a small
"plugin" that fills a canvas with ordinary primitive nodes (lines, circles, rectangles,
text), so nothing renderer-side needs to know about charts at all.

A chart always needs explicit ``width``/``height`` and a ``style``, and contains one or
more ``<Series>`` children supplying its data:

.. code-block:: xml

   <Chart style="scatter" width="300" height="200" title="My Chart"
          x_label="X" y_label="Y">
     <Series name="A" color="blue" model='[[1,2],[3,4]]' />
   </Chart>

``<Chart>`` Attributes
----------------------

In addition to the common node attributes (see :doc:`structure`), ``<Chart>`` accepts:

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - Attribute
     - Type
     - Description
   * - ``style``
     - string
     - **Required.** One of ``scatter``, ``spline``, ``line``, ``histogram``, ``pie``.
       An unrecognized value is a parse error.
   * - ``width``, ``height``
     - float
     - **Required.** Pixel size of the chart's canvas.
   * - ``title``
     - string
     - Optional chart title, centered above the plot.
   * - ``x_label``, ``y_label``
     - string
     - Optional axis labels. Ignored by ``pie`` (no axes).
   * - ``axis_position``
     - string
     - Where the axis lines cross: ``left`` (default), ``right``, ``top-left``,
       ``top-right``, ``bottom-left``, ``bottom-right``, or ``center``. Ignored by
       ``pie``.
   * - ``show_percentage``
     - bool
     - ``pie`` only: whether each slice draws its share of the total as a centered
       percentage label (default ``true``).

``<Series>`` Attributes
------------------------

Each ``<Chart>`` contains one or more ``<Series>`` elements supplying its data:

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - Attribute
     - Type
     - Description
   * - ``name``
     - string
     - Series label, shown in the legend (and, for ``pie``, used as a slice's label
       when the point itself doesn't carry one).
   * - ``color``
     - color
     - Series color. When omitted, a color is picked from a fixed palette, cycled by
       series index.
   * - ``model``
     - JSON
     - **Required.** The series' data points, in one of two shapes (see below).

A ``model`` is a JSON array whose entries are either:

- **Coordinate pairs** -- ``[x, y]`` or ``{"x": ..., "y": ...}`` -- for continuous data
  (scatter, spline, line), or
- **Labeled values** -- ``{"label": value}`` -- for categorical data (histogram, pie).
  Each entry becomes one point at ``x`` = its ordinal position in the array, ``y`` =
  the value, with the key kept as that point's category label (used for the X-axis
  tick in a histogram, or the slice label in a pie chart).

Data can also come from ``${...}`` template substitution, the same as any other
``model`` attribute (see :doc:`templating`).

Chart Styles
------------

Scatter
~~~~~~~

``style="scatter"`` plots one dot per data point, unconnected -- for showing the
relationship (or lack of one) between two continuous variables across one or more
series.

.. code-block:: xml

   <Chart style="scatter" width="450" height="280" title="Height vs. Weight"
          x_label="Height (cm)" y_label="Weight (kg)">
     <Series name="Group A" color="#1F77B4" model='[[160,55],[165,60],[170,68]]' />
     <Series name="Group B" color="#D62728" model='[[162,50],[168,58],[172,63]]' />
   </Chart>

.. image:: ../_static/charts/scatter.png
   :alt: Scatter chart example with two series
   :width: 500px

Spline
~~~~~~

``style="spline"`` draws a smooth, interpolated curve through each series' points
(plus a marker dot at every point) -- for a trend that should read as continuous
rather than piecewise-linear.

.. code-block:: xml

   <Chart style="spline" width="450" height="280" title="Monthly Active Users"
          x_label="Month" y_label="Users (k)">
     <Series name="2025" color="#2CA02C" model='[[1,12],[2,15],[3,14],[4,20],[5,26]]' />
   </Chart>

.. image:: ../_static/charts/spline.png
   :alt: Spline chart example
   :width: 500px

Line
~~~~

``style="line"`` connects each series' points with straight segments (plus a marker
dot at every point) -- the same point model as ``spline``, differing only in how
points are connected.

.. code-block:: xml

   <Chart style="line" width="450" height="280" title="Quarterly Revenue"
          x_label="Quarter" y_label="Revenue ($k)">
     <Series name="Product X" color="#9467BD" model='[[1,40],[2,55],[3,50],[4,70]]' />
     <Series name="Product Y" color="#FF7F0E" model='[[1,25],[2,30],[3,45],[4,42]]' />
   </Chart>

.. image:: ../_static/charts/line.png
   :alt: Line chart example with two series
   :width: 500px

Histogram
~~~~~~~~~

``style="histogram"`` draws grouped bars, one per data point, grouped side-by-side by
series at each shared category -- for comparing values across categories and, when
there's more than one series, across groups within each category. Use the
``{"label": value}`` model shape to get named categories on the X axis:

.. code-block:: xml

   <Chart style="histogram" width="450" height="280" title="Sales by Region"
          y_label="Units Sold">
     <Series name="Q1" color="#1F77B4" model='[{"North":30},{"South":45},{"East":25}]' />
     <Series name="Q2" color="#FF7F0E" model='[{"North":36},{"South":40},{"East":30}]' />
   </Chart>

.. image:: ../_static/charts/histogram.png
   :alt: Histogram chart example with two series
   :width: 500px

.. note::

   The Y axis auto-zooms to the actual data range rather than always starting at
   zero (visible above: the axis starts at 25, not 0) -- so small differences
   between bars stay readable. A bar never sits flush against a non-zero baseline;
   a small gap always separates it from the axis line.

Pie
~~~

``style="pie"`` draws one slice per data point across every ``<Series>``, sized by its
share of the total. Unlike the other styles, a pie chart has no axes at all. Use the
``{"label": value}`` model shape for a labeled category breakdown:

.. code-block:: xml

   <Chart style="pie" width="450" height="280" title="Market Share">
     <Series model='[{"Alpha":35},{"Beta":25},{"Gamma":20},{"Delta":12},{"Epsilon":8}]' />
   </Chart>

.. image:: ../_static/charts/pie.png
   :alt: Pie chart example
   :width: 500px

A series with a single point (one ``<Series>`` per slice) keeps that series' own
``color``; a series with multiple points (one ``<Series>`` holding a whole category
breakdown, as above) cycles each of its slices through the default palette instead,
since a single series color can't distinguish them. Points with a non-positive value
are skipped.
