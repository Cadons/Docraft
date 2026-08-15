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

#include "docraft/docraft_lib.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>

namespace docraft::templating {
        /**
         * @brief The DocraftTemplateEngine class stores template variables/image data and
         * resolves `${...}` expressions against them.
         *
         * Used by both pipelines: legacy walks the whole document tree calling
         * render_template_string() itself before layout/render, while
         * `docraft::loom::craft::DocraftLoomTreeBuilder` calls it per string as it builds
         * each node (Text/Title/Subtitle content, Image src, `<Foreach>`'s own `model`
         * attribute), passing the current Foreach iteration's item where one is in scope.
         */
        class DOCRAFT_LIB DocraftTemplateEngine {
        public:
                DocraftTemplateEngine() = default;
                /**
                 * @brief Adds a template variable.
                 * @param name Variable name (case-insensitive).
                 * @param value Variable value inserted into templates.
                 */
                void add_template_variable(const std::string &name, const std::string &value);
                /**
                 * @brief Retrieves a template variable value.
                 * @param name Variable name (case-insensitive).
                 * @return Stored value.
                 * @throws docraft::exception::DocraftException if not found.
                 */
                std::string find_template_variable(const std::string &name) const;
                /**
                 * @brief Clears all template variables.
                 */
                void clear_template_variables();
                /**
                 * @brief Removes a template variable by name.
                 * @param name Variable name (case-insensitive).
                 * @throws docraft::exception::DocraftException if not found.
                 */
                void remove_template_variable(const std::string &name);
                /**
                 * @brief Returns the number of stored template variables.
                 */
                int items() const;
                /**
                 * @brief Checks if a template variable exists.
                 * @param name Variable name (case-insensitive).
                 */
                bool has_template_variable(const std::string &name) const;

                /**
                 * @brief Sets strict mode: when enabled, an unresolved `${variable}` or
                 * `${data("field")}` encountered by render_template_string()/
                 * render_template_string_foreach_item() throws
                 * docraft::exception::TemplateVariableNotFoundException instead of logging a
                 * warning and leaving the placeholder text (or an empty string, for
                 * `data(...)`) in the output. Off by default -- lenient/interactive use is
                 * unaffected; callers that need a hard failure for automated pipelines (e.g.
                 * docraft_tool's `--strict` flag) opt in explicitly.
                 * @param strict Whether strict mode should be enabled.
                 */
                void set_strict(bool strict = true);
                /**
                 * @brief Returns whether strict mode is currently enabled.
                 */
                [[nodiscard]] bool is_strict() const;

                /**
                 * @brief Registers every field of a JSON object as a template variable,
                 * flattening nested objects into dot-notation keys -- e.g.
                 * `{"user":{"name":"Bob"}}` registers `"user.name"` -> `"Bob"`, so
                 * `${user.name}` resolves -- rather than registering the nested object as a
                 * single JSON-text variable. Strings are registered as-is; every other leaf
                 * value (numbers/bools/null/arrays) is registered as its compact JSON text --
                 * e.g. an array field becomes a JSON array string usable directly as a
                 * `<Foreach model="${field}">` attribute.
                 * @param json JSON object whose fields to register.
                 * @throws docraft::exception::DataFormatException if `json` is not a JSON
                 * object.
                 * @throws docraft::exception::TemplateVariableExistsException if a flattened
                 * key collides with one already registered.
                 */
                void add_template_variables_from_json(const nlohmann::json& json);

                /**
                 * @brief Struct to hold raw image data along with its dimensions.
                 */
                struct RawImageData {
                        std::vector<unsigned char> data;
                        int width = 0;
                        int height = 0;
                };

                /**
                 * @brief Adds raw RGB image data.
                 * @param image_id Image id used by \<Image data="..."\> in templates (case-insensitive).
                 * @param data Raw RGB bytes (3 bytes per pixel, row-major).
                 * @param width Pixel width.
                 * @param height Pixel height.
                 */
                void add_image_data(const std::string &image_id,
                                    const std::vector<unsigned char> &data,
                                    int width,
                                    int height);
                /**
                 * @brief Adds raw RGB image data from a base64 string.
                 * @param image_id Image id used by \<Image data="..."\> in templates (case-insensitive).
                 * @param base64 Base64 string with raw RGB bytes (no data URI prefix).
                 * @param width Pixel width.
                 * @param height Pixel height.
                 * @throws docraft::exception::DataFormatException if decoded size does not match width*height*3.
                 */
                void add_base64_image_data(const std::string &image_id,
                                           std::string_view base64,
                                           int width,
                                           int height);
                /**
                 * @brief Retrieves raw image data by id.
                 * @param image_id Image id (case-insensitive).
                 * @return RawImageData reference.
                 * @throws docraft::exception::DocraftException if not found.
                 */
                const RawImageData &get_image_data(const std::string &image_id) const;

                /**
                 * @brief Renders a template string, replacing every `${variable}` with its
                 * registered value. Has no notion of a Foreach iteration item -- callers
                 * that may or may not have one in scope (e.g.
                 * `docraft::loom::craft::DocraftLoomTreeBuilder`) decide which of this or
                 * render_template_string_foreach_item() to call. An unknown variable is
                 * left in the output exactly as written.
                 * @param text Template string to render.
                 * @return Rendered string with every resolvable `${variable}` replaced.
                 */
                std::string render_template_string(const std::string& text) const;

                /**
                 * @brief Renders a template string against one Foreach iteration's item,
                 * replacing every `${data("field")}` with that field of `item` and every
                 * `${variable}` with its registered value (`data(...)` is tried first).
                 * @note `data("field_name")` reads a top-level field only; `item` cannot be
                 * nested more than one level.
                 * @param text Template string to render.
                 * @param item JSON object supplying the current Foreach iteration's fields.
                 * @return Rendered string with every resolvable expression replaced.
                 */
                std::string render_template_string_foreach_item(const std::string &text,
                                                                const nlohmann::json &item) const;

            private:
                static std::string normalize_name(const std::string &name);

                /**
                 * @brief Recursion step for add_template_variables_from_json(): registers
                 * every field of `object`, prefixing each key with `prefix.` (or just the
                 * field's own key if `prefix` is empty).
                 */
                void add_json_object_fields(const std::string& prefix, const nlohmann::json& object);

                /**
                 * @brief Shared by render_template_string() and
                 * render_template_string_foreach_item(): scans `text` for `${...}`
                 * expressions and replaces each with whatever `resolve` returns for it.
                 * Owns the scanning/replacing and the exception safety net (a `resolve`
                 * that throws falls back to leaving the expression as written); `resolve`
                 * owns only the decision of what a single expression's value is.
                 * @param text Template string to scan.
                 * @param resolve Callable `(const std::string& expression, const
                 * std::string& variable_name) -> std::string` invoked once per `${...}`
                 * found; `expression` is the full `${...}` text, `variable_name` is the
                 * text between the braces.
                 * @return `text` with every `${...}` replaced by `resolve`'s result.
                 */
                template <typename Resolver>
                std::string scan_and_replace(const std::string &text, Resolver resolve) const;

                std::unordered_map<std::string, std::string> template_variables_;
                std::unordered_map<std::string, RawImageData> image_data_;
                bool strict_ = false;
        };
} // docraft
