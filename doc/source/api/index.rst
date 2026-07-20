API Reference
=============

Complete C++ API documentation for Docraft's **loom** engine — auto-generated
from source code comments. See :doc:`../about` for how these pieces fit
together as a pipeline.

.. toctree::
   :maxdepth: 1
   :caption: Modules
   :hidden:

   document
   exceptions
   model_nodes
   model_shapes
   model_containers
   model_types
   pipeline
   backend
   craft_parser
   templating
   utilities

Core Components
---------------

.. grid:: 2
   :gutter: 3

   .. grid-item-card:: 📄 Orchestrator
      :link: document
      :link-type: doc

      ``DocraftLoomPdfCreator`` — the entry point that owns the node tree and
      drives Measure → Layout → Pagination → Rendering.

   .. grid-item-card:: ⚠️ Exceptions
      :link: exceptions
      :link-type: doc

      Domain-driven exception hierarchy used across parser, layout, rendering,
      backend, and templating subsystems.

Node Tree
---------

.. grid:: 2
   :gutter: 3

   .. grid-item-card:: 🧱 Node Tree — Base & Text
      :link: model_nodes
      :link-type: doc

      ``DocraftLoomNode``, ``LayoutBox``, and text-family nodes — ``Text``,
      ``Title``, ``Subtitle``, ``PageNumber``, ``Paragraph``, ``Image``, ``Blank``, ``NewPage``.

   .. grid-item-card:: 🔷 Shapes
      :link: model_shapes
      :link-type: doc

      Geometric primitives — ``Rectangle``, ``Circle``, ``Triangle``, ``Line``,
      ``Polygon`` with a composed ``DocraftLoomShapeStyle`` (fill/stroke/border).

   .. grid-item-card:: 📦 Containers
      :link: model_containers
      :link-type: doc

      Stacking layout — ``VStack``/``HStack``, ``Table``/``TableCell``,
      ``List`` (ordered/unordered) for composing complex structures.

   .. grid-item-card:: 🏷️ Types & Enums
      :link: model_types
      :link-type: doc

      Shared geometry/color types, ``DocraftDocumentMetadata``, and loom
      enums — ``TextAlignment``, ``ListKind``, ``ImageFormat``, page format, and more.

Pipeline
--------

.. grid:: 2
   :gutter: 3

   .. grid-item-card:: 📐 Pipeline
      :link: pipeline
      :link-type: doc

      The four visitor passes — Measure, Layout, Pagination, Rendering — plus
      the shared visitor interfaces and weighted-column helper.

   .. grid-item-card:: 🔌 Backend
      :link: backend
      :link-type: doc

      Capability-split rendering interfaces — currently implemented with
      ``libharu`` (``DocraftHaruBackend``), consumed but not yet swappable
      from the loom orchestrator.

Template & Parsing
------------------

.. grid:: 2
   :gutter: 3

   .. grid-item-card:: 📝 Craft Parser
      :link: craft_parser
      :link-type: doc

      Generic XML parsing (``docraft::craft``) plus the loom-specific bridge
      (``DocraftLoomCraftLanguageParser``, ``DocraftLoomTreeBuilder``) that
      builds the node tree.

   .. grid-item-card:: 🔄 Templating Engine
      :link: templating
      :link-type: doc

      Data binding and template expansion — ``${variables}``, ``<Foreach>`` loops,
      and JSON model integration.

Utilities
---------

.. grid:: 1
   :gutter: 3

   .. grid-item-card:: 🛠️ Utilities
      :link: utilities
      :link-type: doc

      Helper functions for color parsing, font registration/resolution,
      logging, and base64 decoding.
