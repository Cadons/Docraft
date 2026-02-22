#include <gtest/gtest.h>

#include "craft/docraft_craft_language_parser.h"
#include "model/docraft_text.h"

TEST(DocraftCraftLanguageParserTest, ParsesTitleSubtitleAndTextWithPredefinedDefaults) {
    const char *xml = R"XML(
<Document>
  <Body>
    <Title>Main Heading</Title>
    <Subtitle>Section Heading</Subtitle>
    <Text>Body copy</Text>
  </Body>
</Document>
)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    parser.parse(xml);
    auto document = parser.get_document();
    ASSERT_TRUE(document);

    const auto texts = document->get_by_type<docraft::model::DocraftText>();
    ASSERT_EQ(texts.size(), 3U);

    EXPECT_EQ(texts[0]->text(), "Main Heading");
    EXPECT_FLOAT_EQ(texts[0]->font_size(), 24.0F);
    EXPECT_EQ(texts[0]->style(), docraft::model::TextStyle::kBold);

    EXPECT_EQ(texts[1]->text(), "Section Heading");
    EXPECT_FLOAT_EQ(texts[1]->font_size(), 18.0F);
    EXPECT_EQ(texts[1]->style(), docraft::model::TextStyle::kBold);

    EXPECT_EQ(texts[2]->text(), "Body copy");
    EXPECT_FLOAT_EQ(texts[2]->font_size(), 12.0F);
    EXPECT_EQ(texts[2]->style(), docraft::model::TextStyle::kNormal);
}

TEST(DocraftCraftLanguageParserTest, HeadingAttributesOverridePredefinedDefaults) {
    const char *xml = R"XML(
<Document>
  <Body>
    <Title font_size="30" style="italic">Main Heading</Title>
    <Subtitle style="normal">Section Heading</Subtitle>
  </Body>
</Document>
)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    parser.parse(xml);
    auto document = parser.get_document();
    ASSERT_TRUE(document);

    const auto texts = document->get_by_type<docraft::model::DocraftText>();
    ASSERT_EQ(texts.size(), 2U);

    EXPECT_FLOAT_EQ(texts[0]->font_size(), 30.0F);
    EXPECT_EQ(texts[0]->style(), docraft::model::TextStyle::kItalic);

    EXPECT_FLOAT_EQ(texts[1]->font_size(), 18.0F);
    EXPECT_EQ(texts[1]->style(), docraft::model::TextStyle::kNormal);
}
