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
#include <format>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "docraft/docraft_color.h"
#include "docraft/model/docraft_children_container_node.h"
#include "docraft/model/docraft_clone_utils.h"
#include "docraft/model/docraft_foreach.h"
#include "docraft/model/docraft_image.h"
#include "docraft/model/docraft_list.h"
#include "docraft/model/docraft_rectangle.h"
#include "docraft/model/docraft_table.h"
#include "docraft/model/docraft_text.h"
#include "docraft/utils/docraft_logger.h"
#include "docraft/utils/docraft_base64.h"
#include "docraft/utils/docraft_parser_utilis.h"

namespace docraft::templating {
    void DocraftTemplateEngine::template_nodes(const std::vector<std::shared_ptr<model::DocraftNode> > &nodes) {
        for (const auto &node: nodes) {
            template_node(node);
        }
    }

    void DocraftTemplateEngine::add_template_variable(const std::string &name, const std::string &value) {
        auto normalized_name = normalize_name(name);
        if (has_template_variable(normalized_name)) {
            throw std::runtime_error(std::format("Template variable '{}' already exists.", name));
        }
        template_variables_.insert({normalized_name, value});
    }

    std::string DocraftTemplateEngine::get_template_variable(const std::string &name) {
        auto normalized_name = normalize_name(name);
        auto it = template_variables_.find(normalized_name);
        if (it == template_variables_.end()) {
            throw std::runtime_error(std::format("Template variable '{}' not found.", name));
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
            throw std::runtime_error(std::format("Template variable '{}' not found.", name));
        }
        template_variables_.erase(it);
    }

    int DocraftTemplateEngine::items() const {
        return template_variables_.size();
    }

    bool DocraftTemplateEngine::has_template_variable(const std::string &name) const {
        return template_variables_.contains(normalize_name(name));
    }

    void DocraftTemplateEngine::template_node(const std::shared_ptr<model::DocraftNode> &node,
                                              const nlohmann::json *foreach_item) {
        if (!node) {
            return;
        }
        if (auto table = std::dynamic_pointer_cast<model::DocraftTable>(node)) {
            if (table->has_header_template()) {
                const std::string rendered = render_template_string(table->header_template(), foreach_item);
                table->apply_json_header(rendered);
                table->clear_header_template();
            }
            if (table->has_model_template()) {
                const std::string rendered = render_template_string(table->model_template(), foreach_item);
                table->apply_json_rows(rendered);
                table->clear_model_template();
            }
            for (const auto &title: table->title_nodes()) {
                template_node(title, foreach_item);
            }
            for (const auto &htitle: table->htitle_nodes()) {
                template_node(htitle, foreach_item);
            }
            for (const auto &row: table->content_nodes()) {
                for (const auto &cell: row) {
                    template_node(cell, foreach_item);
                }
            }
            return;
        }

        if (auto list = std::dynamic_pointer_cast<model::DocraftList>(node)) {
            list->apply_text_transform([&](const std::string &text) {
                return render_template_string(text, foreach_item);
            });
            return;
        }
        if (auto text_node = std::dynamic_pointer_cast<model::DocraftText>(node)) {
            text_node->set_text(render_template_string(text_node->text(), foreach_item));

            // Handle color template expressions
            if (text_node->color().is_template_expression()) {
                try {
                    std::string resolved_color = render_template_string(text_node->color().template_expression(), foreach_item);
                    text_node->set_color(DocraftColor(resolved_color));
                } catch (const std::exception &e) {
                    LOG_ERROR("Failed to resolve color template expression: " + std::string(e.what()));
                }
            }
        }
        if (auto rectangle = std::dynamic_pointer_cast<model::DocraftRectangle>(node)) {
            // Handle background color template expression
            if (rectangle->background_color().is_template_expression()) {
                try {
                    std::string resolved_color = render_template_string(rectangle->background_color_template_expression(), foreach_item);
                    rectangle->set_background_color(DocraftColor(resolved_color));
                } catch (const std::exception &e) {
                    LOG_ERROR("Failed to resolve background color template expression: " + std::string(e.what()));
                }
            }
            // Handle border color template expression
            if (rectangle->border_color().is_template_expression()) {
                try {
                    std::string resolved_color = render_template_string(rectangle->border_color_template_expression(), foreach_item);
                    rectangle->set_border_color(DocraftColor(resolved_color));
                } catch (const std::exception &e) {
                    LOG_ERROR("Failed to resolve border color template expression: " + std::string(e.what()));
                }
            }
        }
        if (auto image = std::dynamic_pointer_cast<model::DocraftImage>(node)) {
            //handle image path templates and raw data templates if needed

            if (image->has_raw_data() && image->raw_data().empty()) {
                // Raw data is stored separately from template variables, keyed by image id.
                try {
                    const std::string image_id = render_template_string(image->path(), foreach_item);
                    const auto &rendered_data = get_image_data(image_id);
                    image->set_raw_data(
                        rendered_data.data,
                        rendered_data.width,
                        rendered_data.height);
                } catch (const std::exception &e) {
                    LOG_ERROR("Failed to render raw data for image '" + image->path() + "': " + std::string(e.what()));
                }
            } else if (!image->path().empty()) {
                std::string rendered_path = render_template_string(image->path(), foreach_item);
                image->set_path(rendered_path);
            }
        }
        //handle foreach modeled as a normal node
        if (auto foreach_node = std::dynamic_pointer_cast<model::DocraftForeach>(node)) {
            //handle the model, check if it has a template format or a direct json object vector and create the items based on the number of items in the collection
            auto model = render_template_string(foreach_node->model(), foreach_item);
            //it it doesn't have a template format it will be the same as the original string
            nlohmann::json result = nlohmann::json::parse(model);
            if (result.is_array()) {
                for (const auto &element: result) {
                    //for each element in the array we need to set the template variables for the current item and render the template nodes
                    //copy the nodes and the child and add them to the foreach node
                    std::vector<std::shared_ptr<model::DocraftNode> > rendered_nodes;
                    for (const auto &template_node: foreach_node->template_nodes()) {
                        auto copied_node = model::clone_node(template_node);
                        rendered_nodes.push_back(copied_node);
                    }

                    //template the added nodes with the current element as template variable, in the foreach data must be take with ${data("fieldName")} syntax
                    /*
                     * exmaple of xml
                    * <Foreach model="${employees}">
                     *   <Text>${data("name")}</Text>
                     *   <Text>${data("role")}</Text>
                     *   </Foreach>
                     */
                    //template the nodes using the current element
                    for (const auto &rendered_node: rendered_nodes) {
                        template_node(rendered_node, &element);
                    }
                    //add nodes to the foreach node
                    for (const auto &rendered_node: rendered_nodes) {
                        foreach_node->add_child(rendered_node);
                    }
                }
                //remove template node
                foreach_node->clear_template_nodes();
            } else {
                LOG_WARNING("docraft/model for foreach node is not an array: " + model);
                throw std::runtime_error("docraft/model for foreach node is not an array or a valid json");
            }
            return;
        }
        if (auto container = std::dynamic_pointer_cast<model::DocraftChildrenContainerNode>(node)) {
            for (const auto &child: container->children()) {
                template_node(child, foreach_item);
            }
        }
    }
    std::string DocraftTemplateEngine::render_template_string_foreach_item(const std::string &text,
                                                                            const nlohmann::json &item) {
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
                    variable_value = get_template_variable(variable_name);
                } else {
                    LOG_WARNING("Template variable '" + variable_name + "' not found in template engine.");
                    variable_value = text;
                }
            } catch (const std::exception &e) {
                LOG_ERROR("Template variable '" + variable_name + "' not found: " + std::string(e.what()));
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
                    variable_value = get_template_variable(variable_name);
                } else {
                    LOG_WARNING("Template variable '" + variable_name + "' not found in template engine.");
                    variable_value = text;
                }
            } catch (const std::exception &e) {
                LOG_ERROR("Template variable '" + variable_name + "' not found: " + std::string(e.what()));
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
            throw std::runtime_error(std::format("Image data for '{}' already exists.", image_id));
        }
        if (width <= 0 || height <= 0) {
            throw std::runtime_error(std::format("Image data for '{}' has invalid dimensions.", image_id));
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
            throw std::runtime_error(std::format("Image data for '{}' not found.", image_id));
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
            throw std::runtime_error(std::format(
                "Base64 image '{}' size does not match dimensions (RGB expected).", image_id));
        }
        add_image_data(image_id, decoded, width, height);
    }

    std::string DocraftTemplateEngine::normalize_name(const std::string &name) {
        std::string lower_case_name = name;
        std::ranges::transform(lower_case_name, lower_case_name.begin(), ::tolower);
        return lower_case_name;
    }
} // docraft
