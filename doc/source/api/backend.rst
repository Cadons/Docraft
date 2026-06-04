Rendering Backend
=================

The backend layer defines the low-level contracts used by Docraft to render documents. It is organized around small capability interfaces for drawing text,
shapes, lines, images, and pages, plus provider interfaces that group related capabilities.

Concrete backends, such as the libharu-based PDF backend, implement these contracts and expose them through capability providers.

Capability Providers
--------------------

Capability providers group backend functionality by responsibility. They allow Docraft services to request only the capabilities they need while keeping the individual rendering interfaces independent and focused.

IDocraftRenderingCapabilityProvider
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Provides access to rendering capabilities for lines, text, shapes, images, and pages.

.. doxygenclass:: docraft::backend::IDocraftRenderingCapabilityProvider
   :project: docraft
   :members:

IDocraftResourceCapabilityProvider
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Provides access to resource-related capabilities, such as font handling.

.. doxygenclass:: docraft::backend::IDocraftResourceCapabilityProvider
   :project: docraft
   :members:

IDocraftLifecycleCapabilityProvider
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Provides access to document lifecycle capabilities, such as output persistence and metadata handling.

.. doxygenclass:: docraft::backend::IDocraftLifecycleCapabilityProvider
   :project: docraft
   :members:

Rendering Capability Interfaces
-------------------------------

Rendering capability interfaces are standalone and chain-free. Each interface covers one drawing responsibility.

IDocraftTextRenderingBackend
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenclass:: docraft::backend::IDocraftTextRenderingBackend
   :project: docraft
   :members:

IDocraftShapeRenderingBackend
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenclass:: docraft::backend::IDocraftShapeRenderingBackend
   :project: docraft
   :members:

IDocraftLineRenderingBackend
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenclass:: docraft::backend::IDocraftLineRenderingBackend
   :project: docraft
   :members:

IDocraftImageRenderingBackend
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenclass:: docraft::backend::IDocraftImageRenderingBackend
   :project: docraft
   :members:

IDocraftPageRenderingBackend
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenclass:: docraft::backend::IDocraftPageRenderingBackend
   :project: docraft
   :members:

Resource and Lifecycle Interfaces
---------------------------------

These interfaces define supporting backend responsibilities used by document rendering and export.

IDocraftFontBackend
^^^^^^^^^^^^^^^^^^^

.. doxygenclass:: docraft::backend::IDocraftFontBackend
   :project: docraft
   :members:

IDocraftOutputBackend
^^^^^^^^^^^^^^^^^^^^^

.. doxygenclass:: docraft::backend::IDocraftOutputBackend
   :project: docraft
   :members:

IDocraftMetadataBackend
^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenclass:: docraft::backend::IDocraftMetadataBackend
   :project: docraft
   :members:

Provider Factories
------------------

Factories create the provider set used by the rendering service.

DocraftCapabilityProviders
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenstruct:: docraft::backend::DocraftCapabilityProviders
   :project: docraft
   :members:

IDocraftCapabilityProvidersFactory
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenclass:: docraft::backend::IDocraftCapabilityProvidersFactory
   :project: docraft
   :members:

Concrete Backends
-----------------

DocraftHaruBackend
^^^^^^^^^^^^^^^^^^

PDF backend implementation using libharu. It composes capability-focused internal objects and exposes them through the backend provider model.

.. doxygenclass:: docraft::backend::pdf::DocraftHaruBackend
   :project: docraft
   :members:
