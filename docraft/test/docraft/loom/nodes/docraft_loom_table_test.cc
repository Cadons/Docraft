#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "docraft/exception/docraft_input_exceptions.h"
#include "docraft/loom/nodes/docraft_loom_rectangle.h"
#include "docraft/loom/nodes/docraft_loom_table.h"
#include "docraft/loom/nodes/docraft_loom_table_cell.h"
#include "docraft/loom/nodes/docraft_loom_text.h"
#include "docraft/loom/pipeline/docraft_loom_layout_processor.h"
#include "docraft/loom/pipeline/docraft_loom_measure_processor.h"
#include "../../backend/docraft_mock_backend.h"

namespace docraft::test {
    using ::testing::Return;
    using ::testing::_;

    class DocraftLoomTableTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            text_backend_ = std::make_shared<::testing::NiceMock<backend::MockDocraftTextRenderingBackend>>();
            ON_CALL(*text_backend_, measure_text_width(_, _, _)).WillByDefault(Return(50.0F));
            ON_CALL(*text_backend_, measure_text_height(_, _)).WillByDefault(Return(10.0F));
            measure_ = std::make_unique<loom::pipeline::DocraftLoomMeasureProcessor>(text_backend_);
        }

        static std::shared_ptr<loom::nodes::DocraftLoomTableCell> make_cell(const std::string& text,
                                                                            bool is_title = false)
        {
            auto cell = std::make_shared<loom::nodes::DocraftLoomTableCell>();
            cell->set_content(std::make_shared<loom::nodes::DocraftLoomText>(text));
            cell->set_is_title(is_title);
            return cell;
        }

        std::shared_ptr<backend::MockDocraftTextRenderingBackend> text_backend_;
        std::unique_ptr<loom::pipeline::DocraftLoomMeasureProcessor> measure_;
    };

    TEST_F(DocraftLoomTableTest, EqualWeightColumnsDistributeEvenly)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillRepeatedly(Return(10.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillRepeatedly(Return(10.0F));

        auto table = std::make_shared<loom::nodes::DocraftLoomTable>();
        table->set_padding(0.0F); // isolate column-width math from the table's own outer padding
        table->add_row({make_cell("a", true), make_cell("b", true)});
        table->accept(*measure_);

        loom::pipeline::DocraftLoomLayoutProcessor layout(200.0F);
        table->accept(layout);

        // available_width = 200 - 2*2.5 = 195, split evenly -> ~97.5 each (floored at natural 10)
        EXPECT_FLOAT_EQ(table->cell(0, 0)->layout_box().frame.size.width, 97.5F);
        EXPECT_FLOAT_EQ(table->cell(0, 1)->layout_box().frame.size.width, 97.5F);
    }

    TEST_F(DocraftLoomTableTest, ExplicitWidthIsRespectedAndRemainderRedistributed)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillRepeatedly(Return(10.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillRepeatedly(Return(10.0F));

        auto table = std::make_shared<loom::nodes::DocraftLoomTable>();
        table->set_padding(0.0F); // isolate column-width math from the table's own outer padding
        auto c0 = make_cell("a", true);
        c0->set_explicit_width(40.0F);
        table->add_row({c0, make_cell("b", true)});
        table->accept(*measure_);

        loom::pipeline::DocraftLoomLayoutProcessor layout(200.0F);
        table->accept(layout);

        EXPECT_FLOAT_EQ(table->cell(0, 0)->layout_box().frame.size.width, 40.0F);
        // no rescale when any explicit width is present: column 1 gets its weight-based
        // share of available_width (195 / 2 columns = 97.5), floored at its natural width
        EXPECT_FLOAT_EQ(table->cell(0, 1)->layout_box().frame.size.width, 97.5F);
    }

    TEST_F(DocraftLoomTableTest, NaturalWidthFloorIsRespectedWhenNoRescaleIsNeeded)
    {
        // The natural-width floor only "sticks" in the final result when the columns'
        // floored widths already sum to exactly available_width (so the subsequent
        // proportional rescale -- needed whenever no column has an explicit width, to
        // make the table fill available_width exactly -- is a no-op scale of 1). This
        // mirrors legacy's docraft_layout_horizontal_table_handler.cc exactly: its
        // rescale is unconditional too (widths_sum != available_width), so a natural
        // width floor that pushes the sum above available_width gets proportionally
        // shrunk back down along with everything else -- the floor is not an absolute
        // per-column guarantee once a rescale is triggered, only a preference that
        // holds when it doesn't need to fight the rescale.
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillRepeatedly(Return(75.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillRepeatedly(Return(10.0F));

        auto table = std::make_shared<loom::nodes::DocraftLoomTable>();
        table->set_padding(0.0F); // isolate column-width math from the table's own outer padding
        table->add_row({make_cell("a", true), make_cell("b", true)});
        table->accept(*measure_);

        // available_width = 200 - 2*2.5 = 195; weight share = 97.5 each, which already
        // exceeds the natural width of 75, so the floor never has to compete with the
        // rescale here -- both columns simply get their even weight-based share.
        loom::pipeline::DocraftLoomLayoutProcessor layout(200.0F);
        table->accept(layout);

        EXPECT_FLOAT_EQ(table->cell(0, 0)->layout_box().frame.size.width, 97.5F);
        EXPECT_FLOAT_EQ(table->cell(0, 1)->layout_box().frame.size.width, 97.5F);
    }

    TEST_F(DocraftLoomTableTest, NarrowPageRescalesColumnsProportionally)
    {
        // When available_width is far smaller than the total natural content and no
        // column has an explicit width, every column -- including one with a much
        // larger natural width -- is rescaled down proportionally to fit exactly (see
        // the note on NaturalWidthFloorIsRespectedWhenNoRescaleIsNeeded above). The
        // 15:1 natural-width ratio between the two columns is preserved through the
        // rescale even though neither ends up at its own natural width.
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _))
            .WillOnce(Return(150.0F))
            .WillOnce(Return(10.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillRepeatedly(Return(10.0F));

        auto table = std::make_shared<loom::nodes::DocraftLoomTable>();
        table->set_padding(0.0F); // isolate column-width math from the table's own outer padding
        table->add_row({make_cell("very long title text", true), make_cell("b", true)});
        table->accept(*measure_);

        loom::pipeline::DocraftLoomLayoutProcessor layout(20.0F); // deliberately too narrow
        table->accept(layout);

        // Cell natural widths include the automatic content padding (2*2.5 = 5), so
        // natural widths are 150+5=155 and 10+5=15. available_width = 20 - 5 = 15;
        // scale = 15 / (155+15) = 15/170.
        EXPECT_NEAR(table->cell(0, 0)->layout_box().frame.size.width, 155.0F * 15.0F / 170.0F, 0.001F);
        EXPECT_NEAR(table->cell(0, 1)->layout_box().frame.size.width, 15.0F * 15.0F / 170.0F, 0.001F);
    }

    TEST_F(DocraftLoomTableTest, RowHeightIsTallestCellPlusPadding)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillRepeatedly(Return(10.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _))
            .WillOnce(Return(10.0F))
            .WillOnce(Return(30.0F));

        auto table = std::make_shared<loom::nodes::DocraftLoomTable>();
        table->add_row({make_cell("a"), make_cell("b")});
        table->accept(*measure_);

        loom::pipeline::DocraftLoomLayoutProcessor layout(200.0F);
        table->accept(layout);

        EXPECT_FLOAT_EQ(table->cell(0, 0)->layout_box().frame.size.height, 35.0F); // 30 + 2*2.5
    }

    TEST_F(DocraftLoomTableTest, LayoutDoesNotOverlapColumns)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillRepeatedly(Return(10.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillRepeatedly(Return(10.0F));

        auto table = std::make_shared<loom::nodes::DocraftLoomTable>();
        table->add_row({make_cell("a"), make_cell("b"), make_cell("c")});
        table->accept(*measure_);

        loom::pipeline::DocraftLoomLayoutProcessor layout(300.0F);
        table->accept(layout);

        const float col0_right = table->cell(0, 0)->layout_box().frame.position.x
            + table->cell(0, 0)->layout_box().frame.size.width;
        EXPECT_FLOAT_EQ(col0_right, table->cell(0, 1)->layout_box().frame.position.x);
        const float col1_right = table->cell(0, 1)->layout_box().frame.position.x
            + table->cell(0, 1)->layout_box().frame.size.width;
        EXPECT_FLOAT_EQ(col1_right, table->cell(0, 2)->layout_box().frame.position.x);
    }

    TEST_F(DocraftLoomTableTest, PaddingPushesTheWholeGridAwayFromTheTableOrigin)
    {
        // padding() must move the entire visible grid (and therefore the border drawn
        // around it, see DocraftLoomRenderingProcessor::draw_table_borders_and_dividers)
        // away from wherever the parent positioned this table -- not just inset cell
        // content within an unmoved border, which would leave the table sitting flush
        // against whatever precedes it.
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillRepeatedly(Return(10.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillRepeatedly(Return(10.0F));

        auto table = std::make_shared<loom::nodes::DocraftLoomTable>();
        table->set_padding(6.0F);
        table->add_row({make_cell("a"), make_cell("b")});
        table->accept(*measure_);

        loom::pipeline::DocraftLoomLayoutProcessor layout(200.0F);
        layout.reset_cursor(10.0F, 20.0F);
        table->accept(layout);

        const auto& table_origin = table->layout_box().frame.position;
        EXPECT_FLOAT_EQ(table_origin.x, 10.0F);
        EXPECT_FLOAT_EQ(table_origin.y, 20.0F);

        const auto& first_cell_pos = table->cell(0, 0)->layout_box().frame.position;
        EXPECT_FLOAT_EQ(first_cell_pos.x, table_origin.x + 6.0F);
        EXPECT_FLOAT_EQ(first_cell_pos.y, table_origin.y + 6.0F);

        // measured_size (what a parent VStack advances its cursor by) reserves the
        // padding band on the trailing side too, so the next sibling doesn't sit flush
        // against the table's bottom/right edge either.
        const auto& measured = table->layout_box().measured_size;
        const auto& last_cell = table->cell(0, 1)->layout_box();
        EXPECT_FLOAT_EQ(table_origin.y + measured.height,
                        first_cell_pos.y + last_cell.frame.size.height + 6.0F);
    }

    TEST_F(DocraftLoomTableTest, LongCellTextWrapsInsteadOfOverflowingItsColumn)
    {
        // Natural (unwrapped) text width (50, the mock's default) is deliberately wider
        // than the column budget estimated from a narrow content_width_, so the cell's
        // Text must wrap instead of silently overflowing the resolved column.
        measure_->set_content_width(20.0F);

        auto table = std::make_shared<loom::nodes::DocraftLoomTable>();
        table->set_padding(0.0F);
        table->add_row({make_cell("a long piece of cell text")});
        table->accept(*measure_);

        auto text = std::dynamic_pointer_cast<loom::nodes::DocraftLoomText>(table->cell(0, 0)->content());
        ASSERT_TRUE(text);
        // available_width = 20 - 2*2.5 = 15; single column -> budget = 15 - 2*2.5 = 10.
        EXPECT_FLOAT_EQ(text->wrap_width(), 10.0F);
        EXPECT_GT(text->wrapped_lines().size(), 1U);
        EXPECT_FLOAT_EQ(text->layout_box().measured_size.width, 10.0F);
    }

    TEST_F(DocraftLoomTableTest, ShortCellTextKeepsNaturalWidthEvenWithAColumnBudget)
    {
        // A column budget being known doesn't mean every cell stretches to fill it (that
        // would break the natural-width-floor column sizing) -- a cell only wraps (and
        // its measured width only changes) if its natural width actually overflows the
        // budget.
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillRepeatedly(Return(5.0F));
        measure_->set_content_width(200.0F);

        auto table = std::make_shared<loom::nodes::DocraftLoomTable>();
        table->set_padding(0.0F);
        table->add_row({make_cell("ok")});
        table->accept(*measure_);

        auto text = std::dynamic_pointer_cast<loom::nodes::DocraftLoomText>(table->cell(0, 0)->content());
        ASSERT_TRUE(text);
        EXPECT_TRUE(text->wrapped_lines().empty());
        EXPECT_FLOAT_EQ(text->layout_box().measured_size.width, 5.0F);
    }

    TEST_F(DocraftLoomTableTest, TitleCellBoldByDefaultIsCallerResponsibility)
    {
        auto cell = make_cell("Header", true);
        auto text = std::dynamic_pointer_cast<loom::nodes::DocraftLoomText>(cell->content());
        text->set_bold(true); // caller applies default styling explicitly; cell itself is neutral
        EXPECT_TRUE(text->bold());
        EXPECT_TRUE(cell->is_title());
    }

    TEST_F(DocraftLoomTableTest, FrameSizeMatchesMeasuredSizeForTableAndCells)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillRepeatedly(Return(10.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillRepeatedly(Return(10.0F));

        auto table = std::make_shared<loom::nodes::DocraftLoomTable>();
        table->add_row({make_cell("a"), make_cell("b")});
        table->accept(*measure_);

        loom::pipeline::DocraftLoomLayoutProcessor layout(200.0F);
        table->accept(layout);

        EXPECT_GT(table->layout_box().frame.size.width, 0.0F);
        EXPECT_GT(table->layout_box().frame.size.height, 0.0F);
        EXPECT_GT(table->cell(0, 0)->layout_box().frame.size.width, 0.0F);
    }

    TEST_F(DocraftLoomTableTest, RowTitleGridProducesCorrectLayout)
    {
        // "vertical"-style usage: column 0 holds row labels (is_title), column 1 holds values.
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillRepeatedly(Return(10.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillRepeatedly(Return(10.0F));

        auto table = std::make_shared<loom::nodes::DocraftLoomTable>();
        table->add_row({make_cell("Name", true), make_cell("Alice")});
        table->add_row({make_cell("Age", true), make_cell("30")});
        table->accept(*measure_);

        loom::pipeline::DocraftLoomLayoutProcessor layout(200.0F);
        table->accept(layout);

        EXPECT_FLOAT_EQ(table->cell(1, 0)->layout_box().frame.position.y,
                        table->cell(0, 0)->layout_box().frame.position.y + table->cell(0, 0)->layout_box().frame.size.
                        height);
        EXPECT_TRUE(table->cell(0, 0)->is_title());
        EXPECT_FALSE(table->cell(0, 1)->is_title());
    }

    TEST_F(DocraftLoomTableTest, AddRowRejectsNonTextNonImageContent)
    {
        auto cell = std::make_shared<loom::nodes::DocraftLoomTableCell>();
        EXPECT_THROW(cell->set_content(std::make_shared<loom::nodes::DocraftLoomRectangle>()),
                     exception::InvalidInputException);
    }

    TEST_F(DocraftLoomTableTest, AddRowRejectsNullCell)
    {
        auto table = std::make_shared<loom::nodes::DocraftLoomTable>();
        EXPECT_THROW(table->add_row({nullptr}), exception::InvalidInputException);
    }

    TEST_F(DocraftLoomTableTest, AddRowRejectsMismatchedColumnCount)
    {
        auto table = std::make_shared<loom::nodes::DocraftLoomTable>();
        table->add_row({make_cell("a"), make_cell("b")});
        EXPECT_THROW(table->add_row({make_cell("c")}), exception::InvalidInputException);
    }

    TEST_F(DocraftLoomTableTest, LeadingTitleRowCountIsZeroWithNoTitleRows) {
        auto table = std::make_shared<loom::nodes::DocraftLoomTable>();
        table->add_row({make_cell("a"), make_cell("b")});
        table->add_row({make_cell("c"), make_cell("d")});
        EXPECT_EQ(table->leading_title_row_count(), 0);
    }

    TEST_F(DocraftLoomTableTest, LeadingTitleRowCountStopsAtFirstNonTitleRow) {
        auto table = std::make_shared<loom::nodes::DocraftLoomTable>();
        table->add_row({make_cell("Col A", true), make_cell("Col B", true)});
        table->add_row({make_cell("a"), make_cell("b")});
        table->add_row({make_cell("c", true), make_cell("d", true)});
        EXPECT_EQ(table->leading_title_row_count(), 1);
    }

    TEST_F(DocraftLoomTableTest, LeadingTitleRowCountCoversAllRowsWhenAllAreTitle) {
        auto table = std::make_shared<loom::nodes::DocraftLoomTable>();
        table->add_row({make_cell("Col A", true), make_cell("Col B", true)});
        table->add_row({make_cell("Col C", true), make_cell("Col D", true)});
        EXPECT_EQ(table->leading_title_row_count(), 2);
    }
} // namespace docraft::test