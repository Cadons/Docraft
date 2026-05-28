Rendering Backend
=================

The backend layer provides abstract interfaces for drawing primitives (text,
shapes, images, pages) and a concrete implementation using libharu for PDF
output.

IDocraftRenderingBackend
------------------------

Root backend interface exposing lifecycle/font operations plus explicit
capability accessors such as ``line_rendering()`` / ``edit_line_rendering()``
and the corresponding text, shape, image, and page accessors.

.. doxygenclass:: docraft::backend::IDocraftRenderingBackend
   :project: docraft
   :members:

Sub-backend Interfaces
----------------------

Capability interfaces are standalone and chain-free.

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

PDF backend implementation using libharu, composed from capability-focused
internal objects behind the root backend.

.. doxygenclass:: docraft::backend::pdf::DocraftHaruBackend
   :project: docraft
   :members:
