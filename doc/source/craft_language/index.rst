Craft Language Reference
========================

The **Craft Language** is an XML-based markup language for defining Docraft documents.
Each XML tag maps to a visual element that the library parses, layouts, and renders to PDF.

Write declarative, readable document templates without touching low-level drawing APIs.

.. toctree::
   :maxdepth: 1
   :caption: Topics
   :hidden:

   structure
   text
   shapes
   images
   tables
   lists
   layout
   templating
   settings
   metadata

Core Elements
-------------

.. grid:: 2
   :gutter: 3

   .. grid-item-card:: 🏗️ Document Structure
      :link: structure
      :link-type: doc

      The foundation — ``<Document>``, ``<Header>``, ``<Body>``, ``<Footer>``
      sections and how they compose pages automatically.

   .. grid-item-card:: 📝 Text
      :link: text
      :link-type: doc

      Styled text with fonts, sizes, colors, alignment, bold, italic, underline,
      and justified text support.

Visual Elements
---------------

.. grid:: 2
   :gutter: 3

   .. grid-item-card:: 🔷 Shapes
      :link: shapes
      :link-type: doc

      Geometric primitives — ``<Rectangle>``, ``<Circle>``, ``<Triangle>``,
      ``<Line>``, ``<Polygon>`` with fill and stroke styling.

   .. grid-item-card:: 🖼️ Images
      :link: images
      :link-type: doc

      Embed PNG and JPEG images from files, or inject raw pixel data at runtime
      via the template engine (including base64).

Complex Structures
------------------

.. grid:: 2
   :gutter: 3

   .. grid-item-card:: 📊 Tables
      :link: tables
      :link-type: doc

      Grid-based tables with column titles, row/column weights, cell backgrounds,
      and automatic JSON data binding via ``<Foreach>``.

   .. grid-item-card:: 📋 Lists
      :link: lists
      :link-type: doc

      Ordered and unordered lists — number, roman numeral, alphabetic, dash,
      star, circle, and box markers.

   .. grid-item-card:: 📐 Layout
      :link: layout
      :link-type: doc

      Nested ``<Layout>`` containers with horizontal/vertical orientation
      and weighted child distribution for multi-column designs.

   .. grid-item-card:: 🔄 Templating
      :link: templating
      :link-type: doc

      Data binding with ``${variables}``, ``<Foreach>`` loops, conditional
      rendering, and JSON model integration for dynamic documents.

Configuration
-------------

.. grid:: 2
   :gutter: 3

   .. grid-item-card:: ⚙️ Settings
      :link: settings
      :link-type: doc

      Page format (A3, A4, A5, Letter, Legal), orientation (portrait/landscape),
      custom fonts, and section height ratios.

   .. grid-item-card:: 📄 Metadata
      :link: metadata
      :link-type: doc

      Document properties — title, author, subject, keywords, creation date,
      and automatic keyword extraction from content.

