#include <gtest/gtest.h>

#include "docraft/loom/nodes/docraft_loom_line.h"
#include "docraft/loom/pipeline/docraft_loom_layout_processor.h"
#include "docraft/loom/pipeline/docraft_loom_measure_processor.h"
#include "docraft/utils/docraft_loom_layout_box_test_access.h"

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

    TEST_F(DocraftLoomLineTest, MeasureSpansFromTheAnchorToTheFarthestEndpoint)
    {
        // Endpoints are offsets from the node's anchor, not a self-contained bounding box,
        // so a segment starting 10pt in still occupies the full 60pt from that anchor.
        loom::nodes::DocraftLoomLine line;
        line.set_start({.x = 10.0F, .y = 0.0F});
        line.set_end({.x = 60.0F, .y = 0.0F});
        line.accept(*measure_);

        EXPECT_FLOAT_EQ(line.layout_box().measured_size.width, 60.0F);
    }

    TEST_F(DocraftLoomLineTest, MeasureIgnoresNegativeEndpointsWhenSizingTheBox)
    {
        // Negative offsets draw above/left of the anchor on purpose (a Canvas clips them);
        // they must not drag the box's own extent below the anchor.
        loom::nodes::DocraftLoomLine line;
        line.set_start({.x = -20.0F, .y = -30.0F});
        line.set_end({.x = 40.0F, .y = 50.0F});
        line.accept(*measure_);

        EXPECT_FLOAT_EQ(line.layout_box().measured_size.width, 40.0F);
        EXPECT_FLOAT_EQ(line.layout_box().measured_size.height, 50.0F);
    }

    TEST_F(DocraftLoomLineTest, MeasureCoversAVerticalOffsetSharedByBothEndpoints)
    {
        // Regression test for issue #38: a horizontal segment pushed down by a shared
        // y offset used to measure only |y2 - y1| == 0, collapsing to the 4pt floor.
        loom::nodes::DocraftLoomLine line;
        line.set_start({.x = 0.0F, .y = 75.0F});
        line.set_end({.x = 200.0F, .y = 75.0F});
        line.accept(*measure_);

        EXPECT_FLOAT_EQ(line.layout_box().measured_size.width, 200.0F);
        EXPECT_FLOAT_EQ(line.layout_box().measured_size.height, 75.0F);
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

        EXPECT_FLOAT_EQ(line->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).size.width, 100.0F);
        EXPECT_FLOAT_EQ(line->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).size.height, 4.0F);

        auto next = std::make_shared<loom::nodes::DocraftLoomLine>();
        next->accept(*measure_);
        next->accept(*layout_);

        EXPECT_FLOAT_EQ(next->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).position.x, line->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).position.x);
        EXPECT_FLOAT_EQ(next->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).position.y, line->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).position.y + 4.0F);
    }

    TEST_F(DocraftLoomLineTest, AbsolutePositionDoesNotAdvanceCursor)
    {
        loom::pipeline::DocraftLoomLayoutProcessor layout;
        auto line = std::make_shared<loom::nodes::DocraftLoomLine>();
        line->set_position_mode(loom::nodes::DocraftPositionType::kAbsolute);
        line->set_explicit_position({30.0F, 40.0F});
        line->accept(*measure_);
        line->accept(layout);

        EXPECT_FLOAT_EQ(line->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).position.x, 30.0F);
        EXPECT_FLOAT_EQ(line->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).position.y, 40.0F);

        auto next = std::make_shared<loom::nodes::DocraftLoomLine>();
        next->accept(*measure_);
        next->accept(layout);
        EXPECT_FLOAT_EQ(next->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).position.y, 10.0F); // default top margin, untouched
    }
} // namespace docraft::test