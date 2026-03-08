Renderer
========

The renderer translates model nodes into backend draw calls. A concrete
renderer (e.g. PDF) dispatches each node type to the appropriate painter.

DocraftAbstractRenderer
-----------------------

Abstract base with a virtual method per node type.

.. doxygenclass:: docraft::renderer::DocraftAbstractRenderer
   :project: docraft
   :members:

DocraftPDFRenderer
------------------

Concrete renderer targeting the PDF backend.

.. doxygenclass:: docraft::renderer::DocraftPDFRenderer
   :project: docraft
   :members:

