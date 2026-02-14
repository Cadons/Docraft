#include <gtest/gtest.h>

#include "layout/docraft_layout_engine.h"
#include "model/docraft_list.h"
#include "model/docraft_text.h"

namespace docraft::test::model {
    TEST(DocraftListTest, OrderedListNumbersAreApplied) {
        auto list = std::make_shared<docraft::model::DocraftList>();
        list->set_kind(docraft::model::ListKind::kOrdered);
        list->set_ordered_style(docraft::model::OrderedListStyle::kNumber);

        auto item1 = std::make_shared<docraft::model::DocraftText>();
        item1->set_text("Alpha");
        list->add_child(item1);

        auto item2 = std::make_shared<docraft::model::DocraftText>();
        item2->set_text("Beta");
        list->add_child(item2);

        EXPECT_EQ(item1->text(), "Alpha");
        EXPECT_EQ(item2->text(), "Beta");
    }

    TEST(DocraftListTest, OrderedListRomanIsApplied) {
        auto list = std::make_shared<docraft::model::DocraftList>();
        list->set_kind(docraft::model::ListKind::kOrdered);
        list->set_ordered_style(docraft::model::OrderedListStyle::kRoman);

        auto item1 = std::make_shared<docraft::model::DocraftText>();
        item1->set_text("First");
        list->add_child(item1);

        auto item2 = std::make_shared<docraft::model::DocraftText>();
        item2->set_text("Second");
        list->add_child(item2);

        EXPECT_EQ(item1->text(), "First");
        EXPECT_EQ(item2->text(), "Second");
    }

    TEST(DocraftListTest, UnorderedListDotsAreApplied) {
        auto list = std::make_shared<docraft::model::DocraftList>();
        list->set_kind(docraft::model::ListKind::kUnordered);
        list->set_unordered_dot(docraft::model::UnorderedListDot::kDash);

        auto item1 = std::make_shared<docraft::model::DocraftText>();
        item1->set_text("Alpha");
        list->add_child(item1);

        auto item2 = std::make_shared<docraft::model::DocraftText>();
        item2->set_text("Beta");
        list->add_child(item2);

        EXPECT_EQ(item1->text(), "Alpha");
        EXPECT_EQ(item2->text(), "Beta");
    }

    TEST(DocraftListTest, ListLayoutProducesHeight) {
        auto context = std::make_shared<docraft::DocraftDocumentContext>();
        docraft::layout::DocraftLayoutEngine engine(context);

        auto list = std::make_shared<docraft::model::DocraftList>();
        list->set_kind(docraft::model::ListKind::kUnordered);
        list->set_unordered_dot(docraft::model::UnorderedListDot::kStar);

        auto item1 = std::make_shared<docraft::model::DocraftText>();
        item1->set_text("Alpha");
        list->add_child(item1);

        auto item2 = std::make_shared<docraft::model::DocraftText>();
        item2->set_text("Beta");
        list->add_child(item2);

        const auto box = engine.compute_layout(list);

        EXPECT_GT(box.height(), 0.0F);
        EXPECT_GT(box.width(), 0.0F);
        EXPECT_GT(list->height(), 0.0F);
        EXPECT_GT(list->width(), 0.0F);
        ASSERT_EQ(list->markers().size(), 2U);
        EXPECT_EQ(list->markers()[0].text, "*");
        EXPECT_EQ(list->markers()[1].text, "*");
    }

    TEST(DocraftListTest, BoxChecklistMarkerIsConfigured) {
        auto context = std::make_shared<docraft::DocraftDocumentContext>();
        docraft::layout::DocraftLayoutEngine engine(context);

        auto list = std::make_shared<docraft::model::DocraftList>();
        list->set_kind(docraft::model::ListKind::kUnordered);
        list->set_unordered_dot(docraft::model::UnorderedListDot::kBox);

        auto item1 = std::make_shared<docraft::model::DocraftText>();
        item1->set_text("Alpha");
        list->add_child(item1);

        (void)engine.compute_layout(list);

        ASSERT_EQ(list->markers().size(), 1U);
        EXPECT_EQ(list->markers()[0].kind, docraft::model::DocraftList::Marker::Kind::kBox);
        EXPECT_GT(list->markers()[0].size, 0.0F);
    }
} // namespace docraft::test::model
