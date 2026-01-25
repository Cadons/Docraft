#include "layout/docraft_layout_engine.h"

#include <gtest/gtest.h>

#include "model/docraft_blank_line.h"
#include "model/docraft_layout.h"
#include "model/docraft_rectangle.h"
#include "model/docraft_text.h"

namespace docraft::test::layout {
    class DocraftLayoutEngineTest : public ::testing::Test {
    protected:
        void SetUp() override {
            context_ = std::make_shared<DocraftPDFContext>();
            engine_ = std::make_unique<docraft::layout::DocraftLayoutEngine>(context_);
        }

        void TearDown() override {
        }

        std::unique_ptr<docraft::layout::DocraftLayoutEngine>& engine() {
            return engine_;
        }
        std::shared_ptr<DocraftPDFContext> &context() {
            return context_;
        }

    private:
        std::unique_ptr<docraft::layout::DocraftLayoutEngine> engine_;
        std::shared_ptr<DocraftPDFContext> context_;
    };

    TEST_F(DocraftLayoutEngineTest, ComputeLayoutSingleNode) {
        auto& engine = this->engine();
        auto context = this->context();
        auto node = std::make_shared<docraft::model::DocraftRectangle>();
        node->set_width(100);
        node->set_height(50);
        auto layout = engine->compute_layout(node);
        EXPECT_EQ(layout.width(), 100);
        EXPECT_EQ(layout.height(), 50);
    }
    TEST_F(DocraftLayoutEngineTest, ComputeLayoutMultipleNodesVerticalLayout) {
        auto& engine = this->engine();
        auto context = this->context();
        auto layout_node = std::make_shared<docraft::model::DocraftLayout>();
        layout_node->set_orientation(docraft::model::LayoutOrientation::kVertical);

        auto child1 = std::make_shared<docraft::model::DocraftText>();
        child1->set_text("Hello World");
        layout_node->add_child(child1);
        auto child2 = std::make_shared<docraft::model::DocraftRectangle>();
        child2->set_width(150);
        child2->set_height(30);
        layout_node->add_child(child2);

        EXPECT_EQ(layout_node->children().size(), 2);
        auto layout = engine->compute_layout(layout_node);
        EXPECT_EQ(layout.width(), 150);
        EXPECT_NEAR(layout.height(), 44.40,0.0001); //fontSize*1.2 + 30=12*1.2 +30=44.4
    }
    TEST_F(DocraftLayoutEngineTest, ComputeLayoutMultipleNodesHorizontalLayout) {
        auto& engine = this->engine();
        auto context = this->context();
        auto layout_node = std::make_shared<docraft::model::DocraftLayout>();
        layout_node->set_orientation(docraft::model::LayoutOrientation::kHorizontal);

        auto child1 = std::make_shared<docraft::model::DocraftText>();
        child1->set_text("Hello");
        layout_node->add_child(child1);
        auto child2 = std::make_shared<docraft::model::DocraftRectangle>();
        child2->set_width(80);
        child2->set_height(40);
        layout_node->add_child(child2);

        EXPECT_EQ(layout_node->children().size(), 2);
        auto layout = engine->compute_layout(layout_node);
        EXPECT_NEAR(layout.width(), 107.335,0.001);// text width approx 48 for "Hello" at default font size
        EXPECT_EQ(layout.height(), 40);// max height between text and rectangle, in this case rectangle height
    }
    TEST_F(DocraftLayoutEngineTest, ComputeLayoutInsideAnotherLayout) {
        auto& engine = this->engine();
        auto context = this->context();
        auto outer_layout = std::make_shared<docraft::model::DocraftLayout>();
        outer_layout->set_orientation(docraft::model::LayoutOrientation::kVertical);

        auto inner_layout = std::make_shared<docraft::model::DocraftLayout>();
        inner_layout->set_orientation(docraft::model::LayoutOrientation::kHorizontal);

        auto child1 = std::make_shared<docraft::model::DocraftText>();
        child1->set_text("Inner");
        inner_layout->add_child(child1);
        auto child2 = std::make_shared<docraft::model::DocraftRectangle>();
        child2->set_width(60);
        child2->set_height(20);
        inner_layout->add_child(child2);

        outer_layout->add_child(inner_layout);
        auto child3 = std::make_shared<docraft::model::DocraftRectangle>();
        child3->set_width(200);
        child3->set_height(50);
        outer_layout->add_child(child3);

        EXPECT_EQ(outer_layout->children().size(), 2);
        auto layout = engine->compute_layout(outer_layout);
        EXPECT_EQ(layout.width(), 200); // max width between inner layout and rectangle
        EXPECT_NEAR(layout.height(), 70,0.001); // inner layout height + rectangle height
        //Test position for each child
        EXPECT_EQ(inner_layout->position().x, 0);
        EXPECT_EQ(inner_layout->position().y, 841); //default page height
        //Child 1 position
        EXPECT_EQ(child1->position().x, 0);
        EXPECT_EQ(child1->position().y, inner_layout->anchors().top_left.y);
        //Child 2 position
        EXPECT_EQ(child2->position().x, child1->anchors().top_right.x);
        EXPECT_EQ(child2->position().y, child1->anchors().top_right.y);
        //Child 3 position
        EXPECT_EQ(child3->position().x, inner_layout->anchors().bottom_left.x);
        EXPECT_EQ(child3->position().y, inner_layout->anchors().bottom_left.y);
        //Test widths and heights
        EXPECT_NEAR(inner_layout->width(), child1->width()+child2->width(),0.001); // text width approx 47.335 for "Inner" at default font size
        EXPECT_NEAR(inner_layout->height(), 20,0.001);

    }
    TEST_F(DocraftLayoutEngineTest, ComputeBlankLineNode) {
        auto& engine = this->engine();
        auto context = this->context();
        auto blank_line = std::make_shared<docraft::model::DocraftBlankLine>();
        auto layout = engine->compute_layout(blank_line);
        EXPECT_EQ(layout.height(), 1.0F);
        EXPECT_NE(layout.width(), 0.0F);
        EXPECT_EQ(layout.width(),context->page_width());
    }
}
