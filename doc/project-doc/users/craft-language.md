# Docraft Craft Language and Library Usage Guide

This guide is for application developers using the Docraft library.

It covers:

- which library components are required and optional,
- how to write valid `.craft` files,
- metadata and automatic keyword extraction,
- how to manipulate the document DOM from C++ code,
- practical templates and examples.

## 1. What your application must use

### Required for `.craft`-driven generation

To render a `.craft` file, your application must use:

- `docraft::craft::DocraftCraftLanguageParser`
- `docraft::DocraftDocument` (returned by parser)
- `DocraftDocument::render()`

Minimal usage:

```cpp
#include "craft/docraft_craft_language_parser.h"

int main() {
    docraft::craft::DocraftCraftLanguageParser parser;
    parser.load_from_file("document.craft");

    auto document = parser.get_document();
    document->set_document_path("exports/reports"); // optional output directory
    document->set_document_title("output"); // output.pdf
    document->render();
    return 0;
}
```

### Optional but commonly used

- `docraft::templating::DocraftTemplateEngine`
  - for `${variable}` and `Foreach`.
- `docraft::DocraftDocumentMetadata`
  - for author/title/subject/keywords and PDF metadata control.
- `DocraftDocument::set_backend(...)`
  - to use a custom backend implementation instead of default PDF Haru backend.
- `DocraftDocument::set_document_path(...)`
  - to choose output folder while keeping `title` as file name.
- DOM APIs on `DocraftDocument`
  - to modify parsed content before render (`get_by_name`, `get_by_type`, `traverse_dom`, `add_node`).

Custom backend example:

```cpp
auto backend = std::make_shared<my_backend::MyRenderingBackend>();
document->set_backend(backend);
document->render();

// Restore default Haru backend
document->set_backend(nullptr);
```

Note:

- `render()` currently uses built-in `DocraftPDFRenderer`.
- Custom backends should be compatible with current renderer/painter backend contracts.
- `set_backend(nullptr)` resets backend selection to default behavior.

### Alternative (no `.craft`)

You can build document nodes directly from C++ without parser.

In that mode, `DocraftDocument` + `render()` are still used, but `DocraftCraftLanguageParser` is not required.

## 2. `.craft` structure you must follow

A valid file must include:

- `<Document>` root
- `<Body>` section
- optional root attribute: `path` (output directory)

Minimal valid file:

```xml
<Document path="exports/reports">
  <Body>
    <Text>Hello Docraft</Text>
  </Body>
</Document>
```

Rules to remember:

- tags are case-sensitive (`Document`, `Body`, `Text`, ...),
- `Header`, `Footer`, `Settings`, `Metadata` are optional,
- if `Header` or `Footer` is missing, body area is expanded automatically.

## 3. Main tags and components for users

Top-level sections:

- `Settings`: page format, section ratios, custom fonts.
- `Metadata`: PDF metadata + auto-keyword controls.
- `Header`: repeated top section.
- `Body`: required main section.
- `Footer`: repeated bottom section.

Most used content tags:

- text: `Title`, `Subtitle`, `Text`, `PageNumber`
- layout: `Layout`, `Rectangle`
- lists: `List`, `UList`
- table: `Table`, `THead`, `TBody`, `Row`, `Cell`, `HTitle`, `VTitle`
- graphics/media: `Image`, `Circle`, `Triangle`, `Line`, `Polygon`
- flow control: `NewPage`, `Blank`, `Foreach`

Common attributes (many nodes):

- `name`, `x`, `y`, `width`, `height`
- `padding`, `weight`, `z_index`
- `position="block|absolute"`
- `visible="true|false"`

Text attributes:

- `font_name`, `font_size`, `style`, `alignment`, `underline`, `color`

## 4. Templating in practice

Docraft supports variable placeholders and foreach expansion.

Example `.craft` snippet:

```xml
<Document>
  <Body>
    <Title>Invoice ${invoice_number}</Title>
    <Text>Customer: ${customer_name}</Text>

    <Foreach model="${items}">
      <Text>${data("name")} x ${data("qty")} = ${data("price")}</Text>
    </Foreach>
  </Body>
</Document>
```

C++ data binding example:

```cpp
#include "templating/docraft_template_engine.h"

auto engine = std::make_shared<docraft::templating::DocraftTemplateEngine>();
engine->add_template_variable("invoice_number", "INV-2026-0008");
engine->add_template_variable("customer_name", "Alice Rossi");
engine->add_template_variable(
    "items",
    R"([{"name":"Keyboard","qty":1,"price":"49.99"},{"name":"Mouse","qty":2,"price":"29.99"}])");

document->set_document_template_engine(engine);
```

## 5. Metadata and automatic keywords

Metadata can be set from `.craft`, from C++, or both.

### 5.1 Metadata in `.craft`

```xml
<Metadata>
  <DocumentTitle>Quarterly Report</DocumentTitle>
  <Author>Finance Team</Author>
  <Creator>My App</Creator>
  <Producer>Docraft</Producer>
  <Subject>Q1 2026</Subject>
  <Keywords>report,finance</Keywords>

  <AutoKeywords enabled="true" max_keywords="8" min_length="4" language="en,it" />
</Metadata>
```

Notes:

- `DocumentTitle` also influences output filename (unless overridden later).
- `AutoKeywords` can generate keywords from text nodes and merge with existing `Keywords`.

### 5.2 Metadata from C++

```cpp
#include "docraft_document.h"

docraft::DocraftDocumentMetadata metadata;
metadata.set_title("Quarterly Report");
metadata.set_author("Finance Team");
metadata.set_subject("Q1 2026");
metadata.set_keywords("report,finance");

document->set_document_metadata(metadata);
```

### 5.3 Auto-keyword extraction from C++

```cpp
#include "utils/docraft_keyword_extractor.h"

docraft::utils::DocraftKeywordExtractor::Config config;
config.max_keywords = 10;
config.min_length = 4;
config.stop_word_languages = {"en", "it"};

document->set_auto_keywords_config(config);
document->enable_auto_keywords(true);

// Optional manual call; render() already calls it when enabled.
document->refresh_auto_keywords();
```

Behavior summary:

- if auto-keywords are enabled, render pipeline extracts keywords from current DOM text;
- generated keywords are merged with existing metadata keywords;
- duplicates are removed.

## 6. DOM manipulation from C++ code

A common pattern is: parse `.craft`, then modify nodes programmatically before render.

### 6.1 Find nodes by name

```cpp
auto title_node = std::dynamic_pointer_cast<docraft::model::DocraftText>(
    document->get_first_by_name("report_title"));
if (title_node) {
    title_node->set_text("Q1 2026 - Final Version");
}
```

### 6.2 Find nodes by type

```cpp
auto texts = document->get_by_type<docraft::model::DocraftText>();
for (const auto &text : texts) {
    if (text->text().empty()) {
        text->set_visible(false);
    }
}
```

### 6.3 Traverse the full DOM

```cpp
document->traverse_dom([](const std::shared_ptr<docraft::model::DocraftNode> &node,
                          docraft::DocraftDomTraverseOp op) {
    if (op == docraft::DocraftDomTraverseOp::kEnter && node->node_name() == "debug_only") {
        node->set_visible(false);
    }
});
```

### 6.4 Add nodes from code

```cpp
auto extra_note = std::make_shared<docraft::model::DocraftText>("Generated by My App");
extra_note->set_font_size(10.0F);
document->add_node(extra_note);
```

This is useful when your app must enrich a static template with dynamic logic.

## 7. Practical examples

### 7.1 Basic report

```xml
<Document>
  <Metadata>
    <DocumentTitle>Sales Report</DocumentTitle>
    <Author>Finance Team</Author>
    <Subject>Quarterly Results</Subject>
  </Metadata>

  <Header margin_left="30" margin_right="30">
    <Text style="bold" font_size="14">Sales Report</Text>
  </Header>

  <Body margin_left="30" margin_right="30">
    <Title>Q1 2026</Title>
    <Text alignment="justified">
      This report summarizes the sales performance for the first quarter.
    </Text>

    <Subtitle>Highlights</Subtitle>
    <UList dot="circle">
      <Text>Revenue increased by 12%</Text>
      <Text>New enterprise customers: 18</Text>
      <Text>Average order value increased</Text>
    </UList>
  </Body>

  <Footer margin_left="30" margin_right="30">
    <PageNumber alignment="center" />
  </Footer>
</Document>
```

### 7.2 Table from JSON

```xml
<Document>
  <Body>
    <Title>Products</Title>
    <Table
      model='[["Keyboard","49.99"],["Mouse","29.99"],["Monitor","199.00"]]'
      header='["Item","Price"]' />
  </Body>
</Document>
```

## 8. Ready templates

Use the templates in `doc/users/templates/`:

- `hello-world.craft`
- `basic-report.craft`
- `invoice-foreach.craft`
- `table-json.craft`

## 9. Common errors and troubleshooting

- wrong tag case (`<document>` instead of `<Document>`),
- missing `<Body>`,
- invalid JSON in `Table model` or `Foreach model`,
- using both `src` and `data` attributes on the same `Image`,
- invalid enum values in attributes (`alignment`, `style`, `position`),
- `weight` outside `0..1` (weights must always stay in the inclusive range `0` to `1`),
- using `<Title>` inside tables (use `<HTitle>` for horizontal headers or `<VTitle>` for vertical titles).

## 10. Related docs

- Contributor architecture map: `doc/contributors/architecture.md`
- Component deep-dives: `doc/contributors/components/`
