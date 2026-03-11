#include <gtest/gtest.h>
#include <pugixml.hpp>

#include "docraft/craft/parser/docraft_parser.h"
#include "docraft/model/docraft_table.h"

namespace {
    docraft::RGB to_rgb(const docraft::DocraftColor &color) {
        return color.toRGB();
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
    auto node = parser.parse(doc.child("Table"));
    auto table = std::dynamic_pointer_cast<docraft::model::DocraftTable>(node);
    ASSERT_TRUE(table);

    EXPECT_EQ(table->orientation(), docraft::model::LayoutOrientation::kHorizontal);
    EXPECT_EQ(table->cols(), 2);
    EXPECT_EQ(table->content_cols(), 2);
    EXPECT_EQ(table->rows(), 1);

    ASSERT_EQ(table->title_nodes().size(), 2U);
    ASSERT_EQ(table->title_backgrounds().size(), 2U);
    ASSERT_TRUE(table->title_backgrounds()[0].has_value());
    auto title_rgb = to_rgb(*table->title_backgrounds()[0]);
    EXPECT_FLOAT_EQ(title_rgb.r, 1.0F);
    EXPECT_FLOAT_EQ(title_rgb.g, 0.0F);
    EXPECT_FLOAT_EQ(title_rgb.b, 0.0F);

    ASSERT_EQ(table->row_backgrounds().size(), 1U);
    ASSERT_TRUE(table->row_backgrounds()[0].has_value());
    auto row_rgb = to_rgb(*table->row_backgrounds()[0]);
    EXPECT_FLOAT_EQ(row_rgb.r, 0.0F);
    EXPECT_FLOAT_EQ(row_rgb.g, 1.0F);
    EXPECT_FLOAT_EQ(row_rgb.b, 0.0F);

    ASSERT_EQ(table->content_backgrounds().size(), 2U);
    ASSERT_TRUE(table->content_backgrounds()[0].has_value());
    auto cell_rgb = to_rgb(*table->content_backgrounds()[0]);
    EXPECT_FLOAT_EQ(cell_rgb.r, 0.0F);
    EXPECT_FLOAT_EQ(cell_rgb.g, 0.0F);
    EXPECT_FLOAT_EQ(cell_rgb.b, 1.0F);

    ASSERT_TRUE(table->default_cell_background().has_value());
    auto default_rgb = to_rgb(*table->default_cell_background());
    EXPECT_NEAR(default_rgb.r, 0.8F, 0.01F);
    EXPECT_NEAR(default_rgb.g, 0.8F, 0.01F);
    EXPECT_NEAR(default_rgb.b, 0.8F, 0.01F);
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
    EXPECT_THROW(parser.parse(doc.child("Table")), std::invalid_argument);
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
    auto node = parser.parse(doc.child("Table"));
    auto table = std::dynamic_pointer_cast<docraft::model::DocraftTable>(node);
    ASSERT_TRUE(table);

    EXPECT_EQ(table->orientation(), docraft::model::LayoutOrientation::kVertical);
    EXPECT_EQ(table->rows(), 2);
    EXPECT_EQ(table->content_cols(), 2);
    EXPECT_EQ(table->cols(), 3);

    ASSERT_EQ(table->htitle_nodes().size(), 2U);
    ASSERT_EQ(table->htitle_backgrounds().size(), 2U);
    ASSERT_TRUE(table->htitle_backgrounds()[0].has_value());

    ASSERT_EQ(table->title_nodes().size(), 2U);
    ASSERT_EQ(table->title_backgrounds().size(), 2U);
    ASSERT_TRUE(table->title_backgrounds()[0].has_value());

    ASSERT_EQ(table->row_backgrounds().size(), 2U);
    ASSERT_TRUE(table->row_backgrounds()[0].has_value());

    ASSERT_EQ(table->content_backgrounds().size(), 4U);
    ASSERT_TRUE(table->content_backgrounds()[0].has_value());
}

TEST(DocraftTableParserTest, ParsesTableFromJsonRowsModelAttribute) {
    const char *xml = R"XML(
<Table model='[["v1","v2"],["v3","v4"]]' />
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftTableParser parser;
    auto node = parser.parse(doc.child("Table"));
    auto table = std::dynamic_pointer_cast<docraft::model::DocraftTable>(node);
    ASSERT_TRUE(table);

    EXPECT_EQ(table->orientation(), docraft::model::LayoutOrientation::kHorizontal);
    EXPECT_EQ(table->cols(), 2);
    EXPECT_EQ(table->content_cols(), 2);
    EXPECT_EQ(table->rows(), 2);
    EXPECT_TRUE(table->title_nodes().empty());

    auto content = table->content_nodes();
    ASSERT_EQ(content.size(), 2U);
    ASSERT_EQ(content[0].size(), 2U);
    ASSERT_EQ(content[1].size(), 2U);
    auto c00 = std::dynamic_pointer_cast<docraft::model::DocraftText>(content[0][0]);
    auto c01 = std::dynamic_pointer_cast<docraft::model::DocraftText>(content[0][1]);
    auto c10 = std::dynamic_pointer_cast<docraft::model::DocraftText>(content[1][0]);
    auto c11 = std::dynamic_pointer_cast<docraft::model::DocraftText>(content[1][1]);
    ASSERT_TRUE(c00);
    ASSERT_TRUE(c01);
    ASSERT_TRUE(c10);
    ASSERT_TRUE(c11);
    EXPECT_EQ(c00->text(), "v1");
    EXPECT_EQ(c01->text(), "v2");
    EXPECT_EQ(c10->text(), "v3");
    EXPECT_EQ(c11->text(), "v4");
}

TEST(DocraftTableParserTest, RejectsInvalidJsonModelAttribute) {
    const char *xml = R"XML(
<Table model='[["H1","H2"],["v1"]]' />
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftTableParser parser;
    EXPECT_THROW(parser.parse(doc.child("Table")), std::invalid_argument);
}

TEST(DocraftTableParserTest, AcceptsTemplateModelAttribute) {
    const char *xml = R"XML(
<Table model="${prodotti}" />
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftTableParser parser;
    auto node = parser.parse(doc.child("Table"));
    auto table = std::dynamic_pointer_cast<docraft::model::DocraftTable>(node);
    ASSERT_TRUE(table);
    EXPECT_TRUE(table->has_model_template());
    EXPECT_EQ(table->model_template(), "${prodotti}");
}

TEST(DocraftTableParserTest, AcceptsJsonHeaderAttribute) {
    const char *xml = R"XML(
<Table model='[["v1","v2"]]' header='["H1","H2"]' />
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftTableParser parser;
    auto node = parser.parse(doc.child("Table"));
    auto table = std::dynamic_pointer_cast<docraft::model::DocraftTable>(node);
    ASSERT_TRUE(table);
    ASSERT_EQ(table->title_nodes().size(), 2U);
    EXPECT_EQ(table->title_nodes()[0]->text(), "H1");
    EXPECT_EQ(table->title_nodes()[1]->text(), "H2");
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
    auto node = parser.parse(doc.child("Table"));
    auto table = std::dynamic_pointer_cast<docraft::model::DocraftTable>(node);
    ASSERT_TRUE(table);

    auto content = table->content_nodes();
    ASSERT_EQ(content.size(), 1U);
    ASSERT_EQ(content[0].size(), 2U);
    ASSERT_TRUE(content[0][0]);
    ASSERT_TRUE(content[0][1]);
    EXPECT_FLOAT_EQ(content[0][0]->width(), 120.0F);
    EXPECT_FLOAT_EQ(content[0][1]->width(), 0.0F);
}
