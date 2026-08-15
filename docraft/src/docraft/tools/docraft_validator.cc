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

#include "docraft/tools/docraft_validator.h"

#include <algorithm>
#include <fstream>
#include <memory>
#include <sstream>

#include <fmt/format.h>
#include <nlohmann/json.hpp>

#include "docraft/craft/docraft_loom_craft_language_parser.h"
#include "docraft/exception/docraft_exceptions.h"
#include "docraft/templating/docraft_template_engine.h"

namespace docraft::tools {
    namespace {
        constexpr std::string_view kCraftExtension = ".craft";

        bool has_craft_extension(const std::filesystem::path& file_path)
        {
            std::string extension = file_path.extension().string();
            std::ranges::transform(extension, extension.begin(), [](const unsigned char ch) {
                return static_cast<char>(std::tolower(ch));
            });
            return extension == kCraftExtension;
        }

        void add_error(DocraftValidationResult& result, const std::string& message)
        {
            result.issues.push_back({DocraftValidationIssue::Severity::kError, message});
        }

        void add_warning(DocraftValidationResult& result, const std::string& message)
        {
            result.issues.push_back({DocraftValidationIssue::Severity::kWarning, message});
        }

        std::optional<std::string> read_file(const std::filesystem::path& path)
        {
            std::ifstream file(path);
            if (!file)
            {
                return std::nullopt;
            }
            std::stringstream buffer;
            buffer << file.rdbuf();
            return buffer.str();
        }

        // Builds a template engine from a JSON data file's top-level fields, reporting
        // any failure (missing file, invalid JSON, non-object top level) as a
        // validation error instead of throwing -- mirrors docraft_tool's --data
        // handling, but never aborts the run.
        std::shared_ptr<docraft::templating::DocraftTemplateEngine> build_template_engine(
            const std::filesystem::path& data_file, DocraftValidationResult& result)
        {
            if (!std::filesystem::exists(data_file))
            {
                add_error(result, fmt::format("Data file not found: {}", data_file.string()));
                return nullptr;
            }

            const auto content = read_file(data_file);
            if (!content)
            {
                add_error(result, fmt::format("Could not open data file: {}", data_file.string()));
                return nullptr;
            }

            nlohmann::json root;
            try
            {
                root = nlohmann::json::parse(*content);
            }
            catch (const nlohmann::json::exception& ex)
            {
                add_error(result, fmt::format("Invalid JSON in data file {}: {}", data_file.string(), ex.what()));
                return nullptr;
            }

            if (!root.is_object())
            {
                add_error(result,
                          fmt::format("Data file must contain a JSON object at the top level: {}",
                                      data_file.string()));
                return nullptr;
            }

            auto engine = std::make_shared<docraft::templating::DocraftTemplateEngine>();
            try
            {
                engine->add_template_variables_from_json(root);
            }
            catch (const docraft::exception::DocraftException& ex)
            {
                add_error(result, fmt::format("Invalid data file {}: {}", data_file.string(), ex.what()));
                return nullptr;
            }
            return engine;
        }

        // Scans `craft_text` for `${...}` expressions (skipping `${data(...)}`, which is
        // only resolvable per-Foreach-item and can't be checked statically here) and
        // warns about every one not registered in `engine`.
        void warn_about_unresolved_variables(const std::string& craft_text,
                                              const docraft::templating::DocraftTemplateEngine& engine,
                                              DocraftValidationResult& result)
        {
            size_t pos = 0;
            while ((pos = craft_text.find("${", pos)) != std::string::npos)
            {
                const size_t end_pos = craft_text.find('}', pos);
                if (end_pos == std::string::npos)
                {
                    break;
                }
                const std::string variable_name = craft_text.substr(pos + 2, end_pos - pos - 2);
                pos = end_pos + 1;

                if (variable_name.starts_with("data("))
                {
                    continue;
                }
                if (!engine.has_template_variable(variable_name))
                {
                    add_warning(result,
                               fmt::format("Undefined template variable '${{{}}}' referenced in .craft file "
                                           "(not found in data file)",
                                           variable_name));
                }
            }
        }
    } // namespace

    bool DocraftValidationResult::has_errors() const
    {
        return std::ranges::any_of(issues, [](const DocraftValidationIssue& issue) {
            return issue.severity == DocraftValidationIssue::Severity::kError;
        });
    }

    bool DocraftValidationResult::has_warnings() const
    {
        return std::ranges::any_of(issues, [](const DocraftValidationIssue& issue) {
            return issue.severity == DocraftValidationIssue::Severity::kWarning;
        });
    }

    DocraftValidationResult DocraftValidator::validate(
        const std::filesystem::path& craft_file, const std::optional<std::filesystem::path>& data_file) const
    {
        DocraftValidationResult result;

        std::shared_ptr<docraft::templating::DocraftTemplateEngine> engine;
        if (data_file)
        {
            engine = build_template_engine(*data_file, result);
        }

        if (!std::filesystem::exists(craft_file))
        {
            add_error(result, fmt::format("Craft file not found: {}", craft_file.string()));
            return result;
        }
        if (!has_craft_extension(craft_file))
        {
            add_error(result, fmt::format("Input file must have a .craft extension: {}", craft_file.string()));
            return result;
        }

        const auto craft_text = read_file(craft_file);
        if (!craft_text)
        {
            add_error(result, fmt::format("Could not open craft file: {}", craft_file.string()));
            return result;
        }

        docraft::craft::DocraftLoomCraftLanguageParser parser;
        if (engine)
        {
            parser.set_template_engine(engine);
        }
        try
        {
            parser.parse(*craft_text);
        }
        catch (const docraft::exception::DocraftException& ex)
        {
            add_error(result, fmt::format("{}: {}", craft_file.string(), ex.what()));
            return result;
        }

        if (engine)
        {
            warn_about_unresolved_variables(*craft_text, *engine, result);
        }

        return result;
    }
} // namespace docraft::tools
