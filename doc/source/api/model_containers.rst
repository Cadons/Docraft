Model — Content Containers
==========================

Complex content nodes that contain children or structured data.

DocraftText
-----------

Text node with font, color, alignment, styling, and optional multi-line
layout via internal line children.

.. doxygenclass:: docraft::model::DocraftText
   :project: docraft
   :members:

DocraftPageNumber
-----------------

Special text node that renders the current page number.

.. doxygenclass:: docraft::model::DocraftPageNumber
   :project: docraft
   :members:

DocraftImage
------------

Image node supporting file-based and raw pixel sources (PNG, JPEG, raw RGB).

.. doxygenclass:: docraft::model::DocraftImage
   :project: docraft
   :members:

DocraftTable
------------

Table node with titles, headers, column/row weights, and cell content.
Supports horizontal and vertical orientations and JSON model binding.

.. doxygenclass:: docraft::model::DocraftTable
   :project: docraft
   :members:

DocraftList
-----------

List node with markers (ordered or unordered) and child items.

.. doxygenclass:: docraft::model::DocraftList
   :project: docraft
   :members:

DocraftLayout
-------------

Layout node that arranges children horizontally or vertically with weights.

.. doxygenclass:: docraft::model::DocraftLayout
   :project: docraft
   :members:

DocraftForeach
--------------

Template loop node that repeats children for each item in a data collection.

.. doxygenclass:: docraft::model::DocraftForeach
   :project: docraft
   :members:

