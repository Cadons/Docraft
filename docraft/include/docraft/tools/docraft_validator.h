/*
 * Copyright 2026 Matteo Cadoni (https://github.com/cadons)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include "docraft/docraft_lib.h"

namespace docraft::tools {
    /**
     * @brief A single diagnostic produced by DocraftValidator: either a hard error
     * (the .craft/data file is invalid) or a warning (looks suspicious but doesn't
     * block rendering, e.g. an unresolved `${variable}`).
     */
    struct DocraftValidationIssue
    {
        enum class Severity
        {
            kError,
            kWarning
        };

        Severity severity;
        std::string message;
    };

    /**
     * @brief Aggregated result of DocraftValidator::validate(): every diagnostic found,
     * in the order they were discovered.
     */
    struct DOCRAFT_LIB DocraftValidationResult
    {
        std::vector<DocraftValidationIssue> issues;

        /** @brief True if at least one kError diagnostic is present. */
        [[nodiscard]] bool has_errors() const;
        /** @brief True if at least one kWarning diagnostic is present. */
        [[nodiscard]] bool has_warnings() const;
    };

    /**
     * @brief Lints a `.craft` file (and its optional `--data` JSON file) without
     * rendering a PDF: well-formed XML, valid craft-language grammar (reusing
     * `docraft::craft::DocraftLoomCraftLanguageParser` so the linter can never drift
     * from what the real parser accepts), valid JSON for the data file, and
     * `${variable}` references that can't be resolved against the data file's fields.
     */
    class DOCRAFT_LIB DocraftValidator
    {
    public:
        DocraftValidator() = default;

        /**
         * @brief Validates a `.craft` file and, if given, its `--data` JSON file.
         * @param craft_file Path to the `.craft` file to validate.
         * @param data_file Optional path to a JSON data file (same contract as
         * `docraft_tool`'s `--data` option).
         * @param strict When true, a `${variable}` reference that can't be resolved
         * against `data_file` is reported as an error (kError) instead of a warning
         * (kWarning) -- mirrors `docraft_tool`'s `--strict` flag, letting pipelines
         * gate on data-binding mistakes that would otherwise leak into the output.
         * @return Every error/warning found; empty when the input is fully valid.
         */
        [[nodiscard]] DocraftValidationResult validate(
            const std::filesystem::path& craft_file,
            const std::optional<std::filesystem::path>& data_file = std::nullopt,
            bool strict = false) const;
    };
} // namespace docraft::tools
