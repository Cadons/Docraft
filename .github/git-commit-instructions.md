# Commit message rules (for humans and Copilot)

Purpose
---
This document defines the expected commit message format for this repository. It is written in English and follows a Conventional Commits-like style. These rules are intended to make history clear and machine-parsable (for changelogs, CI, and code review).

Commit message format
---
First line (required):

<type>(<scope>): <short summary>

- type: one of feat, fix, docs, style, refactor, perf, test, chore, build, ci, revert
- scope: optional, short identifier (module, component, file), no spaces unless necessary
- short summary: imperative, concise (~74 characters recommended)

Optional body:
- Provide motivation, context, and details that don't fit in the summary.

Optional footer:
- Use to reference issues (e.g. Closes #123) or breaking changes:
  BREAKING CHANGE: description of the breaking change

Examples
---
- feat(parser): support relative font paths from .craft files
- fix(renderer): avoid fallback to Helvetica when custom font registered
- docs: add instructions for commit message rules

Best practices
---
- Keep commits small and focused (one logical change per commit).
- Use the imperative mood in the subject line ("Add", "Fix", "Update").
- If you use Copilot or another AI assistant, start the first line with the type and optional scope (e.g. `feat(parser): `) and let the assistant suggest the rest; always review and edit the suggested message for accuracy and brevity.
- Use the body to explain why the change was made if it's not obvious from the diff.

Hooks and templates
---
Per project policy, Git hooks are not used or enforced in this repository. Do not install or rely on local commit hooks.

However, we provide a commit message template in `.github/commit_template.txt` as a helpful guide. You can configure your local Git client to use that template if desired:

git config --local commit.template .github/commit_template.txt

If your team prefers to enforce commit message rules via CI (recommended for consistent behavior across contributors), add a CI check that validates commit messages on pull requests.

If there is an authoritative `.aiassistant` file with different commit rules, please attach it or paste its contents and I will make this document match exactly.

Quick checklist before committing
---
- [ ] Is the first line in the form `<type>(<scope>): <summary>`?
- [ ] Is the summary imperative and concise?
- [ ] Does the body explain motivation or important context if needed?
- [ ] Are breaking changes listed in the footer as `BREAKING CHANGE:`?

Thank you — clear commit messages make collaboration and automation easier.
