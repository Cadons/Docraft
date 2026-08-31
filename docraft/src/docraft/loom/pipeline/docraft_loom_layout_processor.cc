#include "docraft/loom/pipeline/docraft_loom_layout_processor.h"

#include <algorithm>
#include <memory>

#include <fmt/format.h>

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
#include "docraft/utils/docraft_logger.h"

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
        edit_frame(node.edit_layout_box()).position = processor.resolve_position(node);
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
        edit_frame(layout_box).size = layout_box.measured_size;
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
        edit_frame(layout_box).size = layout_box.measured_size;
        const float padding = node->effective_padding();
        const float own_width = node->width() > 0.0F ? node->width() : incoming_width();
        const float children_width =
            own_width > 0.0F ? std::max(0.0F, own_width - (2.0F * padding)) : 0.0F;

        const int n = node->children_count();
        if (n > 0)
        {
            const float start_x = edit_frame(layout_box).position.x + padding;
            // Mirrors the measure pass: the first child's own top margin is reserved
            // outright, with no preceding sibling to combine it with.
            float current_y = edit_frame(layout_box).position.y + padding
                + node->resolve_outer_margin(*node, /*leading=*/true);
            for (int i = 0; i < n; ++i)
            {
                inherited_width_ = children_width;
                auto child = node->edit_child(i);
                // Cross-axis margin (left/right): no sibling shares this axis to
                // collapse against, so it's a plain per-child offset within the
                // rectangle, mirroring how VStack honors margin_left/right.
                cursor_.set_position(start_x + child->margin().left, current_y);
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
        cursor_.set_position(edit_frame(layout_box).position.x, edit_frame(layout_box).position.y + edit_frame(layout_box).size.height);
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomCanvas* node)
    {
        if (!node)
            return;
        PositionScope scope(*this, *node);
        auto& layout_box = node->edit_layout_box();
        edit_frame(layout_box).size = layout_box.measured_size;

        // Every direct child is positioned by its own x/y relative to the canvas's own
        // origin (defaulting to (0,0) when omitted), never block-stacked. Forcing
        // kAbsolute and pre-translating explicit_position() to page-space lets the
        // child's own PositionScope (entered inside accept()) resolve to exactly that
        // page coordinate unchanged -- the same mechanism any other kAbsolute node
        // already uses, just computed here instead of by the Craft author. The child's
        // original mode/position are restored right after accept() so a second layout
        // pass over the same tree (e.g. after set_page_format()) re-translates from the
        // same local coordinates instead of compounding the previous pass's translation.
        const auto& origin = edit_frame(layout_box).position;
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
        cursor_.set_position(origin.x, origin.y + edit_frame(layout_box).size.height);
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomParagraph* node)
    {
        PositionScope scope(*this, *node);
        auto& layout_box = node->edit_layout_box();
        const auto& position = edit_frame(layout_box).position;
        // Mirrors every other container node (Rectangle/List/Table): frame.size must be
        // set from measured_size, since Pagination's own bookkeeping (paginate_body)
        // advances its next_y using frame.position.y + frame.size.height for each
        // top-level child -- leaving this at its zero-initialized default causes
        // Pagination to treat the paragraph as zero-height and pull the following
        // sibling back on top of it.
        edit_frame(layout_box).size = layout_box.measured_size;
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
        const auto& position = edit_frame(layout_box).position;
        // Mirrors Rectangle/List/Table: frame.size must reflect measured_size so that
        // Pagination's next_y bookkeeping (frame.position.y + frame.size.height) sees
        // this container's real footprint instead of a zero-initialized default.
        edit_frame(layout_box).size = layout_box.measured_size;

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
        const auto gaps = node->resolve_vertical_child_gaps(*node, node->spacing());
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
            auto child = node->edit_child(i);
            // Cross-axis margin (left/right): no sibling shares this axis to collapse
            // against, so it's a plain per-child offset within the column, mirroring
            // how HStack honors margin_top/bottom below.
            cursor_.set_position(start_x + child->margin().left, current_y);
            child->accept(*this);
            float advance = child->layout_box().measured_size.height;
            if (!resolved_heights.empty())
            {
                advance = resolved_heights[static_cast<std::size_t>(i)];
                edit_frame(child->edit_layout_box()).size.height = advance;
            }
            current_y += advance;
            if (i < n - 1)
            {
                current_y += gaps[static_cast<std::size_t>(i)];
            }
        }

        const float content_bottom = current_y + trailing_margin + padding;
        const float actual_height = std::max(edit_frame(layout_box).size.height, content_bottom - position.y);
        if (node->height() > 0.0F && actual_height > edit_frame(layout_box).size.height)
        {
            const std::string node_label =
                node->name().empty() ? std::string{"A vertical Layout"} : fmt::format("Layout '{}'", node->name());
            LOG_WARNING(fmt::format(
                "{} has height=\"{:.1f}\" but its weighted children's own natural heights needed "
                "{:.1f}pt to fit -- the box grew to that instead of clipping them.",
                node_label, node->height(), actual_height));
        }
        edit_frame(layout_box).size.height = actual_height;
        layout_box.measured_size.height = actual_height;
        cursor_.set_position(position.x, position.y + edit_frame(layout_box).size.height);
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomHStack* node)
    {
        if (!node) return;
        PositionScope scope(*this, *node);
        auto& layout_box = node->edit_layout_box();
        const auto& position = edit_frame(layout_box).position;
        // See the matching comment in visit(DocraftLoomVStack*).
        edit_frame(layout_box).size = layout_box.measured_size;

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
            auto child = node->edit_child(i);
            // Cross-axis margin (top/bottom): no sibling shares this axis to collapse
            // against, so it's a plain per-child offset within the row, mirroring how
            // VStack honors margin_left/right above.
            cursor_.set_position(current_x, start_y + child->margin().top);
            child->accept(*this);
            float advance = child->layout_box().measured_size.width;
            if (!resolved_widths.empty())
            {
                advance = resolved_widths[static_cast<std::size_t>(i)];
                // A child that opts out (e.g. an Image with its own declared width --
                // see DocraftLoomNode::keeps_own_size_in_weighted_slot()) keeps its own
                // width instead of being stretched to the resolved slot, which would
                // distort it (only this axis gets overwritten, height is left alone).
                // Every other child type still fills the slot, the whole point of
                // weights() for them.
                if (!child->keeps_own_size_in_weighted_slot())
                {
                    edit_frame(child->edit_layout_box()).size.width = advance;
                }
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
            edit_frame(layout_box).size.width = current_x - position.x;
        }
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomBlankLine* node)
    {
        if (!node) return;
        PositionScope scope(*this, *node);
        auto& layout_box = node->edit_layout_box();
        edit_frame(layout_box).size = {.width = page_size_.width, .height = node->height()};
        cursor_.move(0.0F, node->height());
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomImage* node)
    {
        if (!node)
            return;
        PositionScope scope(*this, *node);
        auto& layout_box = node->edit_layout_box();
        edit_frame(layout_box).size = layout_box.measured_size;
        cursor_.move(0.0F, layout_box.measured_size.height);
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomLine* node)
    {
        if (!node)
            return;
        PositionScope scope(*this, *node);
        auto& layout_box = node->edit_layout_box();
        edit_frame(layout_box).size = layout_box.measured_size;
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
        edit_frame(layout_box).size = layout_box.measured_size;
        cursor_.move(0.0F, layout_box.measured_size.height);
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomTriangle* node)
    {
        if (!node)
            return;
        PositionScope scope(*this, *node);
        auto& layout_box = node->edit_layout_box();
        edit_frame(layout_box).size = layout_box.measured_size;
        cursor_.move(0.0F, layout_box.measured_size.height);
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomPolygon* node)
    {
        if (!node)
            return;
        PositionScope scope(*this, *node);
        auto& layout_box = node->edit_layout_box();
        edit_frame(layout_box).size = layout_box.measured_size;
        cursor_.move(0.0F, layout_box.measured_size.height);
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomCurveLine* node)
    {
        if (!node)
            return;
        PositionScope scope(*this, *node);
        auto& layout_box = node->edit_layout_box();
        edit_frame(layout_box).size = layout_box.measured_size;
        cursor_.move(0.0F, layout_box.measured_size.height);
    }

    void DocraftLoomLayoutProcessor::visit(docraft::loom::nodes::DocraftLoomList* node)
    {
        if (!node)
            return;
        PositionScope scope(*this, *node);
        auto& layout_box = node->edit_layout_box();
        edit_frame(layout_box).size = layout_box.measured_size;

        const float item_x = edit_frame(layout_box).position.x;
        float y = edit_frame(layout_box).position.y;
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
        edit_frame(layout_box).size = layout_box.measured_size;
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
                edit_frame(layout_box).position.x + nodes::DocraftLoomTable::kCellPaddingX + content->margin().left,
                edit_frame(layout_box).position.y + nodes::DocraftLoomTable::kCellPaddingY + content->margin().top);
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
                if (cell->explicit_width().has_value())
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
        // Goal: resolved column widths must sum to available_width, not more.
        //
        // Example: available_width = 200, column A has explicit width = 150.
        // Column B (flexible, weight 1) must get 200 - 150 = 50.
        // The old bug gave B a share of the FULL 200 (e.g. 200/2 = 100 for 2 columns),
        // as if A wasn't taking any space, so A + B = 250 > 200 and the table overflowed.
        //
        // available_width comes from incoming_width (see visit(Table)) minus padding,
        // or -- if there's no incoming_width, e.g. in a unit test -- the sum of the
        // columns' natural widths, so the table just hugs its own content.
        const int cols = table.column_count();

        float sum_natural = 0.0F;
        for (float w : geometry.natural_widths)
            sum_natural += w;
        // kCellPaddingX is a per-cell content inset -- Measure already folds it into every
        // cell's own measured_size (and therefore into geometry.natural_widths), so it must
        // NOT also be subtracted here as if it were a table-wide margin like table.padding():
        // doing so shrank the table by a flat 2*kCellPaddingX with nothing to reclaim it,
        // leaving a gap on the table's trailing edge.
        const float available_width = incoming_width > 0.0F
                                          ? incoming_width - (2.0F * table.padding())
                                          : sum_natural;

        // Warn (rather than silently clip) when the author's own explicit column widths
        // already exceed available_width before any flexible column even gets a share --
        // resolve_fixed_and_flexible_amounts() below still keeps fixed columns verbatim
        // and clamps flexible ones to 0, so the table overflows the page margin visibly
        // instead of crashing, but that's worth surfacing to the caller.
        float explicit_total = 0.0F;
        for (float w : geometry.explicit_widths)
            if (w > 0.0F)
                explicit_total += w;
        if (explicit_total > available_width)
        {
            const std::string node_label =
                table.name().empty() ? std::string{"A table"} : fmt::format("Table '{}'", table.name());
            LOG_WARNING(fmt::format(
                "{} has explicit column widths summing to {:.1f}pt, wider than the {:.1f}pt available "
                "on the page -- it overflows past the page margin.",
                node_label, explicit_total, available_width));
        }

        // Fixed columns (explicit_widths[c] > 0) keep their own width verbatim and
        // reserve it out of available_width before flexible columns split what's left
        // by weight, floored at each flexible column's own natural width so content
        // never gets squeezed narrower than it needs -- see
        // resolve_fixed_and_flexible_amounts for the shared fixed/flexible algorithm
        // (also used by DocraftLoomMeasureProcessor's table wrap-budget estimate, so
        // both agree on which columns are fixed).
        return resolve_fixed_and_flexible_amounts({
            .available_amount = available_width,
            .count = cols,
            .weights = table.column_weights(),
            .explicit_amounts = geometry.explicit_widths,
            .floors = geometry.natural_widths,
        });
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
        const auto& table_position = frame_of(table.layout_box()).position;
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
                const float natural_width = edit_frame(cell_layout).size.width;
                const float natural_height = edit_frame(cell_layout).size.height;
                const float resolved_width = resolved_widths[static_cast<std::size_t>(c)];
                const float resolved_height = row_heights[static_cast<std::size_t>(r)];
                edit_frame(cell_layout).size = {.width = resolved_width, .height = resolved_height};

                if (auto content = cell->content())
                {
                    auto& content_frame = edit_frame(content->edit_layout_box());
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
        edit_frame(table->edit_layout_box()).size = {
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
        edit_frame(node->edit_layout_box()).size = {.width = 0.0F, .height = 0.0F};
    }
} // pipeline
