#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "docraft/exception/docraft_input_exceptions.h"
#include "docraft/loom/nodes/docraft_loom_list.h"
#include "docraft/loom/nodes/docraft_loom_rectangle.h"
#include "docraft/loom/nodes/docraft_loom_text.h"
#include "docraft/loom/pipeline/docraft_loom_layout_processor.h"
#include "docraft/loom/pipeline/docraft_loom_measure_processor.h"
#include "../../backend/docraft_mock_backend.h"

namespace docraft::test {
    using ::testing::Return;
    using ::testing::_;

    class DocraftLoomListTest : public ::testing::Test
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
        EXPECT_FLOAT_EQ(item->layout_box().frame.position.x, list->layout_box().frame.position.x + 56.0F);
        EXPECT_FLOAT_EQ(list->markers()[0].position.x, list->layout_box().frame.position.x);
    }

    TEST_F(DocraftLoomListTest, NonTextChildThrowsOnMeasure)
    {
        auto list = std::make_shared<loom::nodes::DocraftLoomList>();
        list->add_child(std::make_shared<loom::nodes::DocraftLoomRectangle>());
        EXPECT_THROW(list->accept(*measure_), exception::InvalidInputException);
    }
} // namespace docraft::test