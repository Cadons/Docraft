Getting Started
===============

This guide shows how to integrate Docraft into your C++ project and generate
your first PDF.

Requirements
------------

- **CMake** ≥ 3.16
- A C++ compiler with **C++** support (GCC 14+, Clang 16+, MSVC 2022)
- **vcpkg** (recommended) or manually installed dependencies:
  ``libharu``, ``pugixml``, ``nlohmann-json``

Installation
------------

Add Docraft as a subdirectory or install it system-wide:

.. code-block:: bash

   # Option A — subdirectory
   add_subdirectory(docraft)
   target_link_libraries(myapp PRIVATE docraft)

   # Option B — find_package (after install)
   find_package(docraft REQUIRED)
   target_link_libraries(myapp PRIVATE docraft)

Your CMake target automatically gets the correct include paths and
transitive dependencies (pugixml, libharu, nlohmann_json).

Minimal Example — Craft Language File
--------------------------------------

Docraft's engine (codenamed **loom**, see :doc:`about`) is driven by parsing a
``.craft`` XML file into a node tree and running it through the pipeline —
there is currently no public programmatic API for building a document node
by node in C++; ``.craft`` markup is the primary way to describe a document.

Create a file called ``hello.craft``:

.. code-block:: xml

   <Document>
     <Body margin_left="30" margin_right="30">
       <Text font_size="28" style="bold" alignment="center">
         Hello, Docraft!
       </Text>
       <Blank/>
       <Text font_size="12">
         This PDF was generated from a Craft Language file.
       </Text>
     </Body>
   </Document>

Parse and render it from C++:

.. code-block:: cpp

   #include <docraft/craft/docraft_loom_craft_language_parser.h>

   int main() {
       docraft::craft::DocraftLoomCraftLanguageParser parser;
       parser.load_from_file("hello.craft");

       // edit_creator() returns the DocraftLoomPdfCreator built while parsing
       auto creator = parser.edit_creator();
       creator->create();                 // Measure -> Layout -> Paginate
       creator->render("output/hello.pdf"); // Paint each page + save to disk
       return 0;
   }

Or use the CLI tool directly:

.. code-block:: bash

   docraft_tool hello.craft output/hello.pdf

Using the Template Engine
-------------------------

Inject runtime data into a ``.craft`` template before parsing — the template
engine must be attached *before* ``load_from_file()``/``parse()``, since
``${...}`` expressions are resolved while the node tree is being built:

.. code-block:: cpp

   #include <docraft/craft/docraft_loom_craft_language_parser.h>
   #include <docraft/templating/docraft_template_engine.h>

   int main() {
       auto engine = std::make_shared<docraft::templating::DocraftTemplateEngine>();
       engine->add_template_variable("customer_name", "Acme Corp");
       engine->add_template_variable("invoice_number", "INV-2025-042");
       engine->add_template_variable("total", "€ 1,250.00");

       docraft::craft::DocraftLoomCraftLanguageParser parser;
       parser.set_template_engine(engine);
       parser.load_from_file("invoice.craft");

       auto creator = parser.edit_creator();
       creator->create();
       creator->render("output/invoice.pdf");
       return 0;
   }

Using JSON Data with the CLI
-----------------------------

Create a ``data.json`` file:

.. code-block:: json

   {
     "customer_name": "Acme Corp",
     "invoice_number": "INV-2025-042",
     "total": "€ 1,250.00",
     "items": [
       {"name": "Widget A", "qty": "10", "price": "€ 50.00"},
       {"name": "Widget B", "qty": "5",  "price": "€ 150.00"}
     ]
   }

Render with data — every top-level field becomes ``${field}``, and nested
objects flatten to dot notation (``${user.name}``):

.. code-block:: bash

   docraft_tool invoice.craft output/invoice.pdf --data data.json

Document Metadata
-----------------

The ``.craft`` ``<Metadata>`` block (see :doc:`craft_language/metadata`)
covers the common fields. From C++, ``DocraftDocumentMetadata`` exposes the
full libharu info-dict surface, including trapped/GTS-PDFX flags and
creation/modification dates not yet wired up in the XML layer:

.. code-block:: cpp

   #include <docraft/docraft_document_metadata.h>

   docraft::DocraftDocumentMetadata meta;
   meta.set_author("Engineering Team");
   meta.set_title("Quarterly Report");
   meta.set_subject("Q1 2025 Financial Summary");
   meta.set_keywords("finance, quarterly, report");

   creator->set_metadata(meta); // applies immediately; a later call overwrites it

Custom Fonts
------------

Register external TTF fonts at runtime, before calling ``create()``:

.. code-block:: cpp

   // A variant left unregistered falls back to the closest available one.
   creator->register_font("MyFont",
                           "fonts/MyFont-Regular.ttf",
                           "fonts/MyFont-Bold.ttf");

Or declare them in the Craft Language:

.. code-block:: xml

   <Settings>
     <Fonts>
       <Font name="MyFont">
         <FontNormal src="fonts/MyFont-Regular.ttf"/>
         <FontBold src="fonts/MyFont-Bold.ttf"/>
       </Font>
     </Fonts>
   </Settings>

Next Steps
----------

- :doc:`craft_language/index` — Full Craft Language reference.
- :doc:`examples/index` — Real-world document templates.
- :doc:`api/index` — Complete C++ API documentation.

