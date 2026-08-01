#include <gtest/gtest.h>

#include "docraft/craft/docraft_craft_language_parser.h"
#include "docraft/exception/docraft_exceptions.h"
#include "docraft/loom/craft/docraft_loom_tree_builder.h"
#include "docraft/loom/nodes/docraft_loom_blank_line.h"
#include "docraft/loom/nodes/docraft_loom_canvas.h"
#include "docraft/loom/nodes/docraft_loom_circle.h"
#include "docraft/loom/nodes/docraft_loom_hstack.h"
#include "docraft/loom/nodes/docraft_loom_image.h"
#include "docraft/loom/nodes/docraft_loom_list.h"
#include "docraft/loom/nodes/docraft_loom_new_page.h"
#include "docraft/loom/nodes/docraft_loom_page_number.h"
#include "docraft/loom/nodes/docraft_loom_paragraph.h"
#include "docraft/loom/nodes/docraft_loom_rectangle.h"
#include "docraft/loom/nodes/docraft_loom_table.h"
#include "docraft/loom/nodes/docraft_loom_table_cell.h"
#include "docraft/loom/nodes/docraft_loom_text.h"
#include "docraft/loom/nodes/docraft_loom_vstack.h"
#include "docraft/templating/docraft_template_engine.h"

namespace {
    std::shared_ptr<docraft::loom::nodes::DocraftLoomNode> parse_and_build(const char* xml)
    {
        docraft::craft::DocraftCraftLanguageParser parser;
        const auto element = parser.parse(xml);
        docraft::loom::craft::DocraftLoomTreeBuilder builder;
        return builder.build(element);
    }

    std::shared_ptr<docraft::loom::nodes::DocraftLoomNode> parse_and_build_with_engine(
        const char* xml, std::shared_ptr<docraft::templating::DocraftTemplateEngine> engine)
    {
        docraft::craft::DocraftCraftLanguageParser parser;
        const auto element = parser.parse(xml);
        docraft::loom::craft::DocraftLoomTreeBuilder builder(std::move(engine));
        return builder.build(element);
    }
} // namespace

TEST(DocraftLoomTreeBuilderTest, BuildsRectangleWithStyleAndCommonAttributes)
{
    const char* xml = R"XML(
<Rectangle name="box" width="100" height="50" z_index="3" background_color="#FF0000" border_width="2">
  <Text>inside</Text>
</Rectangle>
)XML";

    const auto node = parse_and_build(xml);
    const auto rectangle = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomRectangle>(node);
    ASSERT_TRUE(rectangle);

    EXPECT_FLOAT_EQ(rectangle->width(), 100.0F);
    EXPECT_FLOAT_EQ(rectangle->height(), 50.0F);
    EXPECT_EQ(rectangle->name(), "box");
    EXPECT_EQ(rectangle->z_index(), 3);
    EXPECT_FLOAT_EQ(rectangle->style().border_width, 2.0F);
    EXPECT_FLOAT_EQ(rectangle->style().background_color.toRGB().r, 1.0F);

    ASSERT_EQ(rectangle->children_count(), 1);
    const auto text = std::dynamic_pointer_cast<const docraft::loom::nodes::DocraftLoomText>(rectangle->child(0));
    ASSERT_TRUE(text);
    EXPECT_EQ(text->text(), "inside");
}

TEST(DocraftLoomTreeBuilderTest, BuildsCanvasWithStyleCommonAttributesAndChildLocalPosition)
{
    const char* xml = R"XML(
<Canvas name="art" width="200" height="150" background_color="#00FF00">
  <Text x="10" y="20">inside</Text>
</Canvas>
)XML";

    const auto node = parse_and_build(xml);
    const auto canvas = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomCanvas>(node);
    ASSERT_TRUE(canvas);

    EXPECT_FLOAT_EQ(canvas->width(), 200.0F);
    EXPECT_FLOAT_EQ(canvas->height(), 150.0F);
    EXPECT_EQ(canvas->name(), "art");
    EXPECT_FLOAT_EQ(canvas->style().background_color.toRGB().g, 1.0F);

    ASSERT_EQ(canvas->children_count(), 1);
    const auto text = std::dynamic_pointer_cast<const docraft::loom::nodes::DocraftLoomText>(canvas->child(0));
    ASSERT_TRUE(text);
    EXPECT_FLOAT_EQ(text->explicit_position().x, 10.0F);
    EXPECT_FLOAT_EQ(text->explicit_position().y, 20.0F);
}

TEST(DocraftLoomTreeBuilderTest, CanvasWithoutWidthOrHeightThrows)
{
    EXPECT_THROW(parse_and_build(R"XML(<Canvas height="150" />)XML"), docraft::exception::InvalidInputException);
    EXPECT_THROW(parse_and_build(R"XML(<Canvas width="200" />)XML"), docraft::exception::InvalidInputException);
}

TEST(DocraftLoomTreeBuilderTest, BuildsScatterChartWithSeriesData)
{
    const char* xml = R"XML(
<Chart name="c1" style="scatter" width="300" height="200" title="My Chart">
  <Series name="A" color="blue" model='[[1,2],[3,4]]' />
</Chart>
)XML";

    const auto node = parse_and_build(xml);
    const auto canvas = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomCanvas>(node);
    ASSERT_TRUE(canvas);
    EXPECT_FLOAT_EQ(canvas->width(), 300.0F);
    EXPECT_FLOAT_EQ(canvas->height(), 200.0F);
    EXPECT_EQ(canvas->name(), "c1");

    int circle_count = 0;
    for (int i = 0; i < canvas->children_count(); ++i)
    {
        if (std::dynamic_pointer_cast<const docraft::loom::nodes::DocraftLoomCircle>(canvas->child(i)))
        {
            ++circle_count;
        }
    }
    EXPECT_EQ(circle_count, 2);
}

TEST(DocraftLoomTreeBuilderTest, ChartMissingStyleThrows)
{
    EXPECT_THROW(parse_and_build(R"XML(<Chart width="100" height="100"><Series model='[[1,2]]'/></Chart>)XML"),
                 docraft::exception::InvalidInputException);
}

TEST(DocraftLoomTreeBuilderTest, ChartMissingWidthOrHeightThrows)
{
    EXPECT_THROW(
        parse_and_build(R"XML(<Chart style="scatter" height="100"><Series model='[[1,2]]'/></Chart>)XML"),
        docraft::exception::InvalidInputException);
    EXPECT_THROW(
        parse_and_build(R"XML(<Chart style="scatter" width="100"><Series model='[[1,2]]'/></Chart>)XML"),
        docraft::exception::InvalidInputException);
}

TEST(DocraftLoomTreeBuilderTest, ChartUnknownStyleThrows)
{
    EXPECT_THROW(
        parse_and_build(
            R"XML(<Chart style="not-a-style" width="100" height="100"><Series model='[[1,2]]'/></Chart>)XML"),
        docraft::exception::InvalidInputException);
}

TEST(DocraftLoomTreeBuilderTest, ChartNonSeriesChildThrows)
{
    EXPECT_THROW(
        parse_and_build(R"XML(<Chart style="scatter" width="100" height="100"><Text>oops</Text></Chart>)XML"),
        docraft::exception::InvalidInputException);
}

TEST(DocraftLoomTreeBuilderTest, SeriesModelSupportsArrayOfPairsAndArrayOfObjects)
{
    const char* xml_pairs = R"XML(
<Chart style="scatter" width="100" height="100">
  <Series model='[[1,2],[3,4]]' />
</Chart>
)XML";
    const char* xml_objects = R"XML(
<Chart style="scatter" width="100" height="100">
  <Series model='[{"x":1,"y":2},{"x":3,"y":4}]' />
</Chart>
)XML";

    for (const char* xml : {xml_pairs, xml_objects})
    {
        const auto node = parse_and_build(xml);
        const auto canvas = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomCanvas>(node);
        ASSERT_TRUE(canvas);
        int circle_count = 0;
        for (int i = 0; i < canvas->children_count(); ++i)
        {
            if (std::dynamic_pointer_cast<const docraft::loom::nodes::DocraftLoomCircle>(canvas->child(i)))
            {
                ++circle_count;
            }
        }
        EXPECT_EQ(circle_count, 2);
    }
}

TEST(DocraftLoomTreeBuilderTest, SeriesModelMalformedEntryThrows)
{
    EXPECT_THROW(
        parse_and_build(R"XML(<Chart style="scatter" width="100" height="100"><Series model='[[1]]'/></Chart>)XML"),
        docraft::exception::DataFormatException);
}

TEST(DocraftLoomTreeBuilderTest, SeriesNameReusesCommonNameAttribute)
{
    const char* xml = R"XML(
<Chart style="scatter" width="100" height="100">
  <Series name="Alpha" model='[[1,2]]' />
  <Series name="Beta" model='[[3,4]]' />
</Chart>
)XML";

    const auto node = parse_and_build(xml);
    const auto canvas = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomCanvas>(node);
    ASSERT_TRUE(canvas);

    bool found_alpha = false;
    bool found_beta = false;
    for (int i = 0; i < canvas->children_count(); ++i)
    {
        if (auto text = std::dynamic_pointer_cast<const docraft::loom::nodes::DocraftLoomText>(canvas->child(i)))
        {
            if (text->text() == "Alpha") found_alpha = true;
            if (text->text() == "Beta") found_beta = true;
        }
    }
    EXPECT_TRUE(found_alpha);
    EXPECT_TRUE(found_beta);
}

TEST(DocraftLoomTreeBuilderTest, BuildsCircleRadius)
{
    const char* xml = R"XML(<Circle radius="20" background_color="red" />)XML";

    const auto node = parse_and_build(xml);
    const auto circle = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomCircle>(node);
    ASSERT_TRUE(circle);
    EXPECT_FLOAT_EQ(circle->radius(), 20.0F);
}

TEST(DocraftLoomTreeBuilderTest, BuildsTitleWithHeadingDefaults)
{
    const char* xml = R"XML(<Title>Heading</Title>)XML";

    const auto node = parse_and_build(xml);
    const auto title = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomTitle>(node);
    ASSERT_TRUE(title);
    EXPECT_EQ(title->text(), "Heading");
    EXPECT_FLOAT_EQ(title->font_size(), 24.0F);
    EXPECT_TRUE(title->bold());
    EXPECT_FLOAT_EQ(title->margin().top, 24.0F); // 1em, coherent with font_size
    EXPECT_FLOAT_EQ(title->margin().bottom, 24.0F);
}

TEST(DocraftLoomTreeBuilderTest, BuildsSubtitleWithHeadingDefaults)
{
    const char* xml = R"XML(<Subtitle>Heading</Subtitle>)XML";

    const auto node = parse_and_build(xml);
    const auto subtitle = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomSubtitle>(node);
    ASSERT_TRUE(subtitle);
    EXPECT_EQ(subtitle->text(), "Heading");
    EXPECT_FLOAT_EQ(subtitle->font_size(), 18.0F);
    EXPECT_TRUE(subtitle->bold());
    EXPECT_FLOAT_EQ(subtitle->margin().top, 18.0F); // 1em, coherent with font_size
    EXPECT_FLOAT_EQ(subtitle->margin().bottom, 18.0F);
}

TEST(DocraftLoomTreeBuilderTest, TitleMarginTracksAnExplicitFontSizeOverride)
{
    // Margin must stay coherent with the *actual* font_size in use -- not the
    // constructor's own default -- when an explicit font-size attribute shrinks it.
    const char* xml = R"XML(<Title font_size="11">Heading</Title>)XML";

    const auto node = parse_and_build(xml);
    const auto title = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomTitle>(node);
    ASSERT_TRUE(title);
    EXPECT_FLOAT_EQ(title->font_size(), 11.0F);
    EXPECT_FLOAT_EQ(title->margin().top, 11.0F);
}

TEST(DocraftLoomTreeBuilderTest, TitleExplicitMarginAttributeOverridesFontSizeDerivedDefault)
{
    const char* xml = R"XML(<Title margin="40">Heading</Title>)XML";

    const auto node = parse_and_build(xml);
    const auto title = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomTitle>(node);
    ASSERT_TRUE(title);
    EXPECT_FLOAT_EQ(title->font_size(), 24.0F);
    EXPECT_FLOAT_EQ(title->margin().top, 40.0F);
}

TEST(DocraftLoomTreeBuilderTest, TitlePerEdgeMarginAttributesOverrideIndividualSides)
{
    const char* xml = R"XML(<Title margin_top="5" margin_bottom="30">Heading</Title>)XML";

    const auto node = parse_and_build(xml);
    const auto title = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomTitle>(node);
    ASSERT_TRUE(title);
    EXPECT_FLOAT_EQ(title->margin().top, 5.0F);
    EXPECT_FLOAT_EQ(title->margin().bottom, 30.0F);
    // left/right fall back to the 1em heading default, untouched by the per-edge override.
    EXPECT_FLOAT_EQ(title->margin().left, 24.0F);
    EXPECT_FLOAT_EQ(title->margin().right, 24.0F);
}

TEST(DocraftLoomTreeBuilderTest, BuildsPageNumber)
{
    const char* xml = R"XML(<PageNumber font_size="9" style="italic" />)XML";

    const auto node = parse_and_build(xml);
    const auto page_number = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomPageNumber>(node);
    ASSERT_TRUE(page_number);
    EXPECT_FLOAT_EQ(page_number->font_size(), 9.0F);
    EXPECT_TRUE(page_number->italic());
}

TEST(DocraftLoomTreeBuilderTest, DefaultFontFamilyAppliesToTextWithoutFontNameAttribute)
{
    const char* xml = R"XML(<Text>hello</Text>)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    const auto element = parser.parse(xml);
    docraft::loom::craft::DocraftLoomTreeBuilder builder;
    builder.set_default_font_family("Roboto");
    const auto node = builder.build(element);

    const auto text = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomText>(node);
    ASSERT_TRUE(text);
    EXPECT_EQ(text->font_family(), "Roboto");
}

TEST(DocraftLoomTreeBuilderTest, ExplicitFontNameAttributeOverridesDefaultFontFamily)
{
    const char* xml = R"XML(<Text font_name="Times-Roman">hello</Text>)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    const auto element = parser.parse(xml);
    docraft::loom::craft::DocraftLoomTreeBuilder builder;
    builder.set_default_font_family("Roboto");
    const auto node = builder.build(element);

    const auto text = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomText>(node);
    ASSERT_TRUE(text);
    EXPECT_EQ(text->font_family(), "Times-Roman");
}

TEST(DocraftLoomTreeBuilderTest, DefaultFontFamilyAppliesToPageNumberWithoutFontNameAttribute)
{
    const char* xml = R"XML(<PageNumber />)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    const auto element = parser.parse(xml);
    docraft::loom::craft::DocraftLoomTreeBuilder builder;
    builder.set_default_font_family("Roboto");
    const auto node = builder.build(element);

    const auto page_number = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomPageNumber>(node);
    ASSERT_TRUE(page_number);
    EXPECT_EQ(page_number->font_family(), "Roboto");
}

TEST(DocraftLoomTreeBuilderTest, BuildsImageFromSrc)
{
    const char* xml = R"XML(<Image src="assets/logo.png" width="30" height="30" />)XML";

    const auto node = parse_and_build(xml);
    const auto image = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomImage>(node);
    ASSERT_TRUE(image);
    EXPECT_EQ(image->path(), "assets/logo.png");
    EXPECT_FLOAT_EQ(image->width(), 30.0F);
    EXPECT_FLOAT_EQ(image->height(), 30.0F);
}

TEST(DocraftLoomTreeBuilderTest, BuildsBlankLineWithHeight)
{
    const char* xml = R"XML(<Blank height="12" />)XML";

    const auto node = parse_and_build(xml);
    const auto blank = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomBlankLine>(node);
    ASSERT_TRUE(blank);
    EXPECT_FLOAT_EQ(blank->height(), 12.0F);
}

TEST(DocraftLoomTreeBuilderTest, BuildsListWithTextChildren)
{
    const char* xml = R"XML(
<UList dot="*">
  <Text>First</Text>
  <Text>Second</Text>
</UList>
)XML";

    const auto node = parse_and_build(xml);
    const auto list = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomList>(node);
    ASSERT_TRUE(list);
    EXPECT_EQ(list->kind(), docraft::loom::nodes::ListKind::kUnordered);
    EXPECT_EQ(list->unordered_dot(), docraft::loom::nodes::UnorderedListDot::kStar);
    ASSERT_EQ(list->children_count(), 2);
    const auto first = std::dynamic_pointer_cast<const docraft::loom::nodes::DocraftLoomText>(list->child(0));
    ASSERT_TRUE(first);
    EXPECT_EQ(first->text(), "First");
}

TEST(DocraftLoomTreeBuilderTest, BuildsHorizontalLayoutAsHStack)
{
    const char* xml = R"XML(
<Layout orientation="horizontal">
  <Text>A</Text>
  <Text>B</Text>
</Layout>
)XML";

    const auto node = parse_and_build(xml);
    const auto hstack = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomHStack>(node);
    ASSERT_TRUE(hstack);
    EXPECT_EQ(hstack->children_count(), 2);
}

TEST(DocraftLoomTreeBuilderTest, BuildsVerticalLayoutAsVStack)
{
    const char* xml = R"XML(
<Layout orientation="vertical">
  <Text>A</Text>
</Layout>
)XML";

    const auto node = parse_and_build(xml);
    const auto vstack = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomVStack>(node);
    ASSERT_TRUE(vstack);
    EXPECT_EQ(vstack->children_count(), 1);
}

TEST(DocraftLoomTreeBuilderTest, BuildsHorizontalTableGrid)
{
    const char* xml = R"XML(
<Table>
  <THead>
    <HTitle>ColA</HTitle>
    <HTitle>ColB</HTitle>
  </THead>
  <TBody>
    <Row>
      <Cell><Text>v1</Text></Cell>
      <Cell><Text>v2</Text></Cell>
    </Row>
  </TBody>
</Table>
)XML";

    const auto node = parse_and_build(xml);
    const auto table = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomTable>(node);
    ASSERT_TRUE(table);
    EXPECT_EQ(table->row_count(), 2); // header row + 1 body row
    EXPECT_EQ(table->column_count(), 2);

    const auto header_cell = table->cell(0, 0);
    ASSERT_TRUE(header_cell);
    EXPECT_TRUE(header_cell->is_title());

    const auto body_cell = table->cell(1, 0);
    ASSERT_TRUE(body_cell);
    EXPECT_FALSE(body_cell->is_title());
    const auto body_text = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomText>(body_cell->content());
    ASSERT_TRUE(body_text);
    EXPECT_EQ(body_text->text(), "v1");
}

TEST(DocraftLoomTreeBuilderTest, BuildsTableCellWithImageContentAndSize)
{
    const char* xml = R"XML(
<Table>
  <THead>
    <HTitle>Name</HTitle>
    <HTitle>Photo</HTitle>
  </THead>
  <TBody>
    <Row>
      <Cell><Text>v1</Text></Cell>
      <Cell><Image src="assets/logo.png" width="50" height="50" /></Cell>
    </Row>
  </TBody>
</Table>
)XML";

    const auto node = parse_and_build(xml);
    const auto table = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomTable>(node);
    ASSERT_TRUE(table);

    const auto body_cell = table->cell(1, 1);
    ASSERT_TRUE(body_cell);
    const auto image = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomImage>(body_cell->content());
    ASSERT_TRUE(image);
    EXPECT_EQ(image->path(), "assets/logo.png");
    EXPECT_FLOAT_EQ(image->width(), 50.0F);
    EXPECT_FLOAT_EQ(image->height(), 50.0F);
}

TEST(DocraftLoomTreeBuilderTest, BuildsTableFromJsonMatrixModelAndHeader)
{
    const char* xml = R"XML(
<Table model='[["v1","v2"],["v3","v4"]]' header='["H1","H2"]' />
)XML";

    const auto node = parse_and_build(xml);
    const auto table = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomTable>(node);
    ASSERT_TRUE(table);
    EXPECT_EQ(table->row_count(), 3); // header row + 2 data rows
    EXPECT_EQ(table->column_count(), 2);

    const auto header_cell = table->cell(0, 0);
    ASSERT_TRUE(header_cell);
    EXPECT_TRUE(header_cell->is_title());
    const auto header_text = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomText>(header_cell->content());
    ASSERT_TRUE(header_text);
    EXPECT_EQ(header_text->text(), "H1");

    const auto body_cell = table->cell(1, 1);
    ASSERT_TRUE(body_cell);
    EXPECT_FALSE(body_cell->is_title());
    const auto body_text = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomText>(body_cell->content());
    ASSERT_TRUE(body_text);
    EXPECT_EQ(body_text->text(), "v2");

    const auto last_cell = table->cell(2, 0);
    ASSERT_TRUE(last_cell);
    const auto last_text = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomText>(last_cell->content());
    ASSERT_TRUE(last_text);
    EXPECT_EQ(last_text->text(), "v3");
}

TEST(DocraftLoomTreeBuilderTest, BuildsTableFromJsonMatrixModelWithoutHeader)
{
    const char* xml = R"XML(<Table model='[["v1","v2"]]' />)XML";

    const auto node = parse_and_build(xml);
    const auto table = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomTable>(node);
    ASSERT_TRUE(table);
    EXPECT_EQ(table->row_count(), 1);
    EXPECT_EQ(table->column_count(), 2);
    EXPECT_FALSE(table->cell(0, 0)->is_title());
}

TEST(DocraftLoomTreeBuilderTest, TableModelResolvesFromGlobalVariable)
{
    const char* xml = R"XML(<Table model='${summary_rows}' header='${summary_header}' />)XML";

    auto engine = std::make_shared<docraft::templating::DocraftTemplateEngine>();
    engine->add_template_variable("summary_rows", R"([["Alpha","1"],["Beta","2"]])");
    engine->add_template_variable("summary_header", R"(["Name","Count"])");

    const auto node = parse_and_build_with_engine(xml, engine);
    const auto table = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomTable>(node);
    ASSERT_TRUE(table);
    EXPECT_EQ(table->row_count(), 3);
    EXPECT_EQ(table->column_count(), 2);

    const auto header_text = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomText>(
        table->cell(0, 1)->content());
    ASSERT_TRUE(header_text);
    EXPECT_EQ(header_text->text(), "Count");

    const auto body_text = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomText>(
        table->cell(2, 0)->content());
    ASSERT_TRUE(body_text);
    EXPECT_EQ(body_text->text(), "Beta");
}

TEST(DocraftLoomTreeBuilderTest, TableJsonModelHonorsExplicitTHeadInsteadOfHeaderAttribute)
{
    const char* xml = R"XML(
<Table model='[["v1","v2"],["v3","v4"]]'>
  <THead>
    <HTitle>ColA</HTitle>
    <HTitle>ColB</HTitle>
  </THead>
</Table>
)XML";

    const auto node = parse_and_build(xml);
    const auto table = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomTable>(node);
    ASSERT_TRUE(table);
    EXPECT_EQ(table->row_count(), 3); // header row + 2 data rows
    EXPECT_EQ(table->column_count(), 2);

    const auto header_cell = table->cell(0, 0);
    ASSERT_TRUE(header_cell);
    EXPECT_TRUE(header_cell->is_title());
    const auto header_text = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomText>(header_cell->content());
    ASSERT_TRUE(header_text);
    EXPECT_EQ(header_text->text(), "ColA");

    const auto body_cell = table->cell(1, 0);
    ASSERT_TRUE(body_cell);
    EXPECT_FALSE(body_cell->is_title());
}

TEST(DocraftLoomTreeBuilderTest, TableHeaderColumnCountMismatchThrows)
{
    const char* xml = R"XML(<Table model='[["v1","v2"]]' header='["OnlyOne"]' />)XML";
    EXPECT_THROW(parse_and_build(xml), docraft::exception::DataFormatException);
}

TEST(DocraftLoomTreeBuilderTest, TableJsonModelRejectsRaggedRows)
{
    const char* xml = R"XML(<Table model='[["v1","v2"],["v3"]]' />)XML";
    EXPECT_THROW(parse_and_build(xml), docraft::exception::DataFormatException);
}

TEST(DocraftLoomTreeBuilderTest, InvisibleElementIsNotBuilt)
{
    const char* xml = R"XML(<Rectangle visible="false" />)XML";

    docraft::craft::DocraftCraftLanguageParser parser;
    const auto element = parser.parse(xml);
    docraft::loom::craft::DocraftLoomTreeBuilder builder;
    EXPECT_EQ(builder.build(element), nullptr);
}

TEST(DocraftLoomTreeBuilderTest, ThrowsForUnrecognizedTag)
{
    auto element = std::make_shared<docraft::craft::DocraftParsedElement>();
    element->tag_name = "NotARealTag";

    docraft::loom::craft::DocraftLoomTreeBuilder builder;
    EXPECT_THROW(builder.build(element), docraft::exception::DataFormatException);
}

TEST(DocraftLoomTreeBuilderTest, LayoutWeightsAttributeAppliesToHStack)
{
    const char* xml = R"XML(
<Layout orientation="horizontal" weights="1,2,1" spacing="5">
  <Text>A</Text>
  <Text>B</Text>
  <Text>C</Text>
</Layout>
)XML";

    const auto node = parse_and_build(xml);
    const auto hstack = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomHStack>(node);
    ASSERT_TRUE(hstack);
    EXPECT_FLOAT_EQ(hstack->spacing(), 5.0F);
    ASSERT_EQ(hstack->weights().size(), 3U);
    EXPECT_FLOAT_EQ(hstack->weights()[0], 1.0F);
    EXPECT_FLOAT_EQ(hstack->weights()[1], 2.0F);
    EXPECT_FLOAT_EQ(hstack->weights()[2], 1.0F);
}

TEST(DocraftLoomTreeBuilderTest, PerChildWeightAttributeAppliesToHStack)
{
    const char* xml = R"XML(
<Layout orientation="horizontal">
  <Text weight="1">A</Text>
  <Text weight="3">B</Text>
</Layout>
)XML";

    const auto node = parse_and_build(xml);
    const auto hstack = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomHStack>(node);
    ASSERT_TRUE(hstack);
    ASSERT_EQ(hstack->weights().size(), 2U);
    EXPECT_FLOAT_EQ(hstack->weights()[0], 1.0F);
    EXPECT_FLOAT_EQ(hstack->weights()[1], 3.0F);
}

TEST(DocraftLoomTreeBuilderTest, ExplicitLayoutWeightsTakePrecedenceOverPerChildWeight)
{
    const char* xml = R"XML(
<Layout orientation="horizontal" weights="4,4">
  <Text weight="1">A</Text>
  <Text weight="3">B</Text>
</Layout>
)XML";

    const auto node = parse_and_build(xml);
    const auto hstack = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomHStack>(node);
    ASSERT_TRUE(hstack);
    ASSERT_EQ(hstack->weights().size(), 2U);
    EXPECT_FLOAT_EQ(hstack->weights()[0], 4.0F);
    EXPECT_FLOAT_EQ(hstack->weights()[1], 4.0F);
}

TEST(DocraftLoomTreeBuilderTest, NoWeightsLeavesHStackWeightsEmpty)
{
    const char* xml = R"XML(
<Layout orientation="horizontal">
  <Text>A</Text>
  <Text>B</Text>
</Layout>
)XML";

    const auto node = parse_and_build(xml);
    const auto hstack = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomHStack>(node);
    ASSERT_TRUE(hstack);
    EXPECT_TRUE(hstack->weights().empty());
}

TEST(DocraftLoomTreeBuilderTest, VStackAppliesSpacingButNotWeights)
{
    const char* xml = R"XML(
<Layout orientation="vertical" spacing="8" weights="1,1">
  <Text>A</Text>
  <Text>B</Text>
</Layout>
)XML";

    const auto node = parse_and_build(xml);
    const auto vstack = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomVStack>(node);
    ASSERT_TRUE(vstack);
    EXPECT_FLOAT_EQ(vstack->spacing(), 8.0F);
}

TEST(DocraftLoomTreeBuilderTest, BuildsParagraphWithTypographicAttributesAndChildren)
{
    const char* xml = R"XML(
<Paragraph line_spacing="1.5" space_before="4" space_after="6" alignment="center">
  <Text>First</Text>
  <Text>Second</Text>
</Paragraph>
)XML";

    const auto node = parse_and_build(xml);
    const auto paragraph = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomParagraph>(node);
    ASSERT_TRUE(paragraph);
    EXPECT_FLOAT_EQ(paragraph->line_spacing(), 1.5F);
    EXPECT_FLOAT_EQ(paragraph->space_before(), 4.0F);
    EXPECT_FLOAT_EQ(paragraph->space_after(), 6.0F);
    EXPECT_EQ(paragraph->alignment(), docraft::loom::nodes::TextAlignment::kCenter);
    ASSERT_EQ(paragraph->children_count(), 2);
}

TEST(DocraftLoomTreeBuilderTest, BuildsHeaderAsVStackWithStyleAndChildren)
{
    const char* xml = R"XML(
<Header background_color="#00FF00" border_width="1.5" margin_top="3">
  <Text>Header text</Text>
</Header>
)XML";

    const auto node = parse_and_build(xml);
    const auto vstack = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomVStack>(node);
    ASSERT_TRUE(vstack);
    EXPECT_FLOAT_EQ(vstack->style().border_width, 1.5F);
    EXPECT_FLOAT_EQ(vstack->style().background_color.toRGB().g, 1.0F);
    ASSERT_EQ(vstack->children_count(), 1);
}

TEST(DocraftLoomTreeBuilderTest, BuildsBodyAsVStack)
{
    const char* xml = R"XML(
<Body margin_left="20">
  <Text>Body text</Text>
</Body>
)XML";

    const auto node = parse_and_build(xml);
    const auto vstack = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomVStack>(node);
    ASSERT_TRUE(vstack);
    ASSERT_EQ(vstack->children_count(), 1);
}

TEST(DocraftLoomTreeBuilderTest, BuildsFooterAsVStack)
{
    const char* xml = R"XML(
<Footer margin_right="15">
  <PageNumber />
</Footer>
)XML";

    const auto node = parse_and_build(xml);
    const auto vstack = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomVStack>(node);
    ASSERT_TRUE(vstack);
    ASSERT_EQ(vstack->children_count(), 1);
}

TEST(DocraftLoomTreeBuilderTest, BuildsNewPageMarker)
{
    const char* xml = R"XML(<NewPage />)XML";

    const auto node = parse_and_build(xml);
    const auto new_page = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomNewPage>(node);
    ASSERT_TRUE(new_page);
}

TEST(DocraftLoomTreeBuilderTest, ForeachWithNRepeatsChildrenVerbatim)
{
    const char* xml = R"XML(
<Layout orientation="vertical">
  <Foreach n="3">
    <Text>Row</Text>
  </Foreach>
</Layout>
)XML";

    const auto node = parse_and_build(xml);
    ASSERT_TRUE(node);
    ASSERT_EQ(node->children_count(), 3);
    for (int i = 0; i < 3; ++i)
    {
        const auto text = std::dynamic_pointer_cast<const docraft::loom::nodes::DocraftLoomText>(node->child(i));
        ASSERT_TRUE(text);
        EXPECT_EQ(text->text(), "Row");
    }
}

TEST(DocraftLoomTreeBuilderTest, ForeachWithZeroNProducesNoChildren)
{
    const char* xml = R"XML(
<Layout orientation="vertical">
  <Foreach n="0">
    <Text>Row</Text>
  </Foreach>
</Layout>
)XML";

    const auto node = parse_and_build(xml);
    ASSERT_TRUE(node);
    EXPECT_EQ(node->children_count(), 0);
}

TEST(DocraftLoomTreeBuilderTest, ForeachWithModelBindsDataFieldsPerItem)
{
    const char* xml = R"XML(
<Layout orientation="vertical">
  <Foreach model='[{"name":"Alice"},{"name":"Bob"}]'>
    <Text>${data("name")}</Text>
  </Foreach>
</Layout>
)XML";

    const auto node = parse_and_build(xml);
    ASSERT_TRUE(node);
    ASSERT_EQ(node->children_count(), 2);
    const auto first = std::dynamic_pointer_cast<const docraft::loom::nodes::DocraftLoomText>(node->child(0));
    ASSERT_TRUE(first);
    EXPECT_EQ(first->text(), "Alice");
    const auto second = std::dynamic_pointer_cast<const docraft::loom::nodes::DocraftLoomText>(node->child(1));
    ASSERT_TRUE(second);
    EXPECT_EQ(second->text(), "Bob");
}

TEST(DocraftLoomTreeBuilderTest, ForeachSurroundingContentIsNotDisturbed)
{
    const char* xml = R"XML(
<Layout orientation="vertical">
  <Text>Before</Text>
  <Foreach n="2">
    <Text>Repeated</Text>
  </Foreach>
  <Text>After</Text>
</Layout>
)XML";

    const auto node = parse_and_build(xml);
    ASSERT_TRUE(node);
    ASSERT_EQ(node->children_count(), 4);
    const auto before = std::dynamic_pointer_cast<const docraft::loom::nodes::DocraftLoomText>(node->child(0));
    ASSERT_TRUE(before);
    EXPECT_EQ(before->text(), "Before");
    const auto after = std::dynamic_pointer_cast<const docraft::loom::nodes::DocraftLoomText>(node->child(3));
    ASSERT_TRUE(after);
    EXPECT_EQ(after->text(), "After");
}

TEST(DocraftLoomTreeBuilderTest, ResolvesGlobalVariableInTextContent)
{
    const char* xml = R"XML(<Text>Hello ${name}</Text>)XML";

    auto engine = std::make_shared<docraft::templating::DocraftTemplateEngine>();
    engine->add_template_variable("name", "World");

    const auto node = parse_and_build_with_engine(xml, engine);
    const auto text = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomText>(node);
    ASSERT_TRUE(text);
    EXPECT_EQ(text->text(), "Hello World");
}

TEST(DocraftLoomTreeBuilderTest, ResolvesGlobalVariableInImageSrc)
{
    const char* xml = R"XML(<Image src="${logo_path}" />)XML";

    auto engine = std::make_shared<docraft::templating::DocraftTemplateEngine>();
    engine->add_template_variable("logo_path", "assets/logo.png");

    const auto node = parse_and_build_with_engine(xml, engine);
    const auto image = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomImage>(node);
    ASSERT_TRUE(image);
    EXPECT_EQ(image->path(), "assets/logo.png");
}

TEST(DocraftLoomTreeBuilderTest, UnknownVariableIsLeftLiteral)
{
    const char* xml = R"XML(<Text>Hello ${missing}</Text>)XML";

    auto engine = std::make_shared<docraft::templating::DocraftTemplateEngine>();

    const auto node = parse_and_build_with_engine(xml, engine);
    const auto text = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomText>(node);
    ASSERT_TRUE(text);
    EXPECT_EQ(text->text(), "Hello ${missing}");
}

TEST(DocraftLoomTreeBuilderTest, ForeachModelResolvesFromGlobalVariable)
{
    const char* xml = R"XML(
<Layout orientation="vertical">
  <Foreach model="${employees}">
    <Text>${data("name")}</Text>
  </Foreach>
</Layout>
)XML";

    auto engine = std::make_shared<docraft::templating::DocraftTemplateEngine>();
    engine->add_template_variable("employees", R"([{"name":"Alice"},{"name":"Bob"}])");

    const auto node = parse_and_build_with_engine(xml, engine);
    ASSERT_TRUE(node);
    ASSERT_EQ(node->children_count(), 2);
    const auto first = std::dynamic_pointer_cast<const docraft::loom::nodes::DocraftLoomText>(node->child(0));
    ASSERT_TRUE(first);
    EXPECT_EQ(first->text(), "Alice");
    const auto second = std::dynamic_pointer_cast<const docraft::loom::nodes::DocraftLoomText>(node->child(1));
    ASSERT_TRUE(second);
    EXPECT_EQ(second->text(), "Bob");
}

TEST(DocraftLoomTreeBuilderTest, ForeachModelResolvesFromGlobalVariableWithSingleQuotedJson)
{
    const char* xml = R"XML(
<Layout orientation="vertical">
  <Foreach model="${employees}">
    <Text>${data("name")}</Text>
  </Foreach>
</Layout>
)XML";

    auto engine = std::make_shared<docraft::templating::DocraftTemplateEngine>();
    engine->add_template_variable("employees", R"([{'name':'Alice'}])");

    const auto node = parse_and_build_with_engine(xml, engine);
    ASSERT_TRUE(node);
    ASSERT_EQ(node->children_count(), 1);
    const auto first = std::dynamic_pointer_cast<const docraft::loom::nodes::DocraftLoomText>(node->child(0));
    ASSERT_TRUE(first);
    EXPECT_EQ(first->text(), "Alice");
}

TEST(DocraftLoomTreeBuilderTest, NestedForeachModelResolvesFromOuterItemData)
{
    const char* xml = R"XML(
<Layout orientation="vertical">
  <Foreach model='[{"name":"Team A","members":[{"name":"Alice"},{"name":"Bob"}]}]'>
    <Text>${data("name")}</Text>
    <Foreach model='${data("members")}'>
      <Text>${data("name")}</Text>
    </Foreach>
  </Foreach>
</Layout>
)XML";

    const auto node = parse_and_build(xml);
    ASSERT_TRUE(node);
    ASSERT_EQ(node->children_count(), 3);
    const auto team = std::dynamic_pointer_cast<const docraft::loom::nodes::DocraftLoomText>(node->child(0));
    ASSERT_TRUE(team);
    EXPECT_EQ(team->text(), "Team A");
    const auto member1 = std::dynamic_pointer_cast<const docraft::loom::nodes::DocraftLoomText>(node->child(1));
    ASSERT_TRUE(member1);
    EXPECT_EQ(member1->text(), "Alice");
    const auto member2 = std::dynamic_pointer_cast<const docraft::loom::nodes::DocraftLoomText>(node->child(2));
    ASSERT_TRUE(member2);
    EXPECT_EQ(member2->text(), "Bob");
}

// --- Regression tests for confirmed-but-not-yet-fixed bugs from the code review
// (.local/CODE_REVIEW_LOOM_MIGRATION.md). These currently FAIL against the
// unmodified codebase -- they encode the desired behavior and will pass once the
// corresponding fix from the review is implemented.

// Review bug #1: a color attribute containing a ${...} template expression must
// resolve per Foreach item, not throw at parse time.
TEST(DocraftLoomTreeBuilderTest, ForeachColorTemplateExpressionResolvesPerItem)
{
    const char* xml = R"XML(
<Layout orientation="vertical">
  <Foreach model='[{"color":"#FF0000"},{"color":"#00FF00"}]'>
    <Text color='${data("color")}'>x</Text>
  </Foreach>
</Layout>
)XML";

    const auto node = parse_and_build(xml);
    ASSERT_TRUE(node);
    ASSERT_EQ(node->children_count(), 2);
    const auto first = std::dynamic_pointer_cast<const docraft::loom::nodes::DocraftLoomText>(node->child(0));
    ASSERT_TRUE(first);
    EXPECT_FLOAT_EQ(first->color().toRGB().r, 1.0F);
    EXPECT_FLOAT_EQ(first->color().toRGB().g, 0.0F);
    const auto second = std::dynamic_pointer_cast<const docraft::loom::nodes::DocraftLoomText>(node->child(1));
    ASSERT_TRUE(second);
    EXPECT_FLOAT_EQ(second->color().toRGB().g, 1.0F);
}

// Review bug #3: a <Table> combining a JSON array-of-objects `model` with an
// explicit <TBody> must use the TBody as a per-row template, cloning it once per
// JSON object and resolving ${data(field)} against that object -- matching the
// legacy pipeline's behavior (git show main:docraft/src/docraft/model/docraft_table.cc,
// parse_json_object_for_templated_table).
TEST(DocraftLoomTreeBuilderTest, TableJsonModelWithTBodyTemplateClonesRowPerObject)
{
    const char* xml = R"XML(
<Table model='[{"name":"Alice"},{"name":"Bob"}]'>
  <TBody>
    <Row>
      <Cell><Text>${data("name")}</Text></Cell>
    </Row>
  </TBody>
</Table>
)XML";

    const auto node = parse_and_build(xml);
    const auto table = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomTable>(node);
    ASSERT_TRUE(table);
    EXPECT_EQ(table->row_count(), 2);
    EXPECT_EQ(table->column_count(), 1);

    const auto first_text =
        std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomText>(table->cell(0, 0)->content());
    ASSERT_TRUE(first_text);
    EXPECT_EQ(first_text->text(), "Alice");

    const auto second_text =
        std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomText>(table->cell(1, 0)->content());
    ASSERT_TRUE(second_text);
    EXPECT_EQ(second_text->text(), "Bob");
}

// Unlike a JSON array-of-objects `model` (see above), an array-of-arrays `model` has no
// per-row data to bind an explicit <TBody> against, so combining them is rejected rather
// than silently dropping the <TBody> (the legacy pipeline's behavior).
TEST(DocraftLoomTreeBuilderTest, TableStringMatrixModelRejectsExplicitTBody)
{
    const char* xml = R"XML(
<Table model='[["v1","v2"]]'>
  <TBody>
    <Row>
      <Cell><Text>ignored</Text></Cell>
    </Row>
  </TBody>
</Table>
)XML";

    EXPECT_THROW(parse_and_build(xml), docraft::exception::InvalidInputException);
}

// Review bug #4: a <Table> nested inside a <Foreach> must resolve ${data(...)} in
// its model against the outer Foreach item, not leave it as literal text.
TEST(DocraftLoomTreeBuilderTest, TableNestedInForeachResolvesDataFieldsInModel)
{
    const char* xml = R"XML(
<Layout orientation="vertical">
  <Foreach model='[{"name":"Alice"}]'>
    <Table model='[["${data("name")}"]]' />
  </Foreach>
</Layout>
)XML";

    const auto node = parse_and_build(xml);
    ASSERT_TRUE(node);
    ASSERT_EQ(node->children_count(), 1);
    const auto table = std::dynamic_pointer_cast<const docraft::loom::nodes::DocraftLoomTable>(node->child(0));
    ASSERT_TRUE(table);
    const auto text = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomText>(table->cell(0, 0)->content());
    ASSERT_TRUE(text);
    EXPECT_EQ(text->text(), "Alice");
}

// Review bug #5: a table model's JSON normalizer must not corrupt an escaped
// quote inside a double-quoted string when converting the .craft single-quoted-
// JSON convention.
TEST(DocraftLoomTreeBuilderTest, TableModelPreservesEscapedQuotesInsideDoubleQuotedStrings)
{
    const char* xml = R"XML(<Table model='[["say \"hi\"","v2"]]' />)XML";

    const auto node = parse_and_build(xml);
    const auto table = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomTable>(node);
    ASSERT_TRUE(table);
    EXPECT_EQ(table->row_count(), 1);
    EXPECT_EQ(table->column_count(), 2);

    const auto cell = table->cell(0, 0);
    ASSERT_TRUE(cell);
    const auto text = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomText>(cell->content());
    ASSERT_TRUE(text);
    EXPECT_EQ(text->text(), "say \"hi\"");
}

// Review bug #6: normalize_single_quoted_json's backslash handling only special-cased
// in_single, so an escaped double quote (`\"`) while in_double was read as two independent
// characters -- the bare `\` fell through unconsumed, and the `"` then toggled in_double as
// if it were a real (unescaped) delimiter. A single escaped quote inside a double-quoted
// cell therefore leaves in_double permanently flipped for the rest of the model string, so a
// later single-quoted cell's delimiters are never converted to double quotes and the overall
// JSON fails to parse.
TEST(DocraftLoomTreeBuilderTest, TableModelEscapedQuoteDoesNotDesyncLaterSingleQuotedCell)
{
    // The outer XML attribute is double-quote-delimited (with its own `"` escaped as
    // &quot;) so the decoded model text can freely contain both an escaped-double-quote
    // cell and a single-quoted cell: [["a\"", 'b']]
    const char* xml = R"XML(<Table model="[[&quot;a\&quot;&quot;, 'b']]" />)XML";

    const auto node = parse_and_build(xml);
    const auto table = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomTable>(node);
    ASSERT_TRUE(table);
    EXPECT_EQ(table->row_count(), 1);
    EXPECT_EQ(table->column_count(), 2);

    const auto first_text =
        std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomText>(table->cell(0, 0)->content());
    ASSERT_TRUE(first_text);
    EXPECT_EQ(first_text->text(), "a\"");

    const auto second_text =
        std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomText>(table->cell(0, 1)->content());
    ASSERT_TRUE(second_text);
    EXPECT_EQ(second_text->text(), "b");
}

// Review bug #7: table cell content must honor visible="false" the same way
// build()'s top-level dispatcher does for every other node.
TEST(DocraftLoomTreeBuilderTest, TableCellContentHonorsVisibleFalse)
{
    const char* xml = R"XML(
<Table>
  <THead><HTitle>Col</HTitle></THead>
  <TBody>
    <Row><Cell><Text visible="false">hidden</Text></Cell></Row>
  </TBody>
</Table>
)XML";

    const auto node = parse_and_build(xml);
    const auto table = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomTable>(node);
    ASSERT_TRUE(table);
    const auto body_cell = table->cell(1, 0);
    ASSERT_TRUE(body_cell);
    EXPECT_EQ(body_cell->content(), nullptr);
}
