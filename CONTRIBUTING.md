# Contributing to Docraft

Thanks for your interest in contributing to Docraft.
This guide explains the expected workflow, coding style, and quality checks for pull requests.

## Quick Checklist

- [ ] Discuss larger changes in an issue before implementation.
- [ ] Keep each PR focused on one logical change.
- [ ] Build the project locally with CMake.
- [ ] Run tests and keep them green.
- [ ] Update docs when behavior or public APIs change.
- [ ] Use the commit message format from `.github/git-commit-instructions.md`.

## Development Setup

Docraft uses CMake and C++23.

### Prerequisites

- CMake >= 3.16
- A C++23 compiler (GCC/Clang/MSVC with C++23 support)
- Dependencies:
  - `libharu`
  - `pugixml`
  - `nlohmann-json`
  - `gtest` (for tests)

### Configure and build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -- -j"$(nproc)"
```

### Run tests

```bash
ctest --test-dir build -C Release --output-on-failure
```

## Branch and PR Workflow

- Create a feature branch from the main development branch.
- Open a PR early if you want feedback on design or direction.
- Prefer small, reviewable PRs.
- If your PR changes behavior, include before/after details in the description.
- Link related issues (for example: `Closes #123`).

## Commit Message Convention

Follow `.github/git-commit-instructions.md`.

Use the first line format:

```text
<type>(<scope>): <short summary>
```

Common types: `feat`, `fix`, `docs`, `refactor`, `test`, `chore`, `build`, `ci`.

Examples:

- `feat(layout): add default bottom padding for rectangles with children`
- `fix(parser): validate missing attribute in table headers`
- `docs(contributing): document local test workflow`

## Coding Style (C++)

Docraft enforces static analysis with `.clang-tidy` (warnings are treated as errors in that config).

### Language and general rules

- Use C++23 features where they improve clarity and safety.
- Keep code explicit and readable over clever/compact patterns.
- Prefer small functions and focused classes.
- Avoid unrelated refactors in the same PR.

### Naming conventions (from `.clang-tidy`)

- Namespaces: `lower_case`
- Classes/structs: `CamelCase`
- Variables and members: `lower_case`
- Private/protected/class members: trailing underscore (`name_`)
- Constants: `kCamelCase` (for static/member/global/constexpr constants)
- Enum constants: `kCamelCase`

### Includes and headers

- Keep includes minimal and relevant.
- Prefer project include paths consistent with existing code.
- Public headers belong in `docraft/include/`.
- Implementations belong in `docraft/src/`.

## Testing Expectations

- Add or update tests for any behavior change.
- Place tests under `docraft/test/` in the matching module folder.
- Keep tests deterministic and independent.
- Prefer meaningful test names describing behavior.

Typical test command:

```bash
ctest --test-dir build -C Release --output-on-failure --output-junit test-results.xml
```

## Documentation Expectations

Update documentation when you change:

- Craft language syntax
- Public API behavior
- Build or setup instructions
- User-visible output or examples

Relevant docs locations:

- `README.md`
- `doc/source/`
- `doc/source/examples/`

## License and Ownership

By contributing, you agree your contribution is provided under the project license (Apache-2.0).
See `LICENSE` for details.

## Need Help?

- Open an issue for design questions or unclear behavior.
- Ask for guidance in your PR description when touching architecture-level components.

