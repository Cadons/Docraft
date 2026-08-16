#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "docraft/docraft_color.h"
#include "docraft/loom/nodes/docraft_loom_hstack.h"
#include "docraft/loom/nodes/docraft_loom_line.h"
#include "docraft/loom/nodes/docraft_loom_text.h"
#include "docraft/loom/nodes/docraft_loom_vstack.h"
#include "docraft/loom/pipeline/docraft_loom_layout_processor.h"
#include "docraft/loom/pipeline/docraft_loom_measure_processor.h"
#include "docraft/loom/pipeline/docraft_loom_rendering_processor.h"
#include "docraft/utils/docraft_mock_rendering_backend.h"
#include "../../backend/docraft_mock_backend.h"

namespace docraft::test {
    using ::testing::Return;
    using ::testing::_;

    class DocraftLoomStackNodesTest : public ::testing::Test
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

        std::shared_ptr<loom::nodes::DocraftLoomText> make_text(const std::string& content = "hello")
        {
            auto t = std::make_shared<loom::nodes::DocraftLoomText>(content);
            t->set_font_size(12.0F);
            return t;
        }

        std::shared_ptr<backend::MockDocraftTextRenderingBackend> text_backend_;
        std::unique_ptr<loom::pipeline::DocraftLoomMeasureProcessor> measure_;
        std::unique_ptr<loom::pipeline::DocraftLoomLayoutProcessor> layout_;
    };

    // ── VStack measure ──────────────────────────────────────────────────────────

    TEST_F(DocraftLoomStackNodesTest, VStack_MeasureEmptyStack)
    {
        loom::nodes::DocraftLoomVStack vstack;
        vstack.accept(*measure_);
        EXPECT_FLOAT_EQ(vstack.layout_box().measured_size.width, 0.0F);
        EXPECT_FLOAT_EQ(vstack.layout_box().measured_size.height, 0.0F);
    }

    TEST_F(DocraftLoomStackNodesTest, VStack_MeasureSingleChild)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillOnce(Return(80.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillOnce(Return(14.0F));

        auto vstack = std::make_shared<loom::nodes::DocraftLoomVStack>();
        vstack->set_padding(0.0F); // isolate stacking math from the container's own default padding
        vstack->add_child(make_text("single"));
        vstack->accept(*measure_);

        EXPECT_FLOAT_EQ(vstack->layout_box().measured_size.width, 80.0F);
        EXPECT_FLOAT_EQ(vstack->layout_box().measured_size.height, 14.0F);
    }

    TEST_F(DocraftLoomStackNodesTest, VStack_MeasureMultipleChildrenWithSpacing)
    {
        // two children: width 60/40, height 10/10, spacing 5
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _))
        .WillOnce(Return(60.0F))
        .WillOnce(Return(40.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _))
        .WillOnce(Return(10.0F))
        .WillOnce(Return(10.0F));

        auto vstack = std::make_shared<loom::nodes::DocraftLoomVStack>();
        vstack->set_padding(0.0F); // isolate stacking math from the container's own default padding
        vstack->set_spacing(5.0F);
        vstack->add_child(make_text("a"));
        vstack->add_child(make_text("b"));
        vstack->accept(*measure_);

        EXPECT_FLOAT_EQ(vstack->layout_box().measured_size.width, 60.0F);
        EXPECT_FLOAT_EQ(vstack->layout_box().measured_size.height, 25.0F); // 10 + 5 + 10
    }

    TEST_F(DocraftLoomStackNodesTest, VStack_MeasureUsesMaxWidth)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _))
        .WillOnce(Return(30.0F)) //first call
        .WillOnce(Return(120.0F)) //second call
        .WillOnce(Return(50.0F)); //third call
        EXPECT_CALL(*text_backend_, measure_text_height(_, _))
        .WillRepeatedly(Return(10.0F));

        auto vstack = std::make_shared<loom::nodes::DocraftLoomVStack>();
        vstack->set_padding(0.0F); // isolate stacking math from the container's own default padding
        vstack->add_child(make_text("narrow"));
        vstack->add_child(make_text("very wide text"));
        vstack->add_child(make_text("medium"));
        vstack->accept(*measure_);

        EXPECT_FLOAT_EQ(vstack->layout_box().measured_size.width, 120.0F);
        EXPECT_FLOAT_EQ(vstack->layout_box().measured_size.height, 54.0F); // 3 * 10, no spacing
    }

    TEST_F(DocraftLoomStackNodesTest, VStack_MeasureExplicitHeightOverridesDerivedSum)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillRepeatedly(Return(50.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillRepeatedly(Return(10.0F));

        auto vstack = std::make_shared<loom::nodes::DocraftLoomVStack>();
        vstack->set_padding(0.0F); // isolate stacking math from the container's own default padding
        vstack->set_spacing(0.0F);
        vstack->set_height(200.0F);
        vstack->add_child(make_text("a"));
        vstack->add_child(make_text("b"));
        vstack->accept(*measure_);

        // Explicit height() wins outright over the derived sum of natural child
        // heights (2 * 10 = 20), mirroring DocraftLoomRectangle's own explicit width().
        EXPECT_FLOAT_EQ(vstack->layout_box().measured_size.height, 200.0F);
    }

    // ── HStack measure ──────────────────────────────────────────────────────────

    TEST_F(DocraftLoomStackNodesTest, HStack_MeasureEmptyStack)
    {
        loom::nodes::DocraftLoomHStack hstack;
        hstack.accept(*measure_);
        EXPECT_FLOAT_EQ(hstack.layout_box().measured_size.width, 0.0F);
        EXPECT_FLOAT_EQ(hstack.layout_box().measured_size.height, 0.0F);
    }

    TEST_F(DocraftLoomStackNodesTest, HStack_MeasureSingleChild)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillOnce(Return(80.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillOnce(Return(14.0F));

        auto hstack = std::make_shared<loom::nodes::DocraftLoomHStack>();
        hstack->set_padding(0.0F); // isolate stacking math from the container's own default padding
        hstack->add_child(make_text("single"));
        hstack->accept(*measure_);

        EXPECT_FLOAT_EQ(hstack->layout_box().measured_size.width, 80.0F);
        EXPECT_FLOAT_EQ(hstack->layout_box().measured_size.height, 14.0F);
    }

    TEST_F(DocraftLoomStackNodesTest, HStack_MeasureMultipleChildrenWithSpacing)
    {
        // two children: width 60/40, height 10/16, spacing 8
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _))
        .WillOnce(Return(60.0F))
        .WillOnce(Return(40.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _))
        .WillOnce(Return(10.0F))
        .WillOnce(Return(16.0F));

        auto hstack = std::make_shared<loom::nodes::DocraftLoomHStack>();
        hstack->set_padding(0.0F); // isolate stacking math from the container's own default padding
        hstack->set_spacing(8.0F);
        hstack->add_child(make_text("a"));
        hstack->add_child(make_text("b"));
        hstack->accept(*measure_);

        EXPECT_FLOAT_EQ(hstack->layout_box().measured_size.width, 108.0F); // 60 + 8 + 40
        EXPECT_FLOAT_EQ(hstack->layout_box().measured_size.height, 16.0F);
    }

    TEST_F(DocraftLoomStackNodesTest, HStack_MeasureUsesMaxHeight)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _))
        .WillRepeatedly(Return(50.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _))
        .WillOnce(Return(8.0F))
        .WillOnce(Return(20.0F))
        .WillOnce(Return(12.0F));

        auto hstack = std::make_shared<loom::nodes::DocraftLoomHStack>();
        hstack->set_padding(0.0F); // isolate stacking math from the container's own default padding
        hstack->add_child(make_text("a"));
        hstack->add_child(make_text("b"));
        hstack->add_child(make_text("c"));
        hstack->accept(*measure_);

        EXPECT_FLOAT_EQ(hstack->layout_box().measured_size.height, 20.0F);
        EXPECT_FLOAT_EQ(hstack->layout_box().measured_size.width, 150.0F); // 3 * 50, no spacing
    }

    // ── VStack layout ───────────────────────────────────────────────────────────

    TEST_F(DocraftLoomStackNodesTest, VStack_LayoutChildrenStackedVertically)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillRepeatedly(Return(50.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillRepeatedly(Return(10.0F));

        auto vstack = std::make_shared<loom::nodes::DocraftLoomVStack>();
        vstack->set_padding(0.0F); // isolate stacking math from the container's own default padding
        vstack->set_spacing(4.0F);
        auto t1 = make_text("a");
        auto t2 = make_text("b");
        vstack->add_child(t1);
        vstack->add_child(t2);

        vstack->accept(*measure_);
        vstack->accept(*layout_);

        // VStack placed at cursor origin (0, 10 = default top margin)
        EXPECT_FLOAT_EQ(t1->layout_box().frame.position.y, 10.0F);
        EXPECT_FLOAT_EQ(t2->layout_box().frame.position.y, 24.0F); // 10 + 10 + 4
    }

    TEST_F(DocraftLoomStackNodesTest, VStack_LayoutChildrenShareSameX)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillRepeatedly(Return(50.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillRepeatedly(Return(10.0F));

        auto vstack = std::make_shared<loom::nodes::DocraftLoomVStack>();
        auto t1 = make_text("a");
        auto t2 = make_text("b");
        vstack->add_child(t1);
        vstack->add_child(t2);

        vstack->accept(*measure_);
        vstack->accept(*layout_);

        EXPECT_FLOAT_EQ(t1->layout_box().frame.position.x, t2->layout_box().frame.position.x);
    }

    // Bug #75: `weight` on a child of a vertical `<Layout>` used to be silently
    // dropped. A VStack now divides its own explicit height() among weighted children,
    // mirroring DocraftLoomHStack's weighted-width distribution but on the vertical axis.
    TEST_F(DocraftLoomStackNodesTest, VStack_LayoutWeightedHeightDistributesAmongChildren)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillRepeatedly(Return(50.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillRepeatedly(Return(10.0F));

        auto vstack = std::make_shared<loom::nodes::DocraftLoomVStack>();
        vstack->set_padding(0.0F); // isolate weighted-height math from the container's own default padding
        vstack->set_spacing(0.0F);
        vstack->set_height(200.0F);
        vstack->set_weights({1.0F, 3.0F});
        auto t1 = make_text("a");
        auto t2 = make_text("b");
        vstack->add_child(t1);
        vstack->add_child(t2);

        vstack->accept(*measure_);
        vstack->accept(*layout_);

        EXPECT_FLOAT_EQ(t1->layout_box().frame.size.height, 50.0F); // 200 * 1/4
        EXPECT_FLOAT_EQ(t2->layout_box().frame.size.height, 150.0F); // 200 * 3/4
        // 10 = the layout processor's default page top-margin cursor start (see
        // VStack_LayoutChildrenStackedVertically above).
        EXPECT_FLOAT_EQ(t1->layout_box().frame.position.y, 10.0F);
        EXPECT_FLOAT_EQ(t2->layout_box().frame.position.y, 60.0F); // 10 + 50
    }

    // A weighted child is never squeezed shorter than its own natural height, even if
    // that makes the resolved heights sum past the explicit height() budget -- mirrors
    // HStack's natural-width floor.
    TEST_F(DocraftLoomStackNodesTest, VStack_LayoutWeightedHeightNeverShrinksBelowNaturalHeight)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillRepeatedly(Return(50.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillRepeatedly(Return(10.0F));

        auto vstack = std::make_shared<loom::nodes::DocraftLoomVStack>();
        vstack->set_padding(0.0F);
        vstack->set_spacing(0.0F);
        vstack->set_height(12.0F); // smaller than the 20pt sum of natural heights
        vstack->set_weights({1.0F, 1.0F});
        auto t1 = make_text("a");
        auto t2 = make_text("b");
        vstack->add_child(t1);
        vstack->add_child(t2);

        vstack->accept(*measure_);
        vstack->accept(*layout_);

        EXPECT_FLOAT_EQ(t1->layout_box().frame.size.height, 10.0F);
        EXPECT_FLOAT_EQ(t2->layout_box().frame.size.height, 10.0F);

        // Bug: the container's own box used to stay stuck at the too-small declared
        // height() (12) even though its children's natural-height floor pushed the
        // real content down to 20 -- both frame.size (what Pagination advances by) and
        // measured_size (what every other container's own child-advance loop reads
        // while still inside Layout, e.g. an outer VStack placing this one among its
        // siblings) must grow to match, or whatever comes after this VStack gets
        // positioned on top of its overflowed content instead of below it.
        EXPECT_FLOAT_EQ(vstack->layout_box().frame.size.height, 20.0F);
        EXPECT_FLOAT_EQ(vstack->layout_box().measured_size.height, 20.0F);
    }

    // Regression test for the exact end-to-end symptom: an outer VStack (e.g. <Body>)
    // sequencing an inner VStack with a too-small explicit height() followed by a
    // sibling Text. The sibling must land below the inner VStack's real (overflowed)
    // content, not below its declared-but-too-small height.
    TEST_F(DocraftLoomStackNodesTest, VStack_OuterSiblingAdvancesPastInnerVStacksOverflowedContent)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillRepeatedly(Return(50.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillRepeatedly(Return(10.0F));

        auto inner = std::make_shared<loom::nodes::DocraftLoomVStack>();
        inner->set_padding(0.0F);
        inner->set_spacing(0.0F);
        inner->set_height(12.0F); // smaller than the 20pt sum of its children's natural heights
        inner->set_weights({1.0F, 1.0F});
        inner->add_child(make_text("a"));
        inner->add_child(make_text("b"));

        auto outer = std::make_shared<loom::nodes::DocraftLoomVStack>();
        outer->set_padding(0.0F);
        outer->set_spacing(0.0F);
        outer->add_child(inner);
        auto sibling = make_text("after");
        outer->add_child(sibling);

        outer->accept(*measure_);
        outer->accept(*layout_);

        // 10 = the layout processor's default page top-margin cursor start (see
        // VStack_LayoutChildrenStackedVertically above).
        EXPECT_FLOAT_EQ(sibling->layout_box().frame.position.y, 30.0F); // 10 + 20, not 10 + 12
    }

    // Regression guard for the opt-in gate: weights() alone, without an explicit
    // height(), has nothing well-defined to divide (a VStack has no ambient "page
    // height" budget the way HStack always has a page width) -- so it must keep
    // today's shrink-to-fit behavior unchanged.
    TEST_F(DocraftLoomStackNodesTest, VStack_WeightsWithoutExplicitHeightKeepsShrinkToFit)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillRepeatedly(Return(50.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillRepeatedly(Return(10.0F));

        auto vstack = std::make_shared<loom::nodes::DocraftLoomVStack>();
        vstack->set_padding(0.0F);
        vstack->set_spacing(0.0F);
        vstack->set_weights({1.0F, 3.0F}); // no set_height() call -- stays 0.0F (unset)
        auto t1 = make_text("a");
        auto t2 = make_text("b");
        vstack->add_child(t1);
        vstack->add_child(t2);

        vstack->accept(*measure_);
        vstack->accept(*layout_);

        EXPECT_FLOAT_EQ(t1->layout_box().frame.size.height, 10.0F);
        EXPECT_FLOAT_EQ(t2->layout_box().frame.size.height, 10.0F);
        EXPECT_FLOAT_EQ(t2->layout_box().frame.position.y, 20.0F); // 10 (default page top margin) + 10
        EXPECT_FLOAT_EQ(vstack->layout_box().measured_size.height, 20.0F);
    }

    // ── HStack layout ───────────────────────────────────────────────────────────

    TEST_F(DocraftLoomStackNodesTest, HStack_LayoutChildrenArrangedHorizontally)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillRepeatedly(Return(50.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillRepeatedly(Return(10.0F));

        auto hstack = std::make_shared<loom::nodes::DocraftLoomHStack>();
        hstack->set_padding(0.0F); // isolate stacking math from the container's own default padding
        hstack->set_spacing(6.0F);
        auto t1 = make_text("a");
        auto t2 = make_text("b");
        hstack->add_child(t1);
        hstack->add_child(t2);

        hstack->accept(*measure_);
        hstack->accept(*layout_);

        EXPECT_FLOAT_EQ(t1->layout_box().frame.position.x, 0.0F);
        EXPECT_FLOAT_EQ(t2->layout_box().frame.position.x, 56.0F); // 50 + 6
    }

    TEST_F(DocraftLoomStackNodesTest, HStack_LayoutChildrenShareSameY)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillRepeatedly(Return(50.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillRepeatedly(Return(10.0F));

        auto hstack = std::make_shared<loom::nodes::DocraftLoomHStack>();
        auto t1 = make_text("a");
        auto t2 = make_text("b");
        hstack->add_child(t1);
        hstack->add_child(t2);

        hstack->accept(*measure_);
        hstack->accept(*layout_);

        EXPECT_FLOAT_EQ(t1->layout_box().frame.position.y, t2->layout_box().frame.position.y);
    }

    // ── Spacing property ────────────────────────────────────────────────────────

    TEST_F(DocraftLoomStackNodesTest, VStack_DefaultSpacingIsZero)
    {
        loom::nodes::DocraftLoomVStack vstack;
        EXPECT_FLOAT_EQ(vstack.spacing(), 12.0F);
    }

    TEST_F(DocraftLoomStackNodesTest, VStack_SetSpacing)
    {
        loom::nodes::DocraftLoomVStack vstack;
        vstack.set_spacing(12.5F);
        EXPECT_FLOAT_EQ(vstack.spacing(), 12.5F);
    }

    TEST_F(DocraftLoomStackNodesTest, HStack_DefaultSpacingIsZero)
    {
        loom::nodes::DocraftLoomHStack hstack;
        EXPECT_FLOAT_EQ(hstack.spacing(), 0.0F);
    }

    TEST_F(DocraftLoomStackNodesTest, HStack_SetSpacing)
    {
        loom::nodes::DocraftLoomHStack hstack;
        hstack.set_spacing(8.0F);
        EXPECT_FLOAT_EQ(hstack.spacing(), 8.0F);
    }

    // ── Absolute positioning ─────────────────────────────────────────────────────

    TEST_F(DocraftLoomStackNodesTest, VStack_AbsolutePositionOverridesCursor)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillRepeatedly(Return(50.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillRepeatedly(Return(10.0F));

        auto vstack = std::make_shared<loom::nodes::DocraftLoomVStack>();
        vstack->set_position_mode(loom::nodes::DocraftPositionType::kAbsolute);
        vstack->set_explicit_position({50.0F, 50.0F});
        vstack->add_child(make_text("a"));

        vstack->accept(*measure_);
        vstack->accept(*layout_);

        EXPECT_FLOAT_EQ(vstack->layout_box().frame.position.x, 50.0F);
        EXPECT_FLOAT_EQ(vstack->layout_box().frame.position.y, 50.0F);
    }

    TEST_F(DocraftLoomStackNodesTest, VStack_AbsolutePositionDoesNotAdvanceCursorForNextSibling)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillRepeatedly(Return(50.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillRepeatedly(Return(10.0F));

        auto absolute_vstack = std::make_shared<loom::nodes::DocraftLoomVStack>();
        absolute_vstack->set_position_mode(loom::nodes::DocraftPositionType::kAbsolute);
        absolute_vstack->set_explicit_position({200.0F, 200.0F});
        absolute_vstack->add_child(make_text("a"));
        absolute_vstack->add_child(make_text("b"));

        auto next_sibling = make_text("sibling");

        absolute_vstack->accept(*measure_);
        absolute_vstack->accept(*layout_);
        next_sibling->accept(*measure_);
        next_sibling->accept(*layout_);

        // The absolute subtree must be fully out of flow: the next sibling is placed as if
        // the absolutely-positioned VStack (and its two children) had never been visited.
        EXPECT_FLOAT_EQ(next_sibling->layout_box().frame.position.x, 0.0F);
        EXPECT_FLOAT_EQ(next_sibling->layout_box().frame.position.y, 10.0F);
    }

    // Regression tests: an HStack/VStack with no background/border (a purely
    // structural <Layout>, the common case in real .craft documents) used to still
    // reserve 2x kDefaultPadding of invisible space around its content -- inflating
    // its measured/laid-out height with a buffer nothing ever paints into. Stacked
    // across a few nested unstyled Layouts, that invisible padding could add up to
    // enough height that Pagination decided a block didn't fit anywhere and fell
    // back to accepting it as unsplittable overflow, even though the content it
    // actually painted fit comfortably on a fresh page. effective_padding() fixes
    // this by only reserving the default inset once the container has something to
    // keep content clear of (a visible background or border).

    TEST_F(DocraftLoomStackNodesTest, VStack_NoDefaultPaddingWithoutBackgroundOrBorder)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillRepeatedly(Return(50.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillRepeatedly(Return(10.0F));

        auto vstack = std::make_shared<loom::nodes::DocraftLoomVStack>();
        vstack->add_child(make_text("a"));

        vstack->accept(*measure_);

        // No 2x kDefaultPadding (20pt) inflation: just the one 10pt-tall text line.
        EXPECT_FLOAT_EQ(vstack->layout_box().measured_size.height, 10.0F);
    }

    TEST_F(DocraftLoomStackNodesTest, VStack_DefaultPaddingAppliesWhenBackgroundColorSet)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillRepeatedly(Return(50.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillRepeatedly(Return(10.0F));

        auto vstack = std::make_shared<loom::nodes::DocraftLoomVStack>();
        vstack->edit_style().background_color = DocraftColor(ColorName::kWhite);
        vstack->add_child(make_text("a"));

        vstack->accept(*measure_);

        // A VStack that actually paints a background keeps its default 2x10pt inset,
        // so content doesn't sit flush against that painted edge.
        EXPECT_FLOAT_EQ(vstack->layout_box().measured_size.height, 30.0F);
    }

    TEST_F(DocraftLoomStackNodesTest, VStack_DefaultPaddingAppliesWhenBorderSet)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillRepeatedly(Return(50.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillRepeatedly(Return(10.0F));

        auto vstack = std::make_shared<loom::nodes::DocraftLoomVStack>();
        vstack->edit_style().border_color = DocraftColor(ColorName::kBlack);
        vstack->edit_style().border_width = 1.0F;
        vstack->add_child(make_text("a"));

        vstack->accept(*measure_);

        EXPECT_FLOAT_EQ(vstack->layout_box().measured_size.height, 30.0F);
    }

    TEST_F(DocraftLoomStackNodesTest, VStack_ExplicitPaddingAlwaysHonoredEvenWithoutChrome)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillRepeatedly(Return(50.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillRepeatedly(Return(10.0F));

        auto vstack = std::make_shared<loom::nodes::DocraftLoomVStack>();
        vstack->set_padding(4.0F); // an author-chosen value, still no background/border
        vstack->add_child(make_text("a"));

        vstack->accept(*measure_);

        // An explicit padding is never zeroed out, chrome or not.
        EXPECT_FLOAT_EQ(vstack->layout_box().measured_size.height, 18.0F);
    }

    TEST_F(DocraftLoomStackNodesTest, HStack_NoDefaultPaddingWithoutBackgroundOrBorder)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillRepeatedly(Return(50.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillRepeatedly(Return(10.0F));

        auto hstack = std::make_shared<loom::nodes::DocraftLoomHStack>();
        hstack->add_child(make_text("a"));

        hstack->accept(*measure_);

        EXPECT_FLOAT_EQ(hstack->layout_box().measured_size.height, 10.0F);
    }

    TEST_F(DocraftLoomStackNodesTest, HStack_DefaultPaddingAppliesWhenBackgroundColorSet)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillRepeatedly(Return(50.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillRepeatedly(Return(10.0F));

        auto hstack = std::make_shared<loom::nodes::DocraftLoomHStack>();
        hstack->edit_style().background_color = DocraftColor(ColorName::kWhite);
        hstack->add_child(make_text("a"));

        hstack->accept(*measure_);

        EXPECT_FLOAT_EQ(hstack->layout_box().measured_size.height, 30.0F);
    }

    // End-to-end (Measure + Layout): a nested, unstyled VStack's children must be
    // positioned using zero padding in the *laid-out* frame too, not just in
    // measured_size -- Pagination's fits-on-this-page check reads frame.size, which
    // Layout computes independently of Measure.
    TEST_F(DocraftLoomStackNodesTest, VStack_LayoutFrameHeightMatchesZeroPaddingWithoutChrome)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillRepeatedly(Return(50.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillRepeatedly(Return(10.0F));

        auto vstack = std::make_shared<loom::nodes::DocraftLoomVStack>();
        vstack->set_spacing(0.0F); // isolate padding from VStack's own default inter-child spacing
        vstack->add_child(make_text("a"));
        vstack->add_child(make_text("b"));

        vstack->accept(*measure_);
        vstack->accept(*layout_);

        EXPECT_FLOAT_EQ(vstack->layout_box().frame.size.height, 20.0F);
    }

    // ── z-index paint order ─────────────────────────────────────────────────────

    TEST_F(DocraftLoomStackNodesTest, VStack_RenderingRespectsZIndexOrderAmongSiblings)
    {
        utils::MockRenderingBackend backend;
        loom::pipeline::DocraftLoomMeasureProcessor measure(nullptr);
        loom::pipeline::DocraftLoomLayoutProcessor layout;
        loom::pipeline::DocraftLoomRenderingProcessor rendering(&backend);

        auto vstack = std::make_shared<loom::nodes::DocraftLoomVStack>();
        vstack->set_padding(0.0F);

        // Declared first but with the higher z_index -- should still paint last (on top).
        auto line_a = std::make_shared<loom::nodes::DocraftLoomLine>();
        line_a->set_start({.x = 0.0F, .y = 0.0F});
        line_a->set_end({.x = 30.0F, .y = 0.0F});
        line_a->set_z_index(5);
        vstack->add_child(line_a);

        // Declared second but with the lower z_index -- should paint first (underneath).
        auto line_b = std::make_shared<loom::nodes::DocraftLoomLine>();
        line_b->set_start({.x = 0.0F, .y = 0.0F});
        line_b->set_end({.x = 0.0F, .y = 30.0F});
        line_b->set_z_index(1);
        vstack->add_child(line_b);

        vstack->accept(measure);
        vstack->accept(layout);
        vstack->accept(rendering);

        ASSERT_EQ(backend.draw_line_calls().size(), 2U);
        EXPECT_FLOAT_EQ(backend.draw_line_calls()[0].x2 - backend.draw_line_calls()[0].x1, 0.0F);
        EXPECT_FLOAT_EQ(backend.draw_line_calls()[0].y2 - backend.draw_line_calls()[0].y1, 30.0F);
        EXPECT_FLOAT_EQ(backend.draw_line_calls()[1].x2 - backend.draw_line_calls()[1].x1, 30.0F);
        EXPECT_FLOAT_EQ(backend.draw_line_calls()[1].y2 - backend.draw_line_calls()[1].y1, 0.0F);
    }
} // namespace docraft::test
