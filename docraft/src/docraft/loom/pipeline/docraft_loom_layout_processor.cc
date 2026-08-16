#include "docraft/loom/pipeline/docraft_loom_layout_processor.h"

#include <algorithm>
#include <memory>

#include "docraft/exception/docraft_input_exceptions.h"
#include "docraft/loom/nodes/docraft_loom_blank_line.h"
#include "docraft/loom/nodes/docraft_loom_circle.h"
#include "docraft/loom/nodes/docraft_loom_curve_line.h"
#include "docraft/loom/nodes/docraft_loom_hstack.h"
#include "docraft/loom/nodes/docraft_loom_image.h"
#include "docraft/loom/nodes/docraft_loom_line.h"
#include "docraft/loom/nodes/docraft_loom_list.h"
#include "docraft/loom/nodes/docraft_loom_new_page.h"
#include "docraft/loom/nodes/docraft_loom_polygon.h"
#include "docraft/loom/nodes/docraft_loom_triangle.h"
#include "docraft/loom/nodes/docraft_loom_page_number.h"
#include "docraft/loom/nodes/docraft_loom_paragraph.h"
#include "docraft/loom/nodes/docraft_loom_canvas.h"
#include "docraft/loom/nodes/docraft_loom_rectangle.h"
#include "docraft/loom/nodes/docraft_loom_subtitle.h"
#include "docraft/loom/nodes/docraft_loom_table.h"
#include "docraft/loom/nodes/docraft_loom_text.h"
#include "docraft/loom/nodes/docraft_loom_title.h"
#include "docraft/loom/nodes/docraft_loom_vstack.h"
#include "docraft/loom/pipeline/docraft_loom_weighted_distribution.h"

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

    void DocraftLoomLayoutProcessor::set_content_width(float width)
    {
        page_size_.width = width;
    }

    nodes::Position DocraftLoomLayoutProcessor::resolve_position(const nodes::DocraftLoomNode& node) const
    {
        if (node.position_mode() == nodes::DocraftPositionType::kAbsolute)
        {
            return node.explicit_position();
        }
        return {.x = cursor_.x(), .y = cursor_.y()};
    }

    DocraftLoomLayoutProcessor::PositionScope::PositionScope(DocraftLoomLayoutProcessor& processor,
                                                             nodes::DocraftLoomNode& node)
        : processor_(processor), entry_cursor_(processor.cursor_),
          restore_on_exit_(node.position_mode() == nodes::DocraftPositionType::kAbsolute)
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

    float DocraftLoomLayoutProcessor::incoming_width() const
    {
        return inherited_width_ > 0.0F ? inherited_width_ : page_size_.width;
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomText* node)
    {
        PositionScope scope(*this, *node);
        auto& layout_box = node->edit_layout_box();
        // Mirrors every other node type (Rectangle/Paragraph/VStack/HStack/List/...):
        // frame.size must be set from measured_size, since Pagination's own bookkeeping
        // (paginate_body) advances its next_y using frame.position.y + frame.size.height
        // for each top-level child -- leaving this at its zero-initialized default causes
        // Pagination to treat a bare Text/Title/Subtitle as zero-height and pull the
        // following sibling back on top of it.
        layout_box.frame.size = layout_box.measured_size;

        // Move cursor to the right after placing the text; harmless if this node is
        // absolutely positioned, since the scope above restores the cursor on exit.
        cursor_.move(layout_box.measured_size.width, 0.0F);
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomTitle* node)
    {
        visit(static_cast<docraft::loom::nodes::DocraftLoomText*>(node));
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomSubtitle* node)
    {
        visit(static_cast<docraft::loom::nodes::DocraftLoomText*>(node));
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomRectangle* node)
    {
        if (!node)
            return;
        PositionScope scope(*this, *node);
        auto& layout_box = node->edit_layout_box();
        layout_box.frame.size = layout_box.measured_size;
        const float padding = node->effective_padding();
        const float own_width = node->width() > 0.0F ? node->width() : incoming_width();
        const float children_width =
            own_width > 0.0F ? std::max(0.0F, own_width - (2.0F * padding)) : 0.0F;

        const int n = node->children_count();
        if (n > 0)
        {
            const float start_x = layout_box.frame.position.x + padding;
            // Mirrors the measure pass: the first child's own top margin is reserved
            // outright, with no preceding sibling to combine it with.
            float current_y = layout_box.frame.position.y + padding
                + node->resolve_outer_margin(*node, /*leading=*/true);
            for (int i = 0; i < n; ++i)
            {
                inherited_width_ = children_width;
                cursor_.set_position(start_x, current_y);
                auto child = node->edit_child(i);
                child->accept(*this);
                current_y += child->layout_box().measured_size.height;
                if (i < n - 1)
                {
                    const auto next = node->child(i + 1);
                    current_y += nodes::DocraftLoomLayoutContainer::resolve_child_gap(
                        node->spacing(), child->margin().bottom, next->margin().top);
                }
            }
        }
        cursor_.set_position(layout_box.frame.position.x, layout_box.frame.position.y + layout_box.frame.size.height);
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomCanvas* node)
    {
        if (!node)
            return;
        PositionScope scope(*this, *node);
        auto& layout_box = node->edit_layout_box();
        layout_box.frame.size = layout_box.measured_size;

        // Every direct child is positioned by its own x/y relative to the canvas's own
        // origin (defaulting to (0,0) when omitted), never block-stacked. Forcing
        // kAbsolute and pre-translating explicit_position() to page-space lets the
        // child's own PositionScope (entered inside accept()) resolve to exactly that
        // page coordinate unchanged -- the same mechanism any other kAbsolute node
        // already uses, just computed here instead of by the Craft author. The child's
        // original mode/position are restored right after accept() so a second layout
        // pass over the same tree (e.g. after set_page_format()) re-translates from the
        // same local coordinates instead of compounding the previous pass's translation.
        const auto& origin = layout_box.frame.position;
        for (int i = 0; i < node->children_count(); ++i)
        {
            auto child = node->edit_child(i);
            const auto local = child->explicit_position();
            const auto original_mode = child->position_mode();
            child->set_position_mode(nodes::DocraftPositionType::kAbsolute);
            child->set_explicit_position({.x = origin.x + local.x, .y = origin.y + local.y});
            child->accept(*this);
            child->set_position_mode(original_mode);
            child->set_explicit_position(local);
        }
        cursor_.set_position(origin.x, origin.y + layout_box.frame.size.height);
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
        auto& layout_box = node->edit_layout_box();
        const auto& position = layout_box.frame.position;
        // Mirrors Rectangle/List/Table: frame.size must reflect measured_size so that
        // Pagination's next_y bookkeeping (frame.position.y + frame.size.height) sees
        // this container's real footprint instead of a zero-initialized default.
        layout_box.frame.size = layout_box.measured_size;

        // A VStack has no explicit width of its own, so it neither narrows to a fixed
        // width nor owns the constraint like Rectangle does -- but it does narrow the
        // width relayed to children by its own padding(), the same inset Rectangle
        // already applies around its children.
        const float padding = node->effective_padding();
        const float relay_width = std::max(0.0F, incoming_width() - (2.0F * padding));

        const int n = node->children_count();
        const auto& weights = node->weights();

        // Precomputed once, upfront (see the matching comment in
        // DocraftLoomLayoutProcessor::visit(DocraftLoomHStack*)) so the weighted-height
        // math and the final placement loop agree on the same gaps.
        std::vector<float> gaps;
        if (n > 1)
        {
            gaps.resize(static_cast<std::size_t>(n - 1));
            for (int i = 0; i < n - 1; ++i)
            {
                gaps[static_cast<std::size_t>(i)] = nodes::DocraftLoomLayoutContainer::resolve_child_gap(
                    node->spacing(), node->child(i)->margin().bottom, node->child(i + 1)->margin().top);
            }
        }
        float total_gap = 0.0F;
        for (const float gap : gaps)
        {
            total_gap += gap;
        }

        const float leading_margin = node->resolve_outer_margin(*node, /*leading=*/true);
        const float trailing_margin = node->resolve_outer_margin(*node, /*leading=*/false);

        // Opt-in: only engaged when weights() is non-empty AND this VStack has its own
        // explicit height() -- unlike HStack, a VStack has no ambient "page height"
        // budget to divide (pagination makes vertical space effectively unbounded), so
        // weights() alone has nothing well-defined to divide until height() gives it
        // one. Each child is never squeezed shorter than its own natural height, though
        // (mirrors HStack's natural-width floor).
        std::vector<float> resolved_heights;
        if (!weights.empty() && n > 0 && node->height() > 0.0F)
        {
            const float available_height = std::max(0.0F,
                node->height() - total_gap - leading_margin - trailing_margin - (2.0F * padding));
            std::vector<float> natural_heights(static_cast<std::size_t>(n));
            for (int i = 0; i < n; ++i)
            {
                natural_heights[static_cast<std::size_t>(i)] = node->child(i)->layout_box().measured_size.height;
            }
            resolved_heights = distribute_weighted_amounts(available_height, weights, n, natural_heights);
        }

        const float start_x = position.x + padding;
        // Mirrors the measure pass: the first/last child's own margin is reserved
        // outright, with no sibling on that side to combine it with.
        float current_y = position.y + padding + leading_margin;
        for (int i = 0; i < n; ++i)
        {
            inherited_width_ = relay_width;
            cursor_.set_position(start_x, current_y);
            auto child = node->edit_child(i);
            child->accept(*this);
            float advance = child->layout_box().measured_size.height;
            if (!resolved_heights.empty())
            {
                advance = resolved_heights[static_cast<std::size_t>(i)];
                child->edit_layout_box().frame.size.height = advance;
            }
            current_y += advance;
            if (i < n - 1)
            {
                current_y += gaps[static_cast<std::size_t>(i)];
            }
        }

        const float content_bottom = current_y + trailing_margin + padding;
        const float actual_height = std::max(layout_box.frame.size.height, content_bottom - position.y);
        layout_box.frame.size.height = actual_height;
        layout_box.measured_size.height = actual_height;
        cursor_.set_position(position.x, position.y + layout_box.frame.size.height);
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomHStack* node)
    {
        if (!node) return;
        PositionScope scope(*this, *node);
        auto& layout_box = node->edit_layout_box();
        const auto& position = layout_box.frame.position;
        // See the matching comment in visit(DocraftLoomVStack*).
        layout_box.frame.size = layout_box.measured_size;

        // Capture whatever an ancestor pushed down before clearing it: without
        // weights, HStack's contract is shrink-to-fit (each child keeps its own
        // natural width), so a stale value must not leak into the first child --
        // mirrors DocraftLoomMeasureProcessor::visit(DocraftLoomHStack*).
        const float width_budget = incoming_width();
        inherited_width_ = 0.0F;

        const float padding = node->effective_padding();
        const float start_y = position.y + padding;
        const int n = node->children_count();
        const auto& weights = node->weights();

        // Precomputed once, upfront (see the matching comment in
        // DocraftLoomMeasureProcessor::visit(DocraftLoomHStack*)) so the weighted-width
        // math and the final advance loop agree on the same gaps.
        const auto gaps = node->resolve_horizontal_child_gaps(*node, node->spacing());
        float total_gap = 0.0F;
        for (const float gap : gaps)
        {
            total_gap += gap;
        }

        // See the matching comment in DocraftLoomMeasureProcessor::visit(DocraftLoomHStack*).
        const float leading_margin = node->resolve_outer_margin(*node, /*leading=*/true);
        const float trailing_margin = node->resolve_outer_margin(*node, /*leading=*/false);

        // Opt-in: only engaged when weights() is non-empty, so plain HStacks (headers,
        // footers, shape rows, ...) keep today's shrink-to-fit behavior untouched. When
        // engaged, the available width is divided among children by weight
        // (missing/non-positive entries default to 1.0, i.e. homogeneous division),
        // mirroring DocraftLoomTable's column_weights -- each child is never squeezed
        // narrower than its own natural width, though. Divides width_budget (an
        // ancestor's constraint, or page_size_.width at the root) -- not
        // page_size_.width unconditionally -- so a weighted HStack nested inside a
        // narrower Rectangle/VStack divides that narrower budget instead of the full
        // page (mirrors the matching fix in DocraftLoomMeasureProcessor).
        std::vector<float> resolved_widths;
        if (!weights.empty() && n > 0)
        {
            const float available_width = std::max(0.0F,
                                                   width_budget - total_gap - leading_margin - trailing_margin - (2.0F *
                                                       padding));
            std::vector<float> natural_widths(static_cast<std::size_t>(n));
            for (int i = 0; i < n; ++i)
            {
                natural_widths[static_cast<std::size_t>(i)] = node->child(i)->layout_box().measured_size.width;
            }
            resolved_widths = distribute_weighted_amounts(available_width, weights, n, natural_widths);
        }

        float current_x = position.x + padding + leading_margin;
        for (int i = 0; i < n; ++i)
        {
            // Mirrors DocraftLoomMeasureProcessor::visit(DocraftLoomHStack*): a resolved
            // column width must be armed as this child's own incoming width *before*
            // accept() recurses into it, or a weighted column's content (e.g. a nested
            // Rectangle/VStack with no explicit width of its own) lays out against
            // whatever this HStack's own ancestor pushed down instead of its actual
            // resolved share -- silently diverging from what Measure already assumed
            // that content would be narrowed to.
            if (!resolved_widths.empty())
            {
                inherited_width_ = resolved_widths[static_cast<std::size_t>(i)];
            }
            cursor_.set_position(current_x, start_y);
            auto child = node->edit_child(i);
            child->accept(*this);
            float advance = child->layout_box().measured_size.width;
            if (!resolved_widths.empty())
            {
                advance = resolved_widths[static_cast<std::size_t>(i)];
                child->edit_layout_box().frame.size.width = advance;
            }
            current_x += advance;
            if (i < n - 1)
            {
                current_x += gaps[static_cast<std::size_t>(i)];
            }
            else
            {
                current_x += trailing_margin;
            }
        }
        current_x += padding;
        cursor_.set_position(current_x, start_y);
        if (!resolved_widths.empty())
        {
            layout_box.frame.size.width = current_x - position.x;
        }
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

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomCurveLine* node)
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
            // Center the marker vertically within the item's *first* line only, so a
            // wrapped multi-line item still aligns the marker with the first line of
            // text instead of floating at the midpoint of the whole wrapped block.
            const std::size_t line_count = text_child->wrapped_lines().empty()
                                                ? std::size_t{1}
                                                : text_child->wrapped_lines().size();
            const float first_line_height = text_height / static_cast<float>(line_count);
            float marker_y = y;
            if (marker.kind == nodes::DocraftLoomList::Marker::Kind::kBox)
            {
                marker_y = y + std::max(0.0F, (first_line_height - marker.width) / 2.0F);
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
        if (cell->position_mode() == nodes::DocraftPositionType::kAbsolute)
        {
            //this is would, because it cannot use this position mode, it is an error in the configuration.
            throw exception::InvalidInputException("Table cells cannot be in absolute position mode");
        }
        PositionScope scope(*this, *cell);
        auto& layout_box = cell->edit_layout_box();
        layout_box.frame.size = layout_box.measured_size;
        if (auto content = cell->content())
        {
            // Content is positioned by the cell's own grid slot, same as the cell itself --
            // absolute mode would silently resolve to a raw page coordinate instead,
            // ignoring the cell's layout entirely.
            if (content->position_mode() == nodes::DocraftPositionType::kAbsolute)
            {
                throw exception::InvalidInputException("Table cell content cannot be in absolute position mode");
            }
            // Inset by the same padding folded into the cell's measured_size in Measure,
            // plus the content's own margin -- mirrors how VStack/HStack apply
            // resolve_outer_margin to their children instead of only their own padding.
            cursor_.set_position(
                layout_box.frame.position.x + nodes::DocraftLoomTable::kCellPaddingX + content->margin().left,
                layout_box.frame.position.y + nodes::DocraftLoomTable::kCellPaddingY + content->margin().top);
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
        const nodes::DocraftLoomTable& table, const TableNaturalGeometry& geometry, float incoming_width) const
    {
        // Resolves each column's final width:
        // - available_width is incoming_width (an ancestor's constraint pushed down via
        //   inherited_width_, or page_size_.width at the root -- see visit(Table)) minus
        //   outer padding, or -- if incoming_width is 0 (e.g. a table built without a
        //   page width in a unit test) -- the sum of the natural widths, so the table
        //   just hugs its own content. Mirrors the matching fix in
        //   DocraftLoomMeasureProcessor::visit(Table).
        // - column weights: missing or non-positive entries default to 1.0 (handled by
        //   distribute_weighted_amounts()), so an all-zero weight vector divides evenly.
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
        const float available_width = incoming_width > 0.0F
                                          ? incoming_width - (2.0F * nodes::DocraftLoomTable::kCellPaddingX) -
                                          (2.0F * table.padding())
                                          : sum_natural;

        const auto by_weight =
            distribute_weighted_amounts(available_width, table.column_weights(), cols, geometry.natural_widths);

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
                resolved[static_cast<std::size_t>(c)] = by_weight[static_cast<std::size_t>(c)];
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
        case nodes::TextAlignment::kCenter:
            return extra_width / 2.0F;
        case nodes::TextAlignment::kRight:
            return extra_width;
        case nodes::TextAlignment::kLeft:
        case nodes::TextAlignment::kJustified:
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
        float row_top = table_position.y + table.padding();
        for (int r = 0; r < table.row_count(); ++r)
        {
            float col_left = table_position.x + table.padding();
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

        // Table resolves its columns' widths itself (below) rather than relaying
        // inherited_width_ to children the way Rectangle/VStack do -- capture whatever
        // an ancestor pushed down before clearing it, mirroring
        // DocraftLoomMeasureProcessor::visit(Table).
        const float width_budget = incoming_width();
        inherited_width_ = 0.0F;

        const auto geometry = gather_table_natural_geometry(*table);
        const auto resolved_widths = resolve_table_column_widths(*table, geometry, width_budget);

        float total_width = 0.0F;
        for (float w : resolved_widths)
            total_width += w;
        float total_height = 0.0F;
        for (float h : geometry.row_heights)
            total_height += h;
        // Mirrors Measure's own +2*padding() inflation, so frame.size (what Pagination
        // advances by) matches measured_size exactly, same as every other node type.
        table->edit_layout_box().frame.size = {
            .width = total_width + (2.0F * table->padding()),
            .height = total_height + (2.0F * table->padding())
        };

        place_table_cells(*table, resolved_widths, geometry.row_heights);
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomPageNumber* node)
    {
        // Positioned exactly like ordinary text -- see visit(DocraftLoomText*).
        visit(static_cast<docraft::loom::nodes::DocraftLoomText*>(node));
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomNewPage* node)
    {
        if (!node) return;
        PositionScope scope(*this, *node);
        node->edit_layout_box().frame.size = {.width = 0.0F, .height = 0.0F};
    }
} // pipeline
