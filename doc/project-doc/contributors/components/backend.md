# Backend Contracts and Haru Implementation

The backend layer is the portability boundary for output targets.

## 1. Contract hierarchy

`IDocraftRenderingBackend` aggregates these contracts:

- `IDocraftTextRenderingBackend`
- `IDocraftShapeRenderingBackend`
- `IDocraftImageRenderingBackend`
- `IDocraftPageRenderingBackend`

Plus lifecycle methods:

- `save_to_file(...)`
- `file_extension()`
- font registration/selection helpers
- metadata application

This design lets renderers/painters consume only the primitives they need.

## 2. Concrete implementation: Haru backend

`DocraftHaruBackend` implements all contracts over libharu.

Responsibilities include:

- managing document/page handles,
- text drawing and measurement,
- line/shape/image drawing,
- page navigation and page format,
- metadata mapping to PDF info fields,
- save to `.pdf`.

## 3. State model

Backends are stateful:

- current page pointer,
- current font and encoder,
- graphics state (line width/color/fill/stroke alpha).

Context lifetime is therefore the rendering session boundary.

## 4. Font contract details

Backend exposes font operations because text measurement and rendering depend on selected font/encoder.

`DocraftFontApplier` coordinates font selection and backend registration.

Contributors modifying font behavior should validate both rendering and measurement consistency.

## 5. Metadata handling

Metadata is backend-applied at the end of render.

For Haru, this maps to PDF info fields and date structures.

If adding another backend format, explicitly decide which metadata fields are supported and document unsupported fields.
