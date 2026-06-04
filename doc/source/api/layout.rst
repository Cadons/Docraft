Layout Engine
=============

The layout engine computes position and size for every node in the document
tree. It uses a chain-of-responsibility pattern: specialized handlers claim
specific node types, while a fallback handler covers generic nodes.

How It Works
------------

The public class ``DocraftLayoutEngine`` is a facade over an internal
``DocraftLayoutEngine::Impl``. The split keeps the public header stable while
hiding implementation details (handlers, pagination internals, section plan,
helper state).

Execution model:

1. ``compute_document_layout(...)`` splits top-level sections (Header/Body/Footer).
2. A section plan is computed from navigation ratios and section visibility.
3. Header is laid out (if visible), then Body, then Footer.
4. Body layout applies pagination rules and assigns ``page_owner`` recursively.

Node-level model:

- ``compute_layout(node, cursor)`` evaluates visibility and positioning mode.
- Flow nodes advance the cursor; absolute nodes are laid out independently.
- Container nodes recursively layout children, then handlers compute final box.
- Cursor spacing is adjusted with fixed horizontal/vertical spacing rules.

Pagination model in body:

- ``NewPage`` forces page advance.
- Overflowing non-absolute nodes are re-laid on a new page.
- Tables are split when only a subset of rows fits in remaining body space.

Implementation files
--------------------

- Public API: ``docraft/include/docraft/layout/docraft_layout_engine.h``
- Facade: ``docraft/src/docraft/layout/docraft_layout_engine.cc``
- Private impl declaration: ``docraft/src/docraft/layout/docraft_layout_engine_impl.h``
- Private impl definition: ``docraft/src/docraft/layout/docraft_layout_engine_impl.cc``

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
