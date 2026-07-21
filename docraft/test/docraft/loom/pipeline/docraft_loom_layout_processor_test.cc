#include "docraft/loom/pipeline/docraft_loom_layout_processor.h"

#include <gtest/gtest.h>

#include "docraft/exception/docraft_exceptions.h"
#include "docraft/loom/nodes/docraft_loom_hstack.h"
#include "docraft/loom/nodes/docraft_loom_rectangle.h"
#include "docraft/loom/nodes/docraft_loom_table.h"
#include "docraft/loom/nodes/docraft_loom_table_cell.h"
#include "docraft/loom/nodes/docraft_loom_text.h"

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

        const float first_width = hstack->child(0)->layout_box().frame.size.width;
        const float second_width = hstack->child(1)->layout_box().frame.size.width;
        EXPECT_NEAR(first_width, 100.0F, 0.01F);
        EXPECT_NEAR(second_width, 100.0F, 0.01F);
        EXPECT_LE(first_width + second_width, 200.0F + 0.01F);
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

        const float total_width = table->cell(0, 0)->layout_box().frame.size.width
            + table->cell(0, 1)->layout_box().frame.size.width;
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
        const float w0 = table->cell(0, 0)->layout_box().frame.size.width;
        const float w1 = table->cell(0, 1)->layout_box().frame.size.width;
        const float w2 = table->cell(0, 2)->layout_box().frame.size.width;
        EXPECT_NEAR(w0, w2, 1.0F);
        EXPECT_NEAR(w1, w2, 1.0F);
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
            return content->layout_box().frame.position.y;
        };

        const float y_without_margin = make_table_with_margin(0.0F);
        const float y_with_margin = make_table_with_margin(20.0F);
        EXPECT_NEAR(y_with_margin - y_without_margin, 20.0F, 0.01F);
    }
} // namespace docraft::test
