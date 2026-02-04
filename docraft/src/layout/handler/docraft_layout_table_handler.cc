#include "layout/handler/docraft_layout_table_handler.h"

#include <algorithm>
#include <numeric>
#include <print>
#include <vector>

#include "layout/docraft_layout_engine.h"
#include "model/docraft_layout.h"
#include "model/docraft_text.h"

namespace docraft::layout::handler {
    namespace {
        /**
         * @This function ensures that the title nodes of the table are created based on the titles provided.
         * @param node
         */
        void ensure_title_nodes(const std::shared_ptr<model::DocraftTable> &node) {
            const auto &titles = node->titles();
            if (node->title_nodes().size() < titles.size()) {
                for (std::size_t i = node->title_nodes().size(); i < titles.size(); ++i) {
                    auto title_node = std::make_shared<model::DocraftText>();
                    title_node->set_text(titles[i]);
                    node->add_title_node(title_node);
                }
            }
        }

        /**
        * @This function flattens the content nodes of the table into a single vector.
        * @param node
        * @return std::vector<std::shared_ptr<model::DocraftNode> >
        */
        std::vector<std::shared_ptr<model::DocraftNode> > flatten_content_nodes(
            const std::shared_ptr<model::DocraftTable> &node) {
            std::vector<std::shared_ptr<model::DocraftNode> > flat;
            const auto grid = node->content_nodes();
            for (const auto &row: grid) {
                for (const auto &cell: row) {
                    if (cell) {
                        flat.emplace_back(cell);
                    }
                }
            }
            return flat;
        }

        /**
         * @This function calculates the available width for the table based on its properties and the context.
         * @param node
         * @param context
         * @param fixed_x
         * @param fallback
         * @return float
         */
        float available_width_for(const std::shared_ptr<model::DocraftTable> &node,
                                  const std::shared_ptr<DocraftDocumentContext> &context,
                                  const float fixed_x,
                                  const float fallback) {
            if (node->auto_fill_width()) {
                return context->available_space();
            }
            float w = node->width();
            return (w > 0.0F) ? w : fallback;
        }
        /**
         * @brief Layout a table in horizontal orientation.
         * @param node The table node to layout.
         * @param box Optional transform to store the resulting layout box.
         * @param context The PDF context for layout calculations.
         */
        void layout_horizontal_table(const std::shared_ptr<model::DocraftTable> &node,
                                     model::DocraftTransform *box,
                                     const std::shared_ptr<DocraftDocumentContext> &context) {
            DocraftCursor table_cursor = {context->cursor()};//Use a custom cursor to not affect the main one
            const float fixed_x = table_cursor.x();
            const float fixed_y = table_cursor.y()-node->padding();//Adjust for some top padding

            docraft::layout::DocraftLayoutEngine engine(context);

            const auto &titles = node->titles();
            const std::size_t cols = titles.size();
            ensure_title_nodes(node);

            // --- Measure titles (natural sizes) ---
            std::vector<float> natural_widths(cols, 0.0F);// Natural widths of each column, natural means without constraints
            float title_row_height = 0.0F;

            for (std::size_t i = 0; i < cols; ++i) {
                const auto &title_node = node->title_nodes()[i];
                // Save cursor position
                const float saved_x = table_cursor.x();
                const float saved_y = table_cursor.y();
                table_cursor.move_to(0.0F, context->page_height());
                (void) engine.compute_layout(title_node);
                table_cursor.move_to(saved_x, saved_y);

                natural_widths[i] = title_node->width();
                title_row_height = std::max(title_row_height, title_node->height());
                title_node->set_height(title_row_height); // Enforce uniform title row height
            }

            const float natural_sum = std::accumulate(natural_widths.begin(), natural_widths.end(), 0.0F);
            const float available_width = available_width_for(node, context, fixed_x, natural_sum);

            // --- Column widths from weights (respect natural minima) ---
            std::vector<float> weights = node->column_weights();
            if (weights.size() != cols) {
                weights.assign(cols, 1.0F);
            }
            float total_weight = std::accumulate(weights.begin(), weights.end(), 0.0F);
            if (total_weight <= 0.0F) {
                weights.assign(cols, 1.0F);
                total_weight = static_cast<float>(cols);
            }

            std::vector<float> col_widths(cols, 0.0F);
            float widths_sum = 0.0F;
            for (std::size_t i = 0; i < cols; ++i) {
                const float target = available_width * (weights[i] / total_weight);
                col_widths[i] = std::max(natural_widths[i], target);
                widths_sum += col_widths[i];
            }

            if (available_width > 0.0F && widths_sum > 0.0F && widths_sum != available_width) {
                const float scale = available_width / widths_sum;
                for (auto &w: col_widths) {
                    w *= scale;
                }
                widths_sum = available_width;
            }

            // Column lefts, based on widths
            std::vector<float> col_lefts(cols, fixed_x);
            {
                float x = fixed_x;
                for (std::size_t i = 0; i < cols; ++i) {
                    col_lefts[i] = x;
                    x += col_widths[i];
                }
            }

            // --- Place titles ---
            for (std::size_t i = 0; i < cols; ++i) {
                const auto &title_node = node->title_nodes()[i];
                title_node->set_position({.x = col_lefts[i], .y = fixed_y});
                title_node->set_width(col_widths[i]);
                title_node->set_height(title_row_height);
            }

            // --- Place content rows ---
            const auto grid = node->content_nodes();

            float y = fixed_y - title_row_height;
            float total_content_height = 0.0F;

            for (const auto &row: grid) {
                float row_height = 20.0F;

                for (std::size_t c = 0; c < std::min(row.size(), cols); ++c) {
                    const auto &cell = row[c];
                    if (!cell) {
                        continue;
                    }

                    const float saved_x = table_cursor.x();
                    const float saved_y = table_cursor.y();
                    table_cursor.move_to(col_lefts[c], y);
                    (void) engine.compute_layout(cell);
                    table_cursor.move_to(saved_x, saved_y);

                    row_height = std::max(row_height, cell->height());
                }

                for (std::size_t c = 0; c < std::min(row.size(), cols); ++c) {
                    const auto &cell = row[c];
                    if (!cell) {
                        continue;
                    }
                    cell->set_position({.x = col_lefts[c], .y = y});
                    cell->set_width(col_widths[c]);
                    cell->set_height(row_height);
                }

                total_content_height += row_height;
                y -= row_height;
            }

            // --- Finalize table box ---
            node->set_position({.x = fixed_x, .y = fixed_y});
            node->set_width(widths_sum);
            node->set_height(title_row_height + total_content_height);

            if (box) {
                box->set_position(node->position());
                box->set_width(node->width());
                box->set_height(node->height());
            }
        }

        void layout_vertical_table(const std::shared_ptr<model::DocraftTable> &node,
                                   model::DocraftTransform *box,
                                   const std::shared_ptr<DocraftDocumentContext> &context) {
            DocraftCursor table_cursor = {context->cursor()};//Use a custom cursor to not affect the main one
            const float fixed_x = table_cursor.x();
            const float fixed_y = table_cursor.y();

            docraft::layout::DocraftLayoutEngine engine(context);

            const std::size_t rows = node->titles().size();
            ensure_title_nodes(node);

            // In vertical mode: first column is titles (row headers), remaining columns are values.
            const std::size_t total_cols = static_cast<std::size_t>(std::max(2, node->cols()));
            const std::size_t value_cols = (total_cols > 1) ? (total_cols - 1) : 1;

            // Flatten content sequentially: each row consumes value_cols items.
            const auto flat = flatten_content_nodes(node);

            // Measure title nodes (for natural title column width and minimum row heights).
            float title_col_natural_width = 0.0F;
            std::vector<float> title_row_natural_heights(rows, 0.0F);

            for (std::size_t r = 0; r < rows; ++r) {
                const auto &title_node = node->title_nodes()[r];

                const float saved_x = table_cursor.x();
                const float saved_y = table_cursor.y();
                table_cursor.move_to(fixed_x, context->page_height());
                (void) engine.compute_layout(title_node);
                table_cursor.move_to(saved_x, saved_y);

                title_col_natural_width = std::max(title_col_natural_width, title_node->width());
                title_row_natural_heights[r] = title_node->height();
            }

            const float available_width = available_width_for(node, context, fixed_x, title_col_natural_width);

            // Use two weights: [title_col, values_block]
            std::vector<float> weights = node->column_weights();
            if (weights.size() < 2) {
                weights.assign(2, 1.0F);
            }
            float total_weight = weights[0] + weights[1];
            if (total_weight <= 0.0F) {
                weights = {.5F, .5F};
                total_weight = 1.0F;
            }

            const float title_col_width = std::max(title_col_natural_width,
                                                   available_width * (weights[0] / total_weight));
            const float values_block_width = std::max(0.0F, available_width - title_col_width);
            const float value_col_width = (value_cols > 0)
                                              ? (values_block_width / static_cast<float>(value_cols))
                                              : values_block_width;

            float y = fixed_y;
            float total_height = 0.0F;

            for (std::size_t r = 0; r < rows; ++r) {
                float row_height = title_row_natural_heights[r];

                // Measure the row's value cells to find row height.
                const std::size_t row_first = r * value_cols;
                for (std::size_t vc = 0; vc < value_cols; ++vc) {
                    const std::size_t idx = row_first + vc;
                    if (idx >= flat.size()) {
                        break;
                    }
                    const auto &cell = flat[idx];

                    const float saved_x = table_cursor.x();
                    const float saved_y = table_cursor.y();
                    table_cursor.move_to(fixed_x + title_col_width + (static_cast<float>(vc) * value_col_width), y);
                    (void) engine.compute_layout(cell);
                    table_cursor.move_to(saved_x, saved_y);

                    row_height = std::max(row_height, cell->height());
                }

                // Place title cell.
                {
                    const auto &title_node = node->title_nodes()[r];
                    title_node->set_position({.x = fixed_x, .y = y});
                    title_node->set_width(title_col_width);
                    title_node->set_height(row_height);
                }

                // Place value cells for this row.
                for (std::size_t vc = 0; vc < value_cols; ++vc) {
                    const std::size_t idx = row_first + vc;
                    if (idx >= flat.size()) {
                        break;
                    }
                    const auto &cell = flat[idx];
                    cell->set_position({
                        .x = fixed_x + title_col_width + (static_cast<float>(vc) * value_col_width), .y = y
                    });
                    cell->set_width(value_col_width);
                    cell->set_height(row_height);
                }

                total_height += row_height;
                y -= row_height;
            }

            node->set_position({.x = fixed_x, .y = fixed_y});
            node->set_width(title_col_width + (value_col_width * static_cast<float>(value_cols)));
            node->set_height(total_height);

            if (box) {
                box->set_position(node->position());
                box->set_width(node->width());
                box->set_height(node->height());
            }
        }
    }

    void DocraftLayoutTableHandler::compute(const std::shared_ptr<model::DocraftTable> &node,
                                            model::DocraftTransform *box) {
        if (!node) {
            throw std::invalid_argument("table node is null");
        }

        if (node->titles().empty()) {
            auto &cursor = context()->cursor();
            node->set_position({.x = cursor.x(), .y = cursor.y()});
            node->set_width(0.0F);
            node->set_height(0.0F);
            if (box) {
                box->set_position(node->position());
                box->set_width(node->width());
                box->set_height(node->height());
            }
            return;
        }

        switch (node->orientation()) {
            case model::LayoutOrientation::kHorizontal:
                layout_horizontal_table(node, box, context());
                break;
            case model::LayoutOrientation::kVertical:
                layout_vertical_table(node, box, context());
                break;
            default:
                throw std::runtime_error("unsupported table orientation");
        }
        //print cels positions for debug
        for (const auto &row: node->content_nodes()) {
            for (const auto &cell: row) {
                if (cell) {
                    std::print("Cell at ({}, {}) size ({}, {})\n",
                               cell->position().x,
                               cell->position().y,
                               cell->width(),
                               cell->height());
                }
            }
        }
    }

    bool DocraftLayoutTableHandler::handle(const std::shared_ptr<model::DocraftNode> &request,
                                           model::DocraftTransform *result) {
        if (auto table_node = std::dynamic_pointer_cast<model::DocraftTable>(request)) {
            compute(table_node, result);
            return true;
        }
        return false;
    }
} // namespace docraft::layout::handler
