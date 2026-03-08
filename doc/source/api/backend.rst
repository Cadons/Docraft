Rendering Backend
=================

The backend layer provides abstract interfaces for drawing primitives (text,
shapes, images, pages) and a concrete implementation using libharu for PDF
output.

IDocraftRenderingBackend
------------------------

Aggregated interface inheriting all sub-backend interfaces.

.. doxygenclass:: docraft::backend::IDocraftRenderingBackend
   :project: docraft
   :members:

Sub-backend Interfaces
----------------------

IDocraftTextRenderingBackend
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenclass:: docraft::backend::IDocraftTextRenderingBackend
   :project: docraft
   :members:

IDocraftShapeRenderingBackend
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenclass:: docraft::backend::IDocraftShapeRenderingBackend
   :project: docraft
   :members:

IDocraftLineRenderingBackend
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenclass:: docraft::backend::IDocraftLineRenderingBackend
   :project: docraft
   :members:

IDocraftImageRenderingBackend
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenclass:: docraft::backend::IDocraftImageRenderingBackend
   :project: docraft
   :members:

IDocraftPageRenderingBackend
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenclass:: docraft::backend::IDocraftPageRenderingBackend
   :project: docraft
   :members:

Concrete Backends
-----------------

DocraftHaruBackend
^^^^^^^^^^^^^^^^^^

PDF backend implementation using libharu.

.. doxygenclass:: docraft::backend::pdf::DocraftHaruBackend
   :project: docraft
   :members:

