Images
======

The ``<Image>`` element renders bitmap content from a file or from raw data
supplied by the template engine.

From File
---------

.. code-block:: xml

   <Image src="logo.png" width="150" height="80"/>

From Template Data
------------------

When using the template engine, images can be injected as raw RGB data:

.. code-block:: xml

   <Image data="chart_image" data_width="400" data_height="300"
          width="200" height="150"/>

The ``data`` attribute references an image id registered via
``DocraftTemplateEngine::add_image_data()`` or
``DocraftTemplateEngine::add_base64_image_data()``.

Attributes
----------

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - Attribute
     - Type
     - Description
   * - ``src``
     - string
     - Path to the image file (PNG or JPEG).
   * - ``data``
     - string
     - Template image id for raw data injection.
   * - ``data_width``
     - int
     - Pixel width of the raw data image.
   * - ``data_height``
     - int
     - Pixel height of the raw data image.

Supported formats: **PNG**, **JPEG**, and **raw RGB** (via template engine).

