#include <any>

#include <gtest/gtest.h>
#include <pugixml.hpp>

#include "docraft/craft/parser/docraft_paragraph_parser.h"
#include "docraft/craft/parser/docraft_parser.h"
#include "docraft/exception/docraft_exceptions.h"

TEST(DocraftParagraphParserTest, ParsesTypographicAttributes)
{
    const char* xml = R"XML(
<Paragraph line_spacing="1.5" space_before="4" space_after="6" alignment="justified" />
)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftParagraphParser parser;
    const auto data = std::any_cast<docraft::craft::parser::ParsedParagraphData>(parser.parse(doc.child("Paragraph")));

    ASSERT_TRUE(data.line_spacing.has_value());
    EXPECT_FLOAT_EQ(*data.line_spacing, 1.5F);
    ASSERT_TRUE(data.space_before.has_value());
    EXPECT_FLOAT_EQ(*data.space_before, 4.0F);
    ASSERT_TRUE(data.space_after.has_value());
    EXPECT_FLOAT_EQ(*data.space_after, 6.0F);
    ASSERT_TRUE(data.alignment.has_value());
    EXPECT_EQ(*data.alignment, docraft::craft::parser::ParsedTextAlignment::kJustified);
}

TEST(DocraftParagraphParserTest, AllFieldsAreEmptyWhenNoAttributesGiven)
{
    const char* xml = R"XML(<Paragraph />)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftParagraphParser parser;
    const auto data = std::any_cast<docraft::craft::parser::ParsedParagraphData>(parser.parse(doc.child("Paragraph")));

    EXPECT_FALSE(data.line_spacing.has_value());
    EXPECT_FALSE(data.space_before.has_value());
    EXPECT_FALSE(data.space_after.has_value());
    EXPECT_FALSE(data.alignment.has_value());
}

TEST(DocraftParagraphParserTest, ThrowsForInvalidAlignment)
{
    const char* xml = R"XML(<Paragraph alignment="diagonal" />)XML";

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_string(xml));

    docraft::craft::parser::DocraftParagraphParser parser;
    EXPECT_THROW(parser.parse(doc.child("Paragraph")), docraft::exception::InvalidInputException);
}
