#include <gtest/gtest.h>
#include <pugixml.hpp>

#include "docraft/craft/parser/docraft_circle_parser.h"
#include "docraft/craft/parser/docraft_line_parser.h"
#include "docraft/craft/parser/docraft_polygon_parser.h"
#include "docraft/craft/parser/docraft_triangle_parser.h"
#include "docraft/craft/parser/docraft_parser.h"
#include "docraft/docraft_color.h"
#include "docraft/model/docraft_circle.h"
#include "docraft/model/docraft_line.h"
#include "docraft/model/docraft_polygon.h"
#include "docraft/model/docraft_rectangle.h"
#include "docraft/model/docraft_triangle.h"

namespace {
    docraft::RGB to_rgb(const docraft::DocraftColor &color) {
        return color.toRGB();
    }
}

TEST(DocraftCircleParserTest, ParsesCircleAttributes) {
    const char *xml = R"XML(
<Circle width="40" height="40" background_color="#FF0000" border_color="#00FF00" border_width="2" />
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftCircleParser parser;
    auto node = parser.parse(doc.child("Circle"));
    auto circle = std::dynamic_pointer_cast<docraft::model::DocraftCircle>(node);
    ASSERT_TRUE(circle);

    EXPECT_FLOAT_EQ(circle->width(), 40.0F);
    EXPECT_FLOAT_EQ(circle->height(), 40.0F);
    EXPECT_FLOAT_EQ(circle->border_width(), 2.0F);

    auto bg = to_rgb(circle->background_color());
    EXPECT_FLOAT_EQ(bg.r, 1.0F);
    EXPECT_FLOAT_EQ(bg.g, 0.0F);
    EXPECT_FLOAT_EQ(bg.b, 0.0F);

    auto border = to_rgb(circle->border_color());
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
    auto node = parser.parse(doc.child("Line"));
    auto line = std::dynamic_pointer_cast<docraft::model::DocraftLine>(node);
    ASSERT_TRUE(line);

    EXPECT_FLOAT_EQ(line->start().x, 0.0F);
    EXPECT_FLOAT_EQ(line->start().y, 0.0F);
    EXPECT_FLOAT_EQ(line->end().x, 10.0F);
    EXPECT_FLOAT_EQ(line->end().y, 20.0F);
    EXPECT_FLOAT_EQ(line->border_width(), 1.0F);
    EXPECT_FLOAT_EQ(line->position().x, 5.0F);
    EXPECT_FLOAT_EQ(line->position().y, 50.0F);
}

TEST(DocraftTriangleParserTest, ParsesTrianglePoints) {
    const char *xml = R"XML(
<Triangle points="0,0 10,0 5,5" background_color="red" border_color="blue" border_width="1" />
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftTriangleParser parser;
    auto node = parser.parse(doc.child("Triangle"));
    auto triangle = std::dynamic_pointer_cast<docraft::model::DocraftTriangle>(node);
    ASSERT_TRUE(triangle);

    ASSERT_EQ(triangle->points().size(), 3U);
    EXPECT_FLOAT_EQ(triangle->points()[2].x, 5.0F);
    EXPECT_FLOAT_EQ(triangle->points()[2].y, 5.0F);
    EXPECT_FLOAT_EQ(triangle->border_width(), 1.0F);
}

TEST(DocraftTriangleParserTest, RejectsInvalidPointCount) {
    const char *xml = R"XML(
<Triangle points="0,0 10,0" />
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftTriangleParser parser;
    EXPECT_THROW(parser.parse(doc.child("Triangle")), std::invalid_argument);
}

TEST(DocraftPolygonParserTest, ParsesPolygonPoints) {
    const char *xml = R"XML(
<Polygon points="0,0 10,0 10,10 0,10" background_color="#FFFFFF" border_color="#000000" border_width="0.5" />
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftPolygonParser parser;
    auto node = parser.parse(doc.child("Polygon"));
    auto polygon = std::dynamic_pointer_cast<docraft::model::DocraftPolygon>(node);
    ASSERT_TRUE(polygon);

    ASSERT_EQ(polygon->points().size(), 4U);
    EXPECT_FLOAT_EQ(polygon->points()[3].x, 0.0F);
    EXPECT_FLOAT_EQ(polygon->points()[3].y, 10.0F);
    EXPECT_FLOAT_EQ(polygon->border_width(), 0.5F);
}

TEST(DocraftRectangleParserTest, ParsesBorderWidth) {
    const char *xml = R"XML(
<Rectangle border_width="3" />
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftRectangleParser parser;
    auto node = parser.parse(doc.child("Rectangle"));
    auto rect = std::dynamic_pointer_cast<docraft::model::DocraftRectangle>(node);
    ASSERT_TRUE(rect);

    EXPECT_FLOAT_EQ(rect->border_width(), 3.0F);
}
