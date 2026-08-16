Pipeline
========

Docraft's loom engine computes a document in four visitor passes over the
node tree (see :doc:`../about`): **Measure** → **Layout** → **Paginate** →
**Render**. Each stage implements ``DocraftLoomIVisitor`` — one
``visit(NodeType*)`` overload per node type, dispatched via double-dispatch
(``node->accept(visitor)``) — and writes exactly one field of each node's
``LayoutBox`` (see :doc:`model_nodes`).

Visitor Interfaces
-------------------

Adding a new node type means adding a ``visit()`` overload here and
implementing it in all four processors below — a compile error catches any
processor left behind.

.. doxygenclass:: docraft::loom::interfaces::DocraftLoomIVisitor
   :project: docraft
   :members:

.. doxygenclass:: docraft::loom::interfaces::DocraftLoomIVisitorNode
   :project: docraft
   :members:

DocraftLoomMeasureProcessor
------------------------------

Computes ``layout_box().measured_size`` for every node, bottom-up (children
before parent). Needs a text-rendering backend for font-metric-based
measurement and word-wrapping.

.. doxygenclass:: docraft::loom::pipeline::DocraftLoomMeasureProcessor
   :project: docraft
   :members:

DocraftLoomLayoutProcessor & DocraftLoomCursor
-------------------------------------------------

Walks the tree with a ``DocraftLoomCursor`` (origin top-left, y grows
downward), filling ``layout_box().frame``. Layout happens on one continuous,
unbounded-height canvas — it never thinks about page boundaries.

.. doxygenclass:: docraft::loom::pipeline::DocraftLoomLayoutProcessor
   :project: docraft
   :members:

.. doxygenclass:: docraft::loom::pipeline::DocraftLoomCursor
   :project: docraft
   :members:

DocraftLoomPaginationProcessor
---------------------------------

Fills ``layout_box().page_index`` by walking the body's top-level children,
splitting an overflowing ``DocraftLoomTable`` row-by-row (with repeating
title rows) and moving any other overflowing child whole to a new page.
Header/footer are stamped ``page_index = -1`` (render on every page) rather
than walked by this pass.

.. doxygenclass:: docraft::loom::pipeline::DocraftLoomPaginationProcessor
   :project: docraft
   :members:

DocraftLoomRenderingProcessor
--------------------------------

Run once per physical page; paints only nodes whose ``page_index`` matches
the current page (or is ``-1``). Pulls narrow capability interfaces
(text/shape/line/image rendering, see :doc:`backend`) off an
``IDocraftRenderingCapabilityProvider`` rather than depending on the concrete
Haru backend directly.

.. doxygenclass:: docraft::loom::pipeline::DocraftLoomRenderingProcessor
   :project: docraft
   :members:

DocraftLoomPipelineExecutor
------------------------------

Runs Measure→Layout for one region (header, body, or footer) with a fresh
processor pair per call, so per-traversal state (inherited wrap width,
inherited content width) never leaks between regions.

.. doxygenclass:: docraft::loom::pipeline::DocraftLoomPipelineExecutor
   :project: docraft
   :members:

Weighted Distribution
-------------------------------

Shared, axis-agnostic helper used by Measure and Layout to resolve
``weights`` on a horizontal ``<Layout>`` or ``<Table>`` column set, and on a
vertical ``<Layout>`` with an explicit ``height`` — divides the available
amount (width or height) by weight (missing/non-positive entries default to
an even 1.0 share), with optional per-item floors so no result goes below a
natural minimum.

.. doxygenfunction:: docraft::loom::pipeline::distribute_weighted_amounts
   :project: docraft
