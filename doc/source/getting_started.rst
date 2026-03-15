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

Minimal Example — Programmatic API
-----------------------------------

Build a document entirely from C++ without any ``.craft`` file:

.. code-block:: cpp

   #include <docraft/docraft_document.h>
   #include <docraft/model/docraft_text.h>
   #include <docraft/model/docraft_body.h>

   int main() {
       // 1. Create a document
       docraft::DocraftDocument doc("My First Document");
       doc.set_document_path("output/");

       // 2. Build a body section with text
       auto body = std::make_shared<docraft::model::DocraftBody>();
       body->set_margins(20);

       auto title = std::make_shared<docraft::model::DocraftText>("Hello, Docraft!");
       title->set_font_size(28);
       title->set_style(docraft::model::TextStyle::kBold);
       title->set_alignment(docraft::model::TextAlignment::kCenter);
       body->add_child(title);

       auto paragraph = std::make_shared<docraft::model::DocraftText>(
           "This PDF was generated entirely from C++ code — no external tools."
       );
       paragraph->set_font_size(12);
       body->add_child(paragraph);

       doc.add_node(body);

       // 3. Render
       doc.render();
       return 0;
   }

Minimal Example — Craft Language File
--------------------------------------

Create a file called ``hello.craft``:

.. code-block:: xml

   <Document path="output/">
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

   #include <docraft/craft/docraft_craft_language_parser.h>

   int main() {
       docraft::craft::DocraftCraftLanguageParser parser;
       parser.load_from_file("hello.craft");

       auto document = parser.get_document();
       document->set_document_path("output/");
       document->set_document_title("hello");
       document->render();
       return 0;
   }

Or use the CLI tool directly:

.. code-block:: bash

   docraft_tool hello.craft output/hello.pdf

Using the Template Engine
-------------------------

Inject runtime data into a ``.craft`` template:

.. code-block:: cpp

   #include <docraft/craft/docraft_craft_language_parser.h>
   #include <docraft/templating/docraft_template_engine.h>

   int main() {
       docraft::craft::DocraftCraftLanguageParser parser;
       parser.load_from_file("invoice.craft");

       auto document = parser.get_document();

       // Create and populate the template engine
       auto engine = std::make_shared<docraft::templating::DocraftTemplateEngine>();
       engine->add_template_variable("customer_name", "Acme Corp");
       engine->add_template_variable("invoice_number", "INV-2025-042");
       engine->add_template_variable("total", "€ 1,250.00");

       document->set_document_template_engine(engine);
       document->set_document_path("output/");
       document->set_document_title("invoice");
       document->render();
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

Render with data:

.. code-block:: bash

   docraft_tool invoice.craft output/invoice.pdf -d data.json

Document Metadata
-----------------

Set PDF metadata from C++:

.. code-block:: cpp

   docraft::DocraftDocumentMetadata meta;
   meta.set_author("Engineering Team");
   meta.set_title("Quarterly Report");
   meta.set_subject("Q1 2025 Financial Summary");
   meta.set_keywords("finance, quarterly, report");
   document->set_document_metadata(meta);

   // Optional: automatic keyword extraction from text content
   document->enable_auto_keywords(true);

Custom Fonts
------------

Register external TTF fonts at runtime:

.. code-block:: cpp

   #include <docraft/utils/docraft_font_registry.h>

   auto& registry = docraft::utils::DocraftFontRegistry::instance();
   registry.register_font("MyFont",       "fonts/MyFont-Regular.ttf");
   registry.register_font("MyFont-Bold",  "fonts/MyFont-Bold.ttf");

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

