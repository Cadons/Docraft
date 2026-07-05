#include <gtest/gtest.h>

#include "docraft/loom/nodes/docraft_loom_blank_line.h"
#include "docraft/loom/pipeline/docraft_loom_layout_processor.h"
#include "docraft/loom/pipeline/docraft_loom_measure_processor.h"

namespace docraft::test {
    class DocraftLoomBlankLineTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            measure_ = std::make_unique<loom::pipeline::DocraftLoomMeasureProcessor>(nullptr);
        }

        std::unique_ptr<loom::pipeline::DocraftLoomMeasureProcessor> measure_;
    };

    TEST_F(DocraftLoomBlankLineTest, DefaultHeightIsOne)
    {
        loom::nodes::DocraftLoomBlankLine blank_line;
        EXPECT_FLOAT_EQ(blank_line.height(), 1.0F);
    }

    TEST_F(DocraftLoomBlankLineTest, SetHeightOverridesDefault)
    {
        loom::nodes::DocraftLoomBlankLine blank_line;
        blank_line.set_height(5.0F);
        EXPECT_FLOAT_EQ(blank_line.height(), 5.0F);
    }

    TEST_F(DocraftLoomBlankLineTest, MeasureUsesHeightAndZeroWidth)
    {
        loom::nodes::DocraftLoomBlankLine blank_line;
        blank_line.set_height(3.0F);
        blank_line.accept(*measure_);

        EXPECT_FLOAT_EQ(blank_line.layout_box().measured_size.width, 0.0F);
        EXPECT_FLOAT_EQ(blank_line.layout_box().measured_size.height, 3.0F);
    }

    TEST_F(DocraftLoomBlankLineTest, LayoutStretchesToPageWidthAndAdvancesCursor)
    {
        loom::pipeline::DocraftLoomLayoutProcessor layout(200.0F);
        auto blank_line = std::make_shared<loom::nodes::DocraftLoomBlankLine>();
        blank_line->set_height(3.0F);
        blank_line->accept(*measure_);
        blank_line->accept(layout);

        EXPECT_FLOAT_EQ(blank_line->layout_box().frame.size.width, 200.0F);
        EXPECT_FLOAT_EQ(blank_line->layout_box().frame.size.height, 3.0F);

        auto next = std::make_shared<loom::nodes::DocraftLoomBlankLine>();
        next->accept(*measure_);
        next->accept(layout);
        // The cursor should have advanced by the first blank line's height (plus the
        // default top margin baked into a fresh cursor).
        EXPECT_FLOAT_EQ(next->layout_box().frame.position.y,
                        blank_line->layout_box().frame.position.y + 3.0F);
    }

    TEST_F(DocraftLoomBlankLineTest, LayoutWithDefaultConstructedProcessorHasZeroWidth)
    {
        loom::pipeline::DocraftLoomLayoutProcessor layout;
        auto blank_line = std::make_shared<loom::nodes::DocraftLoomBlankLine>();
        blank_line->accept(*measure_);
        blank_line->accept(layout);

        EXPECT_FLOAT_EQ(blank_line->layout_box().frame.size.width, 0.0F);
    }

    TEST_F(DocraftLoomBlankLineTest, AbsolutePositionDoesNotAdvanceCursor)
    {
        loom::pipeline::DocraftLoomLayoutProcessor layout(200.0F);
        auto blank_line = std::make_shared<loom::nodes::DocraftLoomBlankLine>();
        blank_line->set_position_mode(loom::nodes::DocraftPositionType::kAbsolute);
        blank_line->set_explicit_position({42.0F, 42.0F});
        blank_line->accept(*measure_);
        blank_line->accept(layout);

        EXPECT_FLOAT_EQ(blank_line->layout_box().frame.position.x, 42.0F);
        EXPECT_FLOAT_EQ(blank_line->layout_box().frame.position.y, 42.0F);

        auto next = std::make_shared<loom::nodes::DocraftLoomBlankLine>();
        next->accept(*measure_);
        next->accept(layout);
        // Untouched by the absolute node: still at the cursor's default top margin.
        EXPECT_FLOAT_EQ(next->layout_box().frame.position.y, 10.0F);
    }
} // namespace docraft::test