#include <gtest/gtest.h>

#include "docraft_document_context.h"
#include "model/docraft_rectangle.h"

namespace docraft::test::model {
    class DocraftNodeTestBackend : public backend::IDocraftRenderingBackend {
    public:
        void begin_text() const override {}
        void end_text() const override {}
        void draw_text(const std::string&, float, float) const override {}
        void set_text_color(float, float, float) const override {}
        void draw_text_matrix(const std::string&, float, float, float, float, float, float) const override {}
        float measure_text_width(const std::string& text) const override {
            return static_cast<float>(text.size()) * 4.0F;
        }

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
        void draw_polygon(const std::vector<::docraft::model::DocraftPoint> &) const override {}
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

        void add_new_page() override { ++pages_; current_page_ = pages_ - 1; }

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

        void set_page_format(::docraft::model::DocraftPageSize, ::docraft::model::DocraftPageOrientation) override {}

        std::size_t current_page_number() const override { return current_page_ + 1; }
        std::size_t total_page_count() const override { return pages_; }

        void set_current_page(std::size_t page_number) {
            current_page_ = page_number > 0 ? page_number - 1 : 0;
        }

    private:
        std::size_t pages_ = 3;
        std::size_t current_page_ = 0;
    };

    class DocraftNodeTest : public ::testing::Test {
    protected:
        void SetUp() override {
            backend_ = std::make_shared<DocraftNodeTestBackend>();
            context_ = std::make_shared<DocraftDocumentContext>(backend_);
        }

        std::shared_ptr<DocraftNodeTestBackend> backend_;
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
