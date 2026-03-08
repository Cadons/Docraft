# Templating

The templating subsystem transforms parsed nodes using runtime data.

## 1. Responsibility boundaries

`DocraftTemplateEngine` is responsible for:

- scalar variable substitution (`${name}`),
- foreach expansion (`Foreach model="..."`),
- table JSON hydration (`model`, `header` placeholders),
- image raw-data resolution by key.

It is intentionally not a full template language parser. It applies a pragmatic placeholder model on existing node structures.

## 2. Execution model

Templating runs on the in-memory nodes before layout.

This is important: layout sees final text lengths, final row counts, and expanded foreach content.

## 3. Foreach semantics

Foreach model is expected to resolve to a JSON array.

For each item:

1. template nodes are cloned;
2. `${data("field")}` placeholders are resolved against current item;
3. rendered clones are appended as actual foreach children.

After expansion, template nodes are cleared from the foreach node to avoid reprocessing.

## 4. Table and image templating

- Table can receive row/header JSON via template placeholders.
- Image nodes can resolve raw data IDs from template variables.
- This allows dynamic content without changing parser structure.

## 5. Failure modes

Common failure modes contributors should preserve:

- unresolved variables produce warnings and fallback behavior;
- invalid JSON in foreach/table paths should fail fast;
- type mismatch (expected array/object) should fail with explicit errors.

## 6. Contribution guidelines

When extending templating:

- keep transformations deterministic and explicit;
- avoid hidden global state;
- avoid introducing layout logic in templating stage;
- add tests for nested foreach, mixed placeholders, and malformed input.
