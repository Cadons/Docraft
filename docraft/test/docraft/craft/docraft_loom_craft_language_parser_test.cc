#include <filesystem>

#include <gtest/gtest.h>

#include "docraft/craft/docraft_loom_craft_language_parser.h"
#include "docraft/exception/docraft_exceptions.h"
#include "docraft/loom/nodes/docraft_loom_text.h"
#include "docraft/loom/nodes/docraft_loom_vstack.h"
#include "docraft/utils/docraft_font_registry.h"
#include "docraft/utils/docraft_test_temp_file.h"
#include "docraft/utils/docraft_loom_layout_box_test_access.h"

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

TEST(DocraftLoomCraftLanguageParserTest, HeaderBodyFooterFallBackToDefaultMarginWhenUnspecified)
{
  const char* xml = R"XML(
<Document>
  <Header>
    <Text>Header text</Text>
  </Header>
  <Body>
    <Text>Body copy</Text>
  </Body>
  <Footer>
    <PageNumber />
  </Footer>
</Document>
)XML";

  docraft::craft::DocraftLoomCraftLanguageParser parser;
  EXPECT_NO_THROW(parser.parse(xml));

  const auto creator = parser.edit_creator();
  ASSERT_TRUE(creator);

  // No margin_* attribute given anywhere -- every edge must fall back to
  // DocraftLoomPdfCreator::Margins::kDefaultMarginPt, so content never sits flush
  // against its region's own edge.
  constexpr float kExpectedDefaultMargin = docraft::loom::DocraftLoomPdfCreator::Margins::kDefaultMarginPt;
  EXPECT_FLOAT_EQ(creator->header_margins().top, kExpectedDefaultMargin);
  EXPECT_FLOAT_EQ(creator->header_margins().left, kExpectedDefaultMargin);
  EXPECT_FLOAT_EQ(creator->body_margins().top, kExpectedDefaultMargin);
  EXPECT_FLOAT_EQ(creator->body_margins().left, kExpectedDefaultMargin);
  EXPECT_FLOAT_EQ(creator->footer_margins().bottom, kExpectedDefaultMargin);
  EXPECT_FLOAT_EQ(creator->footer_margins().left, kExpectedDefaultMargin);
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

TEST(DocraftLoomCraftLanguageParserTest, HeaderTallerThanRatioPushesBodyDownInsteadOfOverlapping)
{
  const char* xml = R"XML(
<Document>
  <Settings>
    <SectionRatios header_ratio="0.01" body_ratio="0.9" footer_ratio="0.01" />
  </Settings>
  <Header>
    <Text font_size="20">A tall header that needs more than 1 percent of the page</Text>
  </Header>
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

  ASSERT_TRUE(creator->header());
  const auto& header_frame = creator->header()->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof());
  const float header_bottom = header_frame.position.y + header_frame.size.height;

  const auto body_vstack = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomVStack>(creator->root_node());
  ASSERT_TRUE(body_vstack);
  // Body must start below the header's actual bottom edge -- not at the tiny
  // header_ratio-allocated position, which would overlap this header's real content.
  EXPECT_GT(body_vstack->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).position.y, header_bottom);
}

TEST(DocraftLoomCraftLanguageParserTest, FooterTallerThanRatioStaysFullyOnThePage)
{
  const char* xml = R"XML(
<Document>
  <Settings>
    <Page size="A4" orientation="portrait" />
    <SectionRatios header_ratio="0.06" body_ratio="0.9" footer_ratio="0.01" />
  </Settings>
  <Body>
    <Text>Body</Text>
  </Body>
  <Footer>
    <Text font_size="20">A tall footer that needs more than 1 percent of the page</Text>
  </Footer>
</Document>
)XML";

  docraft::craft::DocraftLoomCraftLanguageParser parser;
  EXPECT_NO_THROW(parser.parse(xml));

  const auto creator = parser.edit_creator();
  ASSERT_TRUE(creator);
  EXPECT_NO_THROW(creator->create());

  ASSERT_TRUE(creator->footer());
  const auto& footer_frame = creator->footer()->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof());
  // The footer's whole box must still fit on the (portrait A4, ~841.89pt-tall) page --
  // not overflow past the bottom edge just because footer_ratio's allocation was too
  // small for its real content.
  EXPECT_LE(footer_frame.position.y + footer_frame.size.height, 842.0F);
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

// Regression test: font_name="Roboto"/"OpenSans" (bundled via docraft/fonts.json) must
// resolve to a real embedded font with no <Settings><Fonts> registration -- previously
// this silently fell back to Helvetica (see DocraftLoomPdfCreator::register_bundled_fonts()).
TEST(DocraftLoomCraftLanguageParserTest, ParsesAndRendersWithBundledFontFamily)
{
  const char* xml = R"XML(
<Document>
  <Body>
    <Text font_name="Roboto">Roboto text</Text>
    <Text font_name="OpenSans">OpenSans text</Text>
  </Body>
</Document>
)XML";

  docraft::craft::DocraftLoomCraftLanguageParser parser;
  EXPECT_NO_THROW(parser.parse(xml));

  const auto creator = parser.edit_creator();
  ASSERT_TRUE(creator);

  auto& registry = docraft::utils::DocraftFontRegistry::instance();
  EXPECT_NE(registry.resolve_font_alias("Roboto"), "Roboto");
  EXPECT_NE(registry.resolve_font_alias("OpenSans"), "OpenSans");

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
  EXPECT_THROW(parser.parse(xml), docraft::exception::InvalidInputException);
}
TEST(DocraftCraftLanguageParserTest, TypoTopLevelElement)
{
  const char* xml = R"XML(<Document><Body></Body><Boddy></Boddy></Document>)XML";
  docraft::craft::DocraftLoomCraftLanguageParser parser;
  EXPECT_THROW(parser.parse(xml), docraft::exception::InvalidInputException);
}
TEST(DocraftCraftLanguageParserTest, DuplicatedTopLevelElements)
{
  const char* xml = R"XML(<Document><Body></Body><Body></Body></Document>)XML";
  docraft::craft::DocraftLoomCraftLanguageParser parser;
  EXPECT_THROW(parser.parse(xml), docraft::exception::InvalidInputException);
}
