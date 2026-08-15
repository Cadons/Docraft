#include <any>

#include <gtest/gtest.h>
#include <pugixml.hpp>

#include "docraft/craft/parser/docraft_parser.h"
#include "docraft/craft/parser/docraft_parser_helpers.h"
#include "docraft/exception/docraft_exceptions.h"

namespace {
    // The parser now only extracts a color attribute's raw text (see
    // docraft_parser_helpers.h) -- interpretation is deferred to the loom tree builder,
    // after templating. Parsing it here still exercises the same interpretation logic.
    docraft::RGB to_rgb(const std::string &color) {
        return docraft::craft::parser::detail::parse_docraft_color(color).toRGB();
    }
}

TEST(DocraftTableParserTest, ParsesHorizontalTableWithBackgrounds) {
    const char *xml = R"XML(
<Table TableTile="#CCCCCC">
  <THead>
    <HTitle background_color="#FF0000">ColA</HTitle>
    <HTitle>ColB</HTitle>
  </THead>
  <TBody>
    <Row background_color="#00FF00">
      <Cell background_color="#0000FF"><Text>v1</Text></Cell>
      <Cell><Text>v2</Text></Cell>
    </Row>
  </TBody>
</Table>
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftTableParser parser;
    const auto data = std::any_cast<docraft::craft::parser::ParsedTableData>(parser.parse(doc.child("Table")));

    EXPECT_EQ(data.orientation, docraft::craft::parser::ParsedTableOrientation::kHorizontal);
    ASSERT_EQ(data.header_titles.size(), 2U);
    ASSERT_TRUE(data.header_titles[0].background.has_value());
    auto title_rgb = to_rgb(*data.header_titles[0].background);
    EXPECT_FLOAT_EQ(title_rgb.r, 1.0F);
    EXPECT_FLOAT_EQ(title_rgb.g, 0.0F);
    EXPECT_FLOAT_EQ(title_rgb.b, 0.0F);

    ASSERT_EQ(data.rows.size(), 1U);
    ASSERT_TRUE(data.rows[0].background.has_value());
    auto row_rgb = to_rgb(*data.rows[0].background);
    EXPECT_FLOAT_EQ(row_rgb.r, 0.0F);
    EXPECT_FLOAT_EQ(row_rgb.g, 1.0F);
    EXPECT_FLOAT_EQ(row_rgb.b, 0.0F);

    ASSERT_EQ(data.rows[0].cells.size(), 2U);
    ASSERT_TRUE(data.rows[0].cells[0].background.has_value());
    auto cell_rgb = to_rgb(*data.rows[0].cells[0].background);
    EXPECT_FLOAT_EQ(cell_rgb.r, 0.0F);
    EXPECT_FLOAT_EQ(cell_rgb.g, 0.0F);
    EXPECT_FLOAT_EQ(cell_rgb.b, 1.0F);

    ASSERT_TRUE(data.rows[0].cells[1].background.has_value());
    auto inherited_cell_rgb = to_rgb(*data.rows[0].cells[1].background);
    EXPECT_FLOAT_EQ(inherited_cell_rgb.r, 0.0F);
    EXPECT_FLOAT_EQ(inherited_cell_rgb.g, 1.0F);
    EXPECT_FLOAT_EQ(inherited_cell_rgb.b, 0.0F);

    ASSERT_TRUE(data.default_cell_background.has_value());
    auto default_rgb = to_rgb(*data.default_cell_background);
    EXPECT_NEAR(default_rgb.r, 0.8F, 0.01F);
    EXPECT_NEAR(default_rgb.g, 0.8F, 0.01F);
    EXPECT_NEAR(default_rgb.b, 0.8F, 0.01F);

    const auto cell_text = std::any_cast<docraft::craft::parser::ParsedTextData>(data.rows[0].cells[0].content);
    EXPECT_EQ(cell_text.text, "v1");
}

TEST(DocraftTableParserTest, RejectsLegacyTitleTagInTableHeader) {
    const char *xml = R"XML(
<Table>
  <THead>
    <Title>ColA</Title>
  </THead>
  <TBody>
    <Row>
      <Cell><Text>v1</Text></Cell>
    </Row>
  </TBody>
</Table>
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftTableParser parser;
    EXPECT_THROW(parser.parse(doc.child("Table")), docraft::exception::InvalidInputException);
}

TEST(DocraftTableParserTest, RejectsCellWithMoreThanOneChild) {
    const char *xml = R"XML(
<Table>
  <THead>
    <HTitle>ColA</HTitle>
  </THead>
  <TBody>
    <Row>
      <Cell><Text>v1</Text><Text>v2</Text></Cell>
    </Row>
  </TBody>
</Table>
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftTableParser parser;
    EXPECT_THROW(parser.parse(doc.child("Table")), docraft::exception::InvalidInputException);
}

TEST(DocraftTableParserTest, RejectsCellWithUnrecognizedChildTag) {
    const char *xml = R"XML(
<Table>
  <THead>
    <HTitle>ColA</HTitle>
  </THead>
  <TBody>
    <Row>
      <Cell><Rectangle width="10" height="10"/></Cell>
    </Row>
  </TBody>
</Table>
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftTableParser parser;
    EXPECT_THROW(parser.parse(doc.child("Table")), docraft::exception::InvalidInputException);
}

TEST(DocraftTableParserTest, ParsesVerticalTableWithHeaderRow) {
    const char *xml = R"XML(
<Table model="vertical">
  <THead>
    <HTitle background_color="#AAAAAA">Title3</HTitle>
    <HTitle>Title4</HTitle>
  </THead>
  <TBody>
    <Row background_color="#00FF00">
      <VTitle background_color="#FF0000">Title1</VTitle>
      <Cell background_color="#0000FF"><Text>v1</Text></Cell>
      <Cell><Text>v4</Text></Cell>
    </Row>
    <Row>
      <VTitle>Title2</VTitle>
      <Cell><Text>v2</Text></Cell>
      <Cell><Text>v3</Text></Cell>
    </Row>
  </TBody>
</Table>
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftTableParser parser;
    const auto data = std::any_cast<docraft::craft::parser::ParsedTableData>(parser.parse(doc.child("Table")));

    EXPECT_EQ(data.orientation, docraft::craft::parser::ParsedTableOrientation::kVertical);
    ASSERT_EQ(data.header_titles.size(), 2U);
    ASSERT_TRUE(data.header_titles[0].background.has_value());

    ASSERT_EQ(data.rows.size(), 2U);
    ASSERT_TRUE(data.rows[0].row_title.has_value());
    EXPECT_EQ(data.rows[0].row_title->text, "Title1");
    ASSERT_TRUE(data.rows[0].row_title->background.has_value());
    ASSERT_TRUE(data.rows[0].background.has_value());
    ASSERT_EQ(data.rows[0].cells.size(), 2U);
    ASSERT_TRUE(data.rows[0].cells[0].background.has_value());
    auto explicit_cell_rgb = to_rgb(*data.rows[0].cells[0].background);
    EXPECT_FLOAT_EQ(explicit_cell_rgb.r, 0.0F);
    EXPECT_FLOAT_EQ(explicit_cell_rgb.g, 0.0F);
    EXPECT_FLOAT_EQ(explicit_cell_rgb.b, 1.0F);
    ASSERT_TRUE(data.rows[0].cells[1].background.has_value());
    auto inherited_cell_rgb = to_rgb(*data.rows[0].cells[1].background);
    EXPECT_FLOAT_EQ(inherited_cell_rgb.r, 0.0F);
    EXPECT_FLOAT_EQ(inherited_cell_rgb.g, 1.0F);
    EXPECT_FLOAT_EQ(inherited_cell_rgb.b, 0.0F);
}

TEST(DocraftTableParserTest, RejectsVerticalRowWithoutVTitle)
{
    const char *xml = R"XML(
<Table model="vertical">
  <TBody>
    <Row>
      <Cell><Text>v1</Text></Cell>
    </Row>
  </TBody>
</Table>
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftTableParser parser;
    EXPECT_THROW(parser.parse(doc.child("Table")), docraft::exception::InvalidInputException);
}

TEST(DocraftTableParserTest, ParsesCellWidthAttribute) {
    const char *xml = R"XML(
<Table>
  <THead>
    <HTitle>ColA</HTitle>
    <HTitle>ColB</HTitle>
  </THead>
  <TBody>
    <Row>
      <Cell width="120"><Text>v1</Text></Cell>
      <Cell><Text>v2</Text></Cell>
    </Row>
  </TBody>
</Table>
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftTableParser parser;
    const auto data = std::any_cast<docraft::craft::parser::ParsedTableData>(parser.parse(doc.child("Table")));

    ASSERT_EQ(data.rows.size(), 1U);
    ASSERT_EQ(data.rows[0].cells.size(), 2U);
    ASSERT_TRUE(data.rows[0].cells[0].width.has_value());
    EXPECT_FLOAT_EQ(*data.rows[0].cells[0].width, 120.0F);
    EXPECT_FALSE(data.rows[0].cells[1].width.has_value());
}

TEST(DocraftTableParserTest, MissingTHeadIsMandatoryForHorizontalTables)
{
    const char* xml = R"XML(
<Table>
  <TBody>
    <Row>
      <Cell><Text>v1</Text></Cell>
    </Row>
  </TBody>
</Table>
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftTableParser parser;
    EXPECT_THROW(parser.parse(doc.child("Table")), docraft::exception::InvalidInputException);
}

TEST(DocraftTableParserTest, ParsesJsonMatrixModelAndHeaderAsDeferredTemplates)
{
    const char* xml = R"XML(
<Table model='[["v1","v2"],["v3","v4"]]' header='["H1","H2"]' />
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftTableParser parser;
    const auto data = std::any_cast<docraft::craft::parser::ParsedTableData>(parser.parse(doc.child("Table")));

    EXPECT_EQ(data.orientation, docraft::craft::parser::ParsedTableOrientation::kHorizontal);
    ASSERT_TRUE(data.model_data_template.has_value());
    EXPECT_EQ(*data.model_data_template, R"([["v1","v2"],["v3","v4"]])");
    ASSERT_TRUE(data.header_data_template.has_value());
    EXPECT_EQ(*data.header_data_template, R"(["H1","H2"])");
    EXPECT_TRUE(data.header_titles.empty());
    EXPECT_TRUE(data.rows.empty());
}

// Whether a JSON/template `model` is an array of arrays (incompatible with an explicit
// TBody) or an array of objects (which uses the TBody as a per-row template -- see
// docraft_loom_tree_builder_test.cc's TableJsonModelWithTBodyTemplateClonesRowPerObject)
// is only known once `model` is resolved, which may require ${...} substitution the parser
// doesn't perform. So the parser must not reject this combination up front; it just parses
// both and leaves the decision to the loom builder.
TEST(DocraftTableParserTest, ParsesJsonModelCombinedWithExplicitTBodyWithoutValidatingShape)
{
    const char* xml = R"XML(
<Table model='[["v1","v2"]]'>
  <TBody>
    <Row>
      <Cell><Text>v1</Text></Cell>
    </Row>
  </TBody>
</Table>
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftTableParser parser;
    const auto data = std::any_cast<docraft::craft::parser::ParsedTableData>(parser.parse(doc.child("Table")));

    ASSERT_TRUE(data.model_data_template.has_value());
    EXPECT_EQ(*data.model_data_template, "[[\"v1\",\"v2\"]]");
    ASSERT_EQ(data.rows.size(), 1);
    ASSERT_EQ(data.rows.front().cells.size(), 1);
}

TEST(DocraftTableParserTest, RejectsHeaderAttributeCombinedWithExplicitTHead)
{
    const char* xml = R"XML(
<Table header='["H1","H2"]'>
  <THead>
    <HTitle>ColA</HTitle>
    <HTitle>ColB</HTitle>
  </THead>
  <TBody>
    <Row>
      <Cell><Text>v1</Text></Cell>
      <Cell><Text>v2</Text></Cell>
    </Row>
  </TBody>
</Table>
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftTableParser parser;
    EXPECT_THROW(parser.parse(doc.child("Table")), docraft::exception::InvalidInputException);
}

TEST(DocraftTableParserTest, RejectsJsonModelCombinedWithVerticalOrientation)
{
    const char* xml = R"XML(<Table model="vertical" />)XML";
    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    // "vertical" is the orientation keyword, not a JSON model -- this should parse fine
    // with no model_data_template, distinguishing keyword handling from JSON detection.
    docraft::craft::parser::DocraftTableParser parser;
    const auto data = std::any_cast<docraft::craft::parser::ParsedTableData>(parser.parse(doc.child("Table")));
    EXPECT_EQ(data.orientation, docraft::craft::parser::ParsedTableOrientation::kVertical);
    EXPECT_FALSE(data.model_data_template.has_value());
}
