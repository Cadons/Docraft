//
// Created by Matteo on 27/06/2026.
//

#include "docraft/loom/pipeline/docraft_loom_pagination_processor.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <vector>

#include "docraft/loom/nodes/docraft_loom_hstack.h"
#include "docraft/loom/nodes/docraft_loom_list.h"
#include "docraft/loom/nodes/docraft_loom_new_page.h"
#include "docraft/loom/nodes/docraft_loom_rectangle.h"
#include "docraft/loom/nodes/docraft_loom_table.h"
#include "docraft/loom/nodes/docraft_loom_vstack.h"

namespace docraft::loom::pipeline {
    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomText*)
    {
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomTitle*)
    {
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomSubtitle*)
    {
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomRectangle* node)
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

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomParagraph*)
    {
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomVStack* node)
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

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomHStack* node)
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

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomBlankLine*)
    {
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomImage*)
    {
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomLine*)
    {
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomCircle*)
    {
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomTriangle*)
    {
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomPolygon*)
    {
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomList* node)
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
    }

    void DocraftLoomPaginationProcessor::visit(docraft::loom::nodes::DocraftLoomNewPage*)
    {
        // The actual forced break is handled directly in paginate_body(), which needs to
        // special-case it before the normal fits-on-this-page check runs.
    }

    void DocraftLoomPaginationProcessor::assign_page_index_recursive(nodes::DocraftLoomNode& node, int page_index)
    {
        node.edit_layout_box().page_index = page_index;

        // Table cells live in the table's own grid_, not in the inherited children_
        // vector, so they need their own recursion branch.
        if (auto* table = dynamic_cast<nodes::DocraftLoomTable*>(&node))
        {
            for (int r = 0; r < table->row_count(); ++r)
            {
                for (int c = 0; c < table->column_count(); ++c)
                {
                    if (auto cell = table->cell(r, c))
                    {
                        assign_page_index_recursive(*cell, page_index);
                    }
                }
            }
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
        node.edit_layout_box().frame.position.y += dy;

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
            for (int r = 0; r < table->row_count(); ++r)
            {
                for (int c = 0; c < table->column_count(); ++c)
                {
                    if (auto cell = table->cell(r, c))
                    {
                        shift_subtree_position(*cell, dy);
                    }
                }
            }
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

    std::shared_ptr<nodes::DocraftLoomTable> DocraftLoomPaginationProcessor::try_split_table(
        nodes::DocraftLoomTable& table, float page_bottom_y, float new_page_top_y)
    {
        const int rows = table.row_count();
        const int cols = table.column_count();
        if (rows <= 1 || cols == 0)
        {
            // Nothing meaningful to split off -- a single row can't be broken further
            // in this simplified model (no intra-row/cell splitting).
            return nullptr;
        }

        // A row "fits" on the current page only if every one of its cells clears
        // page_bottom_y. All cells in a row share the same height (Layout assigns row
        // height uniformly), so reading cell(r, 0) is enough to know the row's bottom.
        int fit_rows = 0;
        for (int r = 0; r < rows; ++r)
        {
            auto reference_cell = table.cell(r, 0);
            if (!reference_cell)
            {
                break;
            }
            const auto& frame = reference_cell->layout_box().frame;
            const float row_bottom = frame.position.y + frame.size.height;
            if (row_bottom > page_bottom_y + 0.01F)
            {
                break;
            }
            ++fit_rows;
        }

        if (fit_rows == 0 || fit_rows >= rows)
        {
            // Either nothing fits (caller's oversized-escape-hatch handles that case by
            // forcing the whole table onto a fresh page) or everything already fits.
            return nullptr;
        }

        auto remainder = table.split_after_row(fit_rows, /*repeat_header_rows=*/true);
        if (!remainder)
        {
            return nullptr;
        }

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
            const float row_height = reference_cell->layout_box().frame.size.height;
            const float dy = row_top - reference_cell->layout_box().frame.position.y;
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
        // unchanged by a row split) and spans exactly the rows it now holds.
        auto& remainder_frame = remainder->edit_layout_box().frame;
        remainder_frame.position = {table.layout_box().frame.position.x, new_page_top_y};
        remainder_frame.size = {table.layout_box().frame.size.width, row_top - new_page_top_y};

        // The kept table's own frame must shrink to cover only the rows left behind.
        float kept_height = 0.0F;
        for (int r = 0; r < table.row_count(); ++r)
        {
            if (auto reference_cell = table.cell(r, 0))
            {
                kept_height += reference_cell->layout_box().frame.size.height;
            }
        }
        table.edit_layout_box().frame.size.height = kept_height;

        return remainder;
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
        body_root.edit_layout_box().page_index = -1;

        std::vector<std::shared_ptr<nodes::DocraftLoomNode>> children;
        children.reserve(static_cast<std::size_t>(body_root.children_count()));
        while (body_root.children_count() > 0)
        {
            children.push_back(body_root.remove_child(0));
        }

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
            const float current_top = child->layout_box().frame.position.y;
            shift_subtree_position(*child, next_y - current_top);

            const auto& frame = child->layout_box().frame;
            const float bottom = frame.position.y + frame.size.height;
            const bool fits = bottom <= page_bottom_y + 0.01F;

            if (fits)
            {
                assign_page_index_recursive(*child, current_page);
                next_y = bottom;
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
                assign_page_index_recursive(*child, current_page);
                next_y = bottom;
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
