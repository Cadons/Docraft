#include <gtest/gtest.h>

#include "docraft/docraft_document_context.h"
#include "docraft/model/docraft_rectangle.h"
#include "docraft/utils/docraft_mock_rendering_backend.h"

namespace docraft::test::model {
    class DocraftNodeTest : public ::testing::Test {
    protected:
        void SetUp() override {
            backend_ = std::make_shared<docraft::test::utils::MockRenderingBackend>(
                docraft::test::utils::MockRenderingBackend::Config{
                    .page_width = 100.0F,
                    .page_height = 100.0F,
                    .text_width_factor = 4.0F,
                    .initial_pages = 3,
                    .extension = ".pdf",
                    .can_use_font = true
                });
            context_ = std::make_shared<DocraftDocumentContext>(backend_);
        }

        std::shared_ptr<docraft::test::utils::MockRenderingBackend> backend_;
        std::shared_ptr<DocraftDocumentContext> context_;
    };

    TEST_F(DocraftNodeTest, RendersWhenPageOwnerIsMinusOne) {
        auto node = std::make_shared<docraft::model::DocraftRectangle>();
        node->set_page_owner(-1);
        EXPECT_TRUE(node->should_render(context_));
    }

    TEST_F(DocraftNodeTest, RendersOnlyOnOwnedPage) {
        auto node = std::make_shared<docraft::model::DocraftRectangle>();
        node->set_page_owner(2);

        backend_->set_current_page(1);
        EXPECT_FALSE(node->should_render(context_));

        backend_->set_current_page(2);
        EXPECT_TRUE(node->should_render(context_));
    }

    TEST_F(DocraftNodeTest, RendersWhenContextIsNull) {
        auto node = std::make_shared<docraft::model::DocraftRectangle>();
        node->set_page_owner(2);
        EXPECT_TRUE(node->should_render(nullptr));
    }
} // namespace docraft::test::model
