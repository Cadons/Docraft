#include <gtest/gtest.h>

#include <vector>

#include "docraft_color.h"
#include "docraft_document_context.h"
#include "model/docraft_header.h"
#include "model/docraft_position.h"
#include "renderer/docraft_pdf_renderer.h"

namespace docraft::test::renderer {
    class TestBackend : public backend::IDocraftRenderingBackend {
    public:
        mutable int line_count = 0;

        void begin_text() const override {}
        void end_text() const override {}
        void draw_text(const std::string&, float, float) const override {}
        void set_text_color(float, float, float) const override {}
        void draw_text_matrix(const std::string&, float, float, float, float, float, float) const override {}
        float measure_text_width(const std::string&) const override { return 0.0F; }

        void set_stroke_color(float, float, float) const override {}
        void set_line_width(float) const override {}
        void draw_line(float, float, float, float) const override { line_count++; }

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

        float page_width() const override { return 100.0F; }
        float page_height() const override { return 100.0F; }
        void save_to_file(const std::string&) const override {}
        const char* register_ttf_font_from_file(const std::string&, bool) const override { return ""; }
        bool can_use_font(const std::string&, const char*) const override { return true; }
        void set_font(const std::string&, float, const char*) const override {}
    };

    TEST(DocraftSectionMarginTest, DrawsMarginLines) {
        auto backend = std::make_shared<TestBackend>();
        auto context = std::make_shared<DocraftDocumentContext>(backend);
        context->set_renderer(std::make_shared<docraft::renderer::DocraftPDFRenderer>(context));

        model::DocraftHeader header;
        header.set_position({.x = 0.0F, .y = 100.0F});
        header.set_width(100.0F);
        header.set_height(50.0F);
        header.set_margin_left(10.0F);
        header.set_margin_right(10.0F);
        header.set_margin_top(5.0F);
        header.set_margin_bottom(5.0F);
        header.set_border_color(DocraftColor(0.0F, 0.0F, 0.0F, 1.0F));
        header.set_border_width(1.0F);

        header.draw(context);

        EXPECT_EQ(backend->line_count, 4);
    }
} // namespace docraft::test::renderer
