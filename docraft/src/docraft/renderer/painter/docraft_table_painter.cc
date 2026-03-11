#include "docraft/renderer/painter/docraft_table_painter.h"
#include <print>
#include "docraft/backend/docraft_line_rendering_backend.h"

#include "docraft/renderer/painter/docraft_image_painter.h"
#include "docraft/utils/docraft_logger.h"

namespace docraft::renderer::painter {
    DocraftTablePainter::DocraftTablePainter(const model::DocraftTable &table_node) : table_node_(table_node) {
    }



    void DocraftTablePainter::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
         if (!context) return;
         auto line_backend = context->line_backend();
         auto shape_backend = context->shape_backend();
         if (!line_backend) return;
         line_backend->set_stroke_color(0.0F, 0.0F, 0.0F);
         float start_x = table_node_.position().x;
         float start_y = table_node_.position().y;
         float table_width = table_node_.width();
         float table_height = table_node_.height();
         auto draw_background = [&](const std::optional<docraft::DocraftColor> &color,
                                    float x, float y, float width, float height) {
             if (!shape_backend || !color.has_value()) {
                 return;
             }
             const auto &rgba = color->toRGB();
             if (rgba.a <= 0.0F) {
                 return;
             }
             shape_backend->save_state();
             shape_backend->set_fill_color(rgba.r, rgba.g, rgba.b);
             if (rgba.a < 1.0F) {
                 shape_backend->set_fill_alpha(rgba.a);
             }
             shape_backend->draw_rectangle(x, y, width, height);
             shape_backend->fill();
             shape_backend->restore_state();
         };

         // draw backgrounds first (rows, titles, cells)
         if (table_node_.orientation() == model::LayoutOrientation::kHorizontal) {
             // Title backgrounds
             const auto &title_bgs = table_node_.title_backgrounds();
             for (std::size_t i = 0; i < table_node_.title_nodes().size(); ++i) {
                 const auto &title = table_node_.title_nodes()[i];
                 if (!title) continue;
                 const auto &anchors = title->anchors();
                 const std::optional<docraft::DocraftColor> bg =
                     (i < title_bgs.size()) ? title_bgs[i] : std::nullopt;
                 draw_background(bg, anchors.bottom_left.x, anchors.bottom_left.y, title->width(), title->height());
             }
         } else {
             // Header title backgrounds (vertical)
             const auto &htitle_bgs = table_node_.htitle_backgrounds();
             for (std::size_t i = 0; i < table_node_.htitle_nodes().size(); ++i) {
                 const auto &title = table_node_.htitle_nodes()[i];
                 if (!title) continue;
                 const auto &anchors = title->anchors();
                 const std::optional<docraft::DocraftColor> bg =
                     (i < htitle_bgs.size()) ? htitle_bgs[i] : std::nullopt;
                 draw_background(bg, anchors.bottom_left.x, anchors.bottom_left.y, title->width(), title->height());
             }
             // Row title backgrounds (vertical)
             const auto &title_bgs = table_node_.title_backgrounds();
             for (std::size_t i = 0; i < table_node_.title_nodes().size(); ++i) {
                 const auto &title = table_node_.title_nodes()[i];
                 if (!title) continue;
                 const auto &anchors = title->anchors();
                 const std::optional<docraft::DocraftColor> bg =
                     (i < title_bgs.size()) ? title_bgs[i] : std::nullopt;
                 draw_background(bg, anchors.bottom_left.x, anchors.bottom_left.y, title->width(), title->height());
             }
         }

         // Row and cell backgrounds (body)
         const auto &row_bgs = table_node_.row_backgrounds();
         const auto &cell_bgs = table_node_.content_backgrounds();
         const auto &default_cell_bg = table_node_.default_cell_background();
         const auto content_rows = table_node_.content_nodes();
         int content_cols = table_node_.content_cols();

         for (std::size_t r = 0; r < content_rows.size(); ++r) {
             const auto &row = content_rows[r];
             // Row background
             if (r < row_bgs.size() && row_bgs[r].has_value()) {
                 const auto &row_bg = row_bgs[r];
                 const model::DocraftNode *row_ref = nullptr;
                 if (table_node_.orientation() == model::LayoutOrientation::kVertical) {
                     if (r < table_node_.title_nodes().size()) {
                         row_ref = table_node_.title_nodes()[r].get();
                     }
                 } else {
                     for (const auto &cell : row) {
                         if (cell) {
                             row_ref = cell.get();
                             break;
                         }
                     }
                 }
                 if (row_ref) {
                     const float row_height = row_ref->height();
                     const float bottom_y = row_ref->anchors().bottom_left.y;
                     draw_background(row_bg, start_x, bottom_y, table_width, row_height);
                 }
             }

             // Cell backgrounds
             for (std::size_t c = 0; c < row.size(); ++c) {
                 const std::size_t flat_index = (r * static_cast<std::size_t>(content_cols)) + c;
                 if (!row[c]) continue;
                 std::optional<docraft::DocraftColor> bg = std::nullopt;
                 if (flat_index < cell_bgs.size() && cell_bgs[flat_index].has_value()) {
                     bg = cell_bgs[flat_index];
                 } else if (r < row_bgs.size() && row_bgs[r].has_value()) {
                     bg = row_bgs[r];
                 } else if (default_cell_bg.has_value()) {
                     bg = default_cell_bg;
                 }
                 if (bg.has_value()) {
                     const auto &anchors = row[c]->anchors();
                     draw_background(bg, anchors.bottom_left.x, anchors.bottom_left.y,
                                     row[c]->width(), row[c]->height());
                 }
             }
         }

         // Draw grid lines and borders
         // Top border
         line_backend->draw_line(
             table_node_.anchors().top_left.x,
             table_node_.anchors().top_left.y,
             table_node_.anchors().top_right.x,
             table_node_.anchors().top_right.y);
         // Left border
         line_backend->draw_line(
             table_node_.anchors().bottom_left.x,
             table_node_.anchors().bottom_left.y,
             table_node_.anchors().top_left.x,
             table_node_.anchors().top_left.y);
         // Right border
         line_backend->draw_line(
             table_node_.anchors().top_right.x,
             table_node_.anchors().top_right.y,
             table_node_.anchors().bottom_right.x,
             table_node_.anchors().bottom_right.y);
         // Bottom border for vertical (horizontal draws through row lines)
         if (table_node_.orientation() != model::LayoutOrientation::kHorizontal) {
             line_backend->draw_line(
                 table_node_.anchors().bottom_right.x,
                 table_node_.anchors().bottom_right.y,
                 table_node_.anchors().bottom_left.x,
                 table_node_.anchors().bottom_left.y);
         }

         if (table_node_.orientation() == model::LayoutOrientation::kHorizontal &&
             !table_node_.title_nodes().empty()) {
             for (const auto &title: table_node_.title_nodes()) {
                 // Line of columns
                 line_backend->draw_line(
                     title->anchors().top_left.x,
                     table_node_.anchors().top_left.y,
                     title->anchors().top_left.x,
                     table_node_.anchors().bottom_left.y);
             }
             float line_y = table_node_.title_nodes().front()->anchors().bottom_left.y;
             line_backend->draw_line(
                 start_x,
                 line_y,
                 start_x + table_width,
                 line_y); // Line below titles
         }

         if (table_node_.orientation() == model::LayoutOrientation::kVertical) {
             // Vertical column separators (use header titles or first row cells)
             if (!table_node_.htitle_nodes().empty()) {
                 for (const auto &title : table_node_.htitle_nodes()) {
                     line_backend->draw_line(
                         title->anchors().top_left.x,
                         table_node_.anchors().top_left.y,
                         title->anchors().top_left.x,
                         table_node_.anchors().bottom_left.y);
                 }
                 const float header_line_y = table_node_.htitle_nodes().front()->anchors().bottom_left.y;
                 line_backend->draw_line(start_x, header_line_y, start_x + table_width, header_line_y);
             } else if (!content_rows.empty() && !content_rows.front().empty() && content_rows.front().front()) {
                 for (const auto &cell : content_rows.front()) {
                     if (!cell) continue;
                     line_backend->draw_line(
                         cell->anchors().top_left.x,
                         table_node_.anchors().top_left.y,
                         cell->anchors().top_left.x,
                         table_node_.anchors().bottom_left.y);
                 }
             }
         }

         // Draw content nodes (text/images) and row lines
         // compute boundaries for horizontal tables (column titles define columns)
         std::vector<float> col_lefts;
         if (table_node_.orientation() == model::LayoutOrientation::kHorizontal) {
             for (const auto &title : table_node_.title_nodes()) {
                 const auto &tb = title->anchors();
                 col_lefts.push_back(tb.top_left.x);
             }
         }

         float content_top = start_y;
         if (table_node_.orientation() == model::LayoutOrientation::kHorizontal &&
             !table_node_.title_nodes().empty()) {
             content_top = table_node_.title_nodes().front()->anchors().bottom_left.y;
         } else if (table_node_.orientation() == model::LayoutOrientation::kVertical &&
                    !table_node_.htitle_nodes().empty()) {
             content_top = table_node_.htitle_nodes().front()->anchors().bottom_left.y;
         }

         for (std::size_t row_index = 0; row_index < content_rows.size(); ++row_index) {
             const auto &content_row = content_rows[row_index];
             float bottom_y = content_top;
             const size_t col_limit = col_lefts.empty()
                                          ? content_row.size()
                                          : std::min(content_row.size(), col_lefts.size());
             for (size_t col_idx = 0; col_idx < col_limit; ++col_idx) {
                 if (!content_row[col_idx]) {
                     continue;
                 }
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
             LOG_DEBUG(std::format("Drawing horizontal line at y={}", line_y_here));
             line_backend->draw_line(
                 start_x,
                 line_y_here,
                 start_x + table_width,
                 line_y_here);
             content_top = line_y_here;
       }

         // Draw titles last for clarity
         if (table_node_.orientation() == model::LayoutOrientation::kHorizontal &&
             !table_node_.title_nodes().empty()) {
             for (const auto &title: table_node_.title_nodes()) {
                 title->draw(context);
             }
         }
         if (table_node_.orientation() == model::LayoutOrientation::kVertical) {
             for (const auto &title: table_node_.title_nodes()) {
                 title->draw(context);
             }
             for (const auto &title: table_node_.htitle_nodes()) {
                 title->draw(context);
             }
         }

    }
} // docraft
