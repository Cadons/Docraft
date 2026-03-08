Model — Types & Enums
=====================

Value types, enumerations, and small structs used throughout the model layer.

DocraftColor
------------

Color helper supporting named colors, RGB components, hex codes, and template
expressions.

.. doxygenclass:: docraft::DocraftColor
   :project: docraft
   :members:

RGB
^^^

.. doxygenstruct:: docraft::RGB
   :project: docraft
   :members:

DocraftCursor
-------------

Tracks the current 2D position for flow layout with a direction stack.

.. doxygenclass:: docraft::DocraftCursor
   :project: docraft
   :members:

DocraftDocumentMetadata
-----------------------

Metadata container for PDF document information (author, title, dates, etc.).
Includes the nested ``DateTime`` struct for creation/modification dates.

.. doxygenclass:: docraft::DocraftDocumentMetadata
   :project: docraft
   :members:

DocraftPoint
------------

.. doxygenstruct:: docraft::model::DocraftPoint
   :project: docraft
   :members:

DocraftAnchor
-------------

.. doxygenstruct:: docraft::model::DocraftAnchor
   :project: docraft
   :members:

Enumerations
------------

.. doxygenenum:: docraft::ColorName
   :project: docraft

.. doxygenenum:: docraft::DocraftCursorDirection
   :project: docraft

.. doxygenenum:: docraft::model::ImageFormat
   :project: docraft

.. doxygenenum:: docraft::model::LayoutOrientation
   :project: docraft

.. doxygenenum:: docraft::model::ListKind
   :project: docraft

.. doxygenenum:: docraft::model::OrderedListStyle
   :project: docraft

.. doxygenenum:: docraft::model::UnorderedListDot
   :project: docraft

.. doxygenenum:: docraft::model::DocraftPageSize
   :project: docraft

.. doxygenenum:: docraft::model::DocraftPageOrientation
   :project: docraft

.. doxygenenum:: docraft::model::DocraftPositionType
   :project: docraft

.. doxygenenum:: docraft::model::TextStyle
   :project: docraft

.. doxygenenum:: docraft::model::TextAlignment
   :project: docraft


