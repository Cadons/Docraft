Node Tree — Base & Text
=======================

Every visual element in a Docraft document is a ``DocraftLoomNode`` in the
**loom** node tree (see :doc:`../about`). Nodes are built by
``DocraftLoomTreeBuilder`` (:doc:`craft_parser`) from parsed Craft Language
elements and then walked by the four pipeline visitors (:doc:`pipeline`).

DocraftLoomNode
---------------

Base class for every node: child management, ``LayoutBox``, positioning mode
(block/absolute), ``z_index``, optional ``name``, ``padding``, and CSS-like
``margin``. Implements ``interfaces::DocraftLoomIVisitorNode`` so any node can
be walked by a ``DocraftLoomIVisitor`` (see :doc:`pipeline`).

.. doxygenclass:: docraft::loom::nodes::DocraftLoomNode
   :project: docraft
   :members:

LayoutBox and supporting types
-------------------------------

``LayoutBox`` is the per-node accumulator the pipeline fills in, one field per
stage: ``measured_size`` (Measure) → ``frame`` (Layout) → ``page_index``
(Pagination). Later stages read only what earlier stages wrote — skipping a
stage for a node type breaks whatever reads its field.

.. doxygenclass:: docraft::loom::nodes::LayoutBox
   :project: docraft
   :members:

.. doxygenstruct:: docraft::loom::nodes::Size
   :project: docraft
   :members:

.. doxygenstruct:: docraft::loom::nodes::Rect
   :project: docraft
   :members:

.. doxygenstruct:: docraft::loom::nodes::DocraftLoomMargin
   :project: docraft
   :members:

.. doxygenenum:: docraft::loom::nodes::DocraftPositionType
   :project: docraft

DocraftLoomText
----------------

The base leaf text node — ``<Text>``. Independent bold/italic/underline/
strikeout flags, font family/size, color, alignment, and optional word
wrapping (``wrap_width``).

.. doxygenclass:: docraft::loom::nodes::DocraftLoomText
   :project: docraft
   :members:

DocraftLoomTitle / DocraftLoomSubtitle
----------------------------------------

Thin ``DocraftLoomText`` subclasses (``<Title>``/``<Subtitle>``) whose
constructors pick larger/bolder defaults and a margin that tracks the
effective ``font_size`` — all overridable via ordinary Text attributes.

.. doxygenclass:: docraft::loom::nodes::DocraftLoomTitle
   :project: docraft
   :members:

.. doxygenclass:: docraft::loom::nodes::DocraftLoomSubtitle
   :project: docraft
   :members:

DocraftLoomPageNumber
-----------------------

``<PageNumber/>`` — a ``DocraftLoomText`` subclass that recomputes its display
string from the current/total page count at render time, then delegates to
the same text-rendering path as any other ``DocraftLoomText``.

.. doxygenclass:: docraft::loom::nodes::DocraftLoomPageNumber
   :project: docraft
   :members:

DocraftLoomParagraph
----------------------

``<Paragraph>`` — groups ``DocraftLoomText`` children into one flowing
vertical block with ``line_spacing``, ``space_before``/``space_after``, and a
default alignment individual children can override.

.. doxygenclass:: docraft::loom::nodes::DocraftLoomParagraph
   :project: docraft
   :members:

DocraftLoomImage
-------------------

``<Image>`` — PNG/JPEG from a file path, or raw pixel data injected via the
template engine (base64-decoded). Dimensions are never auto-derived from the
file; set ``width``/``height`` explicitly.

.. doxygenclass:: docraft::loom::nodes::DocraftLoomImage
   :project: docraft
   :members:

DocraftLoomBlankLine / DocraftLoomNewPage
--------------------------------------------

Pure spacers/markers — ``<Blank/>`` reserves vertical space (default 12pt,
about one line); ``<NewPage/>`` forces an unconditional page break during
pagination. Neither one paints anything.

.. doxygenclass:: docraft::loom::nodes::DocraftLoomBlankLine
   :project: docraft
   :members:

.. doxygenclass:: docraft::loom::nodes::DocraftLoomNewPage
   :project: docraft
   :members:
