#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "docraft/loom/nodes/docraft_loom_hstack.h"
#include "docraft/loom/nodes/docraft_loom_text.h"
#include "docraft/loom/nodes/docraft_loom_vstack.h"
#include "docraft/loom/pipeline/docraft_loom_layout_processor.h"
#include "docraft/loom/pipeline/docraft_loom_measure_processor.h"
#include "../../backend/docraft_mock_backend.h"

namespace docraft::test {
    using ::testing::Return;
    using ::testing::_;

    class DocraftLoomStackNodesTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            text_backend_ = std::make_shared<backend::MockDocraftTextRenderingBackend>();
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
        vstack->add_child(make_text("narrow"));
        vstack->add_child(make_text("very wide text"));
        vstack->add_child(make_text("medium"));
        vstack->accept(*measure_);

        EXPECT_FLOAT_EQ(vstack->layout_box().measured_size.width, 120.0F);
        EXPECT_FLOAT_EQ(vstack->layout_box().measured_size.height, 54.0F); // 3 * 10, no spacing
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

    // ── HStack layout ───────────────────────────────────────────────────────────

    TEST_F(DocraftLoomStackNodesTest, HStack_LayoutChildrenArrangedHorizontally)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillRepeatedly(Return(50.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillRepeatedly(Return(10.0F));

        auto hstack = std::make_shared<loom::nodes::DocraftLoomHStack>();
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
} // namespace docraft::test
