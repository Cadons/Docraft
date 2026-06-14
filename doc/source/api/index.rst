API Reference
=============

Complete C++ API documentation for Docraft — auto-generated from source code comments.
Explore the library's modules below to understand how to build, layout, and render PDF documents.

.. toctree::
   :maxdepth: 1
   :caption: Modules
   :hidden:

   document
   context
   exceptions
   model_nodes
   model_shapes
   model_containers
   model_types
   layout
   renderer
   painters
   backend
   craft_parser
   templating
   utilities

Core Components
---------------

.. grid:: 2
   :gutter: 3

   .. grid-item-card:: 📄 Document
      :link: document
      :link-type: doc

      The main entry point — ``DocraftDocument`` owns the document tree, metadata,
      and drives the entire rendering pipeline.

   .. grid-item-card:: 🎯 Context
      :link: context
      :link-type: doc

      Runtime context and cursor management — tracks the current rendering position,
      page boundaries, and layout state.

   .. grid-item-card:: ⚠️ Exceptions
      :link: exceptions
      :link-type: doc

      Domain-driven exception hierarchy used across parser, layout, rendering,
      backend, and templating subsystems.

Document Model
--------------

.. grid:: 2
   :gutter: 3

   .. grid-item-card:: 🧱 Core Nodes
      :link: model_nodes
      :link-type: doc

      Base classes for all document elements — ``DocraftNode``, ``DocraftText``,
      ``DocraftImage``, and structural components like ``Header``, ``Body``, ``Footer``.

   .. grid-item-card:: 🔷 Shapes
      :link: model_shapes
      :link-type: doc

      Geometric primitives — ``Rectangle``, ``Circle``, ``Triangle``, ``Line``,
      ``Polygon`` with fill, stroke, and border styling.

   .. grid-item-card:: 📦 Containers
      :link: model_containers
      :link-type: doc

      Layout containers — ``DocraftLayout`` (horizontal/vertical), ``DocraftTable``,
      ``DocraftList`` (ordered/unordered) for composing complex structures.

   .. grid-item-card:: 🏷️ Types & Enums
      :link: model_types
      :link-type: doc

      Common types, enums, and data structures — ``TextStyle``, ``TextAlignment``,
      ``PageFormat``, ``Orientation``, and more.

Rendering Pipeline
------------------

.. grid:: 2
   :gutter: 3

   .. grid-item-card:: 📐 Layout Engine
      :link: layout
      :link-type: doc

      Flow-based layout system with automatic page breaking — calculates positions
      and sizes for all document nodes before rendering.

   .. grid-item-card:: 🎨 Renderer
      :link: renderer
      :link-type: doc

      The rendering coordinator — traverses the document tree and delegates
      drawing operations to specialized painters.

   .. grid-item-card:: 🖌️ Painters
      :link: painters
      :link-type: doc

      Specialized painters for each element type — ``TextPainter``, ``ShapePainter``,
      ``ImagePainter``, ``TablePainter``, etc.

   .. grid-item-card:: 🔌 Backend
      :link: backend
      :link-type: doc

      Abstracted PDF backend interface — currently implemented with ``libharu``
      but designed to be pluggable.

Template & Parsing
------------------

.. grid:: 2
   :gutter: 3

   .. grid-item-card:: 📝 Craft Parser
      :link: craft_parser
      :link-type: doc

      XML parser for the Craft Language — converts ``.craft`` files into
      a document object model using ``pugixml``.

   .. grid-item-card:: 🔄 Templating Engine
      :link: templating
      :link-type: doc

      Data binding and template expansion — ``${variables}``, ``<Foreach>`` loops,
      conditional rendering, and JSON model integration.

Utilities
---------

.. grid:: 1
   :gutter: 3

   .. grid-item-card:: 🛠️ Utilities
      :link: utilities
      :link-type: doc

      Helper functions for color parsing, string manipulation, font management,
      base64 decoding, and file operations.

