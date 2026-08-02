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

Docraft is a **C++ library** that lets you define documents using an
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
- **Automatic page layout** — Content is laid out once on a continuous,
  unbounded-height canvas, then paginated across discrete pages automatically
  — including mid-table splits with repeating header rows.
- **Header / Body / Footer sections** — Define them once; Docraft lays them
  out a single time and redraws header/footer on every physical page.
- **Automatic page numbering** — Insert ``<PageNumber/>`` and the library
  fills in the correct current/total page number on each page.
- **Template engine** — Bind ``${variables}`` and iterate over JSON arrays
  with ``<Foreach>`` to generate data-driven documents at runtime.
- **Rich text styling** — Font family, size, independent bold/italic/
  underline/strikeout flags, alignment (left, center, right, justified),
  and color.
- **External font support** — Register custom TTF fonts (regular, bold,
  italic, bold-italic variants) for full typographic control, either from
  C++ or declaratively via ``<Settings><Fonts>``.
- **Shapes** — Rectangle, Circle, Triangle, Line, Polygon with background
  color, border color, and border width.
- **Tables** — Column titles, column weights, per-cell backgrounds, and
  JSON model binding (matrix or array-of-objects) for data-driven tables,
  with automatic pagination across pages.
- **Ordered & unordered lists** — Number and roman-numeral ordered markers;
  dash, star, circle, and box unordered markers.
- **Horizontal & vertical layouts** — Nest ``<Layout>`` elements with
  weighted children to build multi-column compositions (weights apply to
  horizontal layouts and tables).
- **Paragraphs** — Group runs of ``<Text>`` into one flowing block with
  ``line_spacing``, ``space_before``/``space_after``.
- **Image support** — PNG, JPEG from file, and raw RGB pixel data injected
  at runtime via the template engine (including base64 decoding).
- **Document metadata** — Title, author, creator, producer, subject, and
  keywords, applied straight to the PDF info dictionary.
- **Page format control** — A3, A4, A5, Letter, Legal in portrait or
  landscape orientation; configurable header/body/footer height ratios.
- **Absolute & block positioning** — Most elements flow automatically;
  any element can opt into exact (x, y) placement with ``position="absolute"``.
- **Margin & padding** — CSS-like per-edge ``margin`` (adjacent margins
  collapse via ``max()``, not sum) plus ``padding`` for inset content.
- **Charts and canvas** — Draw bar, line, and pie charts with JSON data; draw arbitrary
  shapes and lines on a canvas with a simple DSL.
- **CLI tool** — The ``docraft_tool`` executable renders ``.craft`` files
  to PDF from the command line, with optional JSON data files.

Architecture at a Glance
-------------------------

Docraft's layout/render engine — codenamed **loom** — is a pipeline of
visitor passes over a plain node tree, not a monolithic renderer:

.. code-block:: text

   .craft XML file
         │
         ▼
   DocraftCraftLanguageParser  (pugixml, generic per-tag parsers)
         │
         ▼
   DocraftParsedElement tree   (tag-agnostic, engine-agnostic)
         │
         ▼
   DocraftLoomTreeBuilder + DocraftTemplateEngine
         │
         ▼
   DocraftLoomNode tree        (Text, Paragraph, VStack, Table, ...)
         │
         ▼
   MeasureProcessor    → writes measured_size
         │
         ▼
   LayoutProcessor     → writes frame (cursor-based flow)
         │
         ▼
   PaginationProcessor → writes page_index, splits across pages
         │
         ▼
   RenderingProcessor  → paints one page at a time via backend interfaces
         │
         ▼
   DocraftHaruBackend (libharu) → output.pdf

Each stage has a single responsibility and touches every node exactly once
via double-dispatch (``node->accept(visitor)``). See
:doc:`craft_language/index` for the markup this pipeline consumes and
:doc:`api/index` for the C++ types involved.

Backend Architecture
---------------------

**Default backend: libharu**

Docraft uses `libharu <https://github.com/libharu/libharu>`_ as its PDF
backend. It is embedded and requires no external processes or runtime
dependencies.

**Capability-split interfaces**

Rather than one monolithic backend interface, drawing capabilities are split
by responsibility so consumers depend only on what they need:

- ``IDocraftTextRenderingBackend`` — text drawing, font metrics, measurement
- ``IDocraftShapeRenderingBackend`` — rectangles, circles, polygons, triangles
- ``IDocraftLineRenderingBackend`` — line strokes
- ``IDocraftImageRenderingBackend`` — PNG/JPEG/raw-pixel image drawing
- ``IDocraftPageRenderingBackend`` — page creation, navigation, dimensions
- ``IDocraftFontBackend`` / ``IDocraftOutputBackend`` / ``IDocraftMetadataBackend``
  — font registration, file persistence, PDF metadata

These are grouped into three provider interfaces
(``IDocraftRenderingCapabilityProvider``, ``IDocraftResourceCapabilityProvider``,
``IDocraftLifecycleCapabilityProvider`` — see :doc:`api/backend`), and
``DocraftHaruBackend`` is currently the only implementation. The loom
pipeline consumes these interfaces exclusively — ``DocraftLoomRenderingProcessor``
never references the concrete Haru type — but ``DocraftLoomPdfCreator``
constructs its own ``DocraftHaruBackend`` internally today; there is no
public API yet to inject an alternative implementation at that level. The
abstraction exists to keep the pipeline decoupled from libharu, not (yet) as
an end-user extension point.

License & Credits
-----------------

Docraft is developed by `Cadons <https://github.com/Cadons>`_.

Docraft is licensed under the **Apache License 2.0**.
See the full text in the root ``LICENSE`` file.
