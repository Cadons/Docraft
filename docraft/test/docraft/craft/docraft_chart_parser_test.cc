#include <any>

#include <gtest/gtest.h>
#include <pugixml.hpp>

#include "docraft/craft/parser/docraft_parser.h"

TEST(DocraftChartParserTest, ParsesChartAttributes)
{
    const char* xml = R"XML(
<Chart style="scatter" axis_position="center" title="My Chart"
       x_label="X" y_label="Y" background_color="#FF0000" border_color="#00FF00" border_width="2" />
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftChartParser parser;
    const auto data = std::any_cast<docraft::craft::parser::ParsedChartData>(parser.parse(doc.child("Chart")));

    ASSERT_TRUE(data.style.has_value());
    EXPECT_EQ(*data.style, "scatter");
    ASSERT_TRUE(data.axis_position.has_value());
    EXPECT_EQ(*data.axis_position, "center");
    ASSERT_TRUE(data.title.has_value());
    EXPECT_EQ(*data.title, "My Chart");
    ASSERT_TRUE(data.x_label.has_value());
    EXPECT_EQ(*data.x_label, "X");
    ASSERT_TRUE(data.y_label.has_value());
    EXPECT_EQ(*data.y_label, "Y");
    ASSERT_TRUE(data.border_width.has_value());
    EXPECT_FLOAT_EQ(*data.border_width, 2.0F);
}

TEST(DocraftChartParserTest, OptionalAttributesAbsentByDefault)
{
    const char* xml = R"XML(<Chart style="scatter" />)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftChartParser parser;
    const auto data = std::any_cast<docraft::craft::parser::ParsedChartData>(parser.parse(doc.child("Chart")));

    EXPECT_FALSE(data.axis_position.has_value());
    EXPECT_FALSE(data.title.has_value());
    EXPECT_FALSE(data.x_label.has_value());
    EXPECT_FALSE(data.y_label.has_value());
}

TEST(DocraftSeriesParserTest, ParsesSeriesAttributes)
{
    const char* xml = R"XML(<Series color="blue" model='[[1,2],[3,4]]' />)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftSeriesParser parser;
    const auto data = std::any_cast<docraft::craft::parser::ParsedSeriesData>(parser.parse(doc.child("Series")));

    ASSERT_TRUE(data.color.has_value());
    EXPECT_EQ(*data.color, "blue");
    ASSERT_TRUE(data.model.has_value());
    EXPECT_EQ(*data.model, "[[1,2],[3,4]]");
}
