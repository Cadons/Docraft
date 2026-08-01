#include <gtest/gtest.h>

#include "docraft/loom/nodes/docraft_loom_canvas.h"
#include "docraft/loom/nodes/docraft_loom_line.h"
#include "docraft/loom/nodes/docraft_loom_rectangle.h"
#include "docraft/loom/pipeline/docraft_loom_layout_processor.h"
#include "docraft/loom/pipeline/docraft_loom_measure_processor.h"
#include "docraft/loom/pipeline/docraft_loom_pagination_processor.h"
#include "docraft/loom/pipeline/docraft_loom_rendering_processor.h"
#include "docraft/utils/docraft_mock_rendering_backend.h"

namespace docraft::test {
    class DocraftLoomCanvasTest : public ::testing::Test
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

    TEST_F(DocraftLoomCanvasTest, DefaultsToZeroPaddingSinceChildrenAreFreelyPositioned)
    {
        loom::nodes::DocraftLoomCanvas canvas;
        EXPECT_FLOAT_EQ(canvas.padding(), 0.0F);
    }

    TEST_F(DocraftLoomCanvasTest, MeasureUsesExplicitDimensionsRegardlessOfChildren)
    {
        auto canvas = std::make_shared<loom::nodes::DocraftLoomCanvas>();
        canvas->set_width(200.0F);
        canvas->set_height(150.0F);
        auto child = std::make_shared<loom::nodes::DocraftLoomLine>();
        child->set_start({.x = 0.0F, .y = 0.0F});
        child->set_end({.x = 500.0F, .y = 0.0F}); // would overflow a Rectangle-style auto-size
        canvas->add_child(child);
        canvas->accept(*measure_);

        EXPECT_FLOAT_EQ(canvas->layout_box().measured_size.width, 200.0F);
        EXPECT_FLOAT_EQ(canvas->layout_box().measured_size.height, 150.0F);
        // The child is still measured so it has a natural size for rendering geometry.
        EXPECT_FLOAT_EQ(child->layout_box().measured_size.width, 500.0F);
    }

    TEST_F(DocraftLoomCanvasTest, LayoutPlacesChildAtCanvasOriginWhenNoPositionGiven)
    {
        auto canvas = std::make_shared<loom::nodes::DocraftLoomCanvas>();
        canvas->set_width(200.0F);
        canvas->set_height(150.0F);
        auto child = std::make_shared<loom::nodes::DocraftLoomLine>();
        canvas->add_child(child);
        canvas->accept(*measure_);
        canvas->accept(*layout_);

        EXPECT_FLOAT_EQ(child->layout_box().frame.position.x, canvas->layout_box().frame.position.x);
        EXPECT_FLOAT_EQ(child->layout_box().frame.position.y, canvas->layout_box().frame.position.y);
    }

    TEST_F(DocraftLoomCanvasTest, LayoutPlacesChildRelativeToCanvasOrigin)
    {
        auto canvas = std::make_shared<loom::nodes::DocraftLoomCanvas>();
        canvas->set_position_mode(loom::nodes::DocraftPositionType::kAbsolute);
        canvas->set_explicit_position({.x = 20.0F, .y = 30.0F});
        canvas->set_width(200.0F);
        canvas->set_height(150.0F);

        auto child = std::make_shared<loom::nodes::DocraftLoomLine>();
        child->set_explicit_position({.x = 10.0F, .y = 20.0F});
        canvas->add_child(child);

        canvas->accept(*measure_);
        canvas->accept(*layout_);

        EXPECT_FLOAT_EQ(canvas->layout_box().frame.position.x, 20.0F);
        EXPECT_FLOAT_EQ(canvas->layout_box().frame.position.y, 30.0F);
        // 20 (canvas origin x) + 10 (child local x), 30 (canvas origin y) + 20 (child local y)
        EXPECT_FLOAT_EQ(child->layout_box().frame.position.x, 30.0F);
        EXPECT_FLOAT_EQ(child->layout_box().frame.position.y, 50.0F);
    }

    TEST_F(DocraftLoomCanvasTest, LayoutIsIdempotentAcrossRepeatedPasses)
    {
        // Regression test: visit(DocraftLoomCanvas*) used to permanently overwrite each
        // child's explicit_position()/position_mode() with an absolute page coordinate,
        // so a second layout pass over the same tree (e.g. DocraftLoomPdfCreator::create()
        // invoked again after set_page_format()) re-added the canvas origin to the
        // already-translated child position, silently doubling the offset.
        auto canvas = std::make_shared<loom::nodes::DocraftLoomCanvas>();
        canvas->set_position_mode(loom::nodes::DocraftPositionType::kAbsolute);
        canvas->set_explicit_position({.x = 20.0F, .y = 30.0F});
        canvas->set_width(200.0F);
        canvas->set_height(150.0F);

        auto child = std::make_shared<loom::nodes::DocraftLoomLine>();
        child->set_explicit_position({.x = 10.0F, .y = 20.0F});
        canvas->add_child(child);

        canvas->accept(*measure_);
        canvas->accept(*layout_);
        EXPECT_FLOAT_EQ(child->layout_box().frame.position.x, 30.0F);
        EXPECT_FLOAT_EQ(child->layout_box().frame.position.y, 50.0F);

        canvas->accept(*measure_);
        canvas->accept(*layout_);
        EXPECT_FLOAT_EQ(child->layout_box().frame.position.x, 30.0F);
        EXPECT_FLOAT_EQ(child->layout_box().frame.position.y, 50.0F);
    }

    TEST_F(DocraftLoomCanvasTest, LayoutComposesForNestedCanvas)
    {
        auto outer = std::make_shared<loom::nodes::DocraftLoomCanvas>();
        outer->set_position_mode(loom::nodes::DocraftPositionType::kAbsolute);
        outer->set_explicit_position({.x = 5.0F, .y = 5.0F});
        outer->set_width(300.0F);
        outer->set_height(300.0F);

        auto inner = std::make_shared<loom::nodes::DocraftLoomCanvas>();
        inner->set_explicit_position({.x = 10.0F, .y = 10.0F}); // relative to outer
        inner->set_width(100.0F);
        inner->set_height(100.0F);
        outer->add_child(inner);

        auto leaf = std::make_shared<loom::nodes::DocraftLoomLine>();
        leaf->set_explicit_position({.x = 1.0F, .y = 2.0F}); // relative to inner
        inner->add_child(leaf);

        outer->accept(*measure_);
        outer->accept(*layout_);

        EXPECT_FLOAT_EQ(inner->layout_box().frame.position.x, 15.0F); // 5 + 10
        EXPECT_FLOAT_EQ(inner->layout_box().frame.position.y, 15.0F);
        EXPECT_FLOAT_EQ(leaf->layout_box().frame.position.x, 16.0F); // 15 + 1
        EXPECT_FLOAT_EQ(leaf->layout_box().frame.position.y, 17.0F); // 15 + 2
    }

    TEST_F(DocraftLoomCanvasTest, LayoutAdvancesCursorPastCanvasBottomForNextSibling)
    {
        auto canvas = std::make_shared<loom::nodes::DocraftLoomCanvas>();
        canvas->set_width(200.0F);
        canvas->set_height(150.0F);
        canvas->accept(*measure_);
        canvas->accept(*layout_);

        auto next = std::make_shared<loom::nodes::DocraftLoomLine>();
        next->accept(*measure_);
        next->accept(*layout_);

        EXPECT_FLOAT_EQ(next->layout_box().frame.position.y,
                        canvas->layout_box().frame.position.y + canvas->layout_box().frame.size.height);
    }

    TEST_F(DocraftLoomCanvasTest, PaginationRecursesIntoChildrenWithoutSplitting)
    {
        loom::pipeline::DocraftLoomPaginationProcessor pagination;
        auto canvas = std::make_shared<loom::nodes::DocraftLoomCanvas>();
        canvas->set_width(200.0F);
        canvas->set_height(150.0F);
        auto child = std::make_shared<loom::nodes::DocraftLoomLine>();
        canvas->add_child(child);

        EXPECT_NO_THROW(canvas->accept(pagination));
    }

    TEST_F(DocraftLoomCanvasTest, RenderingClipsChildrenToCanvasBoundsBracketedBySaveRestore)
    {
        utils::MockRenderingBackend backend;
        loom::pipeline::DocraftLoomRenderingProcessor rendering(&backend);

        auto canvas = std::make_shared<loom::nodes::DocraftLoomCanvas>();
        canvas->set_position_mode(loom::nodes::DocraftPositionType::kAbsolute);
        canvas->set_explicit_position({.x = 10.0F, .y = 10.0F});
        canvas->set_width(80.0F);
        canvas->set_height(60.0F);
        auto child = std::make_shared<loom::nodes::DocraftLoomLine>();
        canvas->add_child(child);

        canvas->accept(*measure_);
        canvas->accept(*layout_);
        canvas->accept(rendering);

        ASSERT_EQ(backend.clip_calls().size(), 1U);
        EXPECT_FLOAT_EQ(backend.clip_calls()[0].x, 10.0F);
        EXPECT_FLOAT_EQ(backend.clip_calls()[0].y, 10.0F);
        EXPECT_FLOAT_EQ(backend.clip_calls()[0].width, 80.0F);
        EXPECT_FLOAT_EQ(backend.clip_calls()[0].height, 60.0F);
    }

    TEST_F(DocraftLoomCanvasTest, RenderingDrawsDiagonalLineChildAtItsActualEndpoints)
    {
        // Regression test: a Line's own geometry used to be discarded at render time in
        // favor of a horizontal segment derived only from measured width, so a vertical
        // or diagonal Line placed inside a Canvas (a natural, expected use case for
        // free-form graphics) rendered as an invisible/wrong segment.
        utils::MockRenderingBackend backend;
        loom::pipeline::DocraftLoomRenderingProcessor rendering(&backend);

        auto canvas = std::make_shared<loom::nodes::DocraftLoomCanvas>();
        canvas->set_width(100.0F);
        canvas->set_height(100.0F);
        auto diagonal = std::make_shared<loom::nodes::DocraftLoomLine>();
        diagonal->set_start({.x = 0.0F, .y = 0.0F});
        diagonal->set_end({.x = 30.0F, .y = 40.0F});
        diagonal->set_explicit_position({.x = 5.0F, .y = 5.0F}); // canvas-local
        canvas->add_child(diagonal);

        canvas->accept(*measure_);
        canvas->accept(*layout_);
        canvas->accept(rendering);

        ASSERT_EQ(backend.draw_line_calls().size(), 1U);
        const auto& call = backend.draw_line_calls()[0];
        EXPECT_FLOAT_EQ(call.x1, canvas->layout_box().frame.position.x + 5.0F);
        EXPECT_FLOAT_EQ(call.y1, canvas->layout_box().frame.position.y + 5.0F);
        EXPECT_FLOAT_EQ(call.x2, canvas->layout_box().frame.position.x + 5.0F + 30.0F);
        EXPECT_FLOAT_EQ(call.y2, canvas->layout_box().frame.position.y + 5.0F + 40.0F);
    }
} // namespace docraft::test
