#pragma once
#include <string>
#include <nlohmann/adl_serializer.hpp>

namespace docraft::utils {
        class DocraftParserUtilis {
        public:
                /**
                 * @brief Extracts a value from a JSON object based on a data request string.
                 * The data request can be a simple key or a dot-separated path for nested values.
                 * @param data_request The data request string (e.g., "${data(name)" or "${data("age")").
                 * @param item The JSON object to extract data from.
                 * @return The extracted value as a string, or an empty string if not found.
                 */
                static std::string extract_data_attribute(const std::string &data_request, const nlohmann::json& item);
                static std::string extract_data_attribute(const std::vector<unsigned char>& data_request, const nlohmann::json& item);

                static bool is_data_request(const std::string &data_request);

                static bool is_template_variable(const std::string &variable);

                static bool is_data_request(const std::vector<unsigned char> &data);
        };
} // docraft