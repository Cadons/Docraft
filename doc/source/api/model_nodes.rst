Model — Core Nodes
==================

Base node classes that form the document tree. Every visual element in a
Docraft document inherits from ``DocraftNode``.

DocraftNode
-----------

Base class for all document nodes carrying position, size, weight, and
visibility properties.

.. doxygenclass:: docraft::model::DocraftNode
   :project: docraft
   :members:

DocraftTransform
----------------

Position and size transform with cached anchor points.

.. doxygenclass:: docraft::model::DocraftTransform
   :project: docraft
   :members:

DocraftChildrenContainerNode
----------------------------

Node that owns and manages a list of child nodes. Used by composite nodes
such as rectangles, layouts, lists, and text.

.. doxygenclass:: docraft::model::DocraftChildrenContainerNode
   :project: docraft
   :members:

DocraftSection
--------------

Rectangular document section with margin configuration. Base for header,
body, and footer.

.. doxygenclass:: docraft::model::DocraftSection
   :project: docraft
   :members:

DocraftHeader
-------------

.. doxygenclass:: docraft::model::DocraftHeader
   :project: docraft
   :members:

DocraftBody
-----------

.. doxygenclass:: docraft::model::DocraftBody
   :project: docraft
   :members:

DocraftFooter
-------------

.. doxygenclass:: docraft::model::DocraftFooter
   :project: docraft
   :members:

DocraftSettings
---------------

Document-level settings node (page format, fonts).

.. doxygenclass:: docraft::model::DocraftSettings
   :project: docraft
   :members:

DocraftNewPage
--------------

Manual page break node.

.. doxygenclass:: docraft::model::DocraftNewPage
   :project: docraft
   :members:

DocraftBlankLine
----------------

Blank line node for vertical spacing.

.. doxygenclass:: docraft::model::DocraftBlankLine
   :project: docraft
   :members:

DocraftParagraph
----------------

Paragraph placeholder node.

.. doxygenclass:: docraft::model::DocraftParagraph
   :project: docraft
   :members:

IDocraftClonable
----------------

Interface for nodes that support deep cloning.

.. doxygenclass:: docraft::model::IDocraftClonable
   :project: docraft
   :members:

