#include <filesystem>

#include <gtest/gtest.h>

#include "docraft/craft/docraft_loom_craft_language_parser.h"
#include "docraft/exception/docraft_exceptions.h"
#include "docraft/loom/nodes/docraft_loom_text.h"
#include "docraft/loom/nodes/docraft_loom_vstack.h"
#include "docraft/utils/docraft_test_temp_file.h"

TEST(DocraftLoomCraftLanguageParserTest, ParsesFullDocumentWithHeaderBodyFooter)
{
  const char* xml = R"XML(
<Document>
  <Header margin_left="30" margin_right="30" border_width="0.5">
    <Text>Header text</Text>
  </Header>
  <Body margin_left="20" margin_top="15">
    <Title>Hello</Title>
    <Text>Body copy</Text>
  </Body>
  <Footer margin_bottom="5">
    <PageNumber />
  </Footer>
</Document>
)XML";

  docraft::craft::DocraftLoomCraftLanguageParser parser;
  EXPECT_NO_THROW(parser.parse(xml));

  const auto creator = parser.edit_creator();
  ASSERT_TRUE(creator);

  ASSERT_TRUE(creator->root_node());
  ASSERT_TRUE(creator->header());
  ASSERT_TRUE(creator->footer());

  const auto header_vstack = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomVStack>(creator->header());
  ASSERT_TRUE(header_vstack);
  EXPECT_FLOAT_EQ(header_vstack->style().border_width, 0.5F);

  EXPECT_FLOAT_EQ(creator->header_margins().left, 30.0F);
  EXPECT_FLOAT_EQ(creator->header_margins().right, 30.0F);
  EXPECT_FLOAT_EQ(creator->body_margins().left, 20.0F);
  EXPECT_FLOAT_EQ(creator->body_margins().top, 15.0F);
  EXPECT_FLOAT_EQ(creator->footer_margins().bottom, 5.0F);
}

TEST(DocraftLoomCraftLanguageParserTest, ParsesDocumentWithOnlyRequiredBody)
{
  const char* xml = R"XML(
<Document>
  <Body>
    <Text>Just body</Text>
  </Body>
</Document>
)XML";

  docraft::craft::DocraftLoomCraftLanguageParser parser;
  EXPECT_NO_THROW(parser.parse(xml));

  const auto creator = parser.edit_creator();
  ASSERT_TRUE(creator);
  ASSERT_TRUE(creator->root_node());
  EXPECT_FALSE(creator->header());
  EXPECT_FALSE(creator->footer());
}

TEST(DocraftLoomCraftLanguageParserTest, AppliesSettingsPageAndRatios)
{
  const char* xml = R"XML(
<Document>
  <Settings>
    <Page size="A3" orientation="landscape" />
    <SectionRatios header_ratio="0.1" body_ratio="0.8" footer_ratio="0.1" />
    <Fonts />
  </Settings>
  <Metadata>
    <DocumentTitle>Quarterly Report</DocumentTitle>
  </Metadata>
  <Body>
    <Text>Body</Text>
  </Body>
</Document>
)XML";

  docraft::craft::DocraftLoomCraftLanguageParser parser;
  EXPECT_NO_THROW(parser.parse(xml));

  const auto creator = parser.edit_creator();
  ASSERT_TRUE(creator);
  EXPECT_FLOAT_EQ(creator->header_ratio(), 0.1F);
  EXPECT_FLOAT_EQ(creator->body_ratio(), 0.8F);
  EXPECT_FLOAT_EQ(creator->footer_ratio(), 0.1F);
}

TEST(DocraftLoomCraftLanguageParserTest, AppliesMetadataAndRenders)
{
  const char* xml = R"XML(
<Document>
  <Metadata>
    <DocumentTitle>Quarterly Report</DocumentTitle>
    <Author>Mario Rossi</Author>
    <Keywords>report, q1</Keywords>
  </Metadata>
  <Body>
    <Text>Body</Text>
  </Body>
</Document>
)XML";

  docraft::craft::DocraftLoomCraftLanguageParser parser;
  EXPECT_NO_THROW(parser.parse(xml));

  const auto creator = parser.edit_creator();
  ASSERT_TRUE(creator);
  EXPECT_NO_THROW(creator->create());

  const auto output_path = docraft::test::utils::make_unique_temp_path(".pdf");
  EXPECT_NO_THROW(creator->render(output_path));
  ASSERT_TRUE(std::filesystem::exists(output_path));
  EXPECT_GT(std::filesystem::file_size(output_path), 0U);
  std::filesystem::remove(output_path);
}

TEST(DocraftLoomCraftLanguageParserTest, ThrowsOnInvalidPageSize)
{
  const char* xml = R"XML(
<Document>
  <Settings>
    <Page size="Tabloid" />
  </Settings>
  <Body>
    <Text>Body</Text>
  </Body>
</Document>
)XML";

  docraft::craft::DocraftLoomCraftLanguageParser parser;
  EXPECT_THROW(parser.parse(xml), docraft::exception::InvalidInputException);
}

TEST(DocraftLoomCraftLanguageParserTest, ThrowsOnAttributeDirectlyOnSettings)
{
  const char* xml = R"XML(
<Document>
  <Settings page_size="A4" />
  <Body>
    <Text>Body</Text>
  </Body>
</Document>
)XML";

  docraft::craft::DocraftLoomCraftLanguageParser parser;
  EXPECT_THROW(parser.parse(xml), docraft::exception::InvalidInputException);
}

TEST(DocraftLoomCraftLanguageParserTest, ParsesAndRendersWithCustomFontFamily)
{
  const std::string fonts_dir = DOCRAFT_TEST_FONTS_DIR;
  const std::string xml = R"XML(
<Document>
  <Settings>
    <Fonts>
      <Font name="TestOpenSans">
        <FontNormal src=")XML" + fonts_dir + R"XML(/OpenSans/OpenSans.ttf" />
        <FontBold src=")XML" + fonts_dir + R"XML(/OpenSans/OpenSans-Bold.ttf" />
      </Font>
    </Fonts>
  </Settings>
  <Body>
    <Text font_name="TestOpenSans">Regular</Text>
    <Text font_name="TestOpenSans" style="bold">Bold</Text>
  </Body>
</Document>
)XML";

  docraft::craft::DocraftLoomCraftLanguageParser parser;
  EXPECT_NO_THROW(parser.parse(xml));

  const auto creator = parser.edit_creator();
  ASSERT_TRUE(creator);
  EXPECT_NO_THROW(creator->create());

  const auto output_path = docraft::test::utils::make_unique_temp_path(".pdf");
  EXPECT_NO_THROW(creator->render(output_path));
  ASSERT_TRUE(std::filesystem::exists(output_path));
  EXPECT_GT(std::filesystem::file_size(output_path), 0U);
  std::filesystem::remove(output_path);
}

TEST(DocraftLoomCraftLanguageParserTest, ThrowsWhenFontHasNoVariants)
{
  const char* xml = R"XML(
<Document>
  <Settings>
    <Fonts>
      <Font name="Empty" />
    </Fonts>
  </Settings>
  <Body>
    <Text>Body</Text>
  </Body>
</Document>
)XML";

  docraft::craft::DocraftLoomCraftLanguageParser parser;
  EXPECT_THROW(parser.parse(xml), docraft::exception::InvalidInputException);
}

TEST(DocraftLoomCraftLanguageParserTest, FontsDefaultAttributeAppliesToNodesWithoutFontName)
{
  const std::string fonts_dir = DOCRAFT_TEST_FONTS_DIR;
  const std::string xml = R"XML(
<Document>
  <Settings>
    <Fonts default="TestOpenSans">
      <Font name="TestOpenSans">
        <FontNormal src=")XML" + fonts_dir + R"XML(/OpenSans/OpenSans.ttf" />
      </Font>
    </Fonts>
  </Settings>
  <Body>
    <Text>Uses the default</Text>
    <Text font_name="Times-Roman">Uses its own font</Text>
  </Body>
</Document>
)XML";

  docraft::craft::DocraftLoomCraftLanguageParser parser;
  EXPECT_NO_THROW(parser.parse(xml));

  const auto creator = parser.edit_creator();
  ASSERT_TRUE(creator);

  const auto body_vstack = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomVStack>(creator->root_node());
  ASSERT_TRUE(body_vstack);
  ASSERT_EQ(body_vstack->children_count(), 2);

  const auto default_text = std::dynamic_pointer_cast<const docraft::loom::nodes::DocraftLoomText>(
      body_vstack->child(0));
  ASSERT_TRUE(default_text);
  EXPECT_EQ(default_text->font_family(), "TestOpenSans");

  const auto explicit_text = std::dynamic_pointer_cast<const docraft::loom::nodes::DocraftLoomText>(
      body_vstack->child(1));
  ASSERT_TRUE(explicit_text);
  EXPECT_EQ(explicit_text->font_family(), "Times-Roman");
}

TEST(DocraftLoomCraftLanguageParserTest, ThrowsWhenFontsDefaultAttributeIsEmpty)
{
  const char* xml = R"XML(
<Document>
  <Settings>
    <Fonts default="" />
  </Settings>
  <Body>
    <Text>Body</Text>
  </Body>
</Document>
)XML";

  docraft::craft::DocraftLoomCraftLanguageParser parser;
  EXPECT_THROW(parser.parse(xml), docraft::exception::InvalidInputException);
}

TEST(DocraftLoomCraftLanguageParserTest, ThrowsWhenFontMissingName)
{
  const std::string fonts_dir = DOCRAFT_TEST_FONTS_DIR;
  const std::string xml = R"XML(
<Document>
  <Settings>
    <Fonts>
      <Font>
        <FontNormal src=")XML" + fonts_dir + R"XML(/OpenSans/OpenSans.ttf" />
      </Font>
    </Fonts>
  </Settings>
  <Body>
    <Text>Body</Text>
  </Body>
</Document>
)XML";

  docraft::craft::DocraftLoomCraftLanguageParser parser;
  EXPECT_THROW(parser.parse(xml), docraft::exception::InvalidInputException);
}

TEST(DocraftLoomCraftLanguageParserTest, ThrowsWhenFontsAppearsTwiceInSettings)
{
  const char* xml = R"XML(
<Document>
  <Settings>
    <Fonts />
    <Fonts />
  </Settings>
  <Body>
    <Text>Body</Text>
  </Body>
</Document>
)XML";

  docraft::craft::DocraftLoomCraftLanguageParser parser;
  EXPECT_THROW(parser.parse(xml), docraft::exception::InvalidInputException);
}

TEST(DocraftLoomCraftLanguageParserTest, ThrowsWhenBodyIsMissing)
{
  const char* xml = R"XML(
<Document>
  <Header><Text>Header only</Text></Header>
</Document>
)XML";

  docraft::craft::DocraftLoomCraftLanguageParser parser;
  EXPECT_THROW(parser.parse(xml), docraft::exception::DataFormatException);
}
