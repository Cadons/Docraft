Utilities
=========

Helper classes used across the library for fonts, logging, and encoding.

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

