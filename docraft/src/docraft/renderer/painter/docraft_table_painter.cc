/*
 * Copyright 2026 Matteo Cadoni (https://github.com/cadons)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "docraft/renderer/painter/docraft_table_painter.h"
#include "docraft/backend/docraft_line_rendering_backend.h"

#include <fmt/format.h>

#include "docraft/renderer/painter/docraft_image_painter.h"
#include "docraft/utils/docraft_logger.h"

namespace docraft::renderer::painter {
    namespace {
        using ShapeBackend = std::shared_ptr<const backend::IDocraftShapeRenderingBackend>;
        using LineBackend = std::shared_ptr<const backend::IDocraftLineRenderingBackend>;

        void draw_background_if_present(const ShapeBackend &shape_backend,
                                        const std::optional<docraft::DocraftColor> &color,
                                        const float x,
                                        const float y,
                                        const float width,
                                        const float height) {
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
        }

        template<typename TNodes, typename TBgs>
        void draw_title_backgrounds(const TNodes &nodes,
                                    const TBgs &backgrounds,
                                    const ShapeBackend &shape_backend) {
            for (std::size_t i = 0; i < nodes.size(); ++i) {
                const auto &node = nodes[i];
                if (!node) {
                    continue;
                }

                const auto &anchors = node->anchors();
                const std::optional<docraft::DocraftColor> bg =
                        (i < backgrounds.size()) ? backgrounds[i] : std::nullopt;
                draw_background_if_present(shape_backend, bg, anchors.bottom_left.x, anchors.bottom_left.y,
                                           node->width(), node->height());
            }
        }

        const model::DocraftNode *first_non_null_cell(const std::vector<std::shared_ptr<model::DocraftNode> > &row) {
            for (const auto &cell: row) {
                if (cell) {
                    return cell.get();
                }
            }
            return nullptr;
        }

        template<typename TTitleNodes>
        const model::DocraftNode *row_reference_for_background(const model::DocraftTable &table,
                                                               const std::vector<std::shared_ptr<model::DocraftNode> > &
                                                               row,
                                                               const std::size_t row_index,
                                                               const TTitleNodes &title_nodes) {
            if (table.orientation() == model::LayoutOrientation::kVertical) {
                if (row_index < title_nodes.size() && title_nodes[row_index]) {
                    return title_nodes[row_index].get();
                }
                return nullptr;
            }
            return first_non_null_cell(row);
        }

        void draw_row_backgrounds(const model::DocraftTable &table,
                                  const ShapeBackend &shape_backend,
                                  const float start_x,
                                  const float table_width,
                                  const std::vector<std::shared_ptr<model::DocraftNode> > &title_nodes,
                                  const std::vector<std::vector<std::shared_ptr<model::DocraftNode> > > &content_rows,
                                  const std::vector<std::optional<docraft::DocraftColor> > &row_bgs) {
            for (std::size_t r = 0; r < content_rows.size(); ++r) {
                if (r >= row_bgs.size() || !row_bgs[r].has_value()) {
                    continue;
                }

                const auto *const row_ref = row_reference_for_background(table, content_rows[r], r, title_nodes);
                if (!row_ref) {
                    continue;
                }

                draw_background_if_present(shape_backend, row_bgs[r], start_x, row_ref->anchors().bottom_left.y,
                                           table_width, row_ref->height());
            }
        }

        void draw_cell_backgrounds(const std::vector<std::vector<std::shared_ptr<model::DocraftNode> > > &content_rows,
                                   const std::vector<std::optional<docraft::DocraftColor> > &row_bgs,
                                   const std::vector<std::optional<docraft::DocraftColor> > &cell_bgs,
                                   const std::optional<docraft::DocraftColor> &default_cell_bg,
                                   const std::size_t safe_content_cols,
                                   const ShapeBackend &shape_backend) {
            for (std::size_t r = 0; r < content_rows.size(); ++r) {
                const auto &row = content_rows[r];
                for (std::size_t c = 0; c < row.size(); ++c) {
                    if (!row[c]) {
                        continue;
                    }

                    const std::size_t flat_index = safe_content_cols > 0 ? ((r * safe_content_cols) + c) : c;
                    std::optional<docraft::DocraftColor> bg = std::nullopt;
                    if (flat_index < cell_bgs.size() && cell_bgs[flat_index].has_value()) {
                        bg = cell_bgs[flat_index];
                    } else if (r < row_bgs.size() && row_bgs[r].has_value()) {
                        bg = row_bgs[r];
                    } else if (default_cell_bg.has_value()) {
                        bg = default_cell_bg;
                    }

                    if (!bg.has_value()) {
                        continue;
                    }

                    const auto &anchors = row[c]->anchors();
                    draw_background_if_present(shape_backend, bg, anchors.bottom_left.x, anchors.bottom_left.y,
                                               row[c]->width(), row[c]->height());
                }
            }
        }

        void draw_outer_borders(const model::DocraftTable &table_node, const LineBackend &line_backend) {
            line_backend->draw_line(table_node.anchors().top_left.x,
                                    table_node.anchors().top_left.y,
                                    table_node.anchors().top_right.x,
                                    table_node.anchors().top_right.y);
            line_backend->draw_line(table_node.anchors().bottom_left.x,
                                    table_node.anchors().bottom_left.y,
                                    table_node.anchors().top_left.x,
                                    table_node.anchors().top_left.y);
            line_backend->draw_line(table_node.anchors().top_right.x,
                                    table_node.anchors().top_right.y,
                                    table_node.anchors().bottom_right.x,
                                    table_node.anchors().bottom_right.y);
            if (table_node.orientation() != model::LayoutOrientation::kHorizontal) {
                line_backend->draw_line(table_node.anchors().bottom_right.x,
                                        table_node.anchors().bottom_right.y,
                                        table_node.anchors().bottom_left.x,
                                        table_node.anchors().bottom_left.y);
            }
        }

        void draw_horizontal_table_dividers(const model::DocraftTable &table_node,
                                            const std::vector<std::shared_ptr<model::DocraftNode> > &title_nodes,
                                            const float start_x,
                                            const float table_width,
                                            const LineBackend &line_backend) {
            if (title_nodes.empty()) {
                return;
            }

            for (const auto &title: title_nodes) {
                if (!title) {
                    continue;
                }
                line_backend->draw_line(title->anchors().top_left.x,
                                        table_node.anchors().top_left.y,
                                        title->anchors().top_left.x,
                                        table_node.anchors().bottom_left.y);
            }

            for (const auto &title: title_nodes) {
                if (!title) {
                    continue;
                }
                const float line_y = title->anchors().bottom_left.y;
                line_backend->draw_line(start_x, line_y, start_x + table_width, line_y);
                break;
            }
        }

        void draw_vertical_table_dividers(const model::DocraftTable &table_node,
                                          const std::vector<std::shared_ptr<model::DocraftText> > &htitle_nodes,
                                          const std::vector<std::vector<std::shared_ptr<model::DocraftNode> > > &
                                          content_rows,
                                          const float start_x,
                                          const float table_width,
                                          const LineBackend &line_backend) {
            if (!htitle_nodes.empty()) {
                for (const auto &title: htitle_nodes) {
                    if (!title) {
                        continue;
                    }
                    line_backend->draw_line(title->anchors().top_left.x,
                                            table_node.anchors().top_left.y,
                                            title->anchors().top_left.x,
                                            table_node.anchors().bottom_left.y);
                }

                for (const auto &title: htitle_nodes) {
                    if (!title) {
                        continue;
                    }
                    const float header_line_y = title->anchors().bottom_left.y;
                    line_backend->draw_line(start_x, header_line_y, start_x + table_width, header_line_y);
                    break;
                }
                return;
            }

            if (content_rows.empty() || content_rows.front().empty() || !content_rows.front().front()) {
                return;
            }

            for (const auto &cell: content_rows.front()) {
                if (!cell) {
                    continue;
                }
                line_backend->draw_line(cell->anchors().top_left.x,
                                        table_node.anchors().top_left.y,
                                        cell->anchors().top_left.x,
                                        table_node.anchors().bottom_left.y);
            }
        }

        std::vector<float> collect_horizontal_column_lefts(
            const std::vector<std::shared_ptr<model::DocraftNode> > &title_nodes) {
            std::vector<float> col_lefts;
            for (const auto &title: title_nodes) {
                if (!title) {
                    continue;
                }
                col_lefts.push_back(title->anchors().top_left.x);
            }
            return col_lefts;
        }

        float resolve_content_top(const model::DocraftTable &table_node,
                                  const std::vector<std::shared_ptr<model::DocraftNode> > &title_nodes,
                                  const std::vector<std::shared_ptr<model::DocraftText> > &htitle_nodes,
                                  const float start_y) {
            if (table_node.orientation() == model::LayoutOrientation::kHorizontal) {
                for (const auto &title: title_nodes) {
                    if (title) {
                        return title->anchors().bottom_left.y;
                    }
                }
            } else {
                for (const auto &title: htitle_nodes) {
                    if (title) {
                        return title->anchors().bottom_left.y;
                    }
                }
            }
            return start_y;
        }

        void draw_content_nodes_and_row_lines(const std::shared_ptr<DocraftDocumentContext> &context,
                                              const std::vector<std::vector<std::shared_ptr<model::DocraftNode> > > &
                                              content_rows,
                                              const std::vector<float> &col_lefts,
                                              const float start_x,
                                              const float table_width,
                                              const float start_y,
                                              const LineBackend &line_backend) {
            float content_top = start_y;
            for (const auto &content_row: content_rows) {
                float bottom_y = content_top;
                const size_t col_limit = col_lefts.empty()
                                             ? content_row.size()
                                             : std::min(content_row.size(), col_lefts.size());

                for (size_t col_idx = 0; col_idx < col_limit; ++col_idx) {
                    const auto &cell = content_row[col_idx];
                    if (!cell) {
                        continue;
                    }
                    if (auto pointer = std::dynamic_pointer_cast<model::DocraftText>(cell)) {
                        pointer->draw(context);
                    } else if (auto img_pointer = std::dynamic_pointer_cast<model::DocraftImage>(cell)) {
                        DocraftImagePainter img_painter(*img_pointer);
                        img_painter.draw(context);
                    }
                    bottom_y = std::min(bottom_y, cell->anchors().bottom_left.y);
                }

                const float line_y_here = bottom_y;
                LOG_DEBUG(fmt::format("Drawing horizontal line at y={}", line_y_here));
                line_backend->draw_line(start_x, line_y_here, start_x + table_width, line_y_here);
                content_top = line_y_here;
            }
        }

        void draw_titles_last(const model::DocraftTable &table_node,
                              const std::shared_ptr<DocraftDocumentContext> &context,
                              const std::vector<std::shared_ptr<model::DocraftNode> > &title_nodes,
                              const std::vector<std::shared_ptr<model::DocraftText> > &htitle_nodes) {
            if (table_node.orientation() == model::LayoutOrientation::kHorizontal) {
                for (const auto &title: title_nodes) {
                    if (!title) {
                        continue;
                    }
                    title->draw(context);
                }
                return;
            }

            for (const auto &title: title_nodes) {
                if (!title) {
                    continue;
                }
                title->draw(context);
            }
            for (const auto &title: htitle_nodes) {
                if (!title) {
                    continue;
                }
                title->draw(context);
            }
        }
    }

    DocraftTablePainter::DocraftTablePainter(const model::DocraftTable &table_node) : table_node_(table_node) {
    }


    void DocraftTablePainter::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
        if (!context) {
            return;
        }

        const auto &rendering_service = context->rendering();
        auto line_backend = rendering_service.line_rendering();
        auto shape_backend = rendering_service.shape_rendering();
        if (!line_backend) {
            return;
        }

        line_backend->set_stroke_color(0.0F, 0.0F, 0.0F);

        const float start_x = table_node_.position().x;
        const float start_y = table_node_.position().y;
        const float table_width = table_node_.width();
        const auto title_nodes = table_node_.title_nodes();
        const auto htitle_nodes = table_node_.htitle_nodes();
        const auto row_bgs = table_node_.row_backgrounds();
        const auto cell_bgs = table_node_.content_backgrounds();
        const auto default_cell_bg = table_node_.default_cell_background();
        const auto content_rows = table_node_.content_nodes();
        const std::size_t safe_content_cols = table_node_.content_cols() > 0
                                                  ? static_cast<std::size_t>(table_node_.content_cols())
                                                  : 0U;

        if (table_node_.orientation() == model::LayoutOrientation::kHorizontal) {
            draw_title_backgrounds(title_nodes, table_node_.title_backgrounds(), shape_backend);
        } else {
            draw_title_backgrounds(htitle_nodes, table_node_.htitle_backgrounds(), shape_backend);
            draw_title_backgrounds(title_nodes, table_node_.title_backgrounds(), shape_backend);
        }

        draw_row_backgrounds(table_node_, shape_backend, start_x, table_width, title_nodes, content_rows, row_bgs);
        draw_cell_backgrounds(content_rows, row_bgs, cell_bgs, default_cell_bg, safe_content_cols, shape_backend);

        draw_outer_borders(table_node_, line_backend);
        if (table_node_.orientation() == model::LayoutOrientation::kHorizontal) {
            draw_horizontal_table_dividers(table_node_, title_nodes, start_x, table_width, line_backend);
        } else {
            draw_vertical_table_dividers(table_node_, htitle_nodes, content_rows, start_x, table_width, line_backend);
        }

        const std::vector<float> col_lefts = table_node_.orientation() == model::LayoutOrientation::kHorizontal
                                                 ? collect_horizontal_column_lefts(title_nodes)
                                                 : std::vector<float>{};
        const float content_top = resolve_content_top(table_node_, title_nodes, htitle_nodes, start_y);
        draw_content_nodes_and_row_lines(context, content_rows, col_lefts,
                                         start_x, table_width, content_top, line_backend);
         draw_titles_last(table_node_, context, title_nodes, htitle_nodes);
    }
} // docraft
