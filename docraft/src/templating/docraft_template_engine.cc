//
// Created by Matteo on 17/02/26.
//

#include "templating/docraft_template_engine.h"

#include <algorithm>
#include <format>

#include "model/docraft_children_container_node.h"
#include "model/docraft_image.h"
#include "model/docraft_list.h"
#include "model/docraft_table.h"
#include "model/docraft_text.h"
#include "utils/docraft_logger.h"
#include "utils/docraft_base64.h"

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

    void DocraftTemplateEngine::template_node(const std::shared_ptr<model::DocraftNode> &node) {
        if (!node) {
            return;
        }
        if (auto table = std::dynamic_pointer_cast<model::DocraftTable>(node)) {
            if (table->has_header_template()) {
                const std::string rendered = render_template_string(table->header_template());
                table->apply_json_header(rendered);
                table->clear_header_template();
            }
            if (table->has_model_template()) {
                const std::string rendered = render_template_string(table->model_template());
                table->apply_json_rows(rendered);
                table->clear_model_template();
            }
            for (const auto &title: table->title_nodes()) {
                template_node(title);
            }
            for (const auto &htitle: table->htitle_nodes()) {
                template_node(htitle);
            }
            for (const auto &row: table->content_nodes()) {
                for (const auto &cell: row) {
                    template_node(cell);
                }
            }
            return;
        }

        if (auto list = std::dynamic_pointer_cast<model::DocraftList>(node)) {
            list->apply_text_transform([&](const std::string &text) { return render_template_string(text); });
            return;
        }
        if (auto text_node = std::dynamic_pointer_cast<model::DocraftText>(node)) {
            text_node->set_text(render_template_string(text_node->text()));
        }
        if (auto image = std::dynamic_pointer_cast<model::DocraftImage>(node)) {
            //handle image path templates and raw data templates if needed

            if (image->has_raw_data()&& image->raw_data().empty()) {
                // Raw data is stored separately from template variables, keyed by image id.
                try {
                    const std::string image_id = render_template_string(image->path());
                    const auto &rendered_data = get_image_data(image_id);
                    image->set_raw_data(
                        rendered_data.data,
                        rendered_data.width,
                        rendered_data.height);
                } catch (const std::exception &e) {
                    LOG_ERROR("Failed to render raw data for image '" + image->path() + "': " + std::string(e.what()));
                }
            } else if (!image->path().empty()) {
                std::string rendered_path = render_template_string(image->path());
                image->set_path(rendered_path);
            }
        }
        if (auto container = std::dynamic_pointer_cast<model::DocraftChildrenContainerNode>(node)) {
            for (const auto &child: container->children()) {
                template_node(child);
            }
        }
    }

    std::string DocraftTemplateEngine::render_template_string(const std::string &text) {
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
        image_data_.emplace(name, RawImageData{.data=data, .width=width, .height=height});
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
            static_cast<size_t>(width) * static_cast<size_t>(height) * 3U;//3 bytes per pixel for RGB
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
