#include <gtest/gtest.h>
#include <pugixml.hpp>

#include "craft/parser/docraft_parser.h"
#include "model/docraft_image.h"

TEST(DocraftImageParserTest, ParsesBase64RawImageData) {
    const char *xml = R"XML(
<Image data="base64:AAAA////" data_width="2" data_height="1" width="20" height="10" />
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftImageParser parser;
    auto node = parser.parse(doc.child("Image"));
    auto image = std::dynamic_pointer_cast<docraft::model::DocraftImage>(node);
    ASSERT_TRUE(image);

    EXPECT_EQ(image->format(), docraft::model::ImageFormat::kRaw);
    EXPECT_TRUE(image->has_raw_data());
    EXPECT_EQ(image->raw_pixel_width(), 2);
    EXPECT_EQ(image->raw_pixel_height(), 1);
    ASSERT_EQ(image->raw_data().size(), 6U);
    EXPECT_EQ(image->raw_data()[0], 0);
    EXPECT_EQ(image->raw_data()[3], 255);
}

TEST(DocraftImageParserTest, RejectsBase64MissingDimensions) {
    const char *xml = R"XML(
<Image data="base64:AAAA////" />
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftImageParser parser;
    EXPECT_THROW(parser.parse(doc.child("Image")), std::invalid_argument);
}
