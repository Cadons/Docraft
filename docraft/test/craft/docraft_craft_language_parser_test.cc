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

TEST(DocraftCraftLanguageParserTest, ParsesMetadataSubtags) {
    const char *xml = R"XML(
<Document>
  <Metadata>
    <DocumentTitle>Metadata Driven Title</DocumentTitle>
    <Author>Mario Rossi</Author>
    <Creator>Docraft Parser</Creator>
    <Producer>Docraft Engine</Producer>
    <Subject>Relazione tecnica</Subject>
    <Keywords>manuale, parser</Keywords>
    <Trapped>False</Trapped>
    <GtsPdfx>PDF/X-3:2002</GtsPdfx>
    <CreationDate year="2026" month="2" day="22" hour="10" minutes="15" seconds="5" ind="+" off_hour="1" off_minutes="0" />
    <ModificationDate>
      <Year>2026</Year>
      <Month>2</Month>
      <Day>23</Day>
      <Hour>11</Hour>
      <Minutes>20</Minutes>
      <Seconds>25</Seconds>
      <Ind>+</Ind>
      <OffHour>1</OffHour>
      <OffMinutes>0</OffMinutes>
    </ModificationDate>
  </Metadata>
  <Body>
    <Text>Body copy</Text>
  </Body>
</Document>
)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    parser.parse(xml);
    auto document = parser.get_document();
    ASSERT_TRUE(document);

    EXPECT_EQ(document->document_title(), "Metadata Driven Title");
    const auto &metadata = document->document_metadata();

    ASSERT_TRUE(metadata.author().has_value());
    EXPECT_EQ(metadata.author().value(), "Mario Rossi");
    ASSERT_TRUE(metadata.creator().has_value());
    EXPECT_EQ(metadata.creator().value(), "Docraft Parser");
    ASSERT_TRUE(metadata.producer().has_value());
    EXPECT_EQ(metadata.producer().value(), "Docraft Engine");
    ASSERT_TRUE(metadata.subject().has_value());
    EXPECT_EQ(metadata.subject().value(), "Relazione tecnica");
    ASSERT_TRUE(metadata.keywords().has_value());
    EXPECT_EQ(metadata.keywords().value(), "manuale, parser");
    ASSERT_TRUE(metadata.trapped().has_value());
    EXPECT_EQ(metadata.trapped().value(), "False");
    ASSERT_TRUE(metadata.gts_pdfx().has_value());
    EXPECT_EQ(metadata.gts_pdfx().value(), "PDF/X-3:2002");

    ASSERT_TRUE(metadata.creation_date().has_value());
    EXPECT_EQ(metadata.creation_date()->year, 2026);
    EXPECT_EQ(metadata.creation_date()->month, 2);
    EXPECT_EQ(metadata.creation_date()->day, 22);
    EXPECT_EQ(metadata.creation_date()->hour, 10);
    EXPECT_EQ(metadata.creation_date()->minutes, 15);
    EXPECT_EQ(metadata.creation_date()->seconds, 5);
    EXPECT_EQ(metadata.creation_date()->ind, '+');
    EXPECT_EQ(metadata.creation_date()->off_hour, 1);
    EXPECT_EQ(metadata.creation_date()->off_minutes, 0);

    ASSERT_TRUE(metadata.modification_date().has_value());
    EXPECT_EQ(metadata.modification_date()->year, 2026);
    EXPECT_EQ(metadata.modification_date()->month, 2);
    EXPECT_EQ(metadata.modification_date()->day, 23);
    EXPECT_EQ(metadata.modification_date()->hour, 11);
    EXPECT_EQ(metadata.modification_date()->minutes, 20);
    EXPECT_EQ(metadata.modification_date()->seconds, 25);
    EXPECT_EQ(metadata.modification_date()->ind, '+');
    EXPECT_EQ(metadata.modification_date()->off_hour, 1);
    EXPECT_EQ(metadata.modification_date()->off_minutes, 0);
}

TEST(DocraftCraftLanguageParserTest, AutoKeywordsAreExtractedAndMergedWhenEnabled) {
    const char *xml = R"XML(
<Document>
  <Metadata>
    <Keywords>manuale</Keywords>
    <AutoKeywords enabled="true" max_keywords="4" min_length="5" />
  </Metadata>
  <Body>
    <Text>Documento parser parser parser metadata tecnica tecnica rendering</Text>
    <Text>Metadata documento keyword extraction parser</Text>
  </Body>
</Document>
)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    parser.parse(xml);
    auto document = parser.get_document();
    ASSERT_TRUE(document);

    const auto &metadata = document->document_metadata();
    ASSERT_TRUE(metadata.keywords().has_value());
    EXPECT_EQ(metadata.keywords().value(), "manuale, parser, documento, metadata, tecnica");
}

TEST(DocraftCraftLanguageParserTest, AutoKeywordsUsesConfiguredStopwordLanguages) {
    const char *xml = R"XML(
<Document>
  <Metadata>
    <AutoKeywords enabled="true" max_keywords="3" min_length="2" language="es,de,fr" />
  </Metadata>
  <Body>
    <Text>el el der der et et modelo system analyse</Text>
  </Body>
</Document>
)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    parser.parse(xml);
    auto document = parser.get_document();
    ASSERT_TRUE(document);

    const auto &metadata = document->document_metadata();
    ASSERT_TRUE(metadata.keywords().has_value());
    EXPECT_EQ(metadata.keywords().value(), "analyse, modelo, system");
}

TEST(DocraftCraftLanguageParserTest, ParsesDocumentPathAttribute) {
    const char *xml = R"XML(
<Document path="exports/reports">
  <Body>
    <Text>Body copy</Text>
  </Body>
</Document>
)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    parser.parse(xml);
    auto document = parser.get_document();
    ASSERT_TRUE(document);

    EXPECT_EQ(document->document_path(), "exports/reports");
}

TEST(DocraftCraftLanguageParserTest, RejectsNestedTextInText) {
    const char *xml = R"XML(
<Document>
  <Body>
    <Text>
      <Text>Nested text</Text>
    </Text>
  </Body>
</Document>
)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    EXPECT_THROW({
        parser.parse(xml);
    }, std::invalid_argument);
}

TEST(DocraftCraftLanguageParserTest, RejectsTitleInText) {
    const char *xml = R"XML(
<Document>
  <Body>
    <Text>
      <Title>Nested title</Title>
    </Text>
  </Body>
</Document>
)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    EXPECT_THROW({
        parser.parse(xml);
    }, std::invalid_argument);
}

TEST(DocraftCraftLanguageParserTest, RejectsSubtitleInText) {
    const char *xml = R"XML(
<Document>
  <Body>
    <Text>
      <Subtitle>Nested subtitle</Subtitle>
    </Text>
  </Body>
</Document>
)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    EXPECT_THROW({
        parser.parse(xml);
    }, std::invalid_argument);
}

TEST(DocraftCraftLanguageParserTest, RejectsPageNumberInText) {
    const char *xml = R"XML(
<Document>
  <Body>
    <Text>
      Page: <PageNumber />
    </Text>
  </Body>
</Document>
)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    EXPECT_THROW({
        parser.parse(xml);
    }, std::invalid_argument);
}

TEST(DocraftCraftLanguageParserTest, AllowsLayoutInBodyWithMultipleText) {
    const char *xml = R"XML(
<Document>
  <Body>
    <Layout orientation="vertical">
      <Text>First line</Text>
      <Text>Second line</Text>
    </Layout>
  </Body>
</Document>
)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    EXPECT_NO_THROW({
        parser.parse(xml);
    });

    auto document = parser.get_document();
    ASSERT_TRUE(document);

    const auto texts = document->get_by_type<docraft::model::DocraftText>();
    ASSERT_EQ(texts.size(), 2U);
    EXPECT_EQ(texts[0]->text(), "First line");
    EXPECT_EQ(texts[1]->text(), "Second line");
}

