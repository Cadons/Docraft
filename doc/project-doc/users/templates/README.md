# User Templates

These templates are designed for application users of the Docraft library.

## Files

- `hello-world.craft`: minimal working example.
- `basic-report.craft`: multi-section report with header/footer/page number.
- `invoice-foreach.craft`: invoice layout using template variables and `Foreach`.
- `table-json.craft`: table populated from JSON template variables.

## Required variables for `invoice-foreach.craft`

- `invoice_number`: string
- `customer_name`: string
- `invoice_date`: string
- `total`: string
- `items`: JSON array of objects, for example:

```json
[
  {"name":"Keyboard", "qty":1, "price":"49.99"},
  {"name":"Mouse", "qty":2, "price":"29.99"}
]
```

## Required variables for `table-json.craft`

- `products_header`: JSON array, for example:

```json
["Item", "Price"]
```

- `products_rows`: JSON matrix, for example `[["Keyboard","49.99"],["Mouse","29.99"]]`
