Layout Engine
=============

The layout engine computes position and size for every node in the document
tree. It uses a chain-of-responsibility pattern: specialized handlers claim
specific node types, while a fallback handler covers generic nodes.

DocraftLayoutEngine
-------------------

Main engine that walks the DOM and delegates box computation to handlers.

.. doxygenclass:: docraft::layout::DocraftLayoutEngine
   :project: docraft
   :members:

AbstractDocraftLayoutHandler
----------------------------

Base template for all layout handlers.

.. doxygenclass:: docraft::layout::handler::AbstractDocraftLayoutHandler
   :project: docraft
   :members:

DocraftBasicLayoutHandler
-------------------------

Fallback handler for generic nodes.

.. doxygenclass:: docraft::layout::handler::DocraftBasicLayoutHandler
   :project: docraft
   :members:

DocraftLayoutTextHandler
------------------------

Handles text measurement, alignment, and line-breaking.

.. doxygenclass:: docraft::layout::handler::DocraftLayoutTextHandler
   :project: docraft
   :members:

DocraftLayoutTableHandler
-------------------------

Calculates cell boxes and header/content areas.

.. doxygenclass:: docraft::layout::handler::DocraftLayoutTableHandler
   :project: docraft
   :members:

DocraftLayoutListHandler
------------------------

Computes marker positions and list item content.

.. doxygenclass:: docraft::layout::handler::DocraftLayoutListHandler
   :project: docraft
   :members:

DocraftLayoutHandler
--------------------

Handler for ``DocraftLayout`` nodes (horizontal/vertical children).

.. doxygenclass:: docraft::layout::handler::DocraftLayoutHandler
   :project: docraft
   :members:

DocraftLayoutBlankLine
----------------------

Advances the cursor for blank-line spacing.

.. doxygenclass:: docraft::layout::handler::DocraftLayoutBlankLine
   :project: docraft
   :members:

