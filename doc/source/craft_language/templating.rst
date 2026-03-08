Templating
==========

Docraft includes a built-in template engine that replaces placeholders in
the document tree with runtime values. This enables data-driven PDF
generation from a single Craft Language template.

Variable Substitution
---------------------

Register variables via ``DocraftTemplateEngine::add_template_variable()`` and
reference them with ``${name}`` syntax:

.. code-block:: xml

   <Text>Invoice for ${customer_name}</Text>

Foreach Loops
-------------

The ``<Foreach>`` element repeats its children for each item in a JSON
array. Inside the loop, use ``${data("field")}`` to access item properties.

.. code-block:: xml

   <Foreach model="${items}">
     <Layout orientation="horizontal">
       <Text weight="1">${data("name")}</Text>
       <Text weight="1">${data("price")}</Text>
     </Layout>
   </Foreach>

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - Attribute
     - Type
     - Description
   * - ``model``
     - string
     - Template variable containing a JSON array.

Image Data Injection
--------------------

Raw RGB images can be injected at runtime and referenced in templates:

.. code-block:: cpp

   engine->add_image_data("chart", pixel_data, 400, 300);
   // or from base64
   engine->add_base64_image_data("chart", base64_string, 400, 300);

.. code-block:: xml

   <Image data="chart" data_width="400" data_height="300"
          width="200" height="150"/>

Templated Colors
----------------

Color attributes can also use template expressions:

.. code-block:: xml

   <Text color="${status_color}">Status: ${status}</Text>

The template engine resolves the color at render time from the registered
variable value (hex string or named color).
