Lists
=====

Docraft supports ordered (``<List>``) and unordered (``<UList>``) lists.

Ordered List
------------

.. code-block:: xml

   <List style="number">
     <Text>First item</Text>
     <Text>Second item</Text>
     <Text>Third item</Text>
   </List>

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - Attribute
     - Type
     - Description
   * - ``style``
     - string
     - ``number`` (1, 2, 3…) or ``roman`` (I, II, III…).

Unordered List
--------------

.. code-block:: xml

   <UList dot="circle">
     <Text>Apple</Text>
     <Text>Banana</Text>
     <Text>Cherry</Text>
   </UList>

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - Attribute
     - Type
     - Description
   * - ``dot``
     - string
     - Marker style: ``-`` (dash), ``*`` (star), ``circle``, or ``box``.

