#include <fstream>

#include <gtest/gtest.h>

#include "docraft/tools/docraft_validator.h"
#include "docraft/utils/docraft_test_temp_file.h"

namespace {
    std::filesystem::path write_temp_file(const std::string& suffix, const std::string& content)
    {
        const auto path = docraft::test::utils::make_unique_temp_path(suffix);
        std::ofstream file(path, std::ios::out | std::ios::trunc);
        file << content;
        return path;
    }
} // namespace

TEST(DocraftValidatorTest, ValidCraftFileHasNoIssues)
{
    const auto craft_file = write_temp_file(".craft", R"XML(
<Document>
  <Body>
    <Text>Hello</Text>
  </Body>
</Document>
)XML");

    const docraft::tools::DocraftValidator validator;
    const auto result = validator.validate(craft_file);

    EXPECT_FALSE(result.has_errors());
    EXPECT_FALSE(result.has_warnings());
    EXPECT_TRUE(result.issues.empty());
}

TEST(DocraftValidatorTest, MissingCraftFileIsReportedAsError)
{
    const docraft::tools::DocraftValidator validator;
    const auto result = validator.validate("/nonexistent/path/does_not_exist.craft");

    EXPECT_TRUE(result.has_errors());
    ASSERT_FALSE(result.issues.empty());
    EXPECT_EQ(result.issues.front().severity, docraft::tools::DocraftValidationIssue::Severity::kError);
}

TEST(DocraftValidatorTest, WrongExtensionIsReportedAsError)
{
    const auto not_craft_file = write_temp_file(".txt", "<Document><Body><Text>Hi</Text></Body></Document>");

    const docraft::tools::DocraftValidator validator;
    const auto result = validator.validate(not_craft_file);

    EXPECT_TRUE(result.has_errors());
}

TEST(DocraftValidatorTest, MalformedXmlIsReportedAsError)
{
    const auto craft_file = write_temp_file(".craft", "<Document><Body><Text>Unclosed</Body></Document>");

    const docraft::tools::DocraftValidator validator;
    const auto result = validator.validate(craft_file);

    EXPECT_TRUE(result.has_errors());
}

TEST(DocraftValidatorTest, MissingRequiredBodyIsReportedAsError)
{
    const auto craft_file = write_temp_file(".craft", "<Document><Header><Text>Only header</Text></Header></Document>");

    const docraft::tools::DocraftValidator validator;
    const auto result = validator.validate(craft_file);

    EXPECT_TRUE(result.has_errors());
}

TEST(DocraftValidatorTest, InvalidEnumAttributeIsReportedAsError)
{
    const auto craft_file = write_temp_file(".craft", R"XML(
<Document>
  <Settings>
    <Page size="A4" orientation="sideways" />
  </Settings>
  <Body>
    <Text>Hello</Text>
  </Body>
</Document>
)XML");

    const docraft::tools::DocraftValidator validator;
    const auto result = validator.validate(craft_file);

    EXPECT_TRUE(result.has_errors());
}

TEST(DocraftValidatorTest, MissingDataFileIsReportedAsError)
{
    const auto craft_file = write_temp_file(".craft", "<Document><Body><Text>Hello</Text></Body></Document>");

    const docraft::tools::DocraftValidator validator;
    const auto result = validator.validate(craft_file, std::filesystem::path{"/nonexistent/data.json"});

    EXPECT_TRUE(result.has_errors());
}

TEST(DocraftValidatorTest, InvalidJsonDataFileIsReportedAsError)
{
    const auto craft_file = write_temp_file(".craft", "<Document><Body><Text>Hello</Text></Body></Document>");
    const auto data_file = write_temp_file(".json", "{not valid json");

    const docraft::tools::DocraftValidator validator;
    const auto result = validator.validate(craft_file, data_file);

    EXPECT_TRUE(result.has_errors());
}

TEST(DocraftValidatorTest, NonObjectDataFileIsReportedAsError)
{
    const auto craft_file = write_temp_file(".craft", "<Document><Body><Text>Hello</Text></Body></Document>");
    const auto data_file = write_temp_file(".json", "[1, 2, 3]");

    const docraft::tools::DocraftValidator validator;
    const auto result = validator.validate(craft_file, data_file);

    EXPECT_TRUE(result.has_errors());
}

TEST(DocraftValidatorTest, ResolvedVariableProducesNoWarning)
{
    const auto craft_file = write_temp_file(".craft", "<Document><Body><Text>${greeting}</Text></Body></Document>");
    const auto data_file = write_temp_file(".json", R"JSON({"greeting": "Hello"})JSON");

    const docraft::tools::DocraftValidator validator;
    const auto result = validator.validate(craft_file, data_file);

    EXPECT_FALSE(result.has_errors());
    EXPECT_FALSE(result.has_warnings());
}

TEST(DocraftValidatorTest, UndefinedVariableProducesWarningNotError)
{
    const auto craft_file = write_temp_file(".craft", "<Document><Body><Text>${missing}</Text></Body></Document>");
    const auto data_file = write_temp_file(".json", R"JSON({"greeting": "Hello"})JSON");

    const docraft::tools::DocraftValidator validator;
    const auto result = validator.validate(craft_file, data_file);

    EXPECT_FALSE(result.has_errors());
    EXPECT_TRUE(result.has_warnings());
}
