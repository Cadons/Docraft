#include "layout/docraft_layout_engine.h"

#include <gtest/gtest.h>

#include "model/docraft_blank_line.h"
#include "model/docraft_body.h"
#include "model/docraft_footer.h"
#include "model/docraft_header.h"
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
    TEST_F(DocraftLayoutEngineTest, ComputeLayoutEmptyLayoutNode) {
        auto& engine = this->engine();
        auto context = this->context();
        auto layout_node = std::make_shared<docraft::model::DocraftLayout>();
        layout_node->set_orientation(docraft::model::LayoutOrientation::kVertical);
        auto layout = engine->compute_layout(layout_node);
        EXPECT_EQ(layout.width(), 0);
        EXPECT_EQ(layout.height(), 0);
    }
    TEST_F(DocraftLayoutEngineTest, ComputeTableNode) {

    }
    TEST_F(DocraftLayoutEngineTest, ComputeFullDocumentLayout) {
        auto& engine =this->engine();
        auto context = this->context();
        std::vector<std::shared_ptr<docraft::model::DocraftNode>> document_nodes;
        auto header = std::make_shared<docraft::model::DocraftHeader>();
        auto header_text = std::make_shared<docraft::model::DocraftText>();
        header_text->set_text("Document Header");
        header->add_child(header_text);
        document_nodes.push_back(header);
        auto body = std::make_shared<docraft::model::DocraftBody>();
        auto body_text = std::make_shared<docraft::model::DocraftText>();
        body_text->set_text("This is the body of the document.");
        body->add_child(body_text);
        document_nodes.push_back(body);
        auto footer = std::make_shared<docraft::model::DocraftFooter>();
        auto footer_text = std::make_shared<docraft::model::DocraftText>();
        footer_text->set_text("Document Footer");
        footer->add_child(footer_text);
        document_nodes.push_back(footer);
        engine->compute_document_layout(document_nodes);
        //Verify header layout
        EXPECT_EQ(header->position().x, 0);
        EXPECT_EQ(header->position().y, context->page_height());
        EXPECT_EQ(header->width(), context->page_width());
        EXPECT_EQ(header->height(), context->page_height()*0.15F);
        // Verify header item position
        EXPECT_EQ(header_text->position().x, 0);
        EXPECT_EQ(header_text->position().y, header->anchors().top_left.y);
        //Verify body layout
        EXPECT_EQ(body->position().x, 0);
        EXPECT_EQ(body->position().y, header->anchors().bottom_left.y);
        EXPECT_EQ(body->width(), context->page_width());
        EXPECT_EQ(body->height(), context->page_height()*0.75F);
        // Verify body item position
        EXPECT_EQ(body_text->position().x, 0);
        EXPECT_EQ(body_text->position().y, body->anchors().top_left.y);
        //Verify footer layout
        EXPECT_EQ(footer->position().x, 0);
        EXPECT_EQ(footer->position().y, body->anchors().bottom_left.y);
        EXPECT_EQ(footer->width(), context->page_width());
        EXPECT_EQ(footer->height(), context->page_height()*0.10F);
        // Verify footer item position
        EXPECT_EQ(footer_text->position().x, 0);
        EXPECT_EQ(footer_text->position().y, footer->anchors().top_left.y);
    }
    TEST_F(DocraftLayoutEngineTest, ComputeDocumentLayoutWithoutBodyThrows) {
        auto& engine =this->engine();
        auto context = this->context();
        std::vector<std::shared_ptr<docraft::model::DocraftNode>> document_nodes;
        auto header = std::make_shared<docraft::model::DocraftHeader>();
        document_nodes.push_back(header);
        auto footer = std::make_shared<docraft::model::DocraftFooter>();
        document_nodes.push_back(footer);
        EXPECT_THROW(engine->compute_document_layout(document_nodes), std::runtime_error);
    }
    TEST_F(DocraftLayoutEngineTest, ComputeDocumentWithOnlyBody) {
        auto& engine =this->engine();
        auto context = this->context();
        std::vector<std::shared_ptr<docraft::model::DocraftNode>> document_nodes;
        auto body = std::make_shared<docraft::model::DocraftBody>();
        auto body_text = std::make_shared<docraft::model::DocraftText>();
        body_text->set_text("This is the body of the document.");
        body->add_child(body_text);
        document_nodes.push_back(body);
        engine->compute_document_layout(document_nodes);
        //Verify body layout
        EXPECT_EQ(body->position().x, 0);
        EXPECT_EQ(body->position().y, context->page_height());
        EXPECT_EQ(body->width(), context->page_width());
        EXPECT_EQ(body->height(), context->page_height()*0.75F);
        // Verify body item position
        EXPECT_EQ(body_text->position().x, 0);
        EXPECT_EQ(body_text->position().y, body->anchors().top_left.y);
    }
    TEST_F(DocraftLayoutEngineTest, ComputeDocumentWithOnlyHeaderAndBody) {
        auto& engine =this->engine();
        auto context = this->context();
        std::vector<std::shared_ptr<docraft::model::DocraftNode>> document_nodes;
        auto header = std::make_shared<docraft::model::DocraftHeader>();
        auto header_text = std::make_shared<docraft::model::DocraftText>();
        header_text->set_text("Document Header");
        header->add_child(header_text);
        document_nodes.push_back(header);
        auto body = std::make_shared<docraft::model::DocraftBody>();
        auto body_text = std::make_shared<docraft::model::DocraftText>();
        body_text->set_text("This is the body of the document.");
        body->add_child(body_text);
        document_nodes.push_back(body);
        engine->compute_document_layout(document_nodes);
        //Verify header layout
        EXPECT_EQ(header->position().x, 0);
        EXPECT_EQ(header->position().y, context->page_height());
        EXPECT_EQ(header->width(), context->page_width());
        EXPECT_EQ(header->height(), context->page_height()*0.15F);
        // Verify header item position
        EXPECT_EQ(header_text->position().x, 0);
        EXPECT_EQ(header_text->position().y, header->anchors().top_left.y);
        //Verify body layout
        EXPECT_EQ(body->position().x, 0);
        EXPECT_EQ(body->position().y, header->anchors().bottom_left.y);
        EXPECT_EQ(body->width(), context->page_width());
        EXPECT_EQ(body->height(), context->page_height()*0.75F);
        // Verify body item position
        EXPECT_EQ(body_text->position().x, 0);
        EXPECT_EQ(body_text->position().y, body->anchors().top_left.y);
    }
    TEST_F(DocraftLayoutEngineTest, ComputeDocumentWithOnlyBodyAndFooter) {
        auto& engine =this->engine();
        auto context = this->context();
        std::vector<std::shared_ptr<docraft::model::DocraftNode>> document_nodes;
        auto body = std::make_shared<docraft::model::DocraftBody>();
        auto body_text = std::make_shared<docraft::model::DocraftText>();
        body_text->set_text("This is the body of the document.");
        body->add_child(body_text);
        document_nodes.push_back(body);
        auto footer = std::make_shared<docraft::model::DocraftFooter>();
        auto footer_text = std::make_shared<docraft::model::DocraftText>();
        footer_text->set_text("Document Footer");
        footer->add_child(footer_text);
        document_nodes.push_back(footer);
        engine->compute_document_layout(document_nodes);
        //Verify body layout
        EXPECT_EQ(body->position().x, 0);
        EXPECT_EQ(body->position().y, context->page_height());
        EXPECT_EQ(body->width(), context->page_width());
        EXPECT_EQ(body->height(), context->page_height()*0.75F);
        // Verify body item position
        EXPECT_EQ(body_text->position().x, 0);
        EXPECT_EQ(body_text->position().y, body->anchors().top_left.y);
        //Verify footer layout
        EXPECT_EQ(footer->position().x, 0);
        EXPECT_EQ(footer->position().y, body->anchors().bottom_left.y);
        EXPECT_EQ(footer->width(), context->page_width());
        EXPECT_EQ(footer->height(), context->page_height()*0.10F);
        // Verify footer item position
        EXPECT_EQ(footer_text->position().x, 0);
        EXPECT_EQ(footer_text->position().y, footer->anchors().top_left.y);
    }

}
