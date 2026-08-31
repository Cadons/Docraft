#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "docraft/loom/nodes/docraft_loom_layout_container.h"
#include "docraft/loom/nodes/docraft_loom_rectangle.h"
#include "docraft/loom/nodes/docraft_loom_text.h"
#include "docraft/loom/pipeline/docraft_loom_layout_processor.h"
#include "docraft/loom/pipeline/docraft_loom_measure_processor.h"
#include "docraft/loom/pipeline/docraft_loom_rendering_processor.h"
#include "docraft/utils/docraft_mock_rendering_backend.h"
#include "../../backend/docraft_mock_backend.h"
#include "docraft/utils/docraft_loom_layout_box_test_access.h"

namespace docraft::test {
    using ::testing::Return;
    using ::testing::_;

    class DocraftLoomRectangleTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            text_backend_ = std::make_shared<::testing::NiceMock<backend::MockDocraftTextRenderingBackend>>();
            ON_CALL(*text_backend_, measure_text_width(_, _, _)).WillByDefault(Return(50.0F));
            ON_CALL(*text_backend_, measure_text_height(_, _)).WillByDefault(Return(10.0F));
            measure_ = std::make_unique<loom::pipeline::DocraftLoomMeasureProcessor>(text_backend_);
            layout_ = std::make_unique<loom::pipeline::DocraftLoomLayoutProcessor>();
        }

        std::shared_ptr<backend::MockDocraftTextRenderingBackend> text_backend_;
        std::unique_ptr<loom::pipeline::DocraftLoomMeasureProcessor> measure_;
        std::unique_ptr<loom::pipeline::DocraftLoomLayoutProcessor> layout_;
    };

    TEST_F(DocraftLoomRectangleTest, DefaultsToNonZeroPaddingSoContentDoesNotTouchBorder)
    {
        loom::nodes::DocraftLoomRectangle rect;
        EXPECT_FLOAT_EQ(rect.padding(), loom::nodes::DocraftLoomLayoutContainer::kDefaultPadding);
    }

    TEST_F(DocraftLoomRectangleTest, MeasureWithoutChildrenUsesExplicitDimensions)
    {
        loom::nodes::DocraftLoomRectangle rect;
        rect.set_width(120.0F);
        rect.set_height(60.0F);
        rect.accept(*measure_);

        EXPECT_FLOAT_EQ(rect.layout_box().measured_size.width, 120.0F);
        EXPECT_FLOAT_EQ(rect.layout_box().measured_size.height, 60.0F);
    }

    TEST_F(DocraftLoomRectangleTest, MeasureWithChildrenSumsHeightAndUsesMaxWidthPlusPadding)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _))
            .WillOnce(Return(40.0F))
            .WillOnce(Return(70.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _))
            .WillOnce(Return(10.0F))
            .WillOnce(Return(10.0F));

        auto rect = std::make_shared<loom::nodes::DocraftLoomRectangle>();
        rect->set_padding(5.0F);
        rect->add_child(std::make_shared<loom::nodes::DocraftLoomText>("a"));
        rect->add_child(std::make_shared<loom::nodes::DocraftLoomText>("b"));
        rect->accept(*measure_);

        EXPECT_FLOAT_EQ(rect->layout_box().measured_size.width, 80.0F); // 70 + 2*5
        EXPECT_FLOAT_EQ(rect->layout_box().measured_size.height, 34.0F); // (10+10) + default spacing(4) + 2*5
    }

    TEST_F(DocraftLoomRectangleTest, MeasureExplicitWidthOverridesChildDerivedWidth)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillOnce(Return(40.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillOnce(Return(10.0F));

        auto rect = std::make_shared<loom::nodes::DocraftLoomRectangle>();
        rect->set_width(200.0F);
        rect->add_child(std::make_shared<loom::nodes::DocraftLoomText>("a"));
        rect->accept(*measure_);

        EXPECT_FLOAT_EQ(rect->layout_box().measured_size.width, 200.0F);
    }

    TEST_F(DocraftLoomRectangleTest, LayoutOffsetsChildrenByPaddingAndAdvancesCursorByTotalHeight)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillRepeatedly(Return(50.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillRepeatedly(Return(10.0F));

        auto rect = std::make_shared<loom::nodes::DocraftLoomRectangle>();
        rect->set_padding(4.0F);
        auto t1 = std::make_shared<loom::nodes::DocraftLoomText>("a");
        rect->add_child(t1);
        rect->accept(*measure_);
        rect->accept(*layout_);

        EXPECT_FLOAT_EQ(t1->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).position.x, rect->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).position.x + 4.0F);
        EXPECT_FLOAT_EQ(t1->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).position.y, rect->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).position.y + 4.0F);

        auto next = std::make_shared<loom::nodes::DocraftLoomText>("next");
        next->accept(*measure_);
        next->accept(*layout_);
        EXPECT_FLOAT_EQ(next->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).position.y,
                        rect->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).position.y + rect->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).size.height);
    }

    TEST_F(DocraftLoomRectangleTest, AbsolutePositionOverridesCursor)
    {
        loom::nodes::DocraftLoomRectangle rect;
        rect.set_position_mode(loom::nodes::DocraftPositionType::kAbsolute);
        rect.set_explicit_position({80.0F, 90.0F});
        rect.set_width(10.0F);
        rect.set_height(10.0F);
        rect.accept(*measure_);
        rect.accept(*layout_);

        EXPECT_FLOAT_EQ(rect.layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).position.x, 80.0F);
        EXPECT_FLOAT_EQ(rect.layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).position.y, 90.0F);
    }

    TEST_F(DocraftLoomRectangleTest, RenderingClipsChildrenToRectangleBoundsBracketedBySaveRestore)
    {
        // Regression test: visit(DocraftLoomRectangle*) used to paint children with no
        // clip at all (unlike the equivalent visit(DocraftLoomCanvas*)), so a child whose
        // computed size exceeded the rectangle's own frame (e.g. a Text node whose own
        // explicit wrap_width overrides the width relayed by this rectangle) painted past
        // the rectangle's edges instead of being contained by it.
        utils::MockRenderingBackend backend;
        loom::pipeline::DocraftLoomRenderingProcessor rendering(&backend);

        auto rect = std::make_shared<loom::nodes::DocraftLoomRectangle>();
        rect->set_position_mode(loom::nodes::DocraftPositionType::kAbsolute);
        rect->set_explicit_position({.x = 10.0F, .y = 10.0F});
        rect->set_width(80.0F);
        rect->set_height(60.0F);
        auto child = std::make_shared<loom::nodes::DocraftLoomText>("overflowing text");
        rect->add_child(child);

        rect->accept(*measure_);
        rect->accept(*layout_);
        rect->accept(rendering);

        // Assert against the rectangle's own resolved frame (rather than the
        // width_/height_ set above) so this test only exercises clipping and stays
        // agnostic to how that frame is sized.
        const auto& frame = rect->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof());
        ASSERT_EQ(backend.clip_calls().size(), 1U);
        EXPECT_FLOAT_EQ(backend.clip_calls()[0].x, frame.position.x);
        EXPECT_FLOAT_EQ(backend.clip_calls()[0].y, frame.position.y);
        EXPECT_FLOAT_EQ(backend.clip_calls()[0].width, frame.size.width);
        EXPECT_FLOAT_EQ(backend.clip_calls()[0].height, frame.size.height);
    }

    TEST_F(DocraftLoomRectangleTest, DashedBorderStyleSendsTheDashPatternBeforeDrawing)
    {
        utils::MockRenderingBackend backend;
        loom::pipeline::DocraftLoomRenderingProcessor rendering(&backend);

        auto rect = std::make_shared<loom::nodes::DocraftLoomRectangle>();
        rect->set_width(80.0F);
        rect->set_height(60.0F);
        rect->edit_style().border_color = DocraftColor::fromRGB(0.0F, 0.0F, 0.0F, 1.0F);
        rect->edit_style().border_width = 2.0F;
        rect->edit_style().border_style = loom::nodes::DocraftLineStyle::kDashed;

        rect->accept(*measure_);
        rect->accept(*layout_);
        rect->accept(rendering);

        ASSERT_EQ(backend.dash_pattern_calls().size(), 1U);
        EXPECT_FALSE(backend.dash_pattern_calls()[0].pattern.empty());
    }

    // Bug #102 (follow-up to #99): DocraftLoomRectangle stacks its children
    // top-to-bottom the same way DocraftLoomVStack does, but never picked up #99's
    // cross-axis margin fix -- margin_left/right on a child was still silently
    // dropped instead of being honored as a per-child offset, same as VStack.
    TEST_F(DocraftLoomRectangleTest, HonorsChildMarginLeftAsCrossAxisOffset)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillRepeatedly(Return(50.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillRepeatedly(Return(10.0F));

        auto rect = std::make_shared<loom::nodes::DocraftLoomRectangle>();
        rect->set_padding(0.0F);
        auto plain = std::make_shared<loom::nodes::DocraftLoomText>("a");
        auto offset = std::make_shared<loom::nodes::DocraftLoomText>("b");
        offset->set_margin(0.0F, 0.0F, 0.0F, 40.0F); // left only
        rect->add_child(plain);
        rect->add_child(offset);

        rect->accept(*measure_);
        rect->accept(*layout_);

        const float plain_x = plain->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).position.x;
        const float offset_x = offset->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).position.x;
        EXPECT_FLOAT_EQ(offset_x - plain_x, 40.0F);
        // The rectangle must widen to still fit the offset child's full extent.
        EXPECT_FLOAT_EQ(rect->layout_box().measured_size.width, 90.0F); // 50 (text) + 40 (margin_left)
    }
} // namespace docraft::test