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
   * - ``strikeout``
     - bool
     - Enable strikeout rendering. Independent of ``underline`` — both can
       be enabled at once.

The text content is the inner text of the XML element. Note there is
currently no Craft Language attribute for word-wrapping (the underlying
``DocraftLoomText::wrap_width()`` isn't wired to an XML token yet) — text
that doesn't fit its container's natural width is not wrapped from markup.

Title & Subtitle
----------------

``<Title>`` and ``<Subtitle>`` are ``<Text>`` variants with larger, bold
defaults and extra margin that tracks the effective ``font_size`` — useful
for headings without repeating styling on every ``<Text>``. Every ``<Text>``
attribute above still applies and overrides the defaults.

.. code-block:: xml

   <Title>Quarterly Report</Title>
   <Subtitle font_size="16">Q1 2026</Subtitle>

PageNumber
----------

``<PageNumber>`` is a special text node whose content is automatically
replaced by the current page number at render time. It supports the same
attributes as ``<Text>``.

.. code-block:: xml

   <Footer>
     <PageNumber font_size="10" alignment="center"/>
   </Footer>

Paragraph
---------

``<Paragraph>`` groups several ``<Text>`` runs into one flowing block — see
:doc:`layout` for how it composes with ``<Layout>`` containers.

.. code-block:: xml

   <Paragraph line_spacing="1.5" space_before="4" space_after="6" alignment="center">
     <Text>First run.</Text>
     <Text>Second run, same paragraph.</Text>
   </Paragraph>

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - Attribute
     - Type
     - Description
   * - ``line_spacing``
     - float
     - Line-height multiplier (default ``1.2``).
   * - ``space_before``
     - float
     - Space above the paragraph in points (default ``0``).
   * - ``space_after``
     - float
     - Space below the paragraph in points (default ``0``).
   * - ``alignment``
     - string
     - Default alignment for children; each ``<Text>`` can still override it.

