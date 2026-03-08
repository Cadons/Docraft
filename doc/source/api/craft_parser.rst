Craft Language Parser
=====================

The parser subsystem converts Craft Language XML sources into the Docraft
model tree. A registry of ``IDocraftParser`` implementations handles each
element tag.

DocraftCraftLanguageParser
--------------------------

Main parser that loads XML, registers tag-specific parsers, and builds the
``DocraftDocument``.

.. doxygenclass:: docraft::craft::DocraftCraftLanguageParser
   :project: docraft
   :members:

IDocraftParser
--------------

Interface for single-tag parsers.

.. doxygenclass:: docraft::craft::IDocraftParser
   :project: docraft
   :members:

Element Parsers
---------------

Each parser translates one XML tag into the corresponding model node.

.. doxygenclass:: docraft::craft::parser::DocraftTextParser
   :project: docraft
   :members:

.. doxygenclass:: docraft::craft::parser::DocraftImageParser
   :project: docraft
   :members:

.. doxygenclass:: docraft::craft::parser::DocraftRectangleParser
   :project: docraft
   :members:

.. doxygenclass:: docraft::craft::parser::DocraftCircleParser
   :project: docraft
   :members:

.. doxygenclass:: docraft::craft::parser::DocraftTriangleParser
   :project: docraft
   :members:

.. doxygenclass:: docraft::craft::parser::DocraftLineParser
   :project: docraft
   :members:

.. doxygenclass:: docraft::craft::parser::DocraftPolygonParser
   :project: docraft
   :members:

.. doxygenclass:: docraft::craft::parser::DocraftTableParser
   :project: docraft
   :members:

.. doxygenclass:: docraft::craft::parser::DocraftListParser
   :project: docraft
   :members:

.. doxygenclass:: docraft::craft::parser::DocraftUListParser
   :project: docraft
   :members:

.. doxygenclass:: docraft::craft::parser::DocraftLayoutParser
   :project: docraft
   :members:

.. doxygenclass:: docraft::craft::parser::DocraftBlackLineParser
   :project: docraft
   :members:

.. doxygenclass:: docraft::craft::parser::DocraftNewPageParser
   :project: docraft
   :members:

.. doxygenclass:: docraft::craft::parser::DocraftPageNumberParser
   :project: docraft
   :members:

.. doxygenclass:: docraft::craft::parser::DocraftSettingsParser
   :project: docraft
   :members:

.. doxygenclass:: docraft::craft::parser::DocraftHeaderParser
   :project: docraft
   :members:

.. doxygenclass:: docraft::craft::parser::DocraftBodyParser
   :project: docraft
   :members:

.. doxygenclass:: docraft::craft::parser::DocraftFooterParser
   :project: docraft
   :members:

.. doxygenclass:: docraft::craft::parser::DocraftForeachParser
   :project: docraft
   :members:

