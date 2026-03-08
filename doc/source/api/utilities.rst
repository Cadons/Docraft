Utilities
=========

Helper classes used across the library for fonts, logging, keywords, and
encoding.

DocraftFontApplier
------------------

Applies fonts to text nodes and manages font registration with the backend.

.. doxygenclass:: docraft::generic::DocraftFontApplier
   :project: docraft
   :members:

DocraftFontRegistry
-------------------

Singleton registry for in-memory and file-based fonts.

.. doxygenclass:: docraft::utils::DocraftFontRegistry
   :project: docraft
   :members:

DocraftFontResolver
-------------------

Resolves a font family + style request into the best available font name.

.. doxygenclass:: docraft::utils::DocraftFontResolver
   :project: docraft
   :members:

DocraftKeywordExtractor
-----------------------

Extracts document keywords using term-frequency statistics and stop-word
filtering.

.. doxygenclass:: docraft::utils::DocraftKeywordExtractor
   :project: docraft
   :members:

DocraftLogger
-------------

Simple console logging utility with configurable levels.

.. doxygenclass:: docraft::utils::DocraftLogger
   :project: docraft
   :members:

DocraftParserUtilis
-------------------

Static helpers for template expression detection and JSON data extraction.

.. doxygenclass:: docraft::utils::DocraftParserUtilis
   :project: docraft
   :members:

Base64 Decoding
---------------

.. doxygenfunction:: docraft::utils::decode_base64
   :project: docraft

Chain of Responsibility
-----------------------

Generic handler interface used by the layout engine.

.. doxygenclass:: docraft::generic::DocraftChainOfResponsibilityHandler
   :project: docraft
   :members:

