Document Metadata
=================

The ``<Metadata>`` section inside ``<Document>`` sets PDF information fields
(author, title, dates, etc.) that are embedded in the output file.

Example
-------

.. code-block:: xml

   <Document>
     <Metadata>
       <DocumentTitle>Annual Report 2025</DocumentTitle>
       <Author>Docraft Team</Author>
       <Subject>Financial summary</Subject>
       <Keywords>finance, report, 2025</Keywords>
       <Creator>Docraft 1.0</Creator>
       <Producer>Docraft/libharu</Producer>
       <CreationDate>
         <Year>2025</Year>
         <Month>12</Month>
         <Day>31</Day>
       </CreationDate>
       <AutoKeywords enabled="true" max_keywords="15"
                     min_length="4" language="en"/>
     </Metadata>
     <Body>
       <!-- content -->
     </Body>
   </Document>

Metadata Fields
---------------

.. list-table::
   :header-rows: 1
   :widths: 25 75

   * - Element
     - Description
   * - ``<DocumentTitle>``
     - PDF title string.
   * - ``<Author>``
     - Author name.
   * - ``<Creator>``
     - Application that created the content.
   * - ``<Producer>``
     - Application that produced the PDF.
   * - ``<Subject>``
     - Document subject.
   * - ``<Keywords>``
     - Comma-separated keyword list.
   * - ``<Trapped>``
     - Trapping flag (``True``, ``False``, ``Unknown``).
   * - ``<GtsPdfx>``
     - GTS_PDFXVersion string.

Date Elements
-------------

``<CreationDate>`` and ``<ModificationDate>`` contain sub-elements:

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - Element
     - Type
     - Description
   * - ``<Year>``
     - int
     - Four-digit year.
   * - ``<Month>``
     - int
     - Month (1–12).
   * - ``<Day>``
     - int
     - Day (1–31).
   * - ``<Hour>``
     - int
     - Hour (0–23).
   * - ``<Minutes>``
     - int
     - Minutes (0–59).
   * - ``<Seconds>``
     - int
     - Seconds (0–59).
   * - ``<Ind>``
     - char
     - UTC indicator (``+``, ``-``, or ``Z``).
   * - ``<OffHour>``
     - int
     - UTC offset hours.
   * - ``<OffMinutes>``
     - int
     - UTC offset minutes.

Automatic Keywords
------------------

Enable ``<AutoKeywords>`` to have the library automatically extract keywords
from text nodes using term-frequency analysis.

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - Attribute
     - Type
     - Description
   * - ``enabled``
     - bool
     - Enable/disable extraction.
   * - ``max_keywords``
     - int
     - Maximum number of keywords (default ``10``).
   * - ``min_length``
     - int
     - Minimum word length (default ``4``).
   * - ``language``
     - string
     - Stop-word languages, comma-separated (``en,it,fr,de,es``).
