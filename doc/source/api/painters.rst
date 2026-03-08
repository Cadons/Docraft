Painters
========

Painters are lightweight classes that know how to draw a single node type
using the document context's rendering backend. Each painter implements
the ``IPainter`` interface.

IPainter
--------

.. doxygenclass:: docraft::renderer::painter::IPainter
   :project: docraft
   :members:

DocraftTextPainter
------------------

Handles alignment, justification, and underline rendering for text nodes.

.. doxygenclass:: docraft::renderer::painter::DocraftTextPainter
   :project: docraft
   :members:

DocraftRectanglePainter
-----------------------

Renders background and border for rectangle nodes.

.. doxygenclass:: docraft::renderer::painter::DocraftRectanglePainter
   :project: docraft
   :members:

DocraftCirclePainter
--------------------

.. doxygenclass:: docraft::renderer::painter::DocraftCirclePainter
   :project: docraft
   :members:

DocraftTrianglePainter
----------------------

.. doxygenclass:: docraft::renderer::painter::DocraftTrianglePainter
   :project: docraft
   :members:

DocraftLinePainter
------------------

.. doxygenclass:: docraft::renderer::painter::DocraftLinePainter
   :project: docraft
   :members:

DocraftPolygonPainter
---------------------

.. doxygenclass:: docraft::renderer::painter::DocraftPolygonPainter
   :project: docraft
   :members:

DocraftImagePainter
-------------------

Supports file-based and in-memory image sources.

.. doxygenclass:: docraft::renderer::painter::DocraftImagePainter
   :project: docraft
   :members:

DocraftTablePainter
-------------------

Draws headers, grid lines, and cell content.

.. doxygenclass:: docraft::renderer::painter::DocraftTablePainter
   :project: docraft
   :members:

docraft_blank_line_painter
--------------------------

Advances layout for blank lines (no drawing).

.. doxygenclass:: docraft::renderer::painter::docraft_blank_line_painter
   :project: docraft
   :members:

