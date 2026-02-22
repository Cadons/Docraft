#pragma once

#include <stdexcept>
#include <string>
#include <vector>

#include "backend/docraft_rendering_backend.h"

namespace docraft::test::utils {
    class MockRenderingBackend : public backend::IDocraftRenderingBackend {
    public:
        struct Config {
            float page_width = 100.0F;
            float page_height = 100.0F;
            float text_width_factor = 5.0F;
            std::size_t initial_pages = 1;
            std::string extension = ".pdf";
            bool can_use_font = true;
        };

        MockRenderingBackend() : MockRenderingBackend(Config{}) {}

        explicit MockRenderingBackend(Config config) : config_(std::move(config)) {
            pages_ = config_.initial_pages > 0 ? config_.initial_pages : 1;
            current_page_ = 0;
        }

        void begin_text() const override {}
        void end_text() const override {}
        void draw_text(const std::string &, float, float) const override {}
        void set_text_color(float, float, float) const override {}
        void draw_text_matrix(const std::string &, float, float, float, float, float, float) const override {}
        float measure_text_width(const std::string &text) const override {
            return static_cast<float>(text.size()) * config_.text_width_factor;
        }

        void set_stroke_color(float, float, float) const override {}
        void set_line_width(float) const override {}
        void draw_line(float, float, float, float) const override { ++line_count; }

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

        void draw_png_image(const std::string &, float, float, float, float) const override {}
        void draw_png_image_from_memory(const unsigned char *, std::size_t, float, float, float, float) const override {}
        void draw_jpeg_image(const std::string &, float, float, float, float) const override {}
        void draw_jpeg_image_from_memory(const unsigned char *, std::size_t, float, float, float, float) const override {}
        void draw_raw_rgb_image(const std::string &, int, int, float, float, float, float) const override {}
        void draw_raw_rgb_image_from_memory(const unsigned char *, int, int, float, float, float, float) const override {}

        void save_to_file(const std::string &) const override {}
        [[nodiscard]] std::string file_extension() const override { return config_.extension; }
        const char *register_ttf_font_from_file(const std::string &, bool) const override { return "Helvetica"; }
        bool can_use_font(const std::string &, const char *) const override { return config_.can_use_font; }
        void set_font(const std::string &, float, const char *) const override {}
        void set_document_metadata(const DocraftDocumentMetadata &) override {}

        float page_width() const override { return config_.page_width; }
        float page_height() const override { return config_.page_height; }

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

        void set_current_page(std::size_t one_based_page_number) {
            current_page_ = one_based_page_number > 0 ? one_based_page_number - 1 : 0;
            if (current_page_ >= pages_) {
                current_page_ = pages_ - 1;
            }
        }

        mutable int line_count = 0;

    private:
        Config config_;
        std::size_t pages_ = 1;
        std::size_t current_page_ = 0;
    };
} // namespace docraft::test::utils
