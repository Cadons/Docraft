#include <gtest/gtest.h>

#include "docraft/craft/docraft_loom_craft_language_parser.h"
#include "docraft/exception/docraft_exceptions.h"
#include "docraft/loom/nodes/docraft_loom_rectangle.h"

TEST(DocraftLoomCraftLanguageParserTest, ParsesFullDocumentWithHeaderBodyFooter)
{
    const char* xml = R"XML(
<Document>
  <Header margin_left="30" margin_right="30" border_width="0.5">
    <Text>Header text</Text>
  </Header>
  <Body margin_left="20" margin_top="15">
    <Title>Hello</Title>
    <Text>Body copy</Text>
  </Body>
  <Footer margin_bottom="5">
    <PageNumber />
  </Footer>
</Document>
)XML";

    docraft::craft::DocraftLoomCraftLanguageParser parser;
    EXPECT_NO_THROW(parser.parse(xml));

    const auto creator = parser.edit_creator();
    ASSERT_TRUE(creator);

    ASSERT_TRUE(creator->root_node());
    ASSERT_TRUE(creator->header());
    ASSERT_TRUE(creator->footer());

    const auto header_rect = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomRectangle>(creator->header());
    ASSERT_TRUE(header_rect);
    EXPECT_FLOAT_EQ(header_rect->style().border_width, 0.5F);

    EXPECT_FLOAT_EQ(creator->header_margins().left, 30.0F);
    EXPECT_FLOAT_EQ(creator->header_margins().right, 30.0F);
    EXPECT_FLOAT_EQ(creator->body_margins().left, 20.0F);
    EXPECT_FLOAT_EQ(creator->body_margins().top, 15.0F);
    EXPECT_FLOAT_EQ(creator->footer_margins().bottom, 5.0F);
}

TEST(DocraftLoomCraftLanguageParserTest, ParsesDocumentWithOnlyRequiredBody)
{
    const char* xml = R"XML(
<Document>
  <Body>
    <Text>Just body</Text>
  </Body>
</Document>
)XML";

    docraft::craft::DocraftLoomCraftLanguageParser parser;
    EXPECT_NO_THROW(parser.parse(xml));

    const auto creator = parser.edit_creator();
    ASSERT_TRUE(creator);
    ASSERT_TRUE(creator->root_node());
    EXPECT_FALSE(creator->header());
    EXPECT_FALSE(creator->footer());
}

TEST(DocraftLoomCraftLanguageParserTest, IgnoresSettingsAndMetadataAtTopLevel)
{
    const char* xml = R"XML(
<Document>
  <Settings page_size="A4" />
  <Metadata>
    <DocumentTitle>Ignored For Now</DocumentTitle>
  </Metadata>
  <Body>
    <Text>Body</Text>
  </Body>
</Document>
)XML";

    docraft::craft::DocraftLoomCraftLanguageParser parser;
    EXPECT_NO_THROW(parser.parse(xml));
    ASSERT_TRUE(parser.edit_creator());
}

TEST(DocraftLoomCraftLanguageParserTest, ThrowsWhenBodyIsMissing)
{
    const char* xml = R"XML(
<Document>
  <Header><Text>Header only</Text></Header>
</Document>
)XML";

    docraft::craft::DocraftLoomCraftLanguageParser parser;
    EXPECT_THROW(parser.parse(xml), docraft::exception::DataFormatException);
}
