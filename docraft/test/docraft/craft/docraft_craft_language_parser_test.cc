#include <any>

#include <gtest/gtest.h>

#include "docraft/craft/docraft_craft_language_parser.h"
#include "docraft/craft/parser/docraft_parser.h"
#include "docraft/exception/docraft_exceptions.h"

TEST(DocraftCraftLanguageParserTest, ParsesTitleSubtitleAndTextWithPredefinedDefaults) {
    const char *xml = R"XML(
<layout orientation="vertical">
  <Title>Main Heading</Title>
  <Subtitle>Section Heading</Subtitle>
  <Text>Body copy</Text>
</layout>
)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    const auto root = parser.parse(xml);
    ASSERT_TRUE(root);
    EXPECT_EQ(root->tag_name, "layout");
    ASSERT_EQ(root->children.size(), 3U);

    const auto title = std::any_cast<docraft::craft::parser::ParsedTextData>(root->children[0]->data);
    EXPECT_EQ(title.text, "Main Heading");
    ASSERT_TRUE(title.font_size.has_value());
    EXPECT_FLOAT_EQ(*title.font_size, 24.0F);
    ASSERT_TRUE(title.style.has_value());
    EXPECT_EQ(*title.style, docraft::craft::parser::ParsedTextStyle::kBold);

    const auto subtitle = std::any_cast<docraft::craft::parser::ParsedTextData>(root->children[1]->data);
    EXPECT_EQ(subtitle.text, "Section Heading");
    ASSERT_TRUE(subtitle.font_size.has_value());
    EXPECT_FLOAT_EQ(*subtitle.font_size, 18.0F);
    ASSERT_TRUE(subtitle.style.has_value());
    EXPECT_EQ(*subtitle.style, docraft::craft::parser::ParsedTextStyle::kBold);

    const auto text = std::any_cast<docraft::craft::parser::ParsedTextData>(root->children[2]->data);
    EXPECT_EQ(text.text, "Body copy");
    EXPECT_FALSE(text.font_size.has_value());
    EXPECT_FALSE(text.style.has_value());
}

TEST(DocraftCraftLanguageParserTest, HeadingAttributesOverridePredefinedDefaults) {
    const char *xml = R"XML(
<layout orientation="vertical">
  <Title font_size="30" style="italic">Main Heading</Title>
  <Subtitle style="normal">Section Heading</Subtitle>
</layout>
)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    const auto root = parser.parse(xml);
    ASSERT_TRUE(root);
    ASSERT_EQ(root->children.size(), 2U);

    const auto title = std::any_cast<docraft::craft::parser::ParsedTextData>(root->children[0]->data);
    ASSERT_TRUE(title.font_size.has_value());
    EXPECT_FLOAT_EQ(*title.font_size, 30.0F);
    ASSERT_TRUE(title.style.has_value());
    EXPECT_EQ(*title.style, docraft::craft::parser::ParsedTextStyle::kItalic);

    const auto subtitle = std::any_cast<docraft::craft::parser::ParsedTextData>(root->children[1]->data);
    ASSERT_TRUE(subtitle.font_size.has_value());
    EXPECT_FLOAT_EQ(*subtitle.font_size, 18.0F);
    ASSERT_TRUE(subtitle.style.has_value());
    EXPECT_EQ(*subtitle.style, docraft::craft::parser::ParsedTextStyle::kNormal);
}

TEST(DocraftCraftLanguageParserTest, ParsesCommonAttributesGenerically)
{
    const char *xml = R"XML(
<Rectangle name="box" width="100" height="50" padding="4" z_index="2" x="5" y="7" />
)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    const auto root = parser.parse(xml);
    ASSERT_TRUE(root);
    EXPECT_EQ(root->tag_name, "Rectangle");
    ASSERT_TRUE(root->common.name.has_value());
    EXPECT_EQ(*root->common.name, "box");
    ASSERT_TRUE(root->common.width.has_value());
    EXPECT_FLOAT_EQ(*root->common.width, 100.0F);
    ASSERT_TRUE(root->common.height.has_value());
    EXPECT_FLOAT_EQ(*root->common.height, 50.0F);
    ASSERT_TRUE(root->common.padding.has_value());
    EXPECT_FLOAT_EQ(*root->common.padding, 4.0F);
    ASSERT_TRUE(root->common.z_index.has_value());
    EXPECT_EQ(*root->common.z_index, 2);
    // Explicit x/y with no "position" attribute implies absolute placement.
    ASSERT_TRUE(root->common.position_mode.has_value());
    EXPECT_EQ(*root->common.position_mode, docraft::craft::PositionMode::kAbsolute);
}

TEST(DocraftCraftLanguageParserTest, RejectsNestedTextInText) {
    const char *xml = R"XML(
<Text>
  <Text>Nested text</Text>
</Text>
)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    EXPECT_THROW(parser.parse(xml), docraft::exception::InvalidInputException);
}

TEST(DocraftCraftLanguageParserTest, RejectsTitleInText) {
    const char *xml = R"XML(
<Text>
  <Title>Nested title</Title>
</Text>
)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    EXPECT_THROW(parser.parse(xml), docraft::exception::InvalidInputException);
}

TEST(DocraftCraftLanguageParserTest, RejectsSubtitleInText) {
    const char *xml = R"XML(
<Text>
  <Subtitle>Nested subtitle</Subtitle>
</Text>
)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    EXPECT_THROW(parser.parse(xml), docraft::exception::InvalidInputException);
}

TEST(DocraftCraftLanguageParserTest, RejectsPageNumberInText) {
    const char *xml = R"XML(
<Text>
  Page: <PageNumber />
</Text>
)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    EXPECT_THROW(parser.parse(xml), docraft::exception::InvalidInputException);
}

TEST(DocraftCraftLanguageParserTest, AllowsLayoutWithMultipleText)
{
    const char *xml = R"XML(
<layout orientation="vertical">
  <Text>First line</Text>
  <Text>Second line</Text>
</layout>
)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    std::shared_ptr<docraft::craft::DocraftParsedElement> root;
    EXPECT_NO_THROW({root = parser.parse(xml); });

    ASSERT_TRUE(root);
    ASSERT_EQ(root->children.size(), 2U);
    EXPECT_EQ(std::any_cast<docraft::craft::parser::ParsedTextData>(root->children[0]->data).text, "First line");
    EXPECT_EQ(std::any_cast<docraft::craft::parser::ParsedTextData>(root->children[1]->data).text, "Second line");
}

TEST(DocraftCraftLanguageParserTest, RejectsNonTextChildInList)
{
    const char *xml = R"XML(
<List>
  <Image src="logo.png" />
</List>
)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    EXPECT_THROW(parser.parse(xml), docraft::exception::InvalidInputException);
}

TEST(DocraftCraftLanguageParserTest, ThrowsForUnregisteredTag)
{
    const char* xml = R"XML(
<Foreach model="${items}" />
)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    EXPECT_THROW(parser.parse(xml), docraft::exception::DataFormatException);
}
