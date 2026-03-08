# Model and DOM

The model subsystem defines Docraft's in-memory document graph.

## 1. Conceptual model

A document is a list of root nodes (`Header`, `Body`, `Footer` in practice). Nodes can be leaves or containers.

Every node derives from `DocraftNode`, which itself carries geometry through `DocraftTransform`.

This design means layout data and render behavior stay attached to node instances during the whole pipeline.

## 2. Core classes

- `DocraftNode`: base node contract + common properties.
- `DocraftChildrenContainerNode`: child ownership and ordered drawing.
- `DocraftTransform`: position, dimensions, anchors, content anchors.
- Structural nodes: sections, layouts, rectangles.
- Content nodes: text, tables, images, lists, shapes, page number.
- Control node: `DocraftForeach`.
- Settings node: `DocraftSettings`.

## 3. Cross-cutting node properties

All nodes can carry:

- geometry (`x/y/width/height/padding`),
- flow mode (`block` vs `absolute`),
- visibility,
- z-index,
- page ownership (`page_owner`).

`page_owner` is crucial for pagination: draw phase checks whether the node should render on current page.

## 4. DOM traversal and mutation API

`DocraftDocument` offers utility APIs to query and mutate the graph:

- `get_by_name`, `get_first_by_name`, `get_last_by_name`,
- `get_by_type<T>()`,
- `traverse_dom(callback)`.

Because nodes are mutable shared pointers, these APIs are used both by internal stages and by application code for runtime customization.

## 5. Clone behavior

Many nodes implement cloning (`IDocraftClonable`) because templating (`Foreach`) duplicates template nodes.

A good clone implementation must:

- preserve node-local properties,
- deep-clone children when needed,
- avoid sharing mutable child state accidentally.

## 6. Practical contributor guidance

When adding/changing node types:

- keep model class backend-agnostic;
- implement `draw(context)` as delegation, not direct backend calls;
- implement `clone()` if node can appear under templating/duplication paths;
- ensure layout and parser invariants are explicit (throw early on invalid values).
