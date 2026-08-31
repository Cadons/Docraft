#include "docraft/loom/pipeline/docraft_loom_layout_processor.h"

#include <gtest/gtest.h>

#include "docraft/exception/docraft_exceptions.h"
#include "docraft/loom/nodes/docraft_loom_hstack.h"
#include "docraft/loom/nodes/docraft_loom_image.h"
#include "docraft/loom/nodes/docraft_loom_rectangle.h"
#include "docraft/loom/nodes/docraft_loom_table.h"
#include "docraft/loom/nodes/docraft_loom_table_cell.h"
#include "docraft/loom/nodes/docraft_loom_text.h"
#include "docraft/utils/docraft_loom_layout_box_test_access.h"

// Regression tests for confirmed-but-not-yet-fixed bugs from the code review
// (.local/CODE_REVIEW_LOOM_MIGRATION.md). These currently FAIL against the
// unmodified codebase -- they encode the desired behavior and will pass once the
// corresponding fix is implemented. No dedicated test file existed for
// DocraftLoomLayoutProcessor before this one.
namespace docraft::test {
    // Review bug #2 (Layout side, HStack): a weighted HStack nested inside a
    // narrower Rectangle must resolve its columns against the Rectangle's own
    // width, not the full page/region width.
    TEST(DocraftLoomLayoutProcessorTest, RectangleNarrowsWeightedHStackColumnsInsteadOfFullPage)
    {
        auto rectangle = std::make_shared<loom::nodes::DocraftLoomRectangle>();
        rectangle->set_width(200.0F);
        rectangle->set_padding(0.0F); // isolate width-narrowing from the container's own default padding
        rectangle->edit_layout_box().measured_size = {.width = 200.0F, .height = 50.0F};

        auto hstack = std::make_shared<loom::nodes::DocraftLoomHStack>();
        hstack->set_padding(0.0F); // isolate width-narrowing from the container's own default padding
        hstack->set_weights({1.0F, 1.0F});
        hstack->edit_layout_box().measured_size = {.width = 20.0F, .height = 10.0F};

        auto first = std::make_shared<loom::nodes::DocraftLoomText>("a");
        first->edit_layout_box().measured_size = {.width = 10.0F, .height = 10.0F};
        hstack->add_child(first);

        auto second = std::make_shared<loom::nodes::DocraftLoomText>("b");
        second->edit_layout_box().measured_size = {.width = 10.0F, .height = 10.0F};
        hstack->add_child(second);

        rectangle->add_child(hstack);

        // Page is far wider than the rectangle -- the bug resolves the HStack's
        // weighted columns against this full page width instead of the rectangle's.
        loom::pipeline::DocraftLoomLayoutProcessor processor(500.0F);
        rectangle->accept(processor);

        const float first_width = hstack->child(0)->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).size.width;
        const float second_width = hstack->child(1)->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).size.width;
        EXPECT_NEAR(first_width, 100.0F, 0.01F);
        EXPECT_NEAR(second_width, 100.0F, 0.01F);
        EXPECT_LE(first_width + second_width, 200.0F + 0.01F);
    }

    // Newly discovered bug (this session, same family as #2): visit(HStack) never
    // arms inherited_width_ to a resolved column's width before recursing into that
    // column via child->accept(*this) -- unlike the Measure pass, which does. The
    // column's own frame.size.width gets force-corrected after accept() returns, so
    // the previous test (which only checks a column's own direct Text children)
    // still passes -- but whatever that column laid out *during* accept() (its own
    // nested content, e.g. an unstyled Rectangle with no explicit width holding a
    // further-nested weighted HStack) used incoming_width() straight from this
    // HStack's own ancestor page width instead of its actual resolved share.
    TEST(DocraftLoomLayoutProcessorTest, WeightedHStackColumnPropagatesResolvedWidthToNestedContentDuringLayout)
    {
        auto outer = std::make_shared<loom::nodes::DocraftLoomHStack>();
        outer->set_padding(0.0F);
        outer->set_weights({1.0F, 1.0F});
        outer->edit_layout_box().measured_size = {.width = 600.0F, .height = 20.0F};

        auto column = std::make_shared<loom::nodes::DocraftLoomRectangle>();
        column->set_padding(0.0F); // no explicit width -- must take its share from outer
        column->edit_layout_box().measured_size = {.width = 300.0F, .height = 20.0F};

        auto inner = std::make_shared<loom::nodes::DocraftLoomHStack>();
        inner->set_padding(0.0F);
        inner->set_weights({1.0F, 1.0F});
        inner->edit_layout_box().measured_size = {.width = 300.0F, .height = 20.0F};

        auto a = std::make_shared<loom::nodes::DocraftLoomText>("a");
        a->edit_layout_box().measured_size = {.width = 10.0F, .height = 10.0F};
        auto b = std::make_shared<loom::nodes::DocraftLoomText>("b");
        b->edit_layout_box().measured_size = {.width = 10.0F, .height = 10.0F};
        inner->add_child(a);
        inner->add_child(b);
        column->add_child(inner);

        auto other_column = std::make_shared<loom::nodes::DocraftLoomText>("x");
        other_column->edit_layout_box().measured_size = {.width = 10.0F, .height = 10.0F};

        outer->add_child(column);
        outer->add_child(other_column);

        loom::pipeline::DocraftLoomLayoutProcessor processor(600.0F);
        outer->accept(processor);

        // The inner HStack must divide `column`'s ~300pt resolved share (150 each),
        // not the outer HStack's own 600pt page width (which is what it would divide
        // if `column` were laid out against the wrong incoming width).
        EXPECT_NEAR(a->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).size.width, 150.0F, 0.01F);
        EXPECT_NEAR(b->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).size.width, 150.0F, 0.01F);
    }

    // Review bug #2 (Layout side, Table): same nesting bug for a weighted Table --
    // its columns must be resolved against the enclosing Rectangle's width, not
    // the full page width.
    TEST(DocraftLoomLayoutProcessorTest, RectangleNarrowsWeightedTableColumnsInsteadOfFullPage)
    {
        auto rectangle = std::make_shared<loom::nodes::DocraftLoomRectangle>();
        rectangle->set_width(200.0F);
        rectangle->edit_layout_box().measured_size = {.width = 200.0F, .height = 50.0F};

        auto table = std::make_shared<loom::nodes::DocraftLoomTable>();
        table->set_padding(0.0F);
        table->set_column_weights({1.0F, 1.0F});

        auto cell_a = std::make_shared<loom::nodes::DocraftLoomTableCell>();
        cell_a->set_content(std::make_shared<loom::nodes::DocraftLoomText>("a"));
        cell_a->edit_layout_box().measured_size = {.width = 10.0F, .height = 10.0F};

        auto cell_b = std::make_shared<loom::nodes::DocraftLoomTableCell>();
        cell_b->set_content(std::make_shared<loom::nodes::DocraftLoomText>("b"));
        cell_b->edit_layout_box().measured_size = {.width = 10.0F, .height = 10.0F};

        table->add_row({cell_a, cell_b});
        rectangle->add_child(table);

        loom::pipeline::DocraftLoomLayoutProcessor processor(500.0F);
        rectangle->accept(processor);

        const float total_width = table->cell(0, 0)->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).size.width
            + table->cell(0, 1)->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).size.width;
        EXPECT_LE(total_width, 200.0F + 0.01F);
    }

    // Review bug #9: a negative column weight must not inflate the other
    // columns' shares -- the same >0.0F guard used everywhere else (HStack,
    // Measure's Table weight loop) must apply to Layout's Table weight sum too.
    TEST(DocraftLoomLayoutProcessorTest, TableColumnWidthsIgnoreNonPositiveWeight)
    {
        auto table = std::make_shared<loom::nodes::DocraftLoomTable>();
        table->set_padding(0.0F);
        table->set_column_weights({1.0F, 1.0F, -1.0F});

        std::vector<std::shared_ptr<loom::nodes::DocraftLoomTableCell>> row;
        for (int i = 0; i < 3; ++i)
        {
            auto cell = std::make_shared<loom::nodes::DocraftLoomTableCell>();
            cell->set_content(std::make_shared<loom::nodes::DocraftLoomText>("x"));
            cell->edit_layout_box().measured_size = {.width = 10.0F, .height = 10.0F};
            row.push_back(cell);
        }
        table->add_row(row);

        loom::pipeline::DocraftLoomLayoutProcessor processor(300.0F);
        table->accept(processor);

        // Effective weight is 1.0 for every column (negative treated as the 1.0
        // default), so all three columns must end up roughly equal -- not with the
        // first two inflated because the negative weight dragged total_weight down.
        const float w0 = table->cell(0, 0)->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).size.width;
        const float w1 = table->cell(0, 1)->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).size.width;
        const float w2 = table->cell(0, 2)->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).size.width;
        EXPECT_NEAR(w0, w2, 1.0F);
        EXPECT_NEAR(w1, w2, 1.0F);
    }

    // Companion to the vertical page-bottom overflow warning (#71): when a table's
    // explicit column widths sum wider than the available width, it silently
    // overflows past the page margin (bug found while validating #81's flexible-column
    // fix). Mirrors DocraftLoomPaginationProcessorTest.OversizedNonTableNodeLogsWarning's
    // stdout-capture approach.
    TEST(DocraftLoomLayoutProcessorTest, ExplicitColumnWidthsWiderThanAvailableWidthLogsWarning)
    {
        auto table = std::make_shared<loom::nodes::DocraftLoomTable>();
        table->set_name("wide_table");
        table->set_padding(0.0F);

        std::vector<std::shared_ptr<loom::nodes::DocraftLoomTableCell>> row;
        for (int i = 0; i < 3; ++i)
        {
            auto cell = std::make_shared<loom::nodes::DocraftLoomTableCell>();
            cell->set_explicit_width(300.0F);
            cell->set_content(std::make_shared<loom::nodes::DocraftLoomText>("x"));
            cell->edit_layout_box().measured_size = {.width = 300.0F, .height = 10.0F};
            row.push_back(cell);
        }
        table->add_row(row);

        // 900pt of explicit column width against a 200pt page -- must overflow.
        loom::pipeline::DocraftLoomLayoutProcessor processor(200.0F);
        testing::internal::CaptureStdout();
        table->accept(processor);
        const std::string stdout_log = testing::internal::GetCapturedStdout();

        EXPECT_NE(stdout_log.find("[WARNING]"), std::string::npos);
        EXPECT_NE(stdout_log.find("wide_table"), std::string::npos);
    }

    TEST(DocraftLoomLayoutProcessorTest, ExplicitColumnWidthsWithinAvailableWidthLogsNoWarning)
    {
        auto table = std::make_shared<loom::nodes::DocraftLoomTable>();
        table->set_name("narrow_table");
        table->set_padding(0.0F);

        std::vector<std::shared_ptr<loom::nodes::DocraftLoomTableCell>> row;
        for (int i = 0; i < 2; ++i)
        {
            auto cell = std::make_shared<loom::nodes::DocraftLoomTableCell>();
            cell->set_explicit_width(50.0F);
            cell->set_content(std::make_shared<loom::nodes::DocraftLoomText>("x"));
            cell->edit_layout_box().measured_size = {.width = 50.0F, .height = 10.0F};
            row.push_back(cell);
        }
        table->add_row(row);

        loom::pipeline::DocraftLoomLayoutProcessor processor(200.0F);
        testing::internal::CaptureStdout();
        table->accept(processor);
        const std::string stdout_log = testing::internal::GetCapturedStdout();

        EXPECT_EQ(stdout_log.find("[WARNING]"), std::string::npos);
    }

    // Review bug #8 (a): table cell content in absolute position mode must be
    // rejected the same way the cell itself already is -- resolving it to a raw
    // page coordinate silently ignores the cell's own layout entirely.
    TEST(DocraftLoomLayoutProcessorTest, AbsolutePositionedCellContentThrows)
    {
        auto table = std::make_shared<loom::nodes::DocraftLoomTable>();
        auto content = std::make_shared<loom::nodes::DocraftLoomText>("x");
        content->set_position_mode(loom::nodes::DocraftPositionType::kAbsolute);
        content->set_explicit_position({.x = 999.0F, .y = 999.0F});
        auto cell = std::make_shared<loom::nodes::DocraftLoomTableCell>();
        cell->set_content(content);
        table->add_row({cell});

        loom::pipeline::DocraftLoomLayoutProcessor processor(300.0F);
        EXPECT_THROW(table->accept(processor), docraft::exception::InvalidInputException);
    }

    // Review bug #8 (b): a margin on table cell content must be honored (mirrors
    // how VStack/HStack apply resolve_outer_margin to their children), not
    // silently dropped in favor of only the cell's own fixed padding.
    TEST(DocraftLoomLayoutProcessorTest, TableCellContentMarginIsHonored)
    {
        auto make_table_with_margin = [](float margin)
        {
            auto table = std::make_shared<loom::nodes::DocraftLoomTable>();
            auto content = std::make_shared<loom::nodes::DocraftLoomText>("x");
            content->set_margin(margin);
            content->edit_layout_box().measured_size = {.width = 10.0F, .height = 10.0F};
            auto cell = std::make_shared<loom::nodes::DocraftLoomTableCell>();
            cell->set_content(content);
            cell->edit_layout_box().measured_size = {.width = 30.0F, .height = 30.0F};
            table->add_row({cell});

            loom::pipeline::DocraftLoomLayoutProcessor processor(300.0F);
            table->accept(processor);
            return content->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).position.y;
        };

        const float y_without_margin = make_table_with_margin(0.0F);
        const float y_with_margin = make_table_with_margin(20.0F);
        EXPECT_NEAR(y_with_margin - y_without_margin, 20.0F, 0.01F);
    }

    // Bug #98: an Image with a declared width() inside a weighted HStack used to be
    // force-stretched to the resolved column width regardless -- height was left
    // alone, so the image came out distorted. The declared width must now survive;
    // the column itself still gets the full resolved share (weights() still divides
    // the row into columns), it just isn't imposed onto the image's own frame.
    TEST(DocraftLoomLayoutProcessorTest, WeightedHStackDoesNotStretchImageWithExplicitWidth)
    {
        auto hstack = std::make_shared<loom::nodes::DocraftLoomHStack>();
        hstack->set_padding(0.0F);
        hstack->set_weights({1.0F, 1.0F});

        auto image = std::make_shared<loom::nodes::DocraftLoomImage>();
        image->set_width(83.1F);
        image->set_height(34.0F);
        image->edit_layout_box().measured_size = {.width = 83.1F, .height = 34.0F};
        hstack->add_child(image);

        auto other = std::make_shared<loom::nodes::DocraftLoomText>("b");
        other->edit_layout_box().measured_size = {.width = 20.0F, .height = 10.0F};
        hstack->add_child(other);

        loom::pipeline::DocraftLoomLayoutProcessor processor(200.0F);
        hstack->accept(processor);

        const auto& image_frame = image->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof());
        // Not stretched to its 100pt resolved slot -- keeps its declared 83.1pt width,
        // undistorted alongside its declared height.
        EXPECT_NEAR(image_frame.size.width, 83.1F, 0.01F);
        EXPECT_NEAR(image_frame.size.height, 34.0F, 0.01F);

        // A weighted child with no explicit width of its own (e.g. a plain Rectangle
        // used as a column background) is unaffected and still fills its slot.
        const auto& other_frame = other->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof());
        EXPECT_NEAR(other_frame.size.width, 100.0F, 0.01F);
    }
} // namespace docraft::test
