#include <any>

#include <gtest/gtest.h>
#include <pugixml.hpp>

#include "docraft/craft/parser/docraft_parser.h"
#include "docraft/exception/docraft_exceptions.h"

TEST(DocraftImageParserTest, ParsesBase64RawImageData) {
    const char *xml = R"XML(
<Image data="base64:AAAA////" data_width="2" data_height="1" width="20" height="10" />
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftImageParser parser;
    const auto data = std::any_cast<docraft::craft::parser::ParsedImageData>(parser.parse(doc.child("Image")));

    ASSERT_TRUE(data.raw_data.has_value());
    ASSERT_TRUE(data.raw_pixel_width.has_value());
    ASSERT_TRUE(data.raw_pixel_height.has_value());
    EXPECT_EQ(*data.raw_pixel_width, 2);
    EXPECT_EQ(*data.raw_pixel_height, 1);
    ASSERT_EQ(data.raw_data->size(), 6U);
    EXPECT_EQ((*data.raw_data)[0], 0);
    EXPECT_EQ((*data.raw_data)[3], 255);
    // width/height (20/10) are common attributes, not part of ParsedImageData itself.
}

TEST(DocraftImageParserTest, RejectsBase64MissingDimensions) {
    const char *xml = R"XML(
<Image data="base64:AAAA////" />
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftImageParser parser;
    EXPECT_THROW(parser.parse(doc.child("Image")), docraft::exception::InvalidInputException);
}

TEST(DocraftImageParserTest, ParsesSrcPath)
{
    const char* xml = R"XML(
<Image src="assets/logo.png" />
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftImageParser parser;
    const auto data = std::any_cast<docraft::craft::parser::ParsedImageData>(parser.parse(doc.child("Image")));

    ASSERT_TRUE(data.path.has_value());
    EXPECT_EQ(*data.path, "assets/logo.png");
    EXPECT_FALSE(data.raw_data.has_value());
}

TEST(DocraftImageParserTest, RejectsBothSrcAndData)
{
    const char* xml = R"XML(
<Image src="assets/logo.png" data="somekey" />
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftImageParser parser;
    EXPECT_THROW(parser.parse(doc.child("Image")), docraft::exception::InvalidInputException);
}
