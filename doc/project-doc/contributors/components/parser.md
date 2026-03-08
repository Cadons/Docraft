# Parser

The parser subsystem translates `.craft` XML into typed model nodes.

## 1. Main architecture

Parsing is split into:

- a coordinator: `DocraftCraftLanguageParser`;
- a parser registry (`std::unordered_map<std::string, std::unique_ptr<IDocraftParser>>`);
- one parser implementation per node/tag family.

The coordinator handles document-level concerns (root, sections, metadata, settings). Individual parsers handle node-specific attributes and constraints.

## 2. How parsing actually works

### 2.1 Top-level document loading

`load_document()` performs:

- root validation (`<Document>`);
- metadata parsing (`<Metadata>`);
- optional settings parsing;
- optional header parsing;
- required body parsing;
- optional footer parsing.

### 2.2 Recursive node parsing

`parse_node(xml_node)`:

1. resolves parser by tag name;
2. creates node via `parser->parse(...)`;
3. if node is a container, recursively parses child elements;
4. applies special rules (example: list allows only `Text` items).

`Foreach` is special: parser stores template nodes and avoids regular child recursion at parse time.

## 3. Tag and attribute handling

Most parsers share attribute wiring through helper functions in `docraft_parser_helpers.cc`.

Shared attribute handling includes:

- geometry and flow (`x`, `y`, `width`, `height`, `weight`, `position`, `padding`),
- visibility and ordering (`visible`, `z_index`),
- naming (`name`),
- color decoding (named colors and hex values).

This central helper approach keeps attribute behavior consistent across nodes.

## 4. Error behavior

The parser is intentionally strict in important places:

- missing required structural tags (for example `Body`) cause hard errors;
- unknown node tags in parsed tree cause hard errors;
- invalid enum-like values (alignment/style/orientation/position) cause hard errors;
- invalid JSON payloads for table/foreach templating cause hard errors.

This is useful for contributors because parser failures are deterministic and easy to test.

## 5. Metadata parsing model

Metadata parsing is document-level, not node-level.

Supported fields include title/author/keywords/date values, plus auto-keyword controls. Parsed metadata is stored in `DocraftDocumentMetadata` and can be merged later with extracted keywords.

## 6. Adding a new tag (contributor workflow)

1. Add token constants if needed (`docraft_craft_language_tokens.h`).
2. Implement a parser class deriving `IDocraftParser`.
3. Register it in `DocraftCraftLanguageParser` constructor.
4. Add model node and draw behavior.
5. Add tests:
   - accepted syntax,
   - invalid syntax,
   - interactions with container recursion.

Keep parser classes narrow: parse only XML-to-model concerns, not layout or backend logic.
