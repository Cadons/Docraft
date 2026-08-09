# Templating

Engine: `DocraftTemplateEngine`, shared by both pipelines. Variable names are **case-insensitive**
(normalized to lowercase internally).

## `${variable}`

- Scans for every `${...}` occurrence via a simple first-`${`/first-`}` match — a variable
  expression **cannot contain a literal `}`**.
- If the variable isn't registered, a warning is logged and the **literal `${...}` text is left
  unchanged** in the output — not an empty string, not a parse error.
- Binding JSON data flattens objects into dotted keys: `{"user":{"name":"Bob"}}` registers
  `user.name` → `"Bob"`, so `${user.name}` works. Non-string leaf values (numbers/bools/null/
  arrays) register as their compact JSON text — an array field can be used directly as a
  `<Foreach model="${field}">` value.
- Applies to: Text/Title/Subtitle/PageNumber text content, `<Image src="...">`,
  `<Foreach model="...">`, `<Table model="...">`/`header="..."`, `<Series model="...">`,
  `<Chart title/x_label/y_label>`, and **any color attribute** (`color`, `background_color`,
  `border_color`) on any node — color resolution happens after templating runs.

## `${data("field")}` — only inside a `<Foreach>` iteration or a Table's per-object row template

- Syntax: `${data("field_name")}`. Also tolerated: unquoted `${data(field_name)}`, single-quoted
  `${data('field_name')}`, with internal whitespace trimmed.
- **Top-level field only** — cannot reach more than one JSON level deep (unlike `${variable}`,
  which supports arbitrary dotted paths). To reach a nested array, nest another `<Foreach>` (see
  below).
- Missing field resolves to an **empty string** (with a warning) — not the literal `${...}` text.
  This is the opposite fallback behavior from `${variable}`.
- Non-string field values stringify via their compact JSON dump.

## `<Foreach>` — element repetition

```xml
<Foreach n="3">              <!-- repeats children verbatim n times, no data binding -->
  <Text>Row</Text>
</Foreach>

<Foreach model='[{"name":"Alice"},{"name":"Bob"}]'>  <!-- one iteration per array item -->
  <Text>${data("name")}</Text>
</Foreach>

<Foreach model="${employees}">   <!-- model can itself be a ${...} expr resolving to a JSON array -->
  <Text>${data("name")}</Text>
</Foreach>
```

- Attributes: `model` XOR `n` — both, or neither, throws `InvalidInputException`.
- `n`: non-negative integer; negative throws; `n="0"` produces zero children.
- `model`: a JSON array literal, or a `${...}` expression resolving to one. Single-quoted-JSON
  inside a double-quoted XML attribute is the ".craft convention" (e.g.
  `model='[{"a":1}]'`) and normalizes the same as any other valid JSON. Must resolve to a JSON
  **array** or it throws.
- Every child of `<Foreach>` is rebuilt once per array item (or `n` times), with the current item
  bound for `${data(...)}` resolution during that iteration. Content before/after the `<Foreach>`
  in its parent is unaffected.

### Nesting (reaching data more than one level deep)

An inner `<Foreach model='${data("members")}'>` resolves its own `model` against the **outer**
Foreach's current item before JSON-parsing it — this is how you iterate a nested array field:

```xml
<Foreach model='[{"name":"Team A","members":[{"name":"Alice"},{"name":"Bob"}]}]'>
  <Text>${data("name")}</Text>
  <Foreach model='${data("members")}'>
    <Text>${data("name")}</Text>
  </Foreach>
</Foreach>
```

`${data(...)}` also resolves correctly inside color attributes
(`<Text color='${data("color")}'>`) and inside a `<Table>` nested inside the Foreach (its `model`
attribute, e.g. `model='[["${data("name")}"]]'`).

## No conditional directive

There is **no** `<If>`/`<When>`/comparison operator/ternary anywhere in the template engine. The
only control-flow primitives in Craft Language are:

- `<Foreach>` — repetition bound to a JSON array or a fixed count `n`.
- `visible` — a boolean common attribute, evaluated at build time (not `${...}`-templated
  itself), that drops one specific element (and its subtree) entirely if `false`.

If you need conditional content, precompute it into the data you bind (e.g. only include the
array items that should render) rather than trying to express a condition in markup.
