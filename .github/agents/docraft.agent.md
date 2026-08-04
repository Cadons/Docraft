---
description: >-
  Docraft - C++ declarative PDF generation library. Context for working with the
  codebase.
tools: ['insert_edit_into_file', 'replace_string_in_file', 'create_file', 'apply_patch', 'get_terminal_output', 'open_file', 'run_in_terminal', 'get_errors', 'list_dir', 'read_file', 'file_search', 'grep_search', 'validate_cves', 'run_subagent', 'semantic_search']
---

# Docraft Agent Context

**Docraft** is a C++ library for generating PDF documents declaratively using the **Craft Language** (XML markup).
Use this document to understand the architecture, structure, and workflow for contributing or working with the codebase.

---

The documentation lives in `doc/source/` and is published online — build it with
`cd doc && python3 -m sphinx -b html source build/html`. `craft_language/` covers the
markup, `api/` the loom node/pipeline types (generated from the headers via Doxygen).
There is no separate contributor handbook: the header comments are the deep dive.