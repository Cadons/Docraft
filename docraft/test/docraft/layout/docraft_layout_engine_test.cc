#include "docraft/layout/docraft_layout_engine.h"

#include <algorithm>
#include <gtest/gtest.h>

#include "docraft/model/docraft_blank_line.h"
#include "docraft/model/docraft_body.h"
#include "docraft/model/docraft_footer.h"
#include "docraft/model/docraft_header.h"
#include "docraft/model/docraft_layout.h"
#include "docraft/model/docraft_rectangle.h"
#include "docraft/model/docraft_table.h"
#include "docraft/model/docraft_text.h"

namespace docraft::test::layout {
    class DocraftLayoutEngineTest : public ::testing::Test {
    protected:
        void SetUp() override {
            context_ = std::make_shared<DocraftDocumentContext>();
            engine_ = std::make_unique<docraft::layout::DocraftLayoutEngine>(context_);
        }

        void TearDown() override {
        }

        std::unique_ptr<docraft::layout::DocraftLayoutEngine>& engine() {
            return engine_;
        }
        std::shared_ptr<DocraftDocumentContext> &context() {
            return context_;
        }
        const float kHeaderHeightRatio_ = 0.06F;
        const float kBodyHeightRatio_ = 0.88F;
        const float kFooterHeightRatio_ = 0.06F;
        const float kLineHeightOffset_ = 12.0F;
        const float kVerticalSpacing_ = 4.0F;
        const float kHorizontalSpacing_ = 4.0F;
    private:
        std::unique_ptr<docraft::layout::DocraftLayoutEngine> engine_;
        std::shared_ptr<DocraftDocumentContext> context_;

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
        EXPECT_NEAR(layout.height(), child1->height() + kVerticalSpacing_ + child2->height(), 1.1);
    }
    TEST_F(DocraftLayoutEngineTest, ComputeLayoutMultipleNodesHorizontalLayout) {
        auto& engine = this->engine();
        auto context = this->context();
        auto layout_node = std::make_shared<docraft::model::DocraftLayout>();
        layout_node->set_orientation(docraft::model::LayoutOrientation::kHorizontal);

        auto child1 = std::make_shared<docraft::model::DocraftText>();
        child1->set_text("Hello");
        child1->set_weight(0.5F);
        layout_node->add_child(child1);
        auto child2 = std::make_shared<docraft::model::DocraftRectangle>();
        child2->set_width(80);
        child2->set_height(40);
        child2->set_weight(0.5F);
        layout_node->add_child(child2);

        EXPECT_EQ(layout_node->children().size(), 2);
        auto layout = engine->compute_layout(layout_node);
        const float available_width_for_children = context->page_width() - kHorizontalSpacing_;
        const float allocated_width = available_width_for_children * child1->weight();
        EXPECT_NEAR(child2->position().x, allocated_width + kHorizontalSpacing_, 0.001);
        EXPECT_GE(layout.width(), child2->anchors().top_right.x - layout_node->position().x);
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
        EXPECT_NEAR(layout.width(), std::max(inner_layout->width(), child3->width()), 0.5);
        // The actual height includes text line height offset which adds extra spacing
        EXPECT_NEAR(layout.height(), inner_layout->height() + kVerticalSpacing_ + child3->height(), 10.0);
        //Test position for each child
        EXPECT_EQ(inner_layout->position().x, 0.0F); //layout starts at cursor origin
        EXPECT_NEAR(inner_layout->position().y, 841.89F, 0.01F); //default page height
        //Child 1 position
        EXPECT_EQ(child1->position().x, 0.0F);
        EXPECT_EQ(child1->position().y, inner_layout->anchors().top_left.y);
        //Child 2 position advances by allocated width, not by child1's rendered width
        const float inner_available_width = context->page_width() - kHorizontalSpacing_;
        const float inner_allocated_width = inner_available_width * child1->weight();
        EXPECT_NEAR(child2->position().x, inner_allocated_width + kHorizontalSpacing_, 0.001);
        EXPECT_EQ(child2->position().y, child1->anchors().top_right.y);
        //Child 3 position - allow for text line height offset
        EXPECT_EQ(child3->position().x, 0.0F);//Child 3 should be left aligned with the outer layout
        EXPECT_NEAR(child3->position().y + kVerticalSpacing_, inner_layout->anchors().bottom_left.y, 10.0);
        //Test widths and heights
        EXPECT_NEAR(inner_layout->width(), 595.0F, 0.5F);//inner layout = the width of the page, inner layout should take all the available width
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
    TEST_F(DocraftLayoutEngineTest, ComputeTableHorizontalNode) {
        auto& engine = this->engine();
        auto context = this->context();

        auto table = std::make_shared<docraft::model::DocraftTable>();
        table->set_titles({"ColA", "ColB"});
        table->set_column_weights({.5F, .5F});
        table->set_auto_fill_width(true);

        // 2x2 grid content (rectangles are deterministic for height)
        auto c00 = std::make_shared<docraft::model::DocraftRectangle>();
        c00->set_height(10.0F);
        auto c01 = std::make_shared<docraft::model::DocraftRectangle>();
        c01->set_height(20.0F);
        auto c10 = std::make_shared<docraft::model::DocraftText>();
        c10->set_text("Cell 10");
        auto c11 = std::make_shared<docraft::model::DocraftText>();
        c11->set_text("Cell 11");

        table->add_content_node(c00);
        table->add_content_node(c01);
        table->add_content_node(c10);
        table->add_content_node(c11);

        const auto box = engine->compute_layout(table);

        // Table should start at the initial cursor (engine ctor places cursor at top-left of page).
        // Note: The cursor may be offset by default body padding
        EXPECT_FLOAT_EQ(table->position().x, 0.0F);
        EXPECT_NEAR(table->position().y, context->page_height(), 15.0F);

        // Auto-fill width should consume the remaining page width.
        EXPECT_NEAR(table->width(), context->page_width(), 0.001F);
        EXPECT_NEAR(box.width(), context->page_width(), 0.001F);

        // Titles should be generated and have a non-zero row height.
        ASSERT_EQ(table->title_nodes().size(), 2U);
        const float title_row_height = table->title_nodes()[0]->height();
        EXPECT_GT(title_row_height, 0.0F);

        // Content is 2 rows: row heights should be max of each row's cell heights.
        // Row0 max(10,20)=20; Row1 max(15,5)=15
        const float expected_height = title_row_height + 20.0F + 20.0F+10;//10 is the default cell height, added to ensure a minimum row height even if text content is small
        EXPECT_NEAR(table->height(), expected_height, 0.01F);
        EXPECT_NEAR(box.height(), expected_height, 0.01F);

        // Cells in the same row must share the same computed height.
        EXPECT_FLOAT_EQ(c00->height(), c01->height());
        EXPECT_FLOAT_EQ(c10->height(), c11->height());
        EXPECT_FLOAT_EQ(c00->height(), 25.0F);
        EXPECT_FLOAT_EQ(c10->height(), 25.0F);
        EXPECT_EQ(c10->text(),"Cell 10");
        EXPECT_EQ(c11->text(),"Cell 11");

        // Basic positioning sanity: second column starts to the right of first column.
        EXPECT_GT(c01->position().x, c00->position().x);
        // Second row should be below the first row (Y decreases downwards in this engine).
        EXPECT_LT(c10->position().y, c00->position().y);
    }
      TEST_F(DocraftLayoutEngineTest, ComputeVerticalTableNode) {
        auto& engine = this->engine();
        auto context = this->context();

        auto table = std::make_shared<docraft::model::DocraftTable>();
        table->set_orientation(docraft::model::LayoutOrientation::kVertical);
        table->set_titles({"KeyA", "KeyB"});
        table->set_cols(2);                 // key/value
        table->set_auto_fill_width(true);

        auto v0 = std::make_shared<docraft::model::DocraftRectangle>();
        v0->set_height(10.0F);
        auto v1 = std::make_shared<docraft::model::DocraftRectangle>();
        v1->set_height(20.0F);

        table->add_content_node(v0);
        table->add_content_node(v1);

        const auto box = engine->compute_layout(table);

        // Starts at initial cursor.
        EXPECT_FLOAT_EQ(table->position().x, 0.0F);
        EXPECT_FLOAT_EQ(table->position().y, context->page_height());

        // Auto-fill width.
        EXPECT_NEAR(table->width(), context->page_width(), 0.001F);
        EXPECT_NEAR(box.width(), context->page_width(), 0.001F);

        // One title per row.
        ASSERT_EQ(table->title_nodes().size(), 2U);

        // Values should be placed to the right of their row title.
        EXPECT_GT(v0->position().x, table->title_nodes()[0]->position().x);
        EXPECT_GT(v1->position().x, table->title_nodes()[1]->position().x);

        // Row heights: handler enforces a uniform height per row (title + value cells share it).
        EXPECT_FLOAT_EQ(v0->height(), table->title_nodes()[0]->height());
        EXPECT_FLOAT_EQ(v1->height(), table->title_nodes()[1]->height());

        // Second row is below the first row (Y decreases downwards).
        EXPECT_LT(table->title_nodes()[1]->position().y, table->title_nodes()[0]->position().y);
        EXPECT_LT(v1->position().y, v0->position().y);

        // Table height is the sum of row heights (which are stored on the title nodes post-layout).
        const float expected_height = table->title_nodes()[0]->height() + table->title_nodes()[1]->height();
        EXPECT_NEAR(table->height(), expected_height, 0.01F);
        EXPECT_NEAR(box.height(), expected_height, 0.01F);
    }

    TEST_F(DocraftLayoutEngineTest, ComputeFullDocumentLayout) {
        auto& engine =this->engine();
        auto context = this->context();
        std::vector<std::shared_ptr<docraft::model::DocraftNode>> document_nodes;
        auto header = std::make_shared<docraft::model::DocraftHeader>();
        header->set_margin_left(0.0F);
        header->set_margin_right(0.0F);
        auto header_text = std::make_shared<docraft::model::DocraftText>();
        header_text->set_text("Document Header");
        header->add_child(header_text);
        document_nodes.push_back(header);
        auto body = std::make_shared<docraft::model::DocraftBody>();
        body->set_margin_left(0.0F);
        body->set_margin_right(0.0F);
        auto body_text = std::make_shared<docraft::model::DocraftText>();
        body_text->set_text("This is the body of the document.");
        body->add_child(body_text);
        document_nodes.push_back(body);
        auto footer = std::make_shared<docraft::model::DocraftFooter>();
        footer->set_margin_left(0.0F);
        footer->set_margin_right(0.0F);
        auto footer_text = std::make_shared<docraft::model::DocraftText>();
        footer_text->set_text("Document Footer");
        footer->add_child(footer_text);
        document_nodes.push_back(footer);
        engine->compute_document_layout(document_nodes);
        //Verify header layout
        EXPECT_EQ(header->position().x, 0);
        EXPECT_EQ(header->position().y, context->page_height());
        EXPECT_EQ(header->width(), context->page_width());
        EXPECT_EQ(header->height(), context->page_height()*kHeaderHeightRatio_);
        // Verify header item position
        EXPECT_EQ(header_text->position().x, 0.0F); // section padding is vertical only
        EXPECT_NEAR(header_text->position().y, header->anchors().top_left.y - 2.0F, 0.01F);
        //Verify body layout
        EXPECT_EQ(body->position().x, 0);
        EXPECT_EQ(body->position().y, header->anchors().bottom_left.y);
        EXPECT_EQ(body->width(), context->page_width());
        const float expected_body_height = context->page_height() -
                                           context->page_height() * kHeaderHeightRatio_ -
                                           context->page_height() * kFooterHeightRatio_;
        EXPECT_NEAR(body->height(), expected_body_height, 0.01F);
        // Verify body item position
        EXPECT_EQ(body_text->position().x, 0.0F); // section padding is vertical only
        EXPECT_EQ(body_text->position().y, body->anchors().top_left.y);
        //Verify footer layout
        EXPECT_EQ(footer->position().x, 0);
        EXPECT_EQ(footer->position().y, body->anchors().bottom_left.y);
        EXPECT_EQ(footer->width(), context->page_width());
        EXPECT_EQ(footer->height(), context->page_height()*kFooterHeightRatio_);
        // Verify footer item position
        EXPECT_EQ(footer_text->position().x, 0.0F); // section padding is vertical only
        EXPECT_NEAR(footer_text->position().y, footer->anchors().top_left.y - 2.0F, 0.01F);
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
        EXPECT_EQ(body->position().x, 10);
        EXPECT_EQ(body->position().y, context->page_height());
        EXPECT_EQ(body->width(), context->page_width()-20);
        EXPECT_EQ(body->height(), context->page_height());
        // Verify body item position
        EXPECT_EQ(body_text->position().x, 10.0F);
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
        EXPECT_EQ(header->position().x, 10);
        EXPECT_EQ(header->position().y, context->page_height());
        EXPECT_EQ(header->width(), context->page_width()-20);
        EXPECT_EQ(header->height(), context->page_height()*kHeaderHeightRatio_);
        // Verify header item position
        EXPECT_EQ(header_text->position().x, 10.0F);
        EXPECT_NEAR(header_text->position().y, header->anchors().top_left.y - 2.0F, 0.01F);
        //Verify body layout
        EXPECT_EQ(body->position().x, 10);
        EXPECT_EQ(body->position().y, header->anchors().bottom_left.y);
        EXPECT_EQ(body->width(), context->page_width()-20);
        const float expected_body_height = context->page_height() -
                                           context->page_height() * kHeaderHeightRatio_;
        EXPECT_EQ(body->height(), expected_body_height);
        // Verify body item position
        EXPECT_EQ(body_text->position().x, 10.0F);
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
        EXPECT_EQ(body->position().x, 10);
        EXPECT_EQ(body->position().y, context->page_height());
        EXPECT_EQ(body->width(), context->page_width()-20);
        const float expected_body_height = context->page_height() -
                                           context->page_height() * kFooterHeightRatio_;
        EXPECT_EQ(body->height(), expected_body_height);
        // Verify body item position
        EXPECT_EQ(body_text->position().x, 10.0F);
        EXPECT_EQ(body_text->position().y, body->anchors().top_left.y);
        //Verify footer layout
        EXPECT_EQ(footer->position().x, 10);
        EXPECT_EQ(footer->position().y, body->anchors().bottom_left.y);
        EXPECT_EQ(footer->width(), context->page_width()-20);
        EXPECT_EQ(footer->height(), context->page_height()*kFooterHeightRatio_);
        // Verify footer item position
        EXPECT_EQ(footer_text->position().x, 10.0F);
        EXPECT_NEAR(footer_text->position().y, footer->anchors().top_left.y - 2.0F, 0.01F);
    }
 TEST_F(DocraftLayoutEngineTest, ComputeComplexNestedHorizontalVerticalLayout) {
            // Reproduces the XML:
            // <Layout orientation="horizontal">
            //   <Layout vertical weight=0.25> Left Panel: Text + Rectangle(h=100) </Layout>
            //   <Layout vertical weight=0.50> Center: Text + nested horizontal(2 vertical columns) </Layout>
            //   <Layout vertical weight=0.25> Right Panel: Text + Rectangle(h=100) + nested horizontal </Layout>
            // </Layout>

            auto& engine = this->engine();
            auto context = this->context();

            auto body = std::make_shared<docraft::model::DocraftBody>();
            // === Root horizontal layout ===
            auto root_layout = std::make_shared<docraft::model::DocraftLayout>();
            root_layout->set_orientation(docraft::model::LayoutOrientation::kHorizontal);

            // === LEFT PANEL (weight 0.25) ===
            auto left_panel = std::make_shared<docraft::model::DocraftLayout>();
            left_panel->set_orientation(docraft::model::LayoutOrientation::kVertical);
            left_panel->set_weight(0.25F);

            auto left_text = std::make_shared<docraft::model::DocraftText>();
            left_text->set_text("Left Panel");
            left_text->set_font_size(7);
            left_text->set_style(docraft::model::TextStyle::kBold);
            left_panel->add_child(left_text);

            auto left_rect = std::make_shared<docraft::model::DocraftRectangle>();
            left_rect->set_height(100);
            left_rect->set_background_color(docraft::DocraftColor(255, 0, 0));
            left_panel->add_child(left_rect);

            root_layout->add_child(left_panel);

            // === CENTER PANEL (weight 0.50) ===
            auto center_panel = std::make_shared<docraft::model::DocraftLayout>();
            center_panel->set_orientation(docraft::model::LayoutOrientation::kVertical);
            center_panel->set_weight(0.50F);

            auto center_text = std::make_shared<docraft::model::DocraftText>();
            center_text->set_text("Center (nested)");
            center_text->set_font_size(7);
            center_text->set_style(docraft::model::TextStyle::kBold);
            center_text->set_color(docraft::DocraftColor(255, 0, 0));
            center_panel->add_child(center_text);

            // Nested horizontal layout inside center
            auto center_inner_h = std::make_shared<docraft::model::DocraftLayout>();
            center_inner_h->set_orientation(docraft::model::LayoutOrientation::kHorizontal);

            // Inner A (left column)
            auto inner_a = std::make_shared<docraft::model::DocraftLayout>();
            inner_a->set_orientation(docraft::model::LayoutOrientation::kVertical);
            inner_a->set_weight(0.5F);

            auto inner_a_rect = std::make_shared<docraft::model::DocraftRectangle>();
            inner_a_rect->set_height(14);
            inner_a->add_child(inner_a_rect);

            auto inner_a_text = std::make_shared<docraft::model::DocraftText>();
            inner_a_text->set_text("Inner A");
            inner_a_text->set_font_size(6);
            inner_a_text->set_alignment(docraft::model::TextAlignment::kCenter);
            inner_a->add_child(inner_a_text);

            auto inner_a_caption = std::make_shared<docraft::model::DocraftText>();
            inner_a_caption->set_text("Caption A");
            inner_a_caption->set_font_size(5);
            inner_a_caption->set_alignment(docraft::model::TextAlignment::kCenter);
            inner_a->add_child(inner_a_caption);

            center_inner_h->add_child(inner_a);

            // Inner B (right column) with table
            auto inner_b = std::make_shared<docraft::model::DocraftLayout>();
            inner_b->set_orientation(docraft::model::LayoutOrientation::kVertical);
            inner_b->set_weight(0.5F);

            auto inner_b_rect = std::make_shared<docraft::model::DocraftRectangle>();
            inner_b_rect->set_height(14);
            inner_b->add_child(inner_b_rect);

            auto inner_b_text = std::make_shared<docraft::model::DocraftText>();
            inner_b_text->set_text("Inner B");
            inner_b_text->set_font_size(6);
            inner_b_text->set_alignment(docraft::model::TextAlignment::kCenter);
            inner_b->add_child(inner_b_text);

            // Vertical table 3x2
            auto table = std::make_shared<docraft::model::DocraftTable>();
            table->set_orientation(docraft::model::LayoutOrientation::kVertical);
            table->set_titles({"Key", "Val"});
            // Actually this is a vertical table with 3 rows, titles are row headers
            // But set_titles takes the row keys. For a 3-row vertical table:
            table->set_titles({"Key", "Val"}); // 2 columns (key/val)
            table->set_cols(2);
            table->set_auto_fill_width(true);

            // 3 rows of data (A/1, B/2, C/3) — each row has key+val
            auto cell_a = std::make_shared<docraft::model::DocraftText>("A");
            cell_a->set_font_size(5);
            auto cell_1 = std::make_shared<docraft::model::DocraftText>("1");
            cell_1->set_font_size(5);
            auto cell_b = std::make_shared<docraft::model::DocraftText>("B");
            cell_b->set_font_size(5);
            auto cell_2 = std::make_shared<docraft::model::DocraftText>("2");
            cell_2->set_font_size(5);
            auto cell_c = std::make_shared<docraft::model::DocraftText>("C");
            cell_c->set_font_size(5);
            auto cell_3 = std::make_shared<docraft::model::DocraftText>("3");
            cell_3->set_font_size(5);

            table->add_content_node(cell_a);
            table->add_content_node(cell_1);
            table->add_content_node(cell_b);
            table->add_content_node(cell_2);
            table->add_content_node(cell_c);
            table->add_content_node(cell_3);

            inner_b->add_child(table);

            center_inner_h->add_child(inner_b);
            center_panel->add_child(center_inner_h);

            root_layout->add_child(center_panel);

            // === RIGHT PANEL (weight 0.25) ===
            auto right_panel = std::make_shared<docraft::model::DocraftLayout>();
            right_panel->set_orientation(docraft::model::LayoutOrientation::kVertical);
            right_panel->set_weight(0.25F);

            auto right_text = std::make_shared<docraft::model::DocraftText>();
            right_text->set_text("Right Panel");
            right_text->set_font_size(7);
            right_text->set_style(docraft::model::TextStyle::kBold);
            right_panel->add_child(right_text);

            auto right_rect = std::make_shared<docraft::model::DocraftRectangle>();
            right_rect->set_height(100);
            right_panel->add_child(right_rect);

            // Nested horizontal inside right panel
            auto right_inner_h = std::make_shared<docraft::model::DocraftLayout>();
            right_inner_h->set_orientation(docraft::model::LayoutOrientation::kHorizontal);

            auto r1_layout = std::make_shared<docraft::model::DocraftLayout>();
            r1_layout->set_orientation(docraft::model::LayoutOrientation::kVertical);
            r1_layout->set_weight(0.5F);
            auto r1_rect = std::make_shared<docraft::model::DocraftRectangle>();
            r1_rect->set_height(10);
            r1_layout->add_child(r1_rect);
            auto r1_text = std::make_shared<docraft::model::DocraftText>();
            r1_text->set_text("R1");
            r1_text->set_font_size(5);
            r1_text->set_alignment(docraft::model::TextAlignment::kCenter);
            r1_layout->add_child(r1_text);

            auto r2_layout = std::make_shared<docraft::model::DocraftLayout>();
            r2_layout->set_orientation(docraft::model::LayoutOrientation::kVertical);
            r2_layout->set_weight(0.5F);
            auto r2_rect = std::make_shared<docraft::model::DocraftRectangle>();
            r2_rect->set_height(10);
            r2_layout->add_child(r2_rect);
            auto r2_text = std::make_shared<docraft::model::DocraftText>();
            r2_text->set_text("R2");
            r2_text->set_font_size(5);
            r2_text->set_alignment(docraft::model::TextAlignment::kCenter);
            r2_layout->add_child(r2_text);

            right_inner_h->add_child(r1_layout);
            right_inner_h->add_child(r2_layout);
            right_panel->add_child(right_inner_h);

            root_layout->add_child(right_panel);

            body->add_child(root_layout);
            // === COMPUTE LAYOUT ===
            auto result = engine->compute_layout(body);

            const float page_w = context->page_width() - (body->margin_left() + body->margin_right());

            // --- Structural assertions ---

            // Root layout should have 3 children
            ASSERT_EQ(root_layout->children().size(), 3U);

            // Root layout should span the full available width
            EXPECT_NEAR(root_layout->width(), page_w, 1.0F);
            EXPECT_GT(root_layout->height(), 0.0F);

            // --- Weight distribution: panels should respect 0.25 / 0.50 / 0.25 ---
            const float available_w = page_w - (2.0F * kHorizontalSpacing_);
            const float expected_left_w = available_w * 0.25F;
            const float expected_center_w = available_w * 0.50F;
            const float expected_right_w = available_w * 0.25F;

            EXPECT_NEAR(left_panel->width(), expected_left_w, 1.0F);
            EXPECT_NEAR(center_panel->width(), expected_center_w, 1.0F);
            EXPECT_NEAR(right_panel->width(), expected_right_w, 1.0F);

            // --- Horizontal positioning: panels should be side by side ---
            EXPECT_NEAR(left_panel->position().x, 10.0F, 0.01F);
            EXPECT_NEAR(center_panel->position().x, left_panel->anchors().top_right.x + kHorizontalSpacing_, 1.0F);
            EXPECT_NEAR(right_panel->position().x, center_panel->anchors().top_right.x + kHorizontalSpacing_, 1.0F);

            // All top-level panels should start at the same Y
            EXPECT_FLOAT_EQ(left_panel->position().y, center_panel->position().y);
            EXPECT_FLOAT_EQ(center_panel->position().y, right_panel->position().y);

            // --- Left panel children ---
            EXPECT_GT(left_text->height(), 0.0F);
            EXPECT_FLOAT_EQ(left_rect->height(), 100.0F);
            // Text should be above rectangle (higher Y)
            EXPECT_GT(left_text->position().y, left_rect->position().y);

            // --- Center panel: nested horizontal inner layout ---
            ASSERT_EQ(center_inner_h->children().size(), 2U);
            // Inner A and Inner B should each get 50% of center panel width
            EXPECT_NEAR(inner_a->width(), (center_panel->width() - kHorizontalSpacing_) * 0.5F, 1.0F);
            EXPECT_NEAR(inner_b->width(), (center_panel->width() - kHorizontalSpacing_) * 0.5F, 1.0F);
            // Inner A should be to the left of Inner B
            EXPECT_LT(inner_a->position().x, inner_b->position().x);

            // --- Table inside Inner B ---
            EXPECT_GT(table->width(), 0.0F);
            EXPECT_GT(table->height(), 0.0F);
            // Table should be within inner_b bounds
            EXPECT_GE(table->position().x, inner_b->position().x - 0.01F);

            // --- Right panel: nested horizontal with R1/R2 ---
            ASSERT_EQ(right_inner_h->children().size(), 2U);
            EXPECT_NEAR(r1_layout->width(), (right_panel->width() - kHorizontalSpacing_) * 0.5F, 1.0F);
            EXPECT_NEAR(r2_layout->width(), (right_panel->width() - kHorizontalSpacing_) * 0.5F, 1.0F);
            // R1 should be to the left of R2
            EXPECT_LT(r1_layout->position().x, r2_layout->position().x);
            // Both R rectangles should have height 10
            EXPECT_FLOAT_EQ(r1_rect->height(), 10.0F);
            EXPECT_FLOAT_EQ(r2_rect->height(), 10.0F);

            // --- Vertical ordering inside panels ---
            // In left panel: text is first child, rect second — rect should be below text
            EXPECT_GT(left_text->position().y, left_rect->position().y);
            // In right panel: text > rect > inner_h (top to bottom = decreasing Y)
            EXPECT_GT(right_text->position().y, right_rect->position().y);
            EXPECT_GT(right_rect->position().y, right_inner_h->position().y);

            // --- Overall height sanity ---
            // The root layout height should be at least the tallest panel
            float max_panel_height = std::max({left_panel->height(), center_panel->height(), right_panel->height()});
            EXPECT_NEAR(root_layout->height(), max_panel_height, 1.0F);
    }
    TEST_F(DocraftLayoutEngineTest, ComputeTableHorizontalNodeRespectsExplicitCellWidth) {
        auto& engine = this->engine();

        auto table = std::make_shared<docraft::model::DocraftTable>();
        table->set_titles({"A", "B"});
        table->set_column_weights({.5F, .5F});
        table->set_width(300.0F);

        auto c00 = std::make_shared<docraft::model::DocraftRectangle>();
        c00->set_width(90.0F); // Simulates <Cell width="90">...
        c00->set_height(10.0F);
        auto c01 = std::make_shared<docraft::model::DocraftRectangle>();
        c01->set_height(10.0F);

        table->add_content_node(c00);
        table->add_content_node(c01);

        (void)engine->compute_layout(table);

        EXPECT_NEAR(c00->width(), 90.0F, 0.001F);
        EXPECT_GT(c01->width(), c00->width());
    }

}
