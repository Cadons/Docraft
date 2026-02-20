#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "docraft_document_context.h"
#include "layout/docraft_layout_engine.h"
#include "model/docraft_body.h"
#include "model/docraft_footer.h"
#include "model/docraft_header.h"
#include "model/docraft_page_number.h"
#include "model/docraft_rectangle.h"
#include "model/docraft_table.h"

namespace docraft::test::layout {
    class PaginationBackend : public backend::IDocraftRenderingBackend {
    public:
        PaginationBackend() = default;

        void begin_text() const override {}
        void end_text() const override {}
        void draw_text(const std::string&, float, float) const override {}
        void set_text_color(float, float, float) const override {}
        void draw_text_matrix(const std::string&, float, float, float, float, float, float) const override {}
        float measure_text_width(const std::string& text) const override { return static_cast<float>(text.size()) * 5.0F; }

        void set_stroke_color(float, float, float) const override {}
        void set_line_width(float) const override {}
        void draw_line(float, float, float, float) const override {}

        void save_state() const override {}
        void restore_state() const override {}
        void set_fill_color(float, float, float) const override {}
        void set_fill_alpha(float) const override {}
        void set_stroke_alpha(float) const override {}
        void draw_rectangle(float, float, float, float) const override {}
        void draw_circle(float, float, float) const override {}
        void draw_polygon(const std::vector<model::DocraftPoint> &) const override {}
        void fill() const override {}
        void stroke() const override {}
        void fill_stroke() const override {}

        void draw_png_image(const std::string&, float, float, float, float) const override {}
        void draw_png_image_from_memory(const unsigned char*, std::size_t, float, float, float, float) const override {}
        void draw_jpeg_image(const std::string&, float, float, float, float) const override {}
        void draw_jpeg_image_from_memory(const unsigned char*, std::size_t, float, float, float, float) const override {}
        void draw_raw_rgb_image(const std::string&, int, int, float, float, float, float) const override {}
        void draw_raw_rgb_image_from_memory(const unsigned char*, int, int, float, float, float, float) const override {}

        void save_to_file(const std::string&) const override {}
        const char* register_ttf_font_from_file(const std::string&, bool) const override { return "Helvetica"; }
        bool can_use_font(const std::string&, const char*) const override { return true; }
        void set_font(const std::string&, float, const char*) const override {}

        float page_width() const override { return 100.0F; }
        float page_height() const override { return 100.0F; }

        void add_new_page() override {
            ++pages_;
            current_page_ = pages_ - 1;
        }

        void move_to_next_page() override {
            if (current_page_ + 1 >= pages_) {
                throw std::runtime_error("Already at the last page");
            }
            ++current_page_;
        }

        void go_to_page(std::size_t page_number) override {
            if (page_number >= pages_) {
                throw std::runtime_error("Invalid page number");
            }
            current_page_ = page_number;
        }

        void go_to_first_page() override {
            if (pages_ == 0) {
                throw std::runtime_error("No pages");
            }
            current_page_ = 0;
        }

        void go_to_previous_page() override {
            if (current_page_ == 0) {
                throw std::runtime_error("Already at first page");
            }
            --current_page_;
        }

        void go_to_last_page() override {
            if (pages_ == 0) {
                throw std::runtime_error("No pages");
            }
            current_page_ = pages_ - 1;
        }

        void set_page_format(model::DocraftPageSize, model::DocraftPageOrientation) override {}

        std::size_t current_page_number() const override { return current_page_ + 1; }
        std::size_t total_page_count() const override { return pages_; }

    private:
        std::size_t pages_ = 1;
        std::size_t current_page_ = 0;
    };

    class DocraftPaginationTest : public ::testing::Test {
    protected:
        void SetUp() override {
            backend_ = std::make_shared<PaginationBackend>();
            context_ = std::make_shared<DocraftDocumentContext>(backend_);
            engine_ = std::make_unique<docraft::layout::DocraftLayoutEngine>(context_);
        }

        std::shared_ptr<PaginationBackend> backend_;
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

        ASSERT_EQ(backend_->total_page_count(), 2U);
        ASSERT_EQ(body->children().size(), 2U);

        auto first_table = std::dynamic_pointer_cast<model::DocraftTable>(body->children()[0]);
        auto second_table = std::dynamic_pointer_cast<model::DocraftTable>(body->children()[1]);
        ASSERT_TRUE(first_table);
        ASSERT_TRUE(second_table);

        EXPECT_EQ(first_table->page_owner(), 1);
        EXPECT_EQ(second_table->page_owner(), 2);

        const std::size_t total_rows =
            static_cast<std::size_t>(first_table->rows() + second_table->rows());
        EXPECT_EQ(total_rows, 5U);
        EXPECT_GT(first_table->rows(), 0);
        EXPECT_GT(second_table->rows(), 0);
    }
} // namespace docraft::test::layout
