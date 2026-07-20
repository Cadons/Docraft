Layout
======

The ``<Layout>`` element arranges its children either horizontally or
vertically. Children can be weighted to control how available space is
distributed.

.. note::

   ``weight`` is a relative share, not a fraction of ``1``: values don't need to sum to
   ``1``, and any value greater than ``1`` is allowed (a column weighted ``2`` simply
   gets twice the share of one weighted ``1``). A missing or non-positive ``weight``
   defaults to an even share (``1.0``) rather than collapsing to zero width.

.. note::

   ``weight`` currently only affects **horizontal** ``<Layout>``; on a ``vertical``
   ``<Layout>`` it is parsed but not yet applied — all rows keep their natural
   (shrink-to-fit) height.

Example — Horizontal Layout
----------------------------

.. code-block:: xml

   <Layout orientation="horizontal">
     <Text weight="1">Left column</Text>
     <Text weight="2">Center column (twice as wide)</Text>
     <Text weight="1">Right column</Text>
   </Layout>

Instead of a per-child ``weight`` attribute, ``<Layout>`` also accepts an
explicit ``weights`` list, which takes precedence over any per-child weight:

.. code-block:: xml

   <Layout orientation="horizontal" weights="1,2,1" spacing="5">
     <Text>Left column</Text>
     <Text>Center column (twice as wide)</Text>
     <Text>Right column</Text>
   </Layout>

Example — Vertical Layout
--------------------------

Vertical layouts stack children top to bottom; ``weight`` has no effect yet (see note
above):

.. code-block:: xml

   <Layout orientation="vertical">
     <Text>Top row</Text>
     <Text>Bottom row</Text>
   </Layout>

Attributes
----------

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - Attribute
     - Type
     - Description
   * - ``orientation``
     - string
     - ``horizontal`` or ``vertical`` (default ``horizontal``).
   * - ``spacing``
     - float
     - Fixed gap in points between adjacent children (default ``0``).
   * - ``weights``
     - string
     - Comma-separated list of per-column weights, e.g. ``"1,2,1"``.
       Horizontal layouts only; takes precedence over any per-child
       ``weight`` attribute.

Child nodes inside a horizontal layout can instead use their own ``weight``
attribute to control proportional sizing (see :doc:`structure`). If no
weights are set at all, children share space equally.
