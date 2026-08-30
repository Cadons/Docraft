#include <any>

#include <gtest/gtest.h>
#include <pugixml.hpp>

#include "docraft/craft/parser/docraft_circle_parser.h"
#include "docraft/craft/parser/docraft_line_parser.h"
#include "docraft/craft/parser/docraft_parser.h"
#include "docraft/craft/parser/docraft_parser_helpers.h"
#include "docraft/craft/parser/docraft_polygon_parser.h"
#include "docraft/craft/parser/docraft_triangle_parser.h"
#include "docraft/docraft_color.h"
#include "docraft/exception/docraft_exceptions.h"

namespace {
    // The parser now only extracts a color attribute's raw text (see
    // docraft_parser_helpers.h) -- interpretation is deferred to the loom tree builder,
    // after templating. Parsing it here still exercises the same interpretation logic.
    docraft::RGB to_rgb(const std::string &color) {
        return docraft::craft::parser::detail::parse_docraft_color(color).toRGB();
    }
}

TEST(DocraftCircleParserTest, ParsesCircleAttributes) {
    const char *xml = R"XML(
<Circle radius="20" background_color="#FF0000" border_color="#00FF00" border_width="2" />
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftCircleParser parser;
    const auto data = std::any_cast<docraft::craft::parser::ParsedCircleData>(parser.parse(doc.child("Circle")));

    ASSERT_TRUE(data.radius.has_value());
    EXPECT_FLOAT_EQ(*data.radius, 20.0F);

    ASSERT_TRUE(data.border_width.has_value());
    EXPECT_FLOAT_EQ(*data.border_width, 2.0F);

    ASSERT_TRUE(data.background_color.has_value());
    auto bg = to_rgb(*data.background_color);
    EXPECT_FLOAT_EQ(bg.r, 1.0F);
    EXPECT_FLOAT_EQ(bg.g, 0.0F);
    EXPECT_FLOAT_EQ(bg.b, 0.0F);

    ASSERT_TRUE(data.border_color.has_value());
    auto border = to_rgb(*data.border_color);
    EXPECT_FLOAT_EQ(border.r, 0.0F);
    EXPECT_FLOAT_EQ(border.g, 1.0F);
    EXPECT_FLOAT_EQ(border.b, 0.0F);
}

TEST(DocraftLineParserTest, ParsesLineAttributes) {
    const char *xml = R"XML(
<Line x1="0" y1="0" x2="10" y2="20" border_color="black" border_width="1" x="5" y="50" />
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftLineParser parser;
    const auto data = std::any_cast<docraft::craft::parser::ParsedLineData>(parser.parse(doc.child("Line")));

    ASSERT_TRUE(data.x1.has_value());
    EXPECT_FLOAT_EQ(*data.x1, 0.0F);
    ASSERT_TRUE(data.y1.has_value());
    EXPECT_FLOAT_EQ(*data.y1, 0.0F);
    ASSERT_TRUE(data.x2.has_value());
    EXPECT_FLOAT_EQ(*data.x2, 10.0F);
    ASSERT_TRUE(data.y2.has_value());
    EXPECT_FLOAT_EQ(*data.y2, 20.0F);
    ASSERT_TRUE(data.border_width.has_value());
    EXPECT_FLOAT_EQ(*data.border_width, 1.0F);

    // x/y are common attributes, parsed separately by DocraftCraftLanguageParser -- not
    // part of this parser's own ParsedLineData payload.
}

TEST(DocraftTriangleParserTest, ParsesTrianglePoints) {
    const char *xml = R"XML(
<Triangle points="0,0 10,0 5,5" background_color="red" border_color="blue" border_width="1" />
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftTriangleParser parser;
    const auto data = std::any_cast<docraft::craft::parser::ParsedTriangleData>(parser.parse(doc.child("Triangle")));

    ASSERT_EQ(data.points.size(), 3U);
    EXPECT_FLOAT_EQ(data.points[2].x, 5.0F);
    EXPECT_FLOAT_EQ(data.points[2].y, 5.0F);
    ASSERT_TRUE(data.border_width.has_value());
    EXPECT_FLOAT_EQ(*data.border_width, 1.0F);
}

TEST(DocraftTriangleParserTest, RejectsInvalidPointCount) {
    const char *xml = R"XML(
<Triangle points="0,0 10,0" />
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftTriangleParser parser;
    EXPECT_THROW(parser.parse(doc.child("Triangle")), docraft::exception::InvalidInputException);
}

TEST(DocraftPolygonParserTest, ParsesPolygonPoints) {
    const char *xml = R"XML(
<Polygon points="0,0 10,0 10,10 0,10" background_color="#FFFFFF" border_color="#000000" border_width="0.5" />
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftPolygonParser parser;
    const auto data = std::any_cast<docraft::craft::parser::ParsedPolygonData>(parser.parse(doc.child("Polygon")));

    ASSERT_EQ(data.points.size(), 4U);
    EXPECT_FLOAT_EQ(data.points[3].x, 0.0F);
    EXPECT_FLOAT_EQ(data.points[3].y, 10.0F);
    ASSERT_TRUE(data.border_width.has_value());
    EXPECT_FLOAT_EQ(*data.border_width, 0.5F);
}

TEST(DocraftRectangleParserTest, ParsesBorderWidth) {
    const char *xml = R"XML(
<Rectangle border_width="3" />
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftRectangleParser parser;
    const auto data = std::any_cast<docraft::craft::parser::ParsedRectangleData>(parser.parse(doc.child("Rectangle")));

    ASSERT_TRUE(data.border_width.has_value());
    EXPECT_FLOAT_EQ(*data.border_width, 3.0F);
}

TEST(DocraftRectangleParserTest, ParsesDashedBorderStyle) {
    const char *xml = R"XML(
<Rectangle border_style="dashed" />
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftRectangleParser parser;
    const auto data = std::any_cast<docraft::craft::parser::ParsedRectangleData>(parser.parse(doc.child("Rectangle")));

    ASSERT_TRUE(data.border_style.has_value());
    EXPECT_EQ(*data.border_style, docraft::craft::parser::ParsedLineStyle::kDashed);
}

TEST(DocraftLineParserTest, ParsesDashedBorderStyle) {
    const char *xml = R"XML(
<Line x1="0" y1="0" x2="10" y2="0" border_style="dashed" />
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftLineParser parser;
    const auto data = std::any_cast<docraft::craft::parser::ParsedLineData>(parser.parse(doc.child("Line")));

    ASSERT_TRUE(data.border_style.has_value());
    EXPECT_EQ(*data.border_style, docraft::craft::parser::ParsedLineStyle::kDashed);
}

TEST(DocraftLineParserTest, DefaultsBorderStyleToUnset)
{
    const char *xml = R"XML(
<Line x1="0" y1="0" x2="10" y2="0" />
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftLineParser parser;
    const auto data = std::any_cast<docraft::craft::parser::ParsedLineData>(parser.parse(doc.child("Line")));

    EXPECT_FALSE(data.border_style.has_value());
}

TEST(DocraftLineParserTest, RejectsUnrecognizedBorderStyle)
{
    const char *xml = R"XML(
<Line x1="0" y1="0" x2="10" y2="0" border_style="dotted" />
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftLineParser parser;
    EXPECT_THROW(parser.parse(doc.child("Line")), docraft::exception::InvalidInputException);
}
