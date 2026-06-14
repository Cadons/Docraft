Exceptions
==========

Docraft provides a domain-based exception system under ``docraft::exception``.

The exception model is designed to:

- keep error types aligned with functional domains,
- avoid direct use of ``std::runtime_error`` and similar standard exception types,
- make API and test expectations explicit and stable.

Base Exception
--------------

All Docraft exceptions derive from ``DocraftException``.

.. doxygenclass:: docraft::exception::DocraftException
   :project: docraft
   :members:

Domain Overview
---------------

- **Configuration**: invalid command-line/configuration state.
- **FileSystem**: missing or unreadable files.
- **DataFormat**: invalid JSON/XML/data structures.
- **Input**: invalid values provided by caller or parsed attributes.
- **Template**: template variables and template image data errors.
- **Layout**: layout pipeline failures and configuration issues.
- **Document**: document/model lifecycle and state errors.
- **Backend**: backend capability/page/runtime state errors.
- **Rendering**: rendering-specific failures.
- **Implementation**: intentionally unimplemented features.

Domain Classes
--------------

Configuration
~~~~~~~~~~~~~

.. doxygenclass:: docraft::exception::ConfigurationException
   :project: docraft
   :members:

.. doxygenclass:: docraft::exception::MissingArgumentException
   :project: docraft
   :members:

FileSystem
~~~~~~~~~~

.. doxygenclass:: docraft::exception::FileSystemException
   :project: docraft
   :members:

.. doxygenclass:: docraft::exception::FileNotFoundException
   :project: docraft
   :members:

.. doxygenclass:: docraft::exception::CannotOpenFileException
   :project: docraft
   :members:

DataFormat
~~~~~~~~~~

.. doxygenclass:: docraft::exception::DataFormatException
   :project: docraft
   :members:

.. doxygenclass:: docraft::exception::InvalidJSONException
   :project: docraft
   :members:

Input
~~~~~

.. doxygenclass:: docraft::exception::InvalidInputException
   :project: docraft
   :members:

Template
~~~~~~~~

.. doxygenclass:: docraft::exception::TemplateException
   :project: docraft
   :members:

.. doxygenclass:: docraft::exception::TemplateVariableExistsException
   :project: docraft
   :members:

.. doxygenclass:: docraft::exception::TemplateVariableNotFoundException
   :project: docraft
   :members:

.. doxygenclass:: docraft::exception::TemplateImageDataException
   :project: docraft
   :members:

Layout
~~~~~~

.. doxygenclass:: docraft::exception::LayoutException
   :project: docraft
   :members:

.. doxygenclass:: docraft::exception::LayoutConfigurationException
   :project: docraft
   :members:

Document
~~~~~~~~

.. doxygenclass:: docraft::exception::DocumentException
   :project: docraft
   :members:

.. doxygenclass:: docraft::exception::DocumentStateException
   :project: docraft
   :members:

.. doxygenclass:: docraft::exception::ModelException
   :project: docraft
   :members:

Backend
~~~~~~~

.. doxygenclass:: docraft::exception::BackendStateException
   :project: docraft
   :members:

.. doxygenclass:: docraft::exception::CapabilityUnavailableException
   :project: docraft
   :members:

.. doxygenclass:: docraft::exception::PageStateException
   :project: docraft
   :members:

Rendering
~~~~~~~~~

.. doxygenclass:: docraft::exception::RenderingException
   :project: docraft
   :members:

.. doxygenclass:: docraft::exception::RenderingFailedException
   :project: docraft
   :members:

Implementation
~~~~~~~~~~~~~~

.. doxygenclass:: docraft::exception::NotImplementedException
   :project: docraft
   :members:

Usage Example
-------------

.. code-block:: cpp

   #include "docraft/exception/docraft_exception.h"

   void use_template(const docraft::templating::DocraftTemplateEngine& engine) {
       try {
           auto value = engine.find_template_variable("invoice_number");
           (void)value;
       } catch (const docraft::exception::TemplateVariableNotFoundException& ex) {
           // Handle only missing-variable errors
       } catch (const docraft::exception::DocraftException& ex) {
           // Fallback for all Docraft domain exceptions
       }
   }

Headers
-------

- ``docraft/exception/docraft_exception.h``: compatibility include for the full exception set.
- ``docraft/exception/docraft_exceptions.h``: master include for all exception domains.
- ``docraft/exception/docraft_*_exceptions.h``: domain-specific headers.

