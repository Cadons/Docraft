//
// Created by Matteo on 27/06/2026.
//

#include "docraft/loom/pipeline/docraft_loom_pagination_processor.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <unordered_map>
#include <vector>

#include <fmt/format.h>

#include "docraft/loom/nodes/docraft_loom_hstack.h"
#include "docraft/loom/nodes/docraft_loom_layout_box_access.h"
#include "docraft/loom/nodes/docraft_loom_list.h"
#include "docraft/loom/nodes/docraft_loom_new_page.h"
#include "docraft/loom/nodes/docraft_loom_canvas.h"
#include "docraft/loom/nodes/docraft_loom_rectangle.h"
#include "docraft/loom/nodes/docraft_loom_table.h"
#include "docraft/loom/nodes/docraft_loom_vstack.h"
#include "docraft/utils/docraft_logger.h"

namespace docraft::loom::pipeline {
    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomText*)
    {
        //Do nothing
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomTitle*)
    {
        //Do nothing
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomSubtitle*)
    {
        //Do nothing
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomRectangle* node)
    {
        recurse_into_children(node);
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomCanvas* node)
    {
        recurse_into_children(node);
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomParagraph*)
    {
        //Do nothing
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomVStack* node)
    {
        recurse_into_children(node);
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomHStack* node)
    {
        recurse_into_children(node);
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomBlankLine*)
    {
        //Do nothing
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomImage*)
    {
        //Do nothing
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomLine*)
    {
        //Do nothing
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomCircle*)
    {
        //Do nothing
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomTriangle*)
    {
        //Do nothing
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomPolygon*)
    {
        //Do nothing
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomCurveLine*)
    {
        //Do nothing
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomList* node)
    {
        recurse_into_children(node);
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomTableCell* cell)
    {
        if (!cell)
        {
            return;
        }
        if (auto content = cell->content())
        {
            content->accept(*this);
        }
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomTable* table)
    {
        if (!table)
        {
            return;
        }
        for (int r = 0; r < table->row_count(); ++r)
        {
            for (int c = 0; c < table->column_count(); ++c)
            {
                if (auto cell = table->cell(r, c))
                {
                    cell->accept(*this);
                }
            }
        }
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomPageNumber*)
    {
        //Do nothing
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomNewPage*)
    {
        // The actual forced break is handled directly in paginate_body(), which needs to
        // special-case it before the normal fits-on-this-page check runs.
    }

    void DocraftLoomPaginationProcessor::recurse_into_children(nodes::DocraftLoomNode* node)
    {
        if (!node)
        {
            return;
        }
        for (int i = 0; i < node->children_count(); ++i)
        {
            if (auto child = node->edit_child(i))
            {
                child->accept(*this);
            }
        }
    }

    void DocraftLoomPaginationProcessor::seal_page_index(nodes::DocraftLoomNode& node, int index)
    {
        node.edit_layout_box().set_page_index(
            index, make_page_index_proof(nodes::layout_proof_or_throw(node)));
    }

    void DocraftLoomPaginationProcessor::assign_page_index_recursive(nodes::DocraftLoomNode& node, int page_index)
    {
        seal_page_index(node, page_index);

        // Table cells live in the table's own grid_, not in the inherited children_
        // vector, so they need their own recursion branch.
        if (auto* table = dynamic_cast<nodes::DocraftLoomTable*>(&node))
        {
            table->for_each_cell([page_index](nodes::DocraftLoomTableCell &cell) {
                assign_page_index_recursive(cell, page_index);
            });
            return;
        }

        for (int i = 0; i < node.children_count(); ++i)
        {
            if (auto child = node.edit_child(i))
            {
                assign_page_index_recursive(*child, page_index);
            }
        }
    }

    void DocraftLoomPaginationProcessor::shift_subtree_position(nodes::DocraftLoomNode& node, float dy)
    {
        sealed_edit_frame(node).position.y += dy;

        // A list's marker positions are stored separately from the layout box, so they
        // need to be shifted in lockstep with the node itself.
        if (auto* list = dynamic_cast<nodes::DocraftLoomList*>(&node))
        {
            for (auto& marker : list->edit_markers())
            {
                marker.position.y += dy;
            }
        }

        // Table cells live in the table's own grid_, not in the inherited children_
        // vector, so they need their own recursion branch (mirrors assign_page_index_recursive).
        if (auto* table = dynamic_cast<nodes::DocraftLoomTable*>(&node))
        {
            table->for_each_cell([dy](nodes::DocraftLoomTableCell &cell) {
                shift_subtree_position(cell, dy);
            });
            return;
        }

        for (int i = 0; i < node.children_count(); ++i)
        {
            if (auto child = node.edit_child(i))
            {
                shift_subtree_position(*child, dy);
            }
        }
    }

    std::shared_ptr<nodes::DocraftLoomTable> DocraftLoomPaginationProcessor::finish_table_split(
        nodes::DocraftLoomTable &table, std::shared_ptr<nodes::DocraftLoomTable> remainder, float new_page_top_y) {
        // Re-stack the remainder's rows starting at the new page's body top -- their
        // positions were computed by Layout for a continuous canvas that no longer
        // applies once the rows move to a fresh physical page.
        float row_top = new_page_top_y;
        const int remainder_cols = remainder->column_count();
        for (int r = 0; r < remainder->row_count(); ++r)
        {
            auto reference_cell = remainder->cell(r, 0);
            if (!reference_cell)
            {
                continue;
            }
            const float row_height = remainder->row_height(r);
            const float dy = row_top - sealed_frame(*reference_cell).position.y;
            for (int c = 0; c < remainder_cols; ++c)
            {
                if (auto cell = remainder->cell(r, c))
                {
                    shift_subtree_position(*cell, dy);
                }
            }
            row_top += row_height;
        }

        // The remainder's own frame mirrors the kept table's column layout (widths are
        // unchanged by a row/cell-content split) and spans exactly the rows it now holds.
        auto& remainder_frame = remainder->edit_layout_box().edit_frame(nodes::layout_proof_or_throw(table));
        remainder_frame.position = {sealed_frame(table).position.x, new_page_top_y};
        remainder_frame.size = {sealed_frame(table).size.width, row_top - new_page_top_y};

        // The kept table's own frame must shrink to cover only the rows left behind.
        float kept_height = 0.0F;
        for (int r = 0; r < table.row_count(); ++r)
        {
            kept_height += table.row_height(r);
        }
        sealed_edit_frame(table).size.height = kept_height;

        return remainder;
    }

    std::shared_ptr<nodes::DocraftLoomTable> DocraftLoomPaginationProcessor::try_split_table(
        nodes::DocraftLoomTable &table, float page_bottom_y, float new_page_top_y) {
        const int rows = table.row_count();
        const int cols = table.column_count();
        if (cols == 0) {
            return nullptr;
        }

        const bool at_fresh_page_top = std::abs(sealed_frame(table).position.y - new_page_top_y) < 0.01F;

        // A row "fits" on the current page only if every one of its cells clears
        // page_bottom_y. All cells in a row share the same height (Layout assigns row
        // height uniformly), so reading cell(r, 0) is enough to know the row's bottom.
        // A table with a single row can't be split at the row level at all -- fit_rows
        // stays 0, since the caller only reaches try_split_table when the child (here,
        // the table) doesn't already fit as a whole, so that lone row can't fit either.
        int fit_rows = 0;
        if (rows > 1) {
            for (int r = 0; r < rows; ++r) {
                auto reference_cell = table.cell(r, 0);
                const float row_bottom = reference_cell
                                             ? sealed_frame(*reference_cell).position.y + table.row_height(r)
                                             : -1.0F;
                //row_bottom can't be negative, if it is, it means the cell is missing or has no layout box yet (e.g. a blank line), so the row doesn't fit.
                if (row_bottom == -1.0F || row_bottom > page_bottom_y + 0.01F) {
                    break;
                }
                ++fit_rows;
            }
        }

        if (fit_rows >= rows) {
            // Everything already fits.
            return nullptr;
        }

        // If the table already starts at a fresh page's top and every row that fits is
        // itself a repeated header row (no genuine content row made it on), row `fit_rows`
        // is a genuinely oversized row: taller than one whole page, so no amount of
        // row-level splitting would ever place it whole -- split_after_row would just
        // reproduce a table shaped [cloned header(s), oversized row] on every following
        // page forever. Try splitting that row's own cell content (its wrapped text)
        // across this page and the next instead of giving up on it.
        if (at_fresh_page_top && fit_rows <= table.leading_title_row_count()) {
            auto reference_cell = table.cell(fit_rows, 0);
            const float row_top = reference_cell ? sealed_frame(*reference_cell).position.y : new_page_top_y;
            const float available_height = page_bottom_y - row_top;
            if (auto content_remainder = table.split_row_content(fit_rows, available_height,
                                                                 /*repeat_header_rows=*/true)) {
                return finish_table_split(table, content_remainder, new_page_top_y);
            }
            // The row's cells couldn't be split either (e.g. non-text content, or not
            // even one line fits) -- let the caller's oversized-escape-hatch accept the
            // whole table as overflow rather than looping.
            return nullptr;
        }

        if (fit_rows == 0) {
            // Nothing fits and the table isn't at a fresh page top yet -- give it a
            // whole-node move to a fresh page first; the check above will get another
            // chance to split its content once it lands there.
            return nullptr;
        }

        auto remainder = table.split_after_row(fit_rows, /*repeat_header_rows=*/true);
        if (!remainder) {
            return nullptr;
        }

        return finish_table_split(table, remainder, new_page_top_y);
    }

    int DocraftLoomPaginationProcessor::paginate_body(nodes::DocraftLoomNode& body_root, float body_top_y,
                                                      float body_height,
                                                      backend::IDocraftPageRenderingBackend* page_backend)
    {
        // DocraftLoomNode only exposes append (add_child) and remove-by-index, not
        // insert-at-index, so the split's remainder can't be spliced directly into
        // body_root's own storage. Instead, the top-level children are pulled out into a
        // local work list (which supports std::vector::insert), processed there, then
        // written back into body_root in final order once pagination is complete.
        // The body's own root container (e.g. a VStack) must be visited on every page so
        // Rendering recurses into it -- the real per-page decision happens at its direct
        // children, which get concrete indices below.
        seal_page_index(body_root, -1);

        std::vector<std::shared_ptr<nodes::DocraftLoomNode>> children;
        children.reserve(static_cast<std::size_t>(body_root.children_count()));
        while (body_root.children_count() > 0)
        {
            children.push_back(body_root.remove_child(0));
        }

        // Layout already computed the correct gap between each pair of top-level
        // siblings (their collapsed margin, or the container's own spacing) while laying
        // them out on its continuous, infinite-height canvas. paginate_body's job is only
        // to re-flow that continuous canvas across finite physical pages, not to
        // re-derive spacing -- so the gap is captured here, from Layout's untouched
        // positions, before any page-breaking shift mutates them, and simply replayed
        // below. Keyed by node pointer rather than index because splitting a table
        // inserts a remainder into the middle of `children`, which would silently
        // misalign a plain index-based lookup.
        std::unordered_map<const nodes::DocraftLoomNode*, float> original_gap_after;
        for (std::size_t k = 0; k + 1 < children.size(); ++k)
        {
            if (!children[k] || !children[k + 1])
            {
                continue;
            }
            original_gap_after[children[k].get()] =
                sealed_frame(*children[k + 1]).position.y
                - (sealed_frame(*children[k]).position.y + sealed_frame(*children[k]).size.height);
        }
        auto gap_after = [&](const nodes::DocraftLoomNode* node) -> float
        {
            const auto it = original_gap_after.find(node);
            return it != original_gap_after.end() ? it->second : 0.0F;
        };

        int current_page = 0;
        float next_y = body_top_y; // where the next child's top should land on the current page
        float page_bottom_y = body_top_y + body_height;

        std::size_t i = 0;
        while (i < children.size())
        {
            auto& child = children[static_cast<std::size_t>(i)];
            if (!child)
            {
                ++i;
                continue;
            }

            // A forced page break: stamp it on the current page (it draws nothing, so
            // which page doesn't matter) and unconditionally advance, regardless of how
            // much room is left on the current page.
            if (dynamic_cast<nodes::DocraftLoomNewPage*>(child.get()))
            {
                assign_page_index_recursive(*child, current_page);
                if (page_backend)
                {
                    page_backend->add_new_page();
                }
                ++current_page;
                page_bottom_y = body_top_y + body_height;
                next_y = body_top_y;
                ++i;
                continue;
            }

            // Move the child into place for this attempt: from wherever it currently
            // sits (either Layout's original continuous-canvas position, the first time
            // it's seen, or a prior failed placement, on a retry) to next_y.
            const float current_top = sealed_frame(*child).position.y;
            shift_subtree_position(*child, next_y - current_top);

            const auto& frame = sealed_frame(*child);
            const float bottom = frame.position.y + frame.size.height;
            if (const bool fits = bottom <= page_bottom_y + 0.01F)
            {
                assign_page_index_recursive(*child, current_page);
                next_y = bottom + gap_after(child.get());
                ++i;
                continue;
            }

            // Doesn't fit as a whole -- try splitting it if it's a table. This must be
            // attempted before the oversized-escape check below: a table's remainder is
            // always re-stacked to start exactly at body_top_y (see try_split_table), so
            // it would otherwise satisfy "starts at body top and still doesn't fit" and
            // get accepted whole as unsplittable overflow instead of being split again
            // across as many further pages as it needs.
            if (auto* table = dynamic_cast<nodes::DocraftLoomTable*>(child.get()))
            {
                if (auto remainder = try_split_table(*table, page_bottom_y, body_top_y))
                {
                    assign_page_index_recursive(*child, current_page);
                    if (page_backend)
                    {
                        page_backend->add_new_page();
                    }
                    ++current_page;
                    page_bottom_y = body_top_y + body_height;
                    assign_page_index_recursive(*remainder, current_page);
                    children.insert(children.begin() + static_cast<std::ptrdiff_t>(i) + 1, remainder);
                    // The remainder stands in for "the rest of the table" from here on,
                    // so it inherits the original table's trailing gap -- whatever
                    // followed the table before splitting must still be separated from
                    // the remainder by that same amount.
                    if (const auto it = original_gap_after.find(child.get()); it != original_gap_after.end())
                    {
                        original_gap_after[remainder.get()] = it->second;
                    }
                    // The remainder was already re-stacked to start exactly at body_top_y
                    // by try_split_table, so the next iteration's shift for it is a no-op.
                    next_y = body_top_y;
                    ++i;
                    continue;
                }
            }

            // A child that already starts exactly at the page's body top and still
            // doesn't fit -- and, if it's a table, couldn't be split further (e.g. down
            // to a single row) -- is simply too tall for one page -- accept it as an
            // overflow rather than looping forever creating empty pages for it.
            const bool oversized_escape = std::abs(frame.position.y - body_top_y) < 0.01F;
            if (oversized_escape)
            {
                // The content past page_bottom_y is still written to the PDF's content
                // stream (nothing is lost), but nothing on the rendered page or in the
                // tool's exit code says so -- log it so it's at least visible in
                // CI/pipeline output, matching how an unresolved ${variable} is reported.
                const std::string node_label =
                    child->name().empty() ? std::string{"A node"} : fmt::format("Node '{}'", child->name());
                LOG_WARNING(fmt::format(
                    "{} on page {} is {:.1f}pt tall, taller than the {:.1f}pt available on the page -- "
                    "it overflows past the page bottom and the content beyond it will not be visible "
                    "in the rendered PDF.",
                    node_label, current_page + 1, frame.size.height, body_height));

                assign_page_index_recursive(*child, current_page);
                next_y = bottom + gap_after(child.get());
                ++i;
                continue;
            }

            // Whole-node move to a fresh page: retry this same child next iteration,
            // now targeting the new page's body top.
            if (page_backend)
            {
                page_backend->add_new_page();
            }
            ++current_page;
            page_bottom_y = body_top_y + body_height;
            next_y = body_top_y;
        }

        for (auto& child : children)
        {
            body_root.add_child(std::move(child));
        }

        return current_page + 1;
    }
} // pipeline
