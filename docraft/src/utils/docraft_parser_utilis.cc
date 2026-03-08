#include "utils/docraft_parser_utilis.h"

#include <nlohmann/json.hpp>

#include "utils/docraft_logger.h"

namespace docraft::utils {
    std::string DocraftParserUtilis::extract_data_attribute(const std::string &data_request, const nlohmann::json &item) {
        // Expected format: ${data("field_name")} or ${data(field_name)}
        const std::string prefix = "${data(";
        const std::string suffix = ")}";
        if (is_data_request(data_request)) {
            std::string field = data_request.substr(prefix.size(), data_request.size() - prefix.size() - suffix.size());

            // Trim leading and trailing spaces, tabs, and newlines
            field.erase(0, field.find_first_not_of(" \t\n\r"));
            const auto last_non_space = field.find_last_not_of(" \t\n\r");
            if (last_non_space == std::string::npos) {
                field.clear();
            } else {
                field.erase(last_non_space + 1);
            }

            // Remove quotes if present
            if (field.size() >= 2 &&
                ((field.front() == '"' && field.back() == '"') ||
                 (field.front() == '\'' && field.back() == '\''))) {
                field = field.substr(1, field.size() - 2);
            }

            try {
                if (!field.empty() && item.is_object() && item.contains(field)) {
                    const auto &value = item.at(field);
                    if (value.is_string()) {
                        return value.get<std::string>();
                    }
                     // For non-string values, convert them to string representation
                        return value.dump();

                }
                return "";
            } catch (const std::exception &e) {
                LOG_ERROR("Error extracting data attribute '" + field + "': " + e.what());
                return "";
            }
        }
        LOG_WARNING("Data request '" + data_request + "' is not in the expected format '${data(\"field_name\")}'.");
        return "";
    }
    std::string DocraftParserUtilis::extract_data_attribute(const std::vector<unsigned char> &data_request, const nlohmann::json &item) {
        std::string data_request_str(reinterpret_cast<const char *>(data_request.data()), data_request.size());
        return extract_data_attribute(data_request_str, item);
    }
    bool DocraftParserUtilis::is_data_request(const std::string &data_request) {
        return data_request.starts_with("${data(") && data_request.ends_with(")}");
    }
    bool DocraftParserUtilis::is_template_variable(const std::string &variable) {
        return variable.starts_with("${") && variable.ends_with("}") && !is_data_request(variable);
    }
    bool DocraftParserUtilis::is_data_request(const std::vector<unsigned char> &data) {
        constexpr std::string_view kPrefix = "${data(";
        constexpr std::string_view kSuffix = ")}";
        if (data.size() < kPrefix.size() + kSuffix.size()) {
            return false;
        }
        std::string_view data_view(reinterpret_cast<const char *>(data.data()), data.size());
        return data_view.starts_with(kPrefix) && data_view.ends_with(kSuffix);
    }
} // docraft