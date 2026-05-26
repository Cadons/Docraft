#pragma once

#include <stdexcept>
#include <string>
#include <vector>

#include "docraft/backend/docraft_rendering_backend.h"

namespace docraft::test::utils {
    class MockRenderingBackend : public backend::IDocraftRenderingBackend {
    public:
        class LineHaruBackend final : public backend::IDocraftLineRenderingBackend {
        public:
            explicit LineHaruBackend(MockRenderingBackend& backend) : backend_(backend) {}

            void set_stroke_color(float r, float g, float b) const override { backend_.set_stroke_color(r, g, b); }
            void set_line_width(float thickness) const override { backend_.set_line_width(thickness); }
            void draw_line(float x1, float y1, float x2, float y2) const override {
                backend_.draw_line(x1, y1, x2, y2);
            }

        private:
            MockRenderingBackend& backend_;
        };

        class ShapeHaruBackend final : public backend::IDocraftShapeRenderingBackend {
        public:
            explicit ShapeHaruBackend(MockRenderingBackend& backend) : backend_(backend) {}

            void save_state() const override { backend_.save_state(); }
            void restore_state() const override { backend_.restore_state(); }
            void set_fill_color(float r, float g, float b) const override { backend_.set_fill_color(r, g, b); }
            void set_fill_alpha(float alpha) const override { backend_.set_fill_alpha(alpha); }
            void set_stroke_alpha(float alpha) const override { backend_.set_stroke_alpha(alpha); }
            void draw_rectangle(float x, float y, float width, float height) const override {
                backend_.draw_rectangle(x, y, width, height);
            }
            void draw_circle(float center_x, float center_y, float radius) const override {
                backend_.draw_circle(center_x, center_y, radius);
            }
            void draw_polygon(const std::vector<model::DocraftPoint> &points) const override {
                backend_.draw_polygon(points);
            }
            void fill() const override { backend_.fill(); }
            void stroke() const override { backend_.stroke(); }
            void fill_stroke() const override { backend_.fill_stroke(); }
            void set_stroke_color(float r, float g, float b) const override { backend_.set_stroke_color(r, g, b); }
            void set_line_width(float thickness) const override { backend_.set_line_width(thickness); }
            void draw_line(float x1, float y1, float x2, float y2) const override {
                backend_.draw_line(x1, y1, x2, y2);
            }

        private:
            MockRenderingBackend& backend_;
        };

        class TextHaruBackend final : public backend::IDocraftTextRenderingBackend {
        public:
            explicit TextHaruBackend(MockRenderingBackend& backend) : backend_(backend) {}

            void begin_text() const override { backend_.begin_text(); }
            void end_text() const override { backend_.end_text(); }
            void draw_text(const std::string &text, float x, float y) const override {
                backend_.draw_text(text, x, y);
            }
            void set_text_color(float r, float g, float b) const override { backend_.set_text_color(r, g, b); }
            void draw_text_matrix(
                const std::string &text,
                float scale_x,
                float skew_x,
                float skew_y,
                float scale_y,
                float translate_x,
                float translate_y) const override {
                backend_.draw_text_matrix(text, scale_x, skew_x, skew_y, scale_y, translate_x, translate_y);
            }
            float measure_text_width(const std::string &text) const override { return backend_.measure_text_width(text); }
            void set_stroke_color(float r, float g, float b) const override { backend_.set_stroke_color(r, g, b); }
            void set_line_width(float thickness) const override { backend_.set_line_width(thickness); }
            void draw_line(float x1, float y1, float x2, float y2) const override {
                backend_.draw_line(x1, y1, x2, y2);
            }

        private:
            MockRenderingBackend& backend_;
        };

        class ImageHaruBackend final : public backend::IDocraftImageRenderingBackend {
        public:
            explicit ImageHaruBackend(MockRenderingBackend& backend) : backend_(backend) {}

            void draw_png_image(const std::string &path, float x, float y, float width, float height) const override {
                backend_.draw_png_image(path, x, y, width, height);
            }
            void draw_png_image_from_memory(
                const unsigned char *data,
                std::size_t size,
                float x,
                float y,
                float width,
                float height) const override {
                backend_.draw_png_image_from_memory(data, size, x, y, width, height);
            }
            void draw_jpeg_image(const std::string &path, float x, float y, float width, float height) const override {
                backend_.draw_jpeg_image(path, x, y, width, height);
            }
            void draw_jpeg_image_from_memory(
                const unsigned char *data,
                std::size_t size,
                float x,
                float y,
                float width,
                float height) const override {
                backend_.draw_jpeg_image_from_memory(data, size, x, y, width, height);
            }
            void draw_raw_rgb_image(
                const std::string &path,
                int pixel_width,
                int pixel_height,
                float x,
                float y,
                float width,
                float height) const override {
                backend_.draw_raw_rgb_image(path, pixel_width, pixel_height, x, y, width, height);
            }
            void draw_raw_rgb_image_from_memory(
                const unsigned char *data,
                int pixel_width,
                int pixel_height,
                float x,
                float y,
                float width,
                float height) const override {
                backend_.draw_raw_rgb_image_from_memory(data, pixel_width, pixel_height, x, y, width, height);
            }

        private:
            MockRenderingBackend& backend_;
        };

        class PageHaruBackend final : public backend::IDocraftPageRenderingBackend {
        public:
            explicit PageHaruBackend(MockRenderingBackend& backend) : backend_(backend) {}

            float page_width() const override { return backend_.page_width(); }
            float page_height() const override { return backend_.page_height(); }
            void add_new_page() override { backend_.add_new_page(); }
            void move_to_next_page() override { backend_.move_to_next_page(); }
            void go_to_page(std::size_t page_number) override { backend_.go_to_page(page_number); }
            void go_to_first_page() override { backend_.go_to_first_page(); }
            void go_to_previous_page() override { backend_.go_to_previous_page(); }
            void go_to_last_page() override { backend_.go_to_last_page(); }
            void set_page_format(model::DocraftPageSize size, model::DocraftPageOrientation orientation) override {
                backend_.set_page_format(size, orientation);
            }
            std::size_t current_page_number() const override { return backend_.current_page_number(); }
            std::size_t total_page_count() const override { return backend_.total_page_count(); }

        private:
            MockRenderingBackend& backend_;
        };

        struct Config {
            float page_width = 100.0F;
            float page_height = 100.0F;
            float text_width_factor = 5.0F;
            std::size_t initial_pages = 1;
            std::string extension = ".pdf";
            bool can_use_font = true;
        };

        MockRenderingBackend() : MockRenderingBackend(Config{}) {}

        explicit MockRenderingBackend(Config config)
            : config_(std::move(config)),
              edit_line_(std::make_shared<LineHaruBackend>(*this)),
              edit_shape_(std::make_shared<ShapeHaruBackend>(*this)),
              edit_text_(std::make_shared<TextHaruBackend>(*this)),
              edit_image_(std::make_shared<ImageHaruBackend>(*this)),
              edit_page_(std::make_shared<PageHaruBackend>(*this)) {
            pages_ = config_.initial_pages > 0 ? config_.initial_pages : 1;
            current_page_ = 0;
        }

        void begin_text() const {}
        void end_text() const {}
        void draw_text(const std::string &, float, float) const {}
        void set_text_color(float, float, float) const {}
        void draw_text_matrix(const std::string &, float, float, float, float, float, float) const {}
        float measure_text_width(const std::string &text) const {
            return static_cast<float>(text.size()) * config_.text_width_factor;
        }

        void set_stroke_color(float, float, float) const {}
        void set_line_width(float) const {}
        void draw_line(float, float, float, float) const { ++line_count; }

        void save_state() const {}
        void restore_state() const {}
        void set_fill_color(float, float, float) const {}
        void set_fill_alpha(float) const {}
        void set_stroke_alpha(float) const {}
        void draw_rectangle(float, float, float, float) const {}
        void draw_circle(float, float, float) const {}
        void draw_polygon(const std::vector<model::DocraftPoint> &) const {}
        void fill() const {}
        void stroke() const {}
        void fill_stroke() const {}

        void draw_png_image(const std::string &, float, float, float, float) const {}
        void draw_png_image_from_memory(const unsigned char *, std::size_t, float, float, float, float) const {}
        void draw_jpeg_image(const std::string &, float, float, float, float) const {}
        void draw_jpeg_image_from_memory(const unsigned char *, std::size_t, float, float, float, float) const {}
        void draw_raw_rgb_image(const std::string &, int, int, float, float, float, float) const {}
        void draw_raw_rgb_image_from_memory(const unsigned char *, int, int, float, float, float, float) const {}

        [[nodiscard]] const std::shared_ptr<backend::IDocraftLineRenderingBackend>& edit_line() const override {
            return edit_line_;
        }

        [[nodiscard]] const std::shared_ptr<backend::IDocraftShapeRenderingBackend>& edit_shape() const override {
            return edit_shape_;
        }

        [[nodiscard]] const std::shared_ptr<backend::IDocraftTextRenderingBackend>& edit_text() const override {
            return edit_text_;
        }

        [[nodiscard]] const std::shared_ptr<backend::IDocraftImageRenderingBackend>& edit_image() const override {
            return edit_image_;
        }

        [[nodiscard]] const std::shared_ptr<backend::IDocraftPageRenderingBackend>& edit_page() const override {
            return edit_page_;
        }

        void save_to_file(const std::string &path) const override { last_saved_path_ = path; }
        [[nodiscard]] std::string file_extension() const override { return config_.extension; }
        const char *register_ttf_font_from_file(const std::string &, bool) const override { return "Helvetica"; }
        bool can_use_font(const std::string &, const char *) const override { return config_.can_use_font; }
        void set_font(const std::string &, float, const char *) const override {}
        void set_document_metadata(const DocraftDocumentMetadata &) override {}

        float page_width() const { return config_.page_width; }
        float page_height() const { return config_.page_height; }

        void add_new_page() {
            ++pages_;
            current_page_ = pages_ - 1;
        }

        void move_to_next_page() {
            if (current_page_ + 1 >= pages_) {
                throw std::runtime_error("Already at the last page");
            }
            ++current_page_;
        }

        void go_to_page(std::size_t page_number) {
            if (page_number >= pages_) {
                throw std::runtime_error("Invalid page number");
            }
            current_page_ = page_number;
        }

        void go_to_first_page() {
            if (pages_ == 0) {
                throw std::runtime_error("No pages");
            }
            current_page_ = 0;
        }

        void go_to_previous_page() {
            if (current_page_ == 0) {
                throw std::runtime_error("Already at first page");
            }
            --current_page_;
        }

        void go_to_last_page() {
            if (pages_ == 0) {
                throw std::runtime_error("No pages");
            }
            current_page_ = pages_ - 1;
        }

        void set_page_format(model::DocraftPageSize, model::DocraftPageOrientation) {}

        std::size_t current_page_number() const { return current_page_ + 1; }
        std::size_t total_page_count() const { return pages_; }

        void set_current_page(std::size_t one_based_page_number) {
            current_page_ = one_based_page_number > 0 ? one_based_page_number - 1 : 0;
            if (current_page_ >= pages_) {
                current_page_ = pages_ - 1;
            }
        }

        mutable int line_count = 0;
        [[nodiscard]] const std::string &last_saved_path() const { return last_saved_path_; }

    private:
        Config config_;
        std::size_t pages_ = 1;
        std::size_t current_page_ = 0;
        mutable std::string last_saved_path_;
        std::shared_ptr<backend::IDocraftLineRenderingBackend> edit_line_;
        std::shared_ptr<backend::IDocraftShapeRenderingBackend> edit_shape_;
        std::shared_ptr<backend::IDocraftTextRenderingBackend> edit_text_;
        std::shared_ptr<backend::IDocraftImageRenderingBackend> edit_image_;
        std::shared_ptr<backend::IDocraftPageRenderingBackend> edit_page_;
    };
} // namespace docraft::test::utils
