#include <gtest/gtest.h>
#include <pugixml.hpp>

#include "craft/parser/docraft_parser.h"
#include "model/docraft_table.h"

namespace {
    docraft::RGB to_rgb(const docraft::DocraftColor &color) {
        return color.toRGB();
    }
}

TEST(DocraftTableParserTest, ParsesHorizontalTableWithBackgrounds) {
    const char *xml = R"XML(
<Table TableTile="#CCCCCC">
  <THead>
    <Title background_color="#FF0000">ColA</Title>
    <Title>ColB</Title>
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
