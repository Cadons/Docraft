#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <algorithm>

#include "docraft/exception/docraft_input_exceptions.h"
#include "docraft/loom/nodes/docraft_loom_list.h"
#include "docraft/loom/nodes/docraft_loom_rectangle.h"
#include "docraft/loom/nodes/docraft_loom_text.h"
#include "docraft/loom/pipeline/docraft_loom_layout_processor.h"
#include "docraft/loom/pipeline/docraft_loom_measure_processor.h"
#include "../../backend/docraft_mock_backend.h"
#include "docraft/utils/docraft_loom_layout_box_test_access.h"

namespace docraft::test {
    using ::testing::Return;
    using ::testing::_;

    class DocraftLoomListTest : public ::testing::Test
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

        std::shared_ptr<loom::nodes::DocraftLoomList> make_list(loom::nodes::ListKind kind,
                                                                int item_count = 2)
        {
            auto list = std::make_shared<loom::nodes::DocraftLoomList>();
            list->set_kind(kind);
            for (int i = 0; i < item_count; ++i)
            {
                list->add_child(std::make_shared<loom::nodes::DocraftLoomText>("item"));
            }
            return list;
        }

        std::shared_ptr<backend::MockDocraftTextRenderingBackend> text_backend_;
        std::unique_ptr<loom::pipeline::DocraftLoomMeasureProcessor> measure_;
        std::unique_ptr<loom::pipeline::DocraftLoomLayoutProcessor> layout_;
    };

    TEST_F(DocraftLoomListTest, OrderedNumberMarkersAreOneTwoThree)
    {
        auto list = make_list(loom::nodes::ListKind::kOrdered, 2);
        list->set_ordered_style(loom::nodes::OrderedListStyle::kNumber);
        list->accept(*measure_);

        EXPECT_EQ(list->markers()[0].text, "1.");
        EXPECT_EQ(list->markers()[1].text, "2.");
    }

    TEST_F(DocraftLoomListTest, OrderedRomanMarkersAreCorrect)
    {
        auto list = make_list(loom::nodes::ListKind::kOrdered, 1);
        list->set_ordered_style(loom::nodes::OrderedListStyle::kRoman);
        EXPECT_EQ(list->marker_text_for_index(0), "I.");
        EXPECT_EQ(list->marker_text_for_index(3), "IV.");
        EXPECT_EQ(list->marker_text_for_index(8), "IX.");
        EXPECT_EQ(list->marker_text_for_index(39), "XL.");
        EXPECT_EQ(list->marker_text_for_index(1993), "MCMXCIV.");
    }

    TEST_F(DocraftLoomListTest, UnorderedDashMarkerIsDash)
    {
        auto list = make_list(loom::nodes::ListKind::kUnordered, 1);
        list->set_unordered_dot(loom::nodes::UnorderedListDot::kDash);
        list->accept(*measure_);
        EXPECT_EQ(list->markers()[0].text, "-");
    }

    TEST_F(DocraftLoomListTest, UnorderedStarMarkerIsStar)
    {
        auto list = make_list(loom::nodes::ListKind::kUnordered, 1);
        list->set_unordered_dot(loom::nodes::UnorderedListDot::kStar);
        list->accept(*measure_);
        EXPECT_EQ(list->markers()[0].text, "*");
    }

    TEST_F(DocraftLoomListTest, UnorderedCircleMarkerIsO)
    {
        auto list = make_list(loom::nodes::ListKind::kUnordered, 1);
        list->set_unordered_dot(loom::nodes::UnorderedListDot::kCircle);
        list->accept(*measure_);
        EXPECT_EQ(list->markers()[0].text, "o");
    }

    TEST_F(DocraftLoomListTest, BoxMarkerHasEmptyTextAndSizeFromFont)
    {
        auto list = std::make_shared<loom::nodes::DocraftLoomList>();
        list->set_kind(loom::nodes::ListKind::kUnordered);
        list->set_unordered_dot(loom::nodes::UnorderedListDot::kBox);
        auto item = std::make_shared<loom::nodes::DocraftLoomText>("item");
        item->set_font_size(20.0F);
        list->add_child(item);
        list->accept(*measure_);

        EXPECT_TRUE(list->markers()[0].text.empty());
        EXPECT_EQ(list->markers()[0].kind, loom::nodes::DocraftLoomList::Marker::Kind::kBox);
        EXPECT_FLOAT_EQ(list->markers()[0].width, 12.0F); // 20 * 0.6
    }

    TEST_F(DocraftLoomListTest, MeasureAggregatesWidthAndHeight)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillRepeatedly(Return(50.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillRepeatedly(Return(10.0F));

        auto list = make_list(loom::nodes::ListKind::kUnordered, 2);
        list->set_unordered_dot(loom::nodes::UnorderedListDot::kDash);
        list->accept(*measure_);

        // marker "-" measured width 50 (mocked) + gap 6 + item width 50 = 106
        EXPECT_FLOAT_EQ(list->layout_box().measured_size.width, 106.0F);
        EXPECT_FLOAT_EQ(list->layout_box().measured_size.height, 20.0F); // 10 + 10
    }

    TEST_F(DocraftLoomListTest, LayoutPositionsItemAfterMarkerAndGap)
    {
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillRepeatedly(Return(50.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillRepeatedly(Return(10.0F));

        auto list = make_list(loom::nodes::ListKind::kUnordered, 1);
        list->set_unordered_dot(loom::nodes::UnorderedListDot::kDash);
        list->accept(*measure_);
        list->accept(*layout_);

        auto item = std::dynamic_pointer_cast<loom::nodes::DocraftLoomText>(list->edit_child(0));
        // item_x (list x) + marker width 50 + gap 6
        EXPECT_FLOAT_EQ(item->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).position.x, list->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).position.x + 56.0F);
        EXPECT_FLOAT_EQ(list->markers()[0].position.x, list->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).position.x);
    }

    TEST_F(DocraftLoomListTest, BoxMarkerAlignsToFirstLineNotWholeWrappedBlock)
    {
        // Regression test: a box marker used to be centered against the item's *total*
        // wrapped height, so a multi-line item's marker floated at the block's midpoint
        // instead of sitting next to the first line. It must instead center against
        // just the first line's height.
        EXPECT_CALL(*text_backend_, measure_text_width(_, _, _)).WillRepeatedly(Return(50.0F));
        EXPECT_CALL(*text_backend_, measure_text_height(_, _)).WillRepeatedly(Return(10.0F));

        auto list = std::make_shared<loom::nodes::DocraftLoomList>();
        list->set_kind(loom::nodes::ListKind::kUnordered);
        list->set_unordered_dot(loom::nodes::UnorderedListDot::kBox);
        auto item = std::make_shared<loom::nodes::DocraftLoomText>("item");
        item->set_font_size(20.0F); // box marker width = 20 * 0.6 = 12
        list->add_child(item);
        list->accept(*measure_);

        // Simulate a 3-line wrapped item (single-line height 10, total block height 30)
        // without depending on the real word-wrap algorithm's line breaks.
        item->set_wrapped_lines({"line one", "line two", "line three"});
        item->edit_layout_box().measured_size.height = 30.0F;

        list->accept(*layout_);

        const float first_line_height = 10.0F;
        const float marker_width = 12.0F;
        const float expected_offset = std::max(0.0F, (first_line_height - marker_width) / 2.0F);
        EXPECT_FLOAT_EQ(list->markers()[0].position.y, list->layout_box().frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof()).position.y + expected_offset);
    }

    TEST_F(DocraftLoomListTest, NonTextChildThrowsOnMeasure)
    {
        auto list = std::make_shared<loom::nodes::DocraftLoomList>();
        list->add_child(std::make_shared<loom::nodes::DocraftLoomRectangle>());
        EXPECT_THROW(list->accept(*measure_), exception::InvalidInputException);
    }
} // namespace docraft::test