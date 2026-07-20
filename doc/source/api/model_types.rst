Types & Enums
=============

Value types and enumerations shared across the loom pipeline, plus a few
shared, pipeline-agnostic types used everywhere colors/positions appear.

Shared Geometry & Color
------------------------

``docraft::Position`` is aliased directly as ``docraft::loom::nodes::Position``
(see :doc:`model_nodes`) so loom nodes, the cursor, and backend rendering
interfaces all share one point type.

.. doxygenstruct:: docraft::Position
   :project: docraft
   :members:

DocraftColor
^^^^^^^^^^^^

Color helper supporting named colors, RGB components, hex codes, and template
expressions (``${variable}``/``${data("field")}``).

.. doxygenclass:: docraft::DocraftColor
   :project: docraft
   :members:

.. doxygenstruct:: docraft::RGB
   :project: docraft
   :members:

.. doxygenenum:: docraft::ColorName
   :project: docraft

DocraftDocumentMetadata
-------------------------

Metadata container for PDF document info-dict fields (author, title, dates,
etc.), applied via ``DocraftLoomPdfCreator::set_metadata()`` (see
:doc:`document`). Includes the nested ``DateTime`` struct for creation/
modification dates — richer than what the ``.craft`` ``<Metadata>`` block
currently wires up (see :doc:`../craft_language/metadata`).

.. doxygenclass:: docraft::DocraftDocumentMetadata
   :project: docraft
   :members:

Loom Node Enums
----------------

``DocraftPositionType`` is documented alongside ``DocraftLoomNode`` in
:doc:`model_nodes` (it's set via ``position_mode()``).

.. doxygenenum:: docraft::loom::nodes::TextAlignment
   :project: docraft

.. doxygenenum:: docraft::loom::nodes::ImageFormat
   :project: docraft

.. doxygenenum:: docraft::loom::nodes::ListKind
   :project: docraft

.. doxygenenum:: docraft::loom::nodes::OrderedListStyle
   :project: docraft

.. doxygenenum:: docraft::loom::nodes::UnorderedListDot
   :project: docraft

Backend/Page Enums
--------------------

.. doxygenenum:: docraft::backend::DocraftPageSize
   :project: docraft

.. doxygenenum:: docraft::backend::DocraftPageOrientation
   :project: docraft
