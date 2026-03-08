#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include "utils/docraft_parser_utilis.h"

namespace docraft::test::utils {
    TEST(DocraftParserUtilis, extract_data_attribute) {
        nlohmann::json item = {
            {"name", "Alice"},
            {"age", "30"},
            {"city", "New York"}
        };

        // Valid data request
        std::string result = docraft::utils::DocraftParserUtilis::extract_data_attribute("${data(name)}", item);
        EXPECT_EQ(result, "Alice");

        // Valid data request with different key
        result = docraft::utils::DocraftParserUtilis::extract_data_attribute("${data(age)}", item);
        EXPECT_EQ(result, "30");

        // Data request for non-existent key
        result = docraft::utils::DocraftParserUtilis::extract_data_attribute("${data(country)}", item);
        EXPECT_EQ(result, "");

        // Invalid format (missing prefix)
        result = docraft::utils::DocraftParserUtilis::extract_data_attribute("data(name)}", item);
        EXPECT_EQ(result, "");

        // Invalid format (missing suffix)
        result = docraft::utils::DocraftParserUtilis::extract_data_attribute("${data(name)", item);
        EXPECT_EQ(result, "");

        // Invalid format (not a data request)
        result = docraft::utils::DocraftParserUtilis::extract_data_attribute("Hello World", item);
        EXPECT_EQ(result, "");
    }

    TEST(DocraftParserUtilis, extract_data_attribute_with_quotes) {
        nlohmann::json item = {
            {"name", "Bob"},
            {"role", "Developer"}
        };

        // Field name with double quotes
        std::string result = docraft::utils::DocraftParserUtilis::extract_data_attribute("${data(\"name\")}", item);
        EXPECT_EQ(result, "Bob");

        // Field name with single quotes
        result = docraft::utils::DocraftParserUtilis::extract_data_attribute("${data('role')}", item);
        EXPECT_EQ(result, "Developer");
    }

    TEST(DocraftParserUtilis, extract_data_attribute_non_string_values) {
        nlohmann::json item = {
            {"count", 42},
            {"price", 19.99},
            {"active", true},
            {"disabled", false}
        };

        // Numeric value (integer)
        std::string result = docraft::utils::DocraftParserUtilis::extract_data_attribute("${data(count)}", item);
        EXPECT_EQ(result, "42");

        // Numeric value (float)
        result = docraft::utils::DocraftParserUtilis::extract_data_attribute("${data(price)}", item);
        EXPECT_EQ(result, "19.99");

        // Boolean value (true)
        result = docraft::utils::DocraftParserUtilis::extract_data_attribute("${data(active)}", item);
        EXPECT_EQ(result, "true");

        // Boolean value (false)
        result = docraft::utils::DocraftParserUtilis::extract_data_attribute("${data(disabled)}", item);
        EXPECT_EQ(result, "false");
    }
}

