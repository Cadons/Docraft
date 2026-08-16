#include <any>

#include <gtest/gtest.h>

#include "docraft/craft/docraft_craft_language_parser.h"
#include "docraft/craft/parser/docraft_foreach_parser.h"
#include "docraft/craft/parser/docraft_parser.h"
#include "docraft/exception/docraft_exceptions.h"

TEST(DocraftCraftLanguageParserTest, ParsesTitleSubtitleAndTextIdentically)
{
    // Heading-like defaults (font size/style/margin) now live on
    // DocraftLoomTitle/DocraftLoomSubtitle's own constructors, not at this
    // engine-agnostic parsing layer (see docraft_loom_tree_builder_test.cc's
    // BuildsTitleWithHeadingDefaults/BuildsSubtitleWithHeadingDefaults) -- so Title/
    // Subtitle/Text all parse into the same shape of ParsedTextData here.
    const char* xml = R"XML(
<Layout orientation="vertical">
  <Title>Main Heading</Title>
  <Subtitle>Section Heading</Subtitle>
  <Text>Body copy</Text>
</Layout>
)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    const auto root = parser.parse(xml);
    ASSERT_TRUE(root);
    EXPECT_EQ(root->tag_name, "Layout");
    ASSERT_EQ(root->children.size(), 3U);

    const auto title = std::any_cast<docraft::craft::parser::ParsedTextData>(root->children[0]->data);
    EXPECT_EQ(title.text, "Main Heading");
    EXPECT_FALSE(title.font_size.has_value());
    EXPECT_FALSE(title.style.has_value());

    const auto subtitle = std::any_cast<docraft::craft::parser::ParsedTextData>(root->children[1]->data);
    EXPECT_EQ(subtitle.text, "Section Heading");
    EXPECT_FALSE(subtitle.font_size.has_value());
    EXPECT_FALSE(subtitle.style.has_value());

    const auto text = std::any_cast<docraft::craft::parser::ParsedTextData>(root->children[2]->data);
    EXPECT_EQ(text.text, "Body copy");
    EXPECT_FALSE(text.font_size.has_value());
    EXPECT_FALSE(text.style.has_value());
}

TEST(DocraftCraftLanguageParserTest, ParsesExplicitHeadingAttributes)
{
    const char* xml = R"XML(
<Layout orientation="vertical">
  <Title font_size="30" style="italic">Main Heading</Title>
  <Subtitle style="normal">Section Heading</Subtitle>
</Layout>
)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    const auto root = parser.parse(xml);
    ASSERT_TRUE(root);
    ASSERT_EQ(root->children.size(), 2U);

    const auto title = std::any_cast<docraft::craft::parser::ParsedTextData>(root->children[0]->data);
    ASSERT_TRUE(title.font_size.has_value());
    EXPECT_FLOAT_EQ(*title.font_size, 30.0F);
    ASSERT_TRUE(title.style.has_value());
    EXPECT_EQ(*title.style, docraft::craft::parser::ParsedTextStyle::kItalic);

    const auto subtitle = std::any_cast<docraft::craft::parser::ParsedTextData>(root->children[1]->data);
    // No font_size attribute given -- this parsing layer no longer pre-fills a
    // tag-based default (that now lives on DocraftLoomSubtitle's own constructor).
    EXPECT_FALSE(subtitle.font_size.has_value());
    ASSERT_TRUE(subtitle.style.has_value());
    EXPECT_EQ(*subtitle.style, docraft::craft::parser::ParsedTextStyle::kNormal);
}

TEST(DocraftCraftLanguageParserTest, ParsesCommonAttributesGenerically)
{
    const char* xml = R"XML(
<Rectangle name="box" width="100" height="50" padding="4" z_index="2" x="5" y="7" />
)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    const auto root = parser.parse(xml);
    ASSERT_TRUE(root);
    EXPECT_EQ(root->tag_name, "Rectangle");
    ASSERT_TRUE(root->common.name.has_value());
    EXPECT_EQ(*root->common.name, "box");
    ASSERT_TRUE(root->common.width.has_value());
    EXPECT_FLOAT_EQ(*root->common.width, 100.0F);
    ASSERT_TRUE(root->common.height.has_value());
    EXPECT_FLOAT_EQ(*root->common.height, 50.0F);
    ASSERT_TRUE(root->common.padding.has_value());
    EXPECT_FLOAT_EQ(*root->common.padding, 4.0F);
    ASSERT_TRUE(root->common.z_index.has_value());
    EXPECT_EQ(*root->common.z_index, 2);
    // Explicit x/y with no "position" attribute implies absolute placement.
    ASSERT_TRUE(root->common.position_mode.has_value());
    EXPECT_EQ(*root->common.position_mode, docraft::craft::PositionMode::kAbsolute);
}

TEST(DocraftCraftLanguageParserTest, RejectsNestedTextInText)
{
    const char* xml = R"XML(
<Text>
  <Text>Nested text</Text>
</Text>
)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    EXPECT_THROW(parser.parse(xml), docraft::exception::InvalidInputException);
}

TEST(DocraftCraftLanguageParserTest, RejectsTitleInText)
{
    const char* xml = R"XML(
<Text>
  <Title>Nested title</Title>
</Text>
)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    EXPECT_THROW(parser.parse(xml), docraft::exception::InvalidInputException);
}

TEST(DocraftCraftLanguageParserTest, RejectsSubtitleInText)
{
    const char* xml = R"XML(
<Text>
  <Subtitle>Nested subtitle</Subtitle>
</Text>
)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    EXPECT_THROW(parser.parse(xml), docraft::exception::InvalidInputException);
}

TEST(DocraftCraftLanguageParserTest, RejectsPageNumberInText)
{
    const char* xml = R"XML(
<Text>
  Page: <PageNumber />
</Text>
)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    EXPECT_THROW(parser.parse(xml), docraft::exception::InvalidInputException);
}

TEST(DocraftCraftLanguageParserTest, AllowsLayoutWithMultipleText)
{
    const char* xml = R"XML(
<Layout orientation="vertical">
  <Text>First line</Text>
  <Text>Second line</Text>
</Layout>
)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    std::shared_ptr<docraft::craft::DocraftParsedElement> root;
    EXPECT_NO_THROW({root = parser.parse(xml); });

    ASSERT_TRUE(root);
    ASSERT_EQ(root->children.size(), 2U);
    EXPECT_EQ(std::any_cast<docraft::craft::parser::ParsedTextData>(root->children[0]->data).text, "First line");
    EXPECT_EQ(std::any_cast<docraft::craft::parser::ParsedTextData>(root->children[1]->data).text, "Second line");
}

TEST(DocraftCraftLanguageParserTest, RejectsNonTextChildInList)
{
    const char* xml = R"XML(
<List>
  <Image src="logo.png" />
</List>
)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    EXPECT_THROW(parser.parse(xml), docraft::exception::InvalidInputException);
}

TEST(DocraftCraftLanguageParserTest, ThrowsForUnregisteredTag)
{
    const char* xml = R"XML(
<Bogus />
)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    EXPECT_THROW(parser.parse(xml), docraft::exception::DataFormatException);
}

TEST(DocraftCraftLanguageParserTest, ParsesNewPageWithNoAttributes)
{
    const char* xml = R"XML(<NewPage />)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    const auto root = parser.parse(xml);
    ASSERT_TRUE(root);
    EXPECT_EQ(root->tag_name, "NewPage");
    EXPECT_NO_THROW(std::any_cast<docraft::craft::parser::ParsedNewPageData>(root->data));
}

TEST(DocraftCraftLanguageParserTest, ParsesForeachWithModel)
{
    const char* xml = R"XML(
<Foreach model="[{'name':'Alice'}]">
  <Text>${data("name")}</Text>
</Foreach>
)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    const auto root = parser.parse(xml);
    ASSERT_TRUE(root);
    ASSERT_EQ(root->children.size(), 1U);
    const auto data = std::any_cast<docraft::craft::parser::ParsedForeachData>(root->data);
    ASSERT_TRUE(data.model.has_value());
    EXPECT_FALSE(data.count.has_value());
    // Stored verbatim -- single-quote normalization and ${...} resolution are deferred to
    // DocraftLoomTreeBuilder::expand_foreach() at build time (see
    // ForeachModel* tests in docraft_loom_tree_builder_test.cc).
    EXPECT_EQ(*data.model, R"([{'name':'Alice'}])");
}

TEST(DocraftCraftLanguageParserTest, ParsesForeachWithN)
{
    const char* xml = R"XML(
<Foreach n="4">
  <Text>Row</Text>
</Foreach>
)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    const auto root = parser.parse(xml);
    ASSERT_TRUE(root);
    const auto data = std::any_cast<docraft::craft::parser::ParsedForeachData>(root->data);
    EXPECT_FALSE(data.model.has_value());
    ASSERT_TRUE(data.count.has_value());
    EXPECT_EQ(*data.count, 4);
}

TEST(DocraftCraftLanguageParserTest, ThrowsWhenForeachHasBothModelAndN)
{
    const char* xml = R"XML(<Foreach model="[]" n="2" />)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    EXPECT_THROW(parser.parse(xml), docraft::exception::InvalidInputException);
}

TEST(DocraftCraftLanguageParserTest, ThrowsWhenForeachHasNeitherModelNorN)
{
    const char* xml = R"XML(<Foreach />)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    EXPECT_THROW(parser.parse(xml), docraft::exception::InvalidInputException);
}

TEST(DocraftCraftLanguageParserTest, ThrowsWhenForeachNIsNegative)
{
    const char* xml = R"XML(<Foreach n="-1" />)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    EXPECT_THROW(parser.parse(xml), docraft::exception::InvalidInputException);
}

// ── Unknown attributes are rejected, like unknown tags ────────────────────────

namespace {
    std::shared_ptr<docraft::craft::DocraftParsedElement> parse_craft(const char* xml)
    {
        docraft::craft::DocraftCraftLanguageParser parser;
        return parser.parse(xml);
    }
} // namespace

TEST(DocraftCraftLanguageParserTest, RejectsAnAttributeNoParserReads)
{
    // The whole point: an attribute that would have been silently dropped, and so had no
    // effect on the rendered document, is now reported instead.
    EXPECT_THROW(parse_craft(R"XML(<Rectangle nonsense="1"/>)XML"),
                 docraft::exception::InvalidInputException);
}

TEST(DocraftCraftLanguageParserTest, UnknownAttributeMessageNamesTheAttributeTagAndWhatIsAccepted)
{
    try
    {
        parse_craft(R"XML(<Rectangle wdith="10"/>)XML");
        FAIL() << "expected a typo'd attribute to be rejected";
    }
    catch (const docraft::exception::InvalidInputException& e)
    {
        const std::string message = e.what();
        EXPECT_NE(message.find("wdith"), std::string::npos);
        EXPECT_NE(message.find("Rectangle"), std::string::npos);
        // Listing what the element does accept is what turns the error into a fix.
        EXPECT_NE(message.find("background_color"), std::string::npos);
    }
}

TEST(DocraftCraftLanguageParserTest, AcceptsTagSpecificAndCommonAttributes)
{
    EXPECT_NO_THROW(parse_craft(R"XML(<Rectangle background_color="red" border_width="2"/>)XML"));
    EXPECT_NO_THROW(parse_craft(R"XML(<Rectangle name="r" x="1" y="2" width="3" height="4"
                                                 padding="5" margin="6"
                                                 position="absolute" z_index="2" visible="true"/>)XML"));
}

// `weight` is only meaningful on a direct child of <Layout> (DocraftLoomTreeBuilder
// collects it there to build the parent HStack/VStack's weights()) -- anywhere else it
// would silently no-op, so it's rejected outright instead.
TEST(DocraftCraftLanguageParserTest, AcceptsWeightAttributeOnLayoutChild)
{
    EXPECT_NO_THROW(parse_craft(R"XML(<Layout><Rectangle weight="0.5"/></Layout>)XML"));
}

TEST(DocraftCraftLanguageParserTest, RejectsWeightAttributeOutsideLayoutChild)
{
    try
    {
        parse_craft(R"XML(<Rectangle weight="1"/>)XML");
        FAIL() << "expected weight outside a <Layout> child to be rejected";
    }
    catch (const docraft::exception::InvalidInputException& e)
    {
        const std::string message = e.what();
        EXPECT_NE(message.find("weight"), std::string::npos);
        EXPECT_NE(message.find("Layout"), std::string::npos);
    }
}

TEST(DocraftCraftLanguageParserTest, RejectsExplicitSizeOnNodesWithoutExplicitSizeSemantics)
{
    for (const std::string_view tag : {"List", "Paragraph", "Line", "CurveLine", "Table"})
    {
        for (const std::string_view attribute : {"width", "height"})
        {
            SCOPED_TRACE(std::string(tag) + " " + std::string(attribute));
            const std::string xml = "<" + std::string(tag) + " " + std::string(attribute) + "=\"400\"/>";
            try
            {
                parse_craft(xml.c_str());
                FAIL() << "expected unsupported explicit sizing to be rejected";
            }
            catch (const docraft::exception::InvalidInputException& e)
            {
                const std::string message = e.what();
                EXPECT_NE(message.find(attribute), std::string::npos);
                EXPECT_NE(message.find(tag), std::string::npos);
                EXPECT_NE(message.find("not supported"), std::string::npos);
            }
        }
    }
}

TEST(DocraftCraftLanguageParserTest, TagSpecificAttributesDoNotLeakBetweenElements)
{
    // `points` belongs to Polygon/Triangle, `radius` to Circle: each element accepts only
    // the names its own parser reads.
    EXPECT_NO_THROW(parse_craft(R"XML(<Polygon points="0,0 10,0 5,10"/>)XML"));
    EXPECT_THROW(parse_craft(R"XML(<Rectangle points="0,0 10,0 5,10"/>)XML"),
                 docraft::exception::InvalidInputException);
    EXPECT_THROW(parse_craft(R"XML(<Polygon radius="10"/>)XML"),
                 docraft::exception::InvalidInputException);
}

TEST(DocraftCraftLanguageParserTest, RejectsTokensThatExistButAreReadNowhere)
{
    // auto_fill_width/auto_fill_height are declared token names that no parser has ever
    // consumed. Accepting them would be the exact silent no-op this check exists to end.
    EXPECT_THROW(parse_craft(R"XML(<Rectangle auto_fill_width="true"/>)XML"),
                 docraft::exception::InvalidInputException);
}

TEST(DocraftCraftLanguageParserTest, TableSubElementsAcceptOnlyTheirOwnAttributes)
{
    const char* cell_with_width = R"XML(
<Table><THead><HTitle>H</HTitle></THead>
  <TBody><Row background_color="#EEEEEE"><Cell width="60" background_color="#FFFFFF"><Text>v</Text></Cell></Row></TBody>
</Table>)XML";
    EXPECT_NO_THROW(parse_craft(cell_with_width));

    // A Cell is a slot in the grid, not a node with its own geometry: alignment belongs
    // to the text inside it, and the common positioning attributes mean nothing here.
    const char* cell_with_alignment = R"XML(
<Table><THead><HTitle>H</HTitle></THead>
  <TBody><Row><Cell alignment="center"><Text>v</Text></Cell></Row></TBody>
</Table>)XML";
    EXPECT_THROW(parse_craft(cell_with_alignment), docraft::exception::InvalidInputException);

    const char* cell_with_x = R"XML(
<Table><THead><HTitle>H</HTitle></THead>
  <TBody><Row><Cell x="5"><Text>v</Text></Cell></Row></TBody>
</Table>)XML";
    EXPECT_THROW(parse_craft(cell_with_x), docraft::exception::InvalidInputException);
}

TEST(DocraftCraftLanguageParserTest, TableTitlesAcceptOnlyTheirOwnAttributes)
{
    const char* styled = R"XML(
<Table><THead><HTitle alignment="left" style="bold" color="red" background_color="#EEE">H</HTitle></THead>
  <TBody><Row><Cell><Text>v</Text></Cell></Row></TBody>
</Table>)XML";
    EXPECT_NO_THROW(parse_craft(styled));

    const char* unknown = R"XML(
<Table><THead><HTitle nonsense="1">H</HTitle></THead>
  <TBody><Row><Cell><Text>v</Text></Cell></Row></TBody>
</Table>)XML";
    EXPECT_THROW(parse_craft(unknown), docraft::exception::InvalidInputException);
}
