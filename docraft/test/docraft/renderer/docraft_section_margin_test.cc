#include <gtest/gtest.h>

#include "docraft/docraft_color.h"
#include "docraft/docraft_document_context.h"
#include "docraft/model/docraft_header.h"
#include "docraft/model/docraft_position.h"
#include "docraft/renderer/docraft_pdf_renderer.h"
#include "docraft/utils/docraft_mock_rendering_backend.h"

namespace docraft::test::renderer {
    TEST(DocraftSectionMarginTest, DrawsMarginLines) {
        auto backend = std::make_shared<docraft::test::utils::MockRenderingBackend>(
            docraft::test::utils::MockRenderingBackend::Config{
                .page_width = 100.0F,
                .page_height = 100.0F,
                .text_width_factor = 0.0F,
                .initial_pages = 1,
                .extension = ".pdf",
                .can_use_font = true
            });
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
