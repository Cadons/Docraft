Orchestrator
============

``DocraftLoomPdfCreator`` is the top-level entry point of the loom pipeline
(see :doc:`../about`) — it owns the header/footer/body node tree, drives
Measure → Layout → Pagination via ``create()``, and Rendering + file output
via ``render()``. It is built for you by
``DocraftLoomCraftLanguageParser::edit_creator()`` when parsing a ``.craft``
file (see :doc:`craft_parser`); see :doc:`../getting_started` for end-to-end
usage.

.. doxygenclass:: docraft::loom::DocraftLoomPdfCreator
   :project: docraft
   :members:
