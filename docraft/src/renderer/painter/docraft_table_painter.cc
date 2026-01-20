#include "renderer/painter/docraft_table_painter.h"
#include <hpdf.h>
#include <print>

#include "generic/docraft_font_applier.h"
#include "renderer/docraft_pdf_renderer.h"
#include "renderer/painter/docraft_image_painter.h"

namespace docraft::renderer::painter {
    DocraftTablePainter::DocraftTablePainter(const model::DocraftTable &table_node) : table_node_(table_node) {
    }



    void DocraftTablePainter::draw(const std::shared_ptr<DocraftPDFContext> &context) {
        // auto *page = context->page();
        // float start_x = table_node_.x();
        // float start_y = table_node_.y();
        // float table_width = table_node_.width();
        // float table_height = table_node_.height();
        //draw lines around
        // DocraftPDFRenderer::draw_line(page, table_node_.transform_box().top_left_, table_node_.transform_box().top_right_); // Top line
        // DocraftPDFRenderer::draw_line(page, table_node_.transform_box().bottom_left_, table_node_.transform_box().top_left_); // Left line
        // DocraftPDFRenderer::draw_line(page, table_node_.transform_box().top_right_, table_node_.transform_box().bottom_right_); // Right line
        // if (table_node_.orientation()!=model::LayoutOrientation::kHorizontal) {
        //     DocraftPDFRenderer::draw_line(page, table_node_.transform_box().bottom_right_, table_node_.transform_box().bottom_left_);
        // }
        // //Bottom line
        // // //draw titles
        // for (const auto &title: table_node_.title_nodes()) {
        //
        //     title->set_style(model::TextStyle::kBold); // reset to normal for table content
        //     context->font_applier()->apply_font(title);
        //     HPDF_Page_BeginText(page);
        //     HPDF_Page_TextRect(page, title->transform_box().top_left_.x, title->transform_box().bottom_left_.y,
        //                        title->transform_box().top_right_.x, title->transform_box().top_right_.y,
        //                        title->text().c_str(), HPDF_TALIGN_CENTER, nullptr);
        //     HPDF_Page_EndText(page);
        //     //Line of columns
        //     DocraftPDFRenderer::draw_line(page, {title->transform_box().top_left_.x, table_node_.transform_box().top_left_.y}, {
        //                   title->transform_box().top_left_.x, table_node_.transform_box().bottom_left_.y
        //               });
        // }
        // constexpr float kOffset = 1.2F;
        // float line_y = start_y - (kOffset * table_node_.title_nodes().front()->height());
        // DocraftPDFRenderer::draw_line(page,
        //           {start_x, line_y},
        //           {
        //               start_x + table_width, line_y
        //           }); // Line below titles
        // //draw content nodes
        // const float row_height = table_node_.title_nodes().front()->height();
        // float current_y = line_y;
        //
        // //compute boundaries
        // std::vector<float> col_lefts;
        // std::vector<float> col_rights;
        // for (const auto &title : table_node_.title_nodes()) {
        //     const auto &tb = title->transform_box();
        //     col_lefts.push_back(tb.top_left_.x);
        //     col_rights.push_back(tb.top_right_.x);
        // }
        //
        // float content_top = current_y; // y at top of first content row
        // size_t row_idx = 0;
        // for (const auto &content_row : table_node_.content_nodes()) {
        //     // draw each column cell text for this row
        //     float bottom_y = content_top - row_height;
        //     for (size_t col_idx = 0; col_idx < std::min(content_row.size(), col_lefts.size()); ++col_idx) {
        //         if (auto pointer = std::dynamic_pointer_cast<model::DocraftText>(content_row[col_idx])) {
        //             context->font_applier()->apply_font(pointer);
        //             HPDF_Page_BeginText(page);
        //             HPDF_Page_TextRect(page, pointer->transform_box().top_left_.x, pointer->transform_box().top_left_.y, pointer->transform_box().top_right_.x, pointer->transform_box().bottom_left_.y,
        //               pointer->text().c_str(), HPDF_TALIGN_CENTER, nullptr);
        //             HPDF_Page_EndText(page);
        //         }
        //         else if (auto img_pointer = std::dynamic_pointer_cast<model::DocraftImage>(content_row[col_idx])) {
        //             DocraftImagePainter img_painter(*img_pointer);
        //             img_painter.draw(context);
        //         }
        //         bottom_y = std::min(bottom_y, content_row[col_idx]->transform_box().bottom_left_.y);
        //     }
        //
        //     // draw horizontal line at bottom of this row
            // float line_y_here =  bottom_y;
            // std::print("Drawing line at y: {}\n", line_y_here);
            // DocraftPDFRenderer::draw_line(page, { .x=start_x, .y=line_y_here }, { .x=start_x + table_width, .y=line_y_here });
            //
            // ++row_idx;
      // }

    }
} // docraft
