Craft Language Parser
=====================

Two collaborating layers convert a ``.craft`` file into a loom node tree
(see :doc:`../about`): a generic, engine-agnostic XML parser
(``docraft::craft``), and a loom-specific bridge that builds
``DocraftLoomNode``\ s from its output.

DocraftLoomCraftLanguageParser
---------------------------------

The top-level driver — the only piece that understands ``<Document>``
structure itself (``<Header>``/``<Body>``/``<Footer>``/``<Settings>``/
``<Metadata>``). Parses each section with ``DocraftCraftLanguageParser``,
builds it into loom nodes via ``DocraftLoomTreeBuilder``, and wraps the
result in a ``DocraftLoomPdfCreator`` (see :doc:`document`) retrievable via
``edit_creator()``.

.. doxygenclass:: docraft::craft::DocraftLoomCraftLanguageParser
   :project: docraft
   :members:

DocraftLoomTreeBuilder
-------------------------

The only component depending on both ``docraft::craft`` and
``docraft::loom`` — converts a generic ``DocraftParsedElement`` tree into a
typed ``DocraftLoomNode`` tree, resolving ``${...}``/``<Foreach>`` templating
per node as it builds (see :doc:`templating`).

.. doxygenclass:: docraft::loom::craft::DocraftLoomTreeBuilder
   :project: docraft
   :members:

DocraftCraftLanguageParser
--------------------------

Generic, engine-agnostic XML parser (pugixml-based). Holds one
``IDocraftParser`` per recognized tag and produces a tag-agnostic
``DocraftParsedElement`` tree with zero knowledge of layout or rendering.

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

Each parser translates one XML tag into a ``Parsed<Tag>Data`` payload
attached to the corresponding ``DocraftParsedElement``.

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

.. doxygenclass:: docraft::craft::parser::DocraftParagraphParser
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

.. doxygenclass:: docraft::craft::parser::DocraftSectionParser
   :project: docraft
   :members:

Note: ``<Header>``/``<Body>``/``<Footer>`` all parse through this single
``DocraftSectionParser`` (producing ``ParsedSectionData``) rather than one
class per tag; ``<Settings>``/``<Metadata>`` are not tag parsers at all —
they're read directly by ``DocraftLoomCraftLanguageParser`` above.

.. doxygenclass:: docraft::craft::parser::DocraftForeachParser
   :project: docraft
   :members:

