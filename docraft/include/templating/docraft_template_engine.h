#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace docraft::model {
    class DocraftNode;
}

namespace docraft::templating {
        /**
         * @brief The DocraftTemplateEngine class is responsible for processing templates
         * and generating the final document output. It takes a parsed document tree
         * and applies template logic to produce the rendered output.
         */
        class DocraftTemplateEngine {
        public:
                DocraftTemplateEngine() = default;
                /**
                 * @brief Renders the document using the provided template variables.
                 * @return Rendered document as a string.
                 */
                void template_nodes(const std::vector<std::shared_ptr<model::DocraftNode>> &nodes);
                /**
                 * @brief Adds a template variable.
                 * @param name Variable name (case-insensitive).
                 * @param value Variable value inserted into templates.
                 */
                void add_template_variable(const std::string &name, const std::string &value);
                /**
                 * @brief Retrieves the value of a template variable by name.
                 * @param name Name of the template variable.
                 * @return Value of the template variable or empty string if not found.
                 */
                /**
                 * @brief Retrieves a template variable value.
                 * @param name Variable name (case-insensitive).
                 * @return Stored value.
                 * @throws std::runtime_error if not found.
                 */
                std::string get_template_variable(const std::string &name);
                /**
                 * @brief Clears all template variables from the engine.
                 */
                /**
                 * @brief Clears all template variables.
                 */
                void clear_template_variables();
                /**
                 * @brief Removes a specific template variable by name.
                 * @param name Name of the template variable to remove.
                 */
                /**
                 * @brief Removes a template variable by name.
                 * @param name Variable name (case-insensitive).
                 * @throws std::runtime_error if not found.
                 */
                void remove_template_variable(const std::string &name);
                /**
                 * @brief Returns the number of stored template variables.
                 */
                int items() const;
                /**
                 * @brief Checks if a template variable exists by name.
                 * @param name Name of the template variable.
                 * @return True if the variable exists, false otherwise.
                 */
                /**
                 * @brief Checks if a template variable exists.
                 * @param name Variable name (case-insensitive).
                 */
                bool has_template_variable(const std::string &name) const;

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
                 * @param image_id Image id used by <Image data="..."> in templates (case-insensitive).
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
                 * @param image_id Image id used by <Image data="..."> in templates (case-insensitive).
                 * @param base64 Base64 string with raw RGB bytes (no data URI prefix).
                 * @param width Pixel width.
                 * @param height Pixel height.
                 * @throws std::runtime_error if decoded size does not match width*height*3.
                 */
                void add_base64_image_data(const std::string &image_id,
                                           std::string_view base64,
                                           int width,
                                           int height);
                /**
                 * @brief Retrieves raw image data by id.
                 * @param image_id Image id (case-insensitive).
                 * @return RawImageData reference.
                 * @throws std::runtime_error if not found.
                 */
                const RawImageData &get_image_data(const std::string &image_id) const;

        private:
                static std::string normalize_name(const std::string &name);
                void template_node(const std::shared_ptr<model::DocraftNode> &node);
                std::string render_template_string(const std::string &text);
                std::unordered_map<std::string, std::string> template_variables_;
                std::unordered_map<std::string, RawImageData> image_data_;
        };
} // docraft
