#include <gtest/gtest.h>

#include "docraft/loom/nodes/docraft_loom_line.h"
#include "docraft/loom/pipeline/docraft_loom_layout_processor.h"
#include "docraft/loom/pipeline/docraft_loom_measure_processor.h"

namespace docraft::test {
    class DocraftLoomLineTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            measure_ = std::make_unique<loom::pipeline::DocraftLoomMeasureProcessor>(nullptr);
            layout_ = std::make_unique<loom::pipeline::DocraftLoomLayoutProcessor>();
        }

        std::unique_ptr<loom::pipeline::DocraftLoomMeasureProcessor> measure_;
        std::unique_ptr<loom::pipeline::DocraftLoomLayoutProcessor> layout_;
    };

    TEST_F(DocraftLoomLineTest, MeasureUsesDefaultPointsAndBorderWidth)
    {
        loom::nodes::DocraftLoomLine line;
        line.accept(*measure_);

        EXPECT_FLOAT_EQ(line.layout_box().measured_size.width, 100.0F); // |100 - 0|
        EXPECT_FLOAT_EQ(line.layout_box().measured_size.height, 4.0F); // max(1, 4)
    }

    TEST_F(DocraftLoomLineTest, MeasureUsesCustomPoints)
    {
        loom::nodes::DocraftLoomLine line;
        line.set_start({.x = 10.0F, .y = 0.0F});
        line.set_end({.x = 60.0F, .y = 0.0F});
        line.accept(*measure_);

        EXPECT_FLOAT_EQ(line.layout_box().measured_size.width, 50.0F);
    }

    TEST_F(DocraftLoomLineTest, MeasureHeightFollowsThickBorderWidth)
    {
        loom::nodes::DocraftLoomLine line;
        line.set_border_width(10.0F);
        line.accept(*measure_);

        EXPECT_FLOAT_EQ(line.layout_box().measured_size.height, 10.0F); // max(10, 4)
    }

    TEST_F(DocraftLoomLineTest, MeasureHeightCoversVerticalExtentOfDiagonalLine)
    {
        loom::nodes::DocraftLoomLine line;
        line.set_start({.x = 0.0F, .y = 0.0F});
        line.set_end({.x = 30.0F, .y = 130.0F});
        line.accept(*measure_);

        EXPECT_FLOAT_EQ(line.layout_box().measured_size.width, 30.0F);
        EXPECT_FLOAT_EQ(line.layout_box().measured_size.height, 130.0F); // |130 - 0|, exceeds the 4pt floor
    }

    TEST_F(DocraftLoomLineTest, MeasureHeightFloorsThinBorderWidth)
    {
        loom::nodes::DocraftLoomLine line;
        line.set_border_width(0.5F);
        line.accept(*measure_);

        EXPECT_FLOAT_EQ(line.layout_box().measured_size.height, 4.0F); // max(0.5, 4)
    }

    TEST_F(DocraftLoomLineTest, LayoutPlacesAtCursorAndAdvancesByHeightOnly)
    {
        auto line = std::make_shared<loom::nodes::DocraftLoomLine>();
        line->accept(*measure_);
        line->accept(*layout_);

        EXPECT_FLOAT_EQ(line->layout_box().frame.size.width, 100.0F);
        EXPECT_FLOAT_EQ(line->layout_box().frame.size.height, 4.0F);

        auto next = std::make_shared<loom::nodes::DocraftLoomLine>();
        next->accept(*measure_);
        next->accept(*layout_);

        EXPECT_FLOAT_EQ(next->layout_box().frame.position.x, line->layout_box().frame.position.x);
        EXPECT_FLOAT_EQ(next->layout_box().frame.position.y, line->layout_box().frame.position.y + 4.0F);
    }

    TEST_F(DocraftLoomLineTest, AbsolutePositionDoesNotAdvanceCursor)
    {
        loom::pipeline::DocraftLoomLayoutProcessor layout;
        auto line = std::make_shared<loom::nodes::DocraftLoomLine>();
        line->set_position_mode(loom::nodes::DocraftPositionType::kAbsolute);
        line->set_explicit_position({30.0F, 40.0F});
        line->accept(*measure_);
        line->accept(layout);

        EXPECT_FLOAT_EQ(line->layout_box().frame.position.x, 30.0F);
        EXPECT_FLOAT_EQ(line->layout_box().frame.position.y, 40.0F);

        auto next = std::make_shared<loom::nodes::DocraftLoomLine>();
        next->accept(*measure_);
        next->accept(layout);
        EXPECT_FLOAT_EQ(next->layout_box().frame.position.y, 10.0F); // default top margin, untouched
    }
} // namespace docraft::test