#include <gtest/gtest.h>

#include "docraft_document.h"
#include "model/docraft_list.h"
#include "model/docraft_rectangle.h"
#include "model/docraft_text.h"

namespace docraft::test {
    TEST(DocraftDocumentTest, GetByNameFindsRootAndChildNodes) {
        DocraftDocument document("Test Document");

        auto rect = std::make_shared<model::DocraftRectangle>();
        rect->set_name("target");

        auto list = std::make_shared<model::DocraftList>();
        list->set_name("container");

        auto child1 = std::make_shared<model::DocraftText>("Hello");
        child1->set_name("target");

        auto child2 = std::make_shared<model::DocraftText>("World");
        child2->set_name("other");

        list->add_child(child1);
        list->add_child(child2);

        document.add_node(rect);
        document.add_node(list);

        const auto matches = document.get_by_name("target");

        ASSERT_EQ(matches.size(), 2U);
        EXPECT_EQ(matches[0], rect);
        EXPECT_EQ(matches[1], child1);
    }

    TEST(DocraftDocumentTest, GetByNameReturnsEmptyWhenMissing) {
        DocraftDocument document("Test Document");

        auto rect = std::make_shared<model::DocraftRectangle>();
        rect->set_name("root");
        document.add_node(rect);

        const auto matches = document.get_by_name("missing");

        EXPECT_TRUE(matches.empty());
    }

    TEST(DocraftDocumentTest, TraverseDomVisitsNodesInPrePostOrder) {
        DocraftDocument document("Test Document");

        auto rect = std::make_shared<model::DocraftRectangle>();
        rect->set_name("root");

        auto list = std::make_shared<model::DocraftList>();
        list->set_name("container");

        auto child = std::make_shared<model::DocraftText>("Hello");
        child->set_name("child");
        list->add_child(child);

        document.add_node(rect);
        document.add_node(list);

        std::vector<std::string> visits;
        document.traverse_dom([&](const std::shared_ptr<model::DocraftNode> &node,
                                  DocraftDomTraverseOp op) {
            const auto tag = op == DocraftDomTraverseOp::kEnter ? "enter:" : "exit:";
            visits.emplace_back(tag + node->node_name());
        });

        const std::vector<std::string> expected = {
            "enter:root",
            "exit:root",
            "enter:container",
            "enter:child",
            "exit:child",
            "exit:container"
        };
        EXPECT_EQ(visits, expected);
    }

    TEST(DocraftDocumentTest, GetFirstAndLastByNameReturnExpectedNodes) {
        DocraftDocument document("Test Document");

        auto first = std::make_shared<model::DocraftRectangle>();
        first->set_name("target");

        auto list = std::make_shared<model::DocraftList>();
        list->set_name("container");

        auto middle = std::make_shared<model::DocraftText>("Middle");
        middle->set_name("target");
        list->add_child(middle);

        auto last = std::make_shared<model::DocraftText>("Last");
        last->set_name("target");

        document.add_node(first);
        document.add_node(list);
        document.add_node(last);

        EXPECT_EQ(document.get_first_by_name("target"), first);
        EXPECT_EQ(document.get_last_by_name("target"), last);
    }

    TEST(DocraftDocumentTest, GetByTypeFindsMatchingNodes) {
        DocraftDocument document("Test Document");

        auto rect1 = std::make_shared<model::DocraftRectangle>();
        auto rect2 = std::make_shared<model::DocraftRectangle>();
        auto list = std::make_shared<model::DocraftList>();
        auto child_text = std::make_shared<model::DocraftText>("Child");
        list->add_child(child_text);

        document.add_node(rect1);
        document.add_node(list);
        document.add_node(rect2);

        const auto rectangles = document.get_by_type<model::DocraftRectangle>();
        const auto texts = document.get_by_type<model::DocraftText>();

        ASSERT_EQ(rectangles.size(), 2U);
        EXPECT_EQ(rectangles[0], rect1);
        EXPECT_EQ(rectangles[1], rect2);

        ASSERT_EQ(texts.size(), 1U);
        EXPECT_EQ(texts[0], child_text);
    }
} // namespace docraft::test
