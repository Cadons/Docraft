#include "layout/handler/docraft_layout_table_handler.h"

#include <numeric>
#include <print>

#include "layout/docraft_layout_engine.h"
#include "model/docraft_text.h"

namespace docraft::layout::handler {

void DocraftLayoutTableHandler::compute(const std::shared_ptr<model::DocraftTable> &node, model::DocraftTransform* box) {
   //  // create table nodes and setup the layout
   //  float height = node->height();
   //  float width = node->width();
   //  set_node_position(node);
   //  const float fixed_y = context()->cursor().y();
   //  const float fixed_x = context()->cursor().x();
   //
   //  // --- first pass: create title nodes and measure natural sizes ---
   //  float title_row_height = 0;
   //  std::vector<float> natural_widths;
   //  natural_widths.reserve(node->titles().size());
   //
   //  for (const auto &title : node->titles()) {
   //      auto title_node = std::make_shared<model::DocraftText>();
   //      title_node->set_text(title);
   //      docraft::layout::DocraftLayoutEngine::layout(title_node, context());
   //      node->add_title_node(title_node);
   //
   //      natural_widths.push_back(title_node->width());
   //      height = std::max(height, title_node->height());
   //      width += title_node->width();
   //      title_row_height = std::max(title_row_height, title_node->height());
   //  }
   //
   //  // --- compute available width and column weights ---
   //  const size_t cols = node->titles().size();
   //  float available_width;
   //  if (node->auto_fill_width()) {
   //     // available_width = context()->current_rect_width() - context()->cursor().offset_x();
   //  } else {
   //      available_width = width > 0.0f ? width : std::accumulate(natural_widths.begin(), natural_widths.end(), 0.0f);
   //  }
   //
   //  // get weights from node (if not provided, use equal weights)
   //  std::vector<float> weights = node->column_weights(); // assume this exists; if empty -> equal
   //  if (weights.size() != cols) {
   //      weights.assign(cols, 1.0f);
   //  }
   //
   //  float total_weight = std::accumulate(weights.begin(), weights.end(), 0.0f);
   //  if (total_weight <= 0.0f) {
   //      weights.assign(cols, 1.0f);
   //      total_weight = static_cast<float>(cols);
   //  }
   //
   //  // compute final column widths (respect natural minima)
   //  std::vector<float> col_widths(cols);
   //  float widths_sum = 0.0f;
   //  for (size_t i = 0; i < cols; ++i) {
   //      float w = available_width * (weights[i] / total_weight);
   //      col_widths[i] = std::max(natural_widths[i], w);
   //      widths_sum += col_widths[i];
   //  }
   //
   //  // If widths_sum differs from available_width (because of minima), optionally scale down/up:
   //  if (widths_sum != available_width && widths_sum > 0.0f) {
   //      // distribute adjustment proportionally to weights (preserve minima)
   //      float scale = available_width / widths_sum;
   //      for (size_t i = 0; i < cols; ++i) {
   //          col_widths[i] *= scale;
   //      }
   //      widths_sum = available_width;
   //  }
   //
   //  // --- position title nodes using computed column widths ---
   //  float current_x = fixed_x;
   //  for (size_t i = 0; i < node->title_nodes().size(); i++) {
   //      auto &title_node = node->title_nodes()[i];
   //      title_node->set_x(current_x);
   //      title_node->set_y(fixed_y+title_row_height);
   //      title_node->set_width(col_widths[i]);
   //      // keep measured height, but ensure transform box is set after updating size/pos
   //      set_node_transform_box(title_node);
   //      current_x += col_widths[i];
   //  }
   //
   //  // --- layout content rows using column lefts ---
   //  // recompute row_height including any content measured below
   //  float content_row_height = title_row_height;
   //
   //  // measure content nodes (first layout pass already happened for content in original code;
   //  // keep measuring here to find max row height)
   //  for (const auto &content_node : node->content_nodes()) {
   //      for (const auto &cell : content_node) {
   //          docraft::layout::DocraftLayoutEngine::layout(cell, context());
   //          content_row_height = std::max(content_row_height, cell->height());
   //      }
   //  }
   //
   //  // compute column left positions
   //  std::vector<float> col_lefts(cols), col_rights(cols);
   //  float acc_x = fixed_x;
   //  for (size_t i = 0; i < cols; ++i) {
   //      col_lefts[i] = acc_x;
   //      col_rights[i] = acc_x + col_widths[i];
   //      acc_x += col_widths[i];
   //  }
   //
   //  // top Y for first content row (below titles)
   //  float content_top = fixed_y  - title_row_height;
   //  constexpr float kLineOffset = 3.0F;
   //  content_top -= kLineOffset;
   //  // place content cells row by row
   //  size_t row_idx = 0;
   //  float final_height = 0;
   //  for (const auto &content_row : node->content_nodes()) {
   //      content_row_height = content_row[0]->height();
   //      for (size_t col_idx = 0; col_idx < std::min(content_row.size(), cols); ++col_idx) {
   //          auto cell = content_row[col_idx];
   //          cell->set_x(col_lefts[col_idx]);
   //          // place cell so its top aligns with content_top - row_idx * content_row_height
   //          float top_y = content_top;
   //          cell->set_y(top_y);
   //          cell->set_width(col_widths[col_idx]);
   //          content_row_height = std::max(content_row_height, cell->height());
   //      }
   //      //update all boxes with the heighest height
   //      for (size_t col_idx = 0; col_idx < std::min(content_row.size(), cols); ++col_idx) {
   //          auto cell = content_row[col_idx];
   //          cell->set_height(content_row_height);
   //          set_node_transform_box(cell);
   //      }
   //      final_height += content_row_height;
   //      content_top -= content_row_height;
   //      ++row_idx;
   //  }
   //
   //  // --- finalize table size and transform box ---
   //  node->set_width(widths_sum);
   //  set_node_transform_box(node);
   //  auto last_content_node = node->content_nodes().back().back();
   // // auto new_box = model::DocraftTransform{
   // //     .top_left = {.x=fixed_x, .y=fixed_y + title_row_height},
   // //     .top_right = {.x=fixed_x + widths_sum, .y=fixed_y + title_row_height},
   // //     .bottom_left = {.x=fixed_x, .y=last_content_node->transform_box().bottom_left_.y},
   // //     .bottom_right = {.x=fixed_x + widths_sum, .y=last_content_node->transform_box().bottom_left_.y}
   // // };
   // // node->set_transform_box(new_box);
   //  node->set_height(node->transform_box().top_left_.y - node->transform_box().bottom_left_.y-title_row_height);
}


    bool DocraftLayoutTableHandler::handle(const std::shared_ptr<model::DocraftNode>& request, model::DocraftTransform * /*result*/) {
        if (auto table_node = std::dynamic_pointer_cast<model::DocraftTable>(request)) {
            compute(table_node, {});
            return true;
        }
        return false;
    }
} // docraft
