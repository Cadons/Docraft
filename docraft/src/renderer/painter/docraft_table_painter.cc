#include "renderer/painter/docraft_table_painter.h"
#include <hpdf.h>
#include <print>

#include "generic/docraft_font_applier.h"
#include "renderer/docraft_pdf_renderer.h"
#include "renderer/painter/docraft_image_painter.h"

namespace docraft::renderer::painter {
    DocraftTablePainter::DocraftTablePainter(const model::DocraftTable &table_node) : table_node_(table_node) {
    }



    void DocraftTablePainter::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
         auto *page = context->page();
         float start_x = table_node_.position().x;
         float start_y = table_node_.position().y;
         float table_width = table_node_.width();
         float table_height = table_node_.height();
            //draw border
         DocraftPDFRenderer::draw_line(page, table_node_.anchors().top_left, table_node_.anchors().top_right); // Top line
         DocraftPDFRenderer::draw_line(page, table_node_.anchors().bottom_left, table_node_.anchors().top_left); // Left line
         DocraftPDFRenderer::draw_line(page, table_node_.anchors().top_right, table_node_.anchors().bottom_right); // Right line
         if (table_node_.orientation()!=model::LayoutOrientation::kHorizontal) {
             DocraftPDFRenderer::draw_line(page, table_node_.anchors().bottom_right, table_node_.anchors().bottom_left);
         }
         //draw titles
         if (table_node_.orientation() == model::LayoutOrientation::kHorizontal &&
             !table_node_.title_nodes().empty()) {
             for (const auto &title: table_node_.title_nodes()) {
                 title->draw(context);
                 // Line of columns
                 DocraftPDFRenderer::draw_line(page,
                     {title->anchors().top_left.x, table_node_.anchors().top_left.y},
                     {title->anchors().top_left.x, table_node_.anchors().bottom_left.y});
             }
             float line_y = table_node_.title_nodes().front()->anchors().bottom_left.y;
             DocraftPDFRenderer::draw_line(page,
                 {start_x, line_y},
                 {start_x + table_width, line_y}); // Line below titles
         }
         //draw content nodes

         //compute boundaries for horizontal tables (column titles define columns)
         std::vector<float> col_lefts;
         std::vector<float> col_rights;
         if (table_node_.orientation() == model::LayoutOrientation::kHorizontal) {
             for (const auto &title : table_node_.title_nodes()) {
                 const auto &tb = title->anchors();
                 col_lefts.push_back(tb.top_left.x);
                 col_rights.push_back(tb.top_right.x);
             }
         }

         float content_top = start_y;
         if (table_node_.orientation() == model::LayoutOrientation::kHorizontal &&
             !table_node_.title_nodes().empty()) {
             content_top = table_node_.title_nodes().front()->anchors().bottom_left.y;
         }
         for (const auto &content_row : table_node_.content_nodes()) {
             // draw each column cell text for this row
             float bottom_y = content_top;
             const size_t col_limit = col_lefts.empty()
                                          ? content_row.size()
                                          : std::min(content_row.size(), col_lefts.size());
             for (size_t col_idx = 0; col_idx < col_limit; ++col_idx) {
                 if (auto pointer = std::dynamic_pointer_cast<model::DocraftText>(content_row[col_idx])) {
                     pointer->draw(context);
                 } else if (auto img_pointer = std::dynamic_pointer_cast<model::DocraftImage>(content_row[col_idx])) {
                     DocraftImagePainter img_painter(*img_pointer);
                     img_painter.draw(context);
                 }
                 bottom_y = std::min(bottom_y, content_row[col_idx]->anchors().bottom_left.y);
             }

             // draw horizontal line at bottom of this row
             float line_y_here =  bottom_y;
             std::print("Drawing line at y: {}\n", line_y_here);
             DocraftPDFRenderer::draw_line(page, { .x=start_x, .y=line_y_here }, { .x=start_x + table_width, .y=line_y_here });
             content_top = line_y_here;
       }

    }
} // docraft
