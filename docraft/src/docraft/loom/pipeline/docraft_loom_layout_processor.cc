#include "docraft/loom/pipeline/docraft_loom_layout_processor.h"

#include <algorithm>
#include <memory>

#include "docraft/exception/docraft_input_exceptions.h"
#include "docraft/loom/nodes/docraft_loom_blank_line.h"
#include "docraft/loom/nodes/docraft_loom_circle.h"
#include "docraft/loom/nodes/docraft_loom_hstack.h"
#include "docraft/loom/nodes/docraft_loom_image.h"
#include "docraft/loom/nodes/docraft_loom_line.h"
#include "docraft/loom/nodes/docraft_loom_list.h"
#include "docraft/loom/nodes/docraft_loom_polygon.h"
#include "docraft/loom/nodes/docraft_loom_triangle.h"
#include "docraft/loom/nodes/docraft_loom_page_number.h"
#include "docraft/loom/nodes/docraft_loom_paragraph.h"
#include "docraft/loom/nodes/docraft_loom_rectangle.h"
#include "docraft/loom/nodes/docraft_loom_table.h"
#include "docraft/loom/nodes/docraft_loom_text.h"
#include "docraft/loom/nodes/docraft_loom_vstack.h"

namespace docraft::loom::pipeline {
#pragma region Cursor

    float DocraftLoomCursor::x() const
    {
        return current_position_.x;
    }

    float DocraftLoomCursor::y() const
    {
        return current_position_.y;
    }

    void DocraftLoomCursor::move(float dx, float dy)
    {
        current_position_.x += dx;
        current_position_.y += dy;
    }

    void DocraftLoomCursor::set_position(float x, float y)
    {
        current_position_.x = x;
        current_position_.y = y;
    }

#pragma endregion

    DocraftLoomLayoutProcessor::DocraftLoomLayoutProcessor(float page_width)
    {
        page_size_.width = page_width;
    }

    void DocraftLoomLayoutProcessor::reset_cursor(float x, float y)
    {
        cursor_.set_position(x, y);
    }

    nodes::Position DocraftLoomLayoutProcessor::resolve_position(const nodes::DocraftLoomNode& node) const
    {
        if (node.position_mode() == model::DocraftPositionType::kAbsolute)
        {
            return node.explicit_position();
        }
        return {.x = cursor_.x(), .y = cursor_.y()};
    }

    DocraftLoomLayoutProcessor::PositionScope::PositionScope(DocraftLoomLayoutProcessor& processor,
                                                             nodes::DocraftLoomNode& node)
        : processor_(processor), entry_cursor_(processor.cursor_),
          restore_on_exit_(node.position_mode() == model::DocraftPositionType::kAbsolute)
    {
        node.edit_layout_box().frame.position = processor.resolve_position(node);
    }

    DocraftLoomLayoutProcessor::PositionScope::~PositionScope()
    {
        if (restore_on_exit_)
        {
            processor_.cursor_.set_position(entry_cursor_.x(), entry_cursor_.y());
        }
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomText* node)
    {
        PositionScope scope(*this, *node);
        // Move cursor to the right after placing the text; harmless if this node is
        // absolutely positioned, since the scope above restores the cursor on exit.
        cursor_.move(node->layout_box().measured_size.width, 0.0F);
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomRectangle* node)
    {
        if (!node)
            return;
        PositionScope scope(*this, *node);
        auto& layout_box = node->edit_layout_box();
        layout_box.frame.size = layout_box.measured_size;

        const int n = node->children_count();
        if (n > 0)
        {
            const float start_x = layout_box.frame.position.x + node->padding();
            float current_y = layout_box.frame.position.y + node->padding();
            for (int i = 0; i < n; ++i)
            {
                cursor_.set_position(start_x, current_y);
                auto child = node->edit_child(i);
                child->accept(*this);
                current_y += child->layout_box().measured_size.height;
            }
        }
        cursor_.set_position(layout_box.frame.position.x, layout_box.frame.position.y + layout_box.frame.size.height);
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomParagraph* node)
    {
        PositionScope scope(*this, *node);
        auto& layout_box = node->edit_layout_box();
        const auto& position = layout_box.frame.position;
        // Mirrors every other container node (Rectangle/List/Table): frame.size must be
        // set from measured_size, since Pagination's own bookkeeping (paginate_body)
        // advances its next_y using frame.position.y + frame.size.height for each
        // top-level child -- leaving this at its zero-initialized default causes
        // Pagination to treat the paragraph as zero-height and pull the following
        // sibling back on top of it.
        layout_box.frame.size = layout_box.measured_size;
        //layout the children of the paragraph
        const float start_x = position.x;
        float current_y = position.y + node->space_before();
        for (int i = 0; i < node->children_count(); ++i)
        {
            cursor_.set_position(start_x, current_y);
            auto child = node->edit_child(i);
            child->accept(*this);
            current_y += child->layout_box().measured_size.height * node->line_spacing();
        }
        cursor_.set_position(start_x, current_y + node->space_after());
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomVStack* node)
    {
        if (!node) return;
        PositionScope scope(*this, *node);
        const auto& position = node->layout_box().frame.position;

        const float start_x = position.x;
        float current_y = position.y;
        const int n = node->children_count();
        for (int i = 0; i < n; ++i)
        {
            cursor_.set_position(start_x, current_y);
            auto child = node->edit_child(i);
            child->accept(*this);
            current_y += child->layout_box().measured_size.height;
            if (i < n - 1)
            {
                current_y += node->spacing();
            }
        }
        cursor_.set_position(start_x, current_y);
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomHStack* node)
    {
        if (!node) return;
        PositionScope scope(*this, *node);
        const auto& position = node->layout_box().frame.position;

        const float start_y = position.y;
        float current_x = position.x;
        const int n = node->children_count();
        for (int i = 0; i < n; ++i)
        {
            cursor_.set_position(current_x, start_y);
            auto child = node->edit_child(i);
            child->accept(*this);
            current_x += child->layout_box().measured_size.width;
            if (i < n - 1)
            {
                current_x += node->spacing();
            }
        }
        cursor_.set_position(current_x, start_y);
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomBlankLine* node)
    {
        if (!node) return;
        PositionScope scope(*this, *node);
        auto& layout_box = node->edit_layout_box();
        layout_box.frame.size = {.width = page_size_.width, .height = node->height()};
        cursor_.move(0.0F, node->height());
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomImage* node)
    {
        if (!node)
            return;
        PositionScope scope(*this, *node);
        auto& layout_box = node->edit_layout_box();
        layout_box.frame.size = layout_box.measured_size;
        cursor_.move(0.0F, layout_box.measured_size.height);
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomLine* node)
    {
        if (!node)
            return;
        PositionScope scope(*this, *node);
        auto& layout_box = node->edit_layout_box();
        layout_box.frame.size = layout_box.measured_size;
        // A line is a block-level element like Text-in-a-VStack: only the vertical extent
        // participates in flow, its width does not push a following sibling to the right.
        cursor_.move(0.0F, layout_box.measured_size.height);
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomCircle* node)
    {
        if (!node)
            return;
        PositionScope scope(*this, *node);
        auto& layout_box = node->edit_layout_box();
        layout_box.frame.size = layout_box.measured_size;
        cursor_.move(0.0F, layout_box.measured_size.height);
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomTriangle* node)
    {
        if (!node)
            return;
        PositionScope scope(*this, *node);
        auto& layout_box = node->edit_layout_box();
        layout_box.frame.size = layout_box.measured_size;
        cursor_.move(0.0F, layout_box.measured_size.height);
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomPolygon* node)
    {
        if (!node)
            return;
        PositionScope scope(*this, *node);
        auto& layout_box = node->edit_layout_box();
        layout_box.frame.size = layout_box.measured_size;
        cursor_.move(0.0F, layout_box.measured_size.height);
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomList* node)
    {
        if (!node)
            return;
        PositionScope scope(*this, *node);
        auto& layout_box = node->edit_layout_box();
        layout_box.frame.size = layout_box.measured_size;

        const float item_x = layout_box.frame.position.x;
        float y = layout_box.frame.position.y;
        auto& markers = node->edit_markers();
        for (int i = 0; i < node->children_count(); ++i)
        {
            auto text_child = std::dynamic_pointer_cast<nodes::DocraftLoomText>(node->edit_child(i));
            auto& marker = markers[static_cast<std::size_t>(i)];
            const float gap = marker.width > 0.0F ? node->marker_gap() : 0.0F;
            cursor_.set_position(item_x + marker.width + gap, y);
            text_child->accept(*this);

            const float text_height = text_child->layout_box().measured_size.height;
            // Center the marker vertically within the item's own line height, so it
            // lines up with the text regardless of marker kind (box or text glyph).
            float marker_y = y;
            if (marker.kind == nodes::DocraftLoomList::Marker::Kind::kBox)
            {
                marker_y = y + std::max(0.0F, (text_height - marker.width) / 2.0F);
            }
            marker.position = {.x = item_x, .y = marker_y};
            y += text_height;
        }
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomTableCell* cell)
    {
        if (!cell)
            return;
        //cells cannot be in absolute position mode, they are always positioned by the table's own grid layout
        if (cell->position_mode() == model::DocraftPositionType::kAbsolute)
        {
            throw exception::InvalidInputException("Table cells cannot be in absolute position mode");
        }
        PositionScope scope(*this, *cell);
        auto& layout_box = cell->edit_layout_box();
        layout_box.frame.size = layout_box.measured_size;
        if (auto content = cell->content())
        {
            // Inset by the same padding folded into the cell's measured_size in Measure,
            // so content never sits flush against the cell's own edge.
            cursor_.set_position(layout_box.frame.position.x + nodes::DocraftLoomTable::kCellPaddingX,
                                 layout_box.frame.position.y + nodes::DocraftLoomTable::kCellPaddingY);
            content->accept(*this);
        }
    }

    // Gathers, per column/row, what Measure already computed on each cell:
    // natural_widths[c]  = widest cell's own intrinsic width in column c.
    // row_heights[r]     = tallest cell's intrinsic height in row r, plus vertical
    //                      padding on both sides -- rows never need a second pass,
    //                      their height doesn't depend on the resolved column widths.
    // explicit_widths[c] = largest per-cell explicit_width() override in column c
    //                      (0 if no cell in that column set one).
    DocraftLoomLayoutProcessor::TableNaturalGeometry DocraftLoomLayoutProcessor::gather_table_natural_geometry(
        const nodes::DocraftLoomTable& table)
    {
        const int rows = table.row_count();
        const int cols = table.column_count();
        TableNaturalGeometry geometry;
        geometry.natural_widths.assign(static_cast<std::size_t>(cols), 0.0F);
        geometry.row_heights.assign(static_cast<std::size_t>(rows), 0.0F);
        geometry.explicit_widths.assign(static_cast<std::size_t>(cols), 0.0F);

        for (int r = 0; r < rows; ++r)
        {
            for (int c = 0; c < cols; ++c)
            {
                auto cell = table.cell(r, c);
                // Measure already folded the cell's own padding inset into sz -- see
                // DocraftLoomMeasureProcessor::visit(DocraftLoomTableCell*) -- so no
                // extra padding term is added here.
                const auto& sz = cell->layout_box().measured_size;
                geometry.natural_widths[static_cast<std::size_t>(c)] = std::max(
                    geometry.natural_widths[static_cast<std::size_t>(c)], sz.width);
                geometry.row_heights[static_cast<std::size_t>(r)] = std::max(
                    geometry.row_heights[static_cast<std::size_t>(r)], sz.height);
                if (cell->explicit_width())
                {
                    geometry.explicit_widths[static_cast<std::size_t>(c)] = std::max(
                        geometry.explicit_widths[static_cast<std::size_t>(c)], *cell->explicit_width());
                }
            }
        }
        return geometry;
    }


    std::vector<float> DocraftLoomLayoutProcessor::resolve_table_column_widths(
        const nodes::DocraftLoomTable& table, const TableNaturalGeometry& geometry) const
    {
        // Resolves each column's final width:
        // - available_width is the page width minus outer padding, or -- if this processor
        //   was built without a page width (page_size_.width <= 0, e.g. in a unit test) --
        //   the sum of the natural widths, so the table just hugs its own content.
        // - column weights: missing entries default to 1.0; if every weight is <= 0 (e.g.
        //   an all-zero weight vector), fall back to one unit of weight per column.
        // - a column with an explicit width uses it verbatim (a hard constraint); otherwise
        //   it gets its proportional share of available_width by weight, floored at its own
        //   natural width (a column is never squeezed narrower than its content).
        // - if no column used an explicit width and the natural-width floor left the columns
        //   not summing exactly to available_width, rescale all of them proportionally so
        //   the table fills available_width exactly. Skipped if any column has an explicit
        //   width, since that width must not be stretched or shrunk to make the total add up.
        const int cols = table.column_count();

        float sum_natural = 0.0F;
        for (float w : geometry.natural_widths)
            sum_natural += w;
        const float available_width = page_size_.width > 0.0F
                                          ? page_size_.width - (2.0F * nodes::DocraftLoomTable::kCellPaddingX)
                                          : sum_natural;

        const auto& weights = table.column_weights();
        float total_weight = 0.0F;
        for (int c = 0; c < cols; ++c)
            total_weight += (c < static_cast<int>(weights.size()) ? weights[static_cast<std::size_t>(c)] : 1.0F);
        if (total_weight <= 0.0F)
        {
            total_weight = static_cast<float>(cols);
        }

        std::vector<float> resolved(static_cast<std::size_t>(cols), 0.0F);
        bool any_explicit = false;
        for (int c = 0; c < cols; ++c)
        {
            const float explicit_w = geometry.explicit_widths[static_cast<std::size_t>(c)];
            if (explicit_w > 0.0F)
            {
                resolved[static_cast<std::size_t>(c)] = explicit_w;
                any_explicit = true;
            }
            else
            {
                const float weight = c < static_cast<int>(weights.size()) ? weights[static_cast<std::size_t>(c)] : 1.0F;
                resolved[static_cast<std::size_t>(c)] = std::max(geometry.natural_widths[static_cast<std::size_t>(c)],
                                                                 available_width * weight / total_weight);
            }
        }
        if (!any_explicit)
        {
            float sum_resolved = 0.0F;
            for (float w : resolved)
                sum_resolved += w;
            if (sum_resolved > 0.0F && available_width > 0.0F)
            {
                const float scale = available_width / sum_resolved;
                for (float& w : resolved)
                {
                    w *= scale;
                }
            }
        }
        return resolved;
    }

    // Horizontal offset for content re-centered within a wider resolved column: text
    // honors its own alignment() (left/justified -> flush left, i.e. no shift; center ->
    // centered; right -> flush right); non-text content (e.g. Image, which has no
    // alignment concept) always stays flush left.
    float DocraftLoomLayoutProcessor::table_cell_horizontal_offset(
        const nodes::DocraftLoomNode& content, float extra_width)
    {
        if (extra_width <= 0.0F)
        {
            return 0.0F;
        }
        const auto* text = dynamic_cast<const nodes::DocraftLoomText*>(&content);
        if (!text)
        {
            return 0.0F;
        }
        switch (text->alignment())
        {
        case model::TextAlignment::kCenter:
            return extra_width / 2.0F;
        case model::TextAlignment::kRight:
            return extra_width;
        case model::TextAlignment::kLeft:
        case model::TextAlignment::kJustified:
        default:
            return 0.0F;
        }
    }

    // Walks the grid left-to-right, top-to-bottom, placing each cell at the cursor (the
    // usual container idiom: move the cursor, then let the cell's own accept() position
    // itself and its content relative to it) and then overwriting the cell's frame.size
    // -- which accept() just set to the cell's own intrinsic measured_size -- with the
    // column's resolved width and the row's height, so the cell visually occupies its
    // full grid cell rather than just its content size. The cell's content is then
    // re-positioned within that larger resolved band: always vertically centered, and
    // horizontally per the content's own alignment (see table_cell_horizontal_offset).
    void DocraftLoomLayoutProcessor::place_table_cells(
        nodes::DocraftLoomTable& table, const std::vector<float>& resolved_widths,
        const std::vector<float>& row_heights)
    {
        const auto& table_position = table.layout_box().frame.position;
        float row_top = table_position.y;
        for (int r = 0; r < table.row_count(); ++r)
        {
            float col_left = table_position.x;
            for (int c = 0; c < table.column_count(); ++c)
            {
                auto cell = table.cell(r, c);
                cursor_.set_position(col_left, row_top);
                cell->accept(*this);

                auto& cell_layout = cell->edit_layout_box();
                const float natural_width = cell_layout.frame.size.width;
                const float natural_height = cell_layout.frame.size.height;
                const float resolved_width = resolved_widths[static_cast<std::size_t>(c)];
                const float resolved_height = row_heights[static_cast<std::size_t>(r)];
                cell_layout.frame.size = {.width = resolved_width, .height = resolved_height};

                if (auto content = cell->content())
                {
                    auto& content_frame = content->edit_layout_box().frame;
                    content_frame.position.x += table_cell_horizontal_offset(*content, resolved_width - natural_width);
                    content_frame.position.y += (resolved_height - natural_height) / 2.0F;
                }

                col_left += resolved_width;
            }
            row_top += row_heights[static_cast<std::size_t>(r)];
        }
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomTable* table)
    {
        if (!table)
            return;
        PositionScope scope(*this, *table);

        const auto geometry = gather_table_natural_geometry(*table);
        const auto resolved_widths = resolve_table_column_widths(*table, geometry);

        float total_width = 0.0F;
        for (float w : resolved_widths)
            total_width += w;
        float total_height = 0.0F;
        for (float h : geometry.row_heights)
            total_height += h;
        table->edit_layout_box().frame.size = {.width = total_width, .height = total_height};

        place_table_cells(*table, resolved_widths, geometry.row_heights);
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomPageNumber* node)
    {
        // Positioned exactly like ordinary text -- see visit(DocraftLoomText*).
        visit(static_cast<docraft::loom::nodes::DocraftLoomText*>(node));
    }
} // pipeline
