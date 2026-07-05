#include <gtest/gtest.h>

#include "docraft/craft/docraft_craft_language_parser.h"
#include "docraft/exception/docraft_exceptions.h"
#include "docraft/loom/craft/docraft_loom_tree_builder.h"
#include "docraft/loom/nodes/docraft_loom_blank_line.h"
#include "docraft/loom/nodes/docraft_loom_circle.h"
#include "docraft/loom/nodes/docraft_loom_hstack.h"
#include "docraft/loom/nodes/docraft_loom_image.h"
#include "docraft/loom/nodes/docraft_loom_list.h"
#include "docraft/loom/nodes/docraft_loom_page_number.h"
#include "docraft/loom/nodes/docraft_loom_paragraph.h"
#include "docraft/loom/nodes/docraft_loom_rectangle.h"
#include "docraft/loom/nodes/docraft_loom_table.h"
#include "docraft/loom/nodes/docraft_loom_table_cell.h"
#include "docraft/loom/nodes/docraft_loom_text.h"
#include "docraft/loom/nodes/docraft_loom_vstack.h"

namespace {
    std::shared_ptr<docraft::loom::nodes::DocraftLoomNode> parse_and_build(const char* xml)
    {
        docraft::craft::DocraftCraftLanguageParser parser;
        const auto element = parser.parse(xml);
        docraft::loom::craft::DocraftLoomTreeBuilder builder;
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
    const auto text = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomText>(node);
    ASSERT_TRUE(text);
    EXPECT_EQ(text->text(), "Heading");
    EXPECT_FLOAT_EQ(text->font_size(), 24.0F);
    EXPECT_TRUE(text->bold());
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
<layout orientation="horizontal">
  <Text>A</Text>
  <Text>B</Text>
</layout>
)XML";

    const auto node = parse_and_build(xml);
    const auto hstack = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomHStack>(node);
    ASSERT_TRUE(hstack);
    EXPECT_EQ(hstack->children_count(), 2);
}

TEST(DocraftLoomTreeBuilderTest, BuildsVerticalLayoutAsVStack)
{
    const char* xml = R"XML(
<layout orientation="vertical">
  <Text>A</Text>
</layout>
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
<layout orientation="horizontal" weights="1,2,1" spacing="5">
  <Text>A</Text>
  <Text>B</Text>
  <Text>C</Text>
</layout>
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
<layout orientation="horizontal">
  <Text weight="1">A</Text>
  <Text weight="3">B</Text>
</layout>
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
<layout orientation="horizontal" weights="4,4">
  <Text weight="1">A</Text>
  <Text weight="3">B</Text>
</layout>
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
<layout orientation="horizontal">
  <Text>A</Text>
  <Text>B</Text>
</layout>
)XML";

    const auto node = parse_and_build(xml);
    const auto hstack = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomHStack>(node);
    ASSERT_TRUE(hstack);
    EXPECT_TRUE(hstack->weights().empty());
}

TEST(DocraftLoomTreeBuilderTest, VStackAppliesSpacingButNotWeights)
{
    const char* xml = R"XML(
<layout orientation="vertical" spacing="8" weights="1,1">
  <Text>A</Text>
  <Text>B</Text>
</layout>
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

TEST(DocraftLoomTreeBuilderTest, BuildsHeaderAsRectangleWithStyleAndChildren)
{
    const char* xml = R"XML(
<Header background_color="#00FF00" border_width="1.5" margin_top="3">
  <Text>Header text</Text>
</Header>
)XML";

    const auto node = parse_and_build(xml);
    const auto rectangle = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomRectangle>(node);
    ASSERT_TRUE(rectangle);
    EXPECT_FLOAT_EQ(rectangle->style().border_width, 1.5F);
    EXPECT_FLOAT_EQ(rectangle->style().background_color.toRGB().g, 1.0F);
    ASSERT_EQ(rectangle->children_count(), 1);
}

TEST(DocraftLoomTreeBuilderTest, BuildsBodyAsRectangle)
{
    const char* xml = R"XML(
<Body margin_left="20">
  <Text>Body text</Text>
</Body>
)XML";

    const auto node = parse_and_build(xml);
    const auto rectangle = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomRectangle>(node);
    ASSERT_TRUE(rectangle);
    ASSERT_EQ(rectangle->children_count(), 1);
}

TEST(DocraftLoomTreeBuilderTest, BuildsFooterAsRectangle)
{
    const char* xml = R"XML(
<Footer margin_right="15">
  <PageNumber />
</Footer>
)XML";

    const auto node = parse_and_build(xml);
    const auto rectangle = std::dynamic_pointer_cast<docraft::loom::nodes::DocraftLoomRectangle>(node);
    ASSERT_TRUE(rectangle);
    ASSERT_EQ(rectangle->children_count(), 1);
}
