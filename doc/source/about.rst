About Docraft
=============

Why Docraft?
------------

Generating PDF documents from C++ has always been a painful experience.
The ecosystem is fragmented: developers must either call low-level libraries
that expose raw page-drawing primitives (coordinates, fonts, streams) or
shell out to external tools like LaTeX, wkhtmltopdf, or headless browsers.

None of the existing solutions offer what modern C++ applications actually
need: **a library-level, declarative approach to PDF generation** that runs
entirely inside your process with no external dependencies.

Docraft solves this problem.

What is Docraft?
----------------

Docraft is a **C++23 library** that lets you define documents using an
XML-based markup language called the **Craft Language**. You describe *what*
your document looks like — text, tables, images, shapes, headers, footers —
and Docraft takes care of *how* to lay it out and render it to PDF.

It ships as a static or shared library that you link into your CMake project.
The rendering pipeline runs entirely in-process using `libharu
<https://github.com/libharu/libharu>`_ as the PDF backend — no spawning of
external processes, no temporary files, no network calls.

Key Features
------------

- **Declarative XML markup** — Define documents with the Craft Language
  instead of writing coordinate-level drawing code.
- **Automatic page layout** — Flow-based layout engine with automatic page
  breaking; no manual page management required.
- **Header / Body / Footer sections** — Define them once; Docraft repeats
  headers and footers on every page automatically.
- **Automatic page numbering** — Insert ``<PageNumber/>`` and the library
  fills in the correct number on each page.
- **Template engine** — Bind ``${variables}`` and iterate over JSON arrays
  with ``<Foreach>`` to generate data-driven documents at runtime.
- **Rich text styling** — Font family, size, bold, italic, underline,
  alignment (left, center, right, justified), and color.
- **External font support** — Register custom TTF fonts (regular, bold,
  italic, bold-italic variants) for full typographic control.
- **Shapes** — Rectangle, Circle, Triangle, Line, Polygon with background
  color, border color, and border width.
- **Tables** — Column titles, row/column weights, per-cell backgrounds, and
  JSON model binding for data-driven tables.
- **Ordered & unordered lists** — Number, roman numeral, dash, star, circle,
  and box markers.
- **Horizontal & vertical layouts** — Nest ``<Layout>`` elements with
  weights to build multi-column or multi-row compositions.
- **Image support** — PNG, JPEG from file, and raw RGB pixel data injected
  at runtime via the template engine (including base64 decoding).
- **Document metadata** — Author, title, subject, keywords, creation date,
  and automatic keyword extraction from document content.
- **Page format control** — A3, A4, A5, Letter, Legal in portrait or
  landscape orientation; configurable header/body/footer ratios.
- **Absolute positioning** — Place any element at exact (x, y) coordinates
  when flow layout is not appropriate.
- **Z-index stacking** — Control rendering order with ``z_index``.
- **DOM traversal & lookup** — Query the document tree by name or type after
  parsing, before rendering.
- **Pluggable backend** — The rendering backend is abstracted behind
  interfaces; swap or extend it without changing the document model.
- **CLI tool** — The ``docraft_tool`` executable renders ``.craft`` files
  to PDF from the command line, with optional JSON / key-value data files.

Architecture at a Glance
-------------------------

.. code-block:: text

   ┌──────────────┐     ┌──────────────┐     ┌──────────────┐
   │  .craft file │────▶│    Parser     │────▶│  Document    │
   │  (XML)       │     │  (pugixml)   │     │  DOM tree    │
   └──────────────┘     └──────────────┘     └──────┬───────┘
                                                    │
                                            ┌───────▼───────┐
                                            │  Template      │
                                            │  Engine        │
                                            └───────┬───────┘
                                                    │
                                            ┌───────▼───────┐
                                            │  Layout        │
                                            │  Engine        │
                                            └───────┬───────┘
                                                    │
                                            ┌───────▼───────┐
                                            │  Renderer +    │
                                            │  Painters      │
                                            └───────┬───────┘
                                                    │
                                            ┌───────▼───────┐
                                            │  Backend       │
                                            │  (libharu)     │
                                            └───────┬───────┘
                                                    │
                                              output.pdf

Pluggable Backend Architecture
------------------------------

**Default Backend: libharu**

By default, Docraft uses `libharu <https://github.com/libharu/libharu>`_ as
its rendering backend for PDF generation. This library is embedded and requires
no external dependencies.

**Custom Backend Support**

The rendering backend is fully pluggable — you can replace or extend it to
support different output formats (e.g., SVG, HTML Canvas, PostScript) or to
integrate with proprietary rendering engines.

To implement a custom backend, you need to create a class that implements the
``docraft::backend::IDocraftRenderingBackend`` interface, which aggregates
four sub-interfaces:

- ``IDocraftTextRenderingBackend`` — Text drawing, font management, and text measurement
- ``IDocraftShapeRenderingBackend`` — Shapes (rectangles, circles, polygons, lines) with fill and stroke
- ``IDocraftImageRenderingBackend`` — Image rendering (PNG, JPEG, raw RGB data)
- ``IDocraftPageRenderingBackend`` — Page creation, navigation, and dimension queries

Your backend implementation must provide:

- **save_to_file(path)** — Writes the rendered document to disk
- **file_extension()** — Returns the output file extension (e.g., ".pdf", ".svg")
- **register_ttf_font_from_file(path, embed)** — Registers custom fonts
- **set_font(name, size, encoder)** — Sets the active font for text rendering
- **set_metadata(metadata)** — Applies document metadata (author, title, etc.)
- All pure virtual methods from the four sub-interfaces

**Setting a Custom Backend**

You can override the backend for a document by calling:

.. code-block:: cpp

   auto custom_backend = std::make_shared<MyCustomBackend>();
   document.set_backend(custom_backend);

This allows Docraft to generate documents in formats beyond PDF, making it
suitable for diverse rendering pipelines and output targets.

License & Credits
-----------------

Docraft is developed by `Cadons <https://github.com/Cadons>`_.

