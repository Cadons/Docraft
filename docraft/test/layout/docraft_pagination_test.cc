#include <gtest/gtest.h>

#include <memory>

#include "docraft_document_context.h"
#include "layout/docraft_layout_engine.h"
#include "model/docraft_body.h"
#include "model/docraft_footer.h"
#include "model/docraft_header.h"
#include "model/docraft_page_number.h"
#include "model/docraft_rectangle.h"
#include "model/docraft_table.h"
#include "utils/docraft_mock_rendering_backend.h"

namespace docraft::test::layout {
    class DocraftPaginationTest : public ::testing::Test {
    protected:
        void SetUp() override {
            backend_ = std::make_shared<docraft::test::utils::MockRenderingBackend>(
                docraft::test::utils::MockRenderingBackend::Config{
                    .page_width = 100.0F,
                    .page_height = 100.0F,
                    .text_width_factor = 5.0F,
                    .initial_pages = 1,
                    .extension = ".pdf",
                    .can_use_font = true
                });
            context_ = std::make_shared<DocraftDocumentContext>(backend_);
            engine_ = std::make_unique<docraft::layout::DocraftLayoutEngine>(context_);
        }

        std::shared_ptr<docraft::test::utils::MockRenderingBackend> backend_;
        std::shared_ptr<DocraftDocumentContext> context_;
        std::unique_ptr<docraft::layout::DocraftLayoutEngine> engine_;
    };

    TEST_F(DocraftPaginationTest, AssignsPageOwnersAndCreatesPages) {
        auto body = std::make_shared<model::DocraftBody>();
        body->set_margin_left(0.0F);
        body->set_margin_right(0.0F);

        auto rect1 = std::make_shared<model::DocraftRectangle>();
        rect1->set_height(70.0F);
        auto rect2 = std::make_shared<model::DocraftRectangle>();
        rect2->set_height(70.0F);

        body->add_child(rect1);
        body->add_child(rect2);

        std::vector<std::shared_ptr<model::DocraftNode>> nodes{body};
        engine_->compute_document_layout(nodes);

        EXPECT_EQ(backend_->total_page_count(), 2U);
        EXPECT_EQ(rect1->page_owner(), 1);
        EXPECT_EQ(rect2->page_owner(), 2);
    }

    TEST_F(DocraftPaginationTest, HeaderAndFooterAreRenderedOnAllPages) {
        auto header = std::make_shared<model::DocraftHeader>();
        auto body = std::make_shared<model::DocraftBody>();
        auto footer = std::make_shared<model::DocraftFooter>();

        auto header_rect = std::make_shared<model::DocraftRectangle>();
        header_rect->set_height(10.0F);
        header->add_child(header_rect);

        auto body_rect = std::make_shared<model::DocraftRectangle>();
        body_rect->set_height(120.0F);
        body->add_child(body_rect);

        auto footer_rect = std::make_shared<model::DocraftRectangle>();
        footer_rect->set_height(10.0F);
        footer->add_child(footer_rect);

        std::vector<std::shared_ptr<model::DocraftNode>> nodes{header, body, footer};
        engine_->compute_document_layout(nodes);

        EXPECT_EQ(header->page_owner(), -1);
        EXPECT_EQ(footer->page_owner(), -1);
    }

    TEST_F(DocraftPaginationTest, SplitsLargeTableAcrossPages) {
        auto body = std::make_shared<model::DocraftBody>();
        body->set_margin_left(0.0F);
        body->set_margin_right(0.0F);

        auto table = std::make_shared<model::DocraftTable>();
        table->apply_json_header(R"(["H1","H2"])");
        table->apply_json_rows(R"([["r1c1","r1c2"],["r2c1","r2c2"],["r3c1","r3c2"],["r4c1","r4c2"],["r5c1","r5c2"]])");

        body->add_child(table);

        std::vector<std::shared_ptr<model::DocraftNode>> nodes{body};
        engine_->compute_document_layout(nodes);

        ASSERT_EQ(backend_->total_page_count(), 3U);
        ASSERT_EQ(body->children().size(), 3U);

        auto first_table = std::dynamic_pointer_cast<model::DocraftTable>(body->children()[0]);
        auto second_table = std::dynamic_pointer_cast<model::DocraftTable>(body->children()[1]);
        ASSERT_TRUE(first_table);
        ASSERT_TRUE(second_table);

        EXPECT_EQ(first_table->page_owner(), 1);
        EXPECT_EQ(second_table->page_owner(), 2);

        const auto total_rows =
            static_cast<std::size_t>(first_table->rows() + second_table->rows());
        EXPECT_EQ(total_rows, 4U);
        EXPECT_GT(first_table->rows(), 0);
        EXPECT_GT(second_table->rows(), 0);

        // Remainder table must be re-laid out at the top of the new page.
        EXPECT_FLOAT_EQ(second_table->position().y+10, body->position().y); // 10 is the padding of the body section (defaultì)
        EXPECT_GE(second_table->anchors().bottom_left.y, body->anchors().bottom_left.y);
    }
} // namespace docraft::test::layout
