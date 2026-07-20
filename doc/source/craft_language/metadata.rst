Document Metadata
=================

The ``<Metadata>`` section inside ``<Document>`` sets PDF information fields
that are embedded in the output file.

.. note::

   Only the six string fields below are currently wired up from Craft
   Language XML. ``<CreationDate>``/``<ModificationDate>``/``<Trapped>``/
   ``<GtsPdfx>``/``<AutoKeywords>`` are silently skipped by the parser today
   — they are not parse errors, they simply have no effect. The underlying
   ``DocraftDocumentMetadata`` C++ type does support dates, trapping, and
   GTS-PDFX (see :doc:`../api/model_types`); there is currently no automatic
   keyword extraction utility in the codebase at all. If you need any of
   these, set them from C++ via ``DocraftLoomPdfCreator::set_metadata()``
   (see :doc:`../getting_started`) rather than from the ``.craft`` file.

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
