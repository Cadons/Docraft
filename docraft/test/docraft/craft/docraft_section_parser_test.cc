#include <any>

#include <gtest/gtest.h>
#include <pugixml.hpp>

#include "docraft/craft/parser/docraft_parser_helpers.h"
#include "docraft/craft/parser/docraft_section_parsers.h"
#include "docraft/docraft_color.h"

TEST(DocraftSectionParserTest, ParsesBackgroundBorderAndMargins)
{
    const char* xml = R"XML(
<Header background_color="#00FF00" border_color="#0000FF" border_width="1.5"
        margin_top="1" margin_bottom="2" margin_left="3" margin_right="4" />
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftSectionParser parser;
    const auto data = std::any_cast<docraft::craft::parser::ParsedSectionData>(parser.parse(doc.child("Header")));

    ASSERT_TRUE(data.background_color.has_value());
    EXPECT_FLOAT_EQ(docraft::craft::parser::detail::parse_docraft_color(*data.background_color).toRGB().g, 1.0F);
    ASSERT_TRUE(data.border_color.has_value());
    EXPECT_FLOAT_EQ(docraft::craft::parser::detail::parse_docraft_color(*data.border_color).toRGB().b, 1.0F);
    ASSERT_TRUE(data.border_width.has_value());
    EXPECT_FLOAT_EQ(*data.border_width, 1.5F);

    ASSERT_TRUE(data.margin_top.has_value());
    EXPECT_FLOAT_EQ(*data.margin_top, 1.0F);
    ASSERT_TRUE(data.margin_bottom.has_value());
    EXPECT_FLOAT_EQ(*data.margin_bottom, 2.0F);
    ASSERT_TRUE(data.margin_left.has_value());
    EXPECT_FLOAT_EQ(*data.margin_left, 3.0F);
    ASSERT_TRUE(data.margin_right.has_value());
    EXPECT_FLOAT_EQ(*data.margin_right, 4.0F);
}

TEST(DocraftSectionParserTest, AllFieldsAreEmptyWhenNoAttributesGiven)
{
    const char* xml = R"XML(<Body />)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftSectionParser parser;
    const auto data = std::any_cast<docraft::craft::parser::ParsedSectionData>(parser.parse(doc.child("Body")));

    EXPECT_FALSE(data.background_color.has_value());
    EXPECT_FALSE(data.border_color.has_value());
    EXPECT_FALSE(data.border_width.has_value());
    EXPECT_FALSE(data.margin_top.has_value());
    EXPECT_FALSE(data.margin_bottom.has_value());
    EXPECT_FALSE(data.margin_left.has_value());
    EXPECT_FALSE(data.margin_right.has_value());
}
