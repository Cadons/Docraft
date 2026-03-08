Text
====

The ``<Text>`` element renders styled text content. Text nodes support font
selection, sizing, color, alignment, and underline.

Example
-------

.. code-block:: xml

   <Text font_size="14" font_name="OpenSans" style="bold"
         alignment="center" color="#333333">
     Welcome to Docraft
   </Text>

Attributes
----------

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - Attribute
     - Type
     - Description
   * - ``font_size``
     - float
     - Font size in points (default ``12``).
   * - ``font_name``
     - string
     - Font family name (default ``OpenSans``).
   * - ``style``
     - string
     - ``normal`` | ``bold`` | ``italic`` | ``bold_italic``.
   * - ``alignment``
     - string
     - ``left`` | ``center`` | ``right`` | ``justified``.
   * - ``color``
     - color
     - Text color (hex or named).
   * - ``underline``
     - bool
     - Enable underline rendering.

The text content is the inner text of the XML element.

PageNumber
----------

``<PageNumber>`` is a special text node whose content is automatically
replaced by the current page number at render time. It supports the same
attributes as ``<Text>``.

.. code-block:: xml

   <Footer>
     <PageNumber font_size="10" alignment="center"/>
   </Footer>

