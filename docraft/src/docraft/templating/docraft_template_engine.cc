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

//
// Created by Matteo on 17/02/26.
//

#include "docraft/templating/docraft_template_engine.h"

#include <algorithm>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "docraft/docraft_color.h"
#include "docraft/utils/docraft_logger.h"
#include "docraft/utils/docraft_base64.h"
#include "docraft/utils/docraft_parser_utilis.h"

namespace docraft::templating {
    using exception::TemplateImageDataException;
    using exception::TemplateVariableExistsException;
    using exception::TemplateVariableNotFoundException;

    void DocraftTemplateEngine::add_template_variable(const std::string &name, const std::string &value) {
        auto normalized_name = normalize_name(name);
        if (has_template_variable(normalized_name)) {
            throw TemplateVariableExistsException(fmt::format("Template variable '{}' already exists.", name));
        }
        template_variables_.insert({normalized_name, value});
    }

    std::string DocraftTemplateEngine::find_template_variable(const std::string &name) const {
        auto normalized_name = normalize_name(name);
        auto it = template_variables_.find(normalized_name);
        if (it == template_variables_.end()) {
            throw TemplateVariableNotFoundException(fmt::format("Template variable '{}' not found.", name));
        }
        return it->second;
    }

    void DocraftTemplateEngine::clear_template_variables() {
        template_variables_.clear();
    }

    void DocraftTemplateEngine::remove_template_variable(const std::string &name) {
        auto normalized_name = normalize_name(name);
        auto it = template_variables_.find(normalized_name);
        if (it == template_variables_.end()) {
            throw TemplateVariableNotFoundException(fmt::format("Template variable '{}' not found.", name));
        }
        template_variables_.erase(it);
    }

    int DocraftTemplateEngine::items() const {
        return template_variables_.size();
    }

    bool DocraftTemplateEngine::has_template_variable(const std::string &name) const {
        return template_variables_.contains(normalize_name(name));
    }

    std::string DocraftTemplateEngine::render_template_string_foreach_item(const std::string &text,
                                                                           const nlohmann::json &item) const {
        std::string result = text;
        size_t pos = 0;
        while ((pos = result.find("${", pos)) != std::string::npos) {
            size_t end_pos = result.find("}", pos);
            if (end_pos == std::string::npos) {
                break;
            }
            std::string variable_expression = result.substr(pos, end_pos - pos + 1);
            std::string variable_name = result.substr(pos + 2, end_pos - pos - 2);
            std::string variable_value;
            try {
                // Handle data("fieldName") syntax for foreach item fields using the utility method
                if (variable_name.rfind("data(", 0) == 0) {
                    variable_value = utils::DocraftParserUtilis::extract_data_attribute(variable_expression, item);
                    if (variable_value.empty()) {
                        LOG_WARNING("Template variable '" + variable_name + "' not found in foreach item.");
                    }
                } else if (has_template_variable(variable_name)) {
                    // Handle normal template variables if they are used in foreach item templates
                    variable_value = find_template_variable(variable_name);
                } else {
                    LOG_WARNING("Template variable '" + variable_name + "' not found in template engine.");
                    variable_value = text;
                }
            } catch (...) {
                LOG_ERROR("Template variable '" + variable_name + "' not found");
                variable_value = "";
            }
            result.replace(pos, end_pos - pos + 1, variable_value);
            pos += variable_value.length();
        }
        return result;
    }
    std::string DocraftTemplateEngine::render_template_string(const std::string &text,
                                                              const nlohmann::json *foreach_item) {
        if (foreach_item != nullptr) {
            return render_template_string_foreach_item(text, *foreach_item);
        }
        std::string result = text;
        size_t pos = 0;
        while ((pos = result.find("${", pos)) != std::string::npos) {
            size_t end_pos = result.find("}", pos);
            if (end_pos == std::string::npos) {
                break;
            }
            std::string variable_name = result.substr(pos + 2, end_pos - pos - 2);
            std::string variable_value;
            try {
                if (has_template_variable(variable_name)) {
                    variable_value = find_template_variable(variable_name);
                } else {
                    LOG_WARNING("Template variable '" + variable_name + "' not found in template engine.");
                    variable_value = text;
                }
            } catch (...) {
                LOG_ERROR("Template variable '" + variable_name + "' not found");
                variable_value = "";
            }
            result.replace(pos, end_pos - pos + 1, variable_value);
            pos += variable_value.length();
        }
        return result;
    }

    void DocraftTemplateEngine::add_image_data(const std::string &image_id,
                                               const std::vector<unsigned char> &data,
                                               int width,
                                               int height) {
        auto name = normalize_name(image_id);
        if (image_data_.contains(name)) {
            throw TemplateImageDataException(fmt::format("Image data for '{}' already exists.", image_id));
        }
        if (width <= 0 || height <= 0) {
            throw TemplateImageDataException(fmt::format("Image data for '{}' has invalid dimensions.", image_id));
        }
        //emplace the key also in the standard unordered_map with the same normalized name to ensure consistency
        template_variables_.insert({name, name});
        image_data_.emplace(name, RawImageData{.data = data, .width = width, .height = height});
    }

    const DocraftTemplateEngine::RawImageData &DocraftTemplateEngine::get_image_data(
        const std::string &image_id) const {
        auto name = normalize_name(image_id);
        auto it = image_data_.find(name);
        if (it == image_data_.end()) {
            throw TemplateImageDataException(fmt::format("Image data for '{}' not found.", image_id));
        }
        return it->second;
    }

    void DocraftTemplateEngine::add_base64_image_data(const std::string &image_id,
                                                      std::string_view base64,
                                                      int width,
                                                      int height) {
        auto decoded = utils::decode_base64(base64);
        const auto expected_size =
                static_cast<size_t>(width) * static_cast<size_t>(height) * 3U; //3 bytes per pixel for RGB
        if (decoded.size() != expected_size) {
            throw TemplateImageDataException(
                fmt::format("Base64 image '{}' size does not match dimensions (RGB expected).", image_id));
        }
        add_image_data(image_id, decoded, width, height);
    }

    std::string DocraftTemplateEngine::normalize_name(const std::string &name) {
        std::string lower_case_name = name;
        std::ranges::transform(lower_case_name, lower_case_name.begin(), ::tolower);
        return lower_case_name;
    }
} // docraft
