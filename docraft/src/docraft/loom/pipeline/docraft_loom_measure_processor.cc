//
// Created by Matteo on 21/06/2026.
//

#include "docraft/loom/pipeline/docraft_loom_measure_processor.h"

#include <algorithm>
#include <cmath>
#include <memory>

#include "docraft/exception/docraft_input_exceptions.h"
#include "docraft/loom/nodes/docraft_loom_blank_line.h"
#include "docraft/loom/nodes/docraft_loom_hstack.h"
#include "docraft/loom/nodes/docraft_loom_circle.h"
#include "docraft/loom/nodes/docraft_loom_curve_line.h"
#include "docraft/loom/nodes/docraft_loom_image.h"
#include "docraft/loom/nodes/docraft_loom_line.h"
#include "docraft/loom/nodes/docraft_loom_list.h"
#include "docraft/loom/nodes/docraft_loom_new_page.h"
#include "docraft/loom/nodes/docraft_loom_paragraph.h"
#include "docraft/loom/nodes/docraft_loom_polygon.h"
#include "docraft/loom/nodes/docraft_loom_triangle.h"
#include "docraft/loom/nodes/docraft_loom_page_number.h"
#include "docraft/loom/nodes/docraft_loom_canvas.h"
#include "docraft/loom/nodes/docraft_loom_rectangle.h"
#include "docraft/loom/nodes/docraft_loom_subtitle.h"
#include "docraft/loom/nodes/docraft_loom_table.h"
#include "docraft/loom/nodes/docraft_loom_text.h"
#include "docraft/loom/nodes/docraft_loom_title.h"
#include "docraft/loom/nodes/docraft_loom_vstack.h"
#include "docraft/loom/pipeline/docraft_loom_weighted_distribution.h"

namespace docraft::loom::pipeline {
    namespace {
        // Standard typographic leading: measure_text_height() alone is just the raw
        // font-metric glyph height, with no gap between consecutive lines -- applied
        // only to wrapped multi-line text (see visit(DocraftLoomText*) and
        // visit(DocraftLoomTableCell*)), never to a single natural-width line, whose
        // own height feeds into inter-node spacing/margin instead (a separate concern).
        constexpr float kWrappedLineHeightMultiplier = 1.2F;
    }

    DocraftLoomMeasureProcessor::DocraftLoomMeasureProcessor(
        const std::shared_ptr<backend::IDocraftTextRenderingBackend> &text_backend)
        : text_backend_(text_backend), text_wrapper_(text_backend) {
    }

    void DocraftLoomMeasureProcessor::set_content_width(float width) {
        content_width_ = width;
        // A single processor instance is reused across header/footer/body (see
        // DocraftLoomPdfCreator::create()), each a wholly separate traversal that must
        // start with no ancestor-pushed wrap constraint. Without this, a weighted HStack
        // whose last column is a leaf node (Image/Line/Circle/Triangle/Polygon -- none of
        // which read or clear inherited_wrap_width_, unlike Text/Paragraph/VStack/HStack/
        // Rectangle/Table) leaves inherited_wrap_width_ set to that column's width after
        // the region finishes, and the next region's first Text without an explicit
        // wrap_width would silently inherit it instead of content_width_.
        inherited_wrap_width_ = 0.0F;
    }

    float DocraftLoomMeasureProcessor::incoming_width() const {
        return inherited_wrap_width_ > 0.0F ? inherited_wrap_width_ : content_width_;
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomText *text) {
        if (!text) {
            return;
        }
        const std::string reg_font = text->resolved_font_name();
        const float font_size = text->font_size();
        auto &measure_size = text->edit_layout_box().measured_size;

        // An explicit wrap_width() always wins; otherwise fall back to whatever width a
        // weighted ancestor (HStack column, ...) pushed down for this text to
        // auto-wrap into, consumed here so it doesn't leak to whatever gets measured
        // next.
        const float effective_wrap_width = text->wrap_width() > 0.0F ? text->wrap_width() : inherited_wrap_width_;
        inherited_wrap_width_ = 0.0F;

        text->set_ascent(text_backend_->measure_text_ascent(reg_font, font_size));
        //ascent is positive the space above the baseline, so it is stored as a positive number
        text->set_descent(text_backend_->measure_text_descent(reg_font, font_size));
        //descent is negative the space below the baseline, so it is stored as a negative number
        if (effective_wrap_width > 0.0F) {
            auto lines = text_wrapper_.wrap(text->text(), effective_wrap_width, reg_font, font_size);
            const float line_height = text_backend_->measure_text_height(reg_font, font_size) *
                                      kWrappedLineHeightMultiplier;
            measure_size.width = effective_wrap_width;
            measure_size.height = line_height * static_cast<float>(lines.size());
            text->set_wrap_width(effective_wrap_width); // so Rendering knows the box width too
            text->set_wrapped_lines(std::move(lines));
        } else {
            measure_size.width = text_backend_->measure_text_width(text->text(), reg_font, font_size);
            measure_size.height = text_backend_->measure_text_height(reg_font, font_size);
            text->set_wrapped_lines({});
        }
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomTitle *node) {
        visit(static_cast<docraft::loom::nodes::DocraftLoomText *>(node));
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomSubtitle *node) {
        visit(static_cast<docraft::loom::nodes::DocraftLoomText *>(node));
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomRectangle *node) {
        if (!node)
            return;

        // A width pushed down by an ancestor (or, at the root -- Body/Header/Footer --
        // the region's own content_width_) constrains this rectangle's children the
        // same way a weighted HStack column or Table cell already does for Text, so
        // plain block content (Text, Paragraph, VStack, ...) wraps to the page/section
        // width instead of measuring at its unbounded natural width.
        const float incoming_width = this->incoming_width();
        inherited_wrap_width_ = 0.0F;
        const float own_width = node->width() > 0.0F ? node->width() : incoming_width;
        const float padding = node->effective_padding();
        const float children_wrap_width =
                own_width > 0.0F ? std::max(0.0F, own_width - (2.0F * padding)) : 0.0F;

        float child_width = 0.0F;
        float child_height = node->resolve_outer_margin(*node, /*leading=*/true);
        const int n = node->children_count();
        for (int i = 0; i < n; ++i) {
            inherited_wrap_width_ = children_wrap_width;
            auto child = node->edit_child(i);
            child->accept(*this);
            const auto &sz = child->layout_box().measured_size;
            child_height += sz.height;
            if (i < n - 1) {
                const auto next = node->child(i + 1);
                child_height += nodes::DocraftLoomLayoutContainer::resolve_child_gap(
                    node->spacing(), child->margin().bottom, next->margin().top);
            }
            // Cross-axis margins (left/right) have no sibling on this axis to collapse
            // against, unlike top/bottom -- they're a plain per-child inset the
            // rectangle must widen to still fit, mirroring how VStack treats them.
            child_width = std::max(child_width, sz.width + child->margin().left + child->margin().right);
        }
        child_height += node->resolve_outer_margin(*node, /*leading=*/false);
        const bool has_children = n > 0;
        auto &measured_size = node->edit_layout_box().measured_size;
        measured_size.height = has_children ? (child_height + (2.0F * padding)) : node->height();
        const auto computed_width = has_children ? (child_width + (2.0F * padding)) : 0.0F;
        measured_size.width = node->width() > 0.0F
                                  ? node->width()
                                  : computed_width;
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomCanvas *node) {
        if (!node)
            return;

        // Unlike Rectangle, Canvas's own size never derives from its children -- they are
        // free-positioned and may legitimately sit partially or fully outside its bounds
        // by design (see visit(DocraftLoomCanvas*) in the layout/rendering processors).
        // Children are still measured (recursed into) so each gets its own natural
        // measured_size, needed for rendering geometry (e.g. a Line's width from its
        // start/end points, a Circle's diameter). Relay the canvas's own width as the
        // wrap width, mirroring Rectangle's children_wrap_width relay, so a Text/
        // Paragraph child wraps within the canvas bounds instead of the page width.
        const int n = node->children_count();
        for (int i = 0; i < n; ++i) {
            inherited_wrap_width_ = node->width();
            auto child = node->edit_child(i);
            child->accept(*this);
        }

        auto &measured_size = node->edit_layout_box().measured_size;
        measured_size.width = node->width();
        measured_size.height = node->height();
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomParagraph *paragraph) {
        if (!paragraph)
            return;

        float total_height = paragraph->space_before() + paragraph->space_after(); // Start with space before and after
        float max_width = 0.0f;

        // A width pushed down by a weighted ancestor applies to every child of this
        // paragraph in turn -- each DocraftLoomText consumes (clears) it for itself, so
        // it must be re-armed here before every child, not just the first.
        const float incoming_wrap_width = inherited_wrap_width_;

        for (int i = 0; i < paragraph->children_count(); ++i) {
            inherited_wrap_width_ = incoming_wrap_width;
            auto child = paragraph->edit_child(i);
            child->accept(*this);
            const auto &child_size = child->layout_box().measured_size;
            total_height += child_size.height * paragraph->line_spacing();
            // Update max_width if the child's width is greater
            if (child_size.width > max_width)
                max_width = child_size.width;
        }
        // Set the measured size of the paragraph node
        auto &measured_size = paragraph->edit_layout_box().measured_size;
        measured_size.width = max_width;
        measured_size.height = total_height;
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomVStack *node) {
        if (!node) return;

        // A VStack has no explicit width of its own -- it shrink-wraps to its widest
        // child (plus 2x padding()) rather than owning a fixed width like Rectangle --
        // but it does narrow the width relayed to children by its own padding(), the
        // same inset Rectangle already applies around its children.
        const float incoming_width = this->incoming_width();
        inherited_wrap_width_ = 0.0F;
        const float padding = node->effective_padding();
        const float children_wrap_width = std::max(0.0F, incoming_width - (2.0F * padding));

        float total_height = node->resolve_outer_margin(*node, /*leading=*/true);
        float max_width = 0.0F;
        const int n = node->children_count();
        const auto gaps = node->resolve_vertical_child_gaps(*node, node->spacing());
        for (int i = 0; i < n; ++i) {
            inherited_wrap_width_ = children_wrap_width;
            auto child = node->edit_child(i);
            child->accept(*this);
            const auto &sz = child->layout_box().measured_size;
            total_height += sz.height;
            if (i < n - 1) {
                total_height += gaps[static_cast<std::size_t>(i)];
            }
            // Cross-axis margins (left/right) have no sibling on this axis to collapse
            // against, unlike top/bottom -- they're a plain per-child inset the column
            // must widen to still fit, mirroring how HStack treats top/bottom below.
            max_width = std::max(max_width, sz.width + child->margin().left + child->margin().right);
        }
        total_height += node->resolve_outer_margin(*node, /*leading=*/false);
        auto &ms = node->edit_layout_box().measured_size;
        ms.width = n > 0 ? max_width + (2.0F * padding) : 0.0F;
        // An explicit height() is a hard override, same as DocraftLoomRectangle's own
        // explicit width() above -- this is what gives DocraftLoomLayoutProcessor's
        // weighted-height distribution a real, bounded amount to divide among children.
        const auto computed_height = total_height + (n > 0 ? (2.0F * padding) : 0.0F);
        ms.height = node->height() > 0.0F ? node->height() : computed_height;
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomHStack *node) {
        if (!node) return;
        float total_width = 0.0F;
        float max_height = 0.0F;
        const int n = node->children_count();
        const auto &weights = node->weights();

        // Capture whatever width an ancestor (Rectangle/VStack now always arm one)
        // pushed down before clearing it: without weights, HStack's contract is
        // shrink-to-fit (each child gets its own natural width, see class doc), so a
        // stale inherited value must not leak into the first child -- but the weighted
        // branch below still needs it (an ancestor's constraint, not content_width_, is
        // what a nested weighted HStack must divide among its columns).
        const float incoming_width = this->incoming_width();
        inherited_wrap_width_ = 0.0F;

        // Precomputed once, upfront: margin() is a static per-child property (doesn't
        // depend on measurement), so every gap can be resolved before either the
        // weighted-width math below or the final accumulation loop need it -- both
        // must agree on the same gaps, or resolved column widths wouldn't actually sum
        // back to content_width_.
        const auto gaps = node->resolve_horizontal_child_gaps(*node, node->spacing());
        float total_gap = 0.0F;
        for (const float gap: gaps) {
            total_gap += gap;
        }

        // Like the inter-child gaps above, the first/last child's own left/right margin
        // has no sibling to combine with -- it's reserved outright between the
        // container's content edge and that child.
        const float leading_margin = node->resolve_outer_margin(*node, /*leading=*/true);
        const float trailing_margin = node->resolve_outer_margin(*node, /*leading=*/false);

        // Opt-in, mirrors DocraftLoomLayoutProcessor's weighted branch: resolve each
        // column's width from weights alone (no natural-width floor here, unlike
        // Layout's) and push it down as a wrap constraint before measuring that child,
        // so e.g. a Paragraph's Text auto-wraps to fit its column instead of the column
        // having to grow to fit unwrapped text. Layout's own floor becomes a no-op once
        // that child's natural width already equals its resolved share. Divides
        // incoming_width (the ancestor's constraint, or content_width_ at the root) --
        // not content_width_ unconditionally -- so a weighted HStack nested inside a
        // narrower Rectangle/VStack divides that narrower budget, not the full page.
        const float padding = node->effective_padding();
        std::vector<float> resolved_widths;
        if (!weights.empty() && n > 0 && incoming_width > 0.0F) {
            const float available_width = std::max(0.0F,
                                                   incoming_width - total_gap - leading_margin - trailing_margin - (2.0F
                                                       * padding));
            resolved_widths = distribute_weighted_amounts(available_width, weights, n);
        }

        for (int i = 0; i < n; ++i) {
            if (!resolved_widths.empty()) {
                inherited_wrap_width_ = resolved_widths[static_cast<std::size_t>(i)];
            }
            auto child = node->edit_child(i);
            child->accept(*this);
            const auto &sz = child->layout_box().measured_size;
            total_width += sz.width;
            if (i < n - 1) {
                total_width += gaps[static_cast<std::size_t>(i)];
            }
            // Cross-axis margins (top/bottom) have no sibling on this axis to collapse
            // against, unlike left/right -- they're a plain per-child inset the row
            // must grow to still fit, mirroring how VStack treats left/right above.
            max_height = std::max(max_height, sz.height + child->margin().top + child->margin().bottom);
        }
        auto &ms = node->edit_layout_box().measured_size;
        ms.width = total_width + leading_margin + trailing_margin + (n > 0 ? (2.0F * padding) : 0.0F);
        ms.height = max_height + (n > 0 ? (2.0F * padding) : 0.0F);
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomBlankLine *node) {
        if (!node) return;
        // Width depends on the container (a blank line stretches to the available width),
        // so it's left to 0 here and resolved by the layout stage, same as legacy's blank line.
        auto &measured_size = node->edit_layout_box().measured_size;
        measured_size.width = 0.0F;
        measured_size.height = node->height();
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomImage *node) {
        if (!node)
            return;
        auto &measured_size = node->edit_layout_box().measured_size;
        measured_size.width = node->width();
        measured_size.height = node->height();
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomLine *node) {
        if (!node)
            return;
        auto &measured_size = node->edit_layout_box().measured_size;
        // start()/end() are offsets from the node's own anchor (frame.position), not a
        // pre-normalized bounding box, so the box the line occupies runs from that anchor
        // out to the farthest endpoint -- see visit(DocraftLoomLine*) in the rendering
        // processor. Endpoints with negative coordinates draw outside this box on purpose
        // (a Canvas clips them); they must not shrink it below the anchor.
        measured_size.width = std::max({node->start().x, node->end().x, 0.0F});
        // The box must cover the line's actual vertical extent too, not just border_width
        // -- otherwise a vertical/diagonal line's box collapses to the border-width floor
        // and the line gets clipped/pushed against following content.
        measured_size.height = std::max({node->start().y, node->end().y, node->border_width(), 4.0F});
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomCircle *node) {
        if (!node)
            return;
        // The node's box is the ellipse's bounding box; for the circular case the two
        // semi-axes are equal, so this stays the diameter on both sides.
        auto &measured_size = node->edit_layout_box().measured_size;
        measured_size.width = node->radius_x() * 2.0F;
        measured_size.height = node->radius_y() * 2.0F;
    }

    namespace {
        void measure_points_bounding_box(const std::vector<nodes::Position> &points, nodes::Size &measured_size) {
            if (points.size() < 2) {
                measured_size = {};
                return;
            }
            float min_x = points[0].x, max_x = points[0].x;
            float min_y = points[0].y, max_y = points[0].y;
            for (const auto &pt: points) {
                min_x = std::min(min_x, pt.x);
                max_x = std::max(max_x, pt.x);
                min_y = std::min(min_y, pt.y);
                max_y = std::max(max_y, pt.y);
            }
            measured_size.width = max_x - min_x;
            measured_size.height = max_y - min_y;
        }
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomTriangle *node) {
        if (!node)
            return;
        measure_points_bounding_box(node->points(), node->edit_layout_box().measured_size);
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomPolygon *node) {
        if (!node)
            return;
        measure_points_bounding_box(node->points(), node->edit_layout_box().measured_size);
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomCurveLine *node) {
        if (!node)
            return;
        // A curve occupies the bounding box of the points it interpolates, exactly like a
        // polygon over the same points. The spline can bow slightly outside that box
        // between two points; that is left alone deliberately, since growing the box to
        // chase the overshoot would push following content around for a few points of ink.
        measure_points_bounding_box(node->points(), node->edit_layout_box().measured_size);
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomList *node) {
        if (!node)
            return;
        const int n = node->children_count();
        auto &markers = node->edit_markers();
        markers.resize(static_cast<std::size_t>(n));

        // A width pushed down by an ancestor (or content_width_ at the root) applies to
        // every item's Text in turn, minus that item's own marker prefix -- captured
        // once here and re-armed before each item below, mirroring
        // DocraftLoomParagraph's relay (a single accept() call per item would otherwise
        // only let the first item consume it, leaving the rest unwrapped).
        const float incoming_width = this->incoming_width();
        inherited_wrap_width_ = 0.0F;

        float max_width = 0.0F;
        float total_height = 0.0F;
        for (int i = 0; i < n; ++i) {
            auto text_child = std::dynamic_pointer_cast<nodes::DocraftLoomText>(node->edit_child(i));
            if (!text_child) {
                throw exception::InvalidInputException("List items must be Text nodes");
            }

            auto &marker = markers[static_cast<std::size_t>(i)];
            marker.text = node->marker_text_for_index(i);
            if (node->marker_is_box()) {
                marker.kind = nodes::DocraftLoomList::Marker::Kind::kBox;
                marker.width = text_child->font_size() * 0.6F;
            } else {
                marker.kind = nodes::DocraftLoomList::Marker::Kind::kText;
                const std::string reg_font = text_child->resolved_font_name();
                marker.width = marker.text.empty()
                                   ? 0.0F
                                   : text_backend_->measure_text_width(marker.text, reg_font, text_child->font_size());
            }
            const float gap = marker.width > 0.0F ? node->marker_gap() : 0.0F;

            if (incoming_width > 0.0F) {
                inherited_wrap_width_ = std::max(0.0F, incoming_width - marker.width - gap);
            }
            text_child->accept(*this);

            const auto &child_size = text_child->layout_box().measured_size;
            max_width = std::max(max_width, marker.width + gap + child_size.width);
            total_height += child_size.height;
        }

        auto &measured_size = node->edit_layout_box().measured_size;
        measured_size.width = max_width;
        measured_size.height = total_height;
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomTableCell *cell) {
        if (!cell)
            return;
        const float wrap_budget = pending_cell_wrap_budget_;
        pending_cell_wrap_budget_ = 0.0F;

        auto &measured_size = cell->edit_layout_box().measured_size;
        if (auto content = cell->content()) {
            content->accept(*this);

            // Only Text can overflow a column and needs wrapping; unlike a weighted
            // HStack child (which always stretches to inherited_wrap_width_), this only
            // kicks in when the cell's natural width actually exceeds the budget, so
            // short content keeps its normal shrink-to-fit natural width.
            if (auto text = std::dynamic_pointer_cast<nodes::DocraftLoomText>(content)) {
                const auto &natural_size = text->layout_box().measured_size;
                if (wrap_budget > 0.0F && natural_size.width > wrap_budget) {
                    const std::string reg_font = text->resolved_font_name();
                    const float font_size = text->font_size();
                    auto lines = text_wrapper_.wrap(text->text(), wrap_budget, reg_font, font_size);
                    const float line_height = text_backend_->measure_text_height(reg_font, font_size) *
                                              kWrappedLineHeightMultiplier;
                    auto &text_measured = text->edit_layout_box().measured_size;
                    text_measured.width = wrap_budget;
                    text_measured.height = line_height * static_cast<float>(lines.size());
                    text->set_wrap_width(wrap_budget);
                    text->set_wrapped_lines(std::move(lines));
                }
            }

            // Cell size includes a small automatic inset around content, so text never
            // sits flush against the cell's own border -- mirrors how Rectangle's own
            // padding_ inflates its measured_size around its children.
            const auto &content_size = content->layout_box().measured_size;
            measured_size.width = content_size.width + (2.0F * nodes::DocraftLoomTable::kCellPaddingX);
            measured_size.height = content_size.height + (2.0F * nodes::DocraftLoomTable::kCellPaddingY);
        } else {
            measured_size = {};
        }
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomTable *table) {
        if (!table)
            return;

        // Table resolves per-column wrap budgets itself (pending_cell_wrap_budget_)
        // rather than through inherited_wrap_width_ -- capture whatever value an
        // ancestor pushed down (Rectangle/VStack now always arm one) before clearing it,
        // so it doesn't leak into a cell's Text measurement alongside the budget
        // computed below, but the weight-based estimate can still divide it (rather than
        // content_width_ unconditionally) for a Table nested in a narrower container.
        const float incoming_width = this->incoming_width();
        inherited_wrap_width_ = 0.0F;

        const int rows = table->row_count();
        const int cols = table->column_count();

        // A column counts as "fixed" as soon as ONE of its cells sets Cell width="...",
        // no matter which row -- so this scans every row before measuring any cell,
        // not just the row currently being visited. Without this, a row that omits
        // width() (inheriting the column from a sibling row) would look flexible here,
        // and the wrap budget below would shrink that column instead of matching what
        // it's actually painted at. Same idea as
        // DocraftLoomLayoutProcessor::gather_table_natural_geometry's explicit_widths,
        // minus natural_widths -- no cell has been measured yet at this point.
        std::vector column_explicit_widths(static_cast<std::size_t>(cols), 0.0F);
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                if (auto width = table->cell(r, c)->explicit_width(); width.has_value()) {
                    column_explicit_widths[static_cast<std::size_t>(c)] =
                            std::max(column_explicit_widths[static_cast<std::size_t>(c)], *width);
                }
            }
        }

        // Upfront estimate of each column's wrap ceiling, using the same fixed-vs-
        // flexible split Layout performs for real once every cell is measured (see
        // resolve_table_column_widths) -- fixed columns keep column_explicit_widths
        // verbatim, flexible columns split whatever's left by weight. The one thing
        // this pass can't do yet is floor flexible columns at their natural width
        // (nothing has been measured), so it's an estimate, not the final word: a cell
        // only wraps if its own text turns out wider than this budget (see
        // visit(DocraftLoomTableCell*)), so a short cell in a wide column is never
        // disturbed by an under-estimate here.
        //
        // resolve_fixed_and_flexible_amounts() always honors a fixed column's own
        // explicit width, even when available_width below is 0 -- so a cell with its
        // own width() still gets a real wrap budget with no page/content width set at
        // all; only the flexible columns then get no budget (0), same as before.
        //
        // kCellPaddingX is a per-cell content inset, already reflected in each cell's
        // own measured_size -- see the matching comment in
        // DocraftLoomLayoutProcessor::resolve_table_column_widths -- so it must not
        // also be subtracted here as a table-wide margin.
        float available_width = 0.0F;
        if (incoming_width > 0.0F) {
            available_width = std::max(0.0F, incoming_width - (2.0F * table->padding()));
        }
        const auto resolved_widths = resolve_fixed_and_flexible_amounts({
                .available_amount = available_width,
                .count = cols,
                .weights = table->column_weights(),
                .explicit_amounts = column_explicit_widths,
        });
        std::vector column_wrap_budget(static_cast<std::size_t>(cols), 0.0F);
        for (int c = 0; c < cols; ++c) {
            column_wrap_budget[static_cast<std::size_t>(c)] =
                    std::max(0.0F, resolved_widths[static_cast<std::size_t>(c)] -
                                   (2.0F * nodes::DocraftLoomTable::kCellPaddingX));
        }

        std::vector col_widths(static_cast<std::size_t>(cols), 0.0F);
        std::vector row_heights(static_cast<std::size_t>(rows), 0.0F);
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                auto cell = table->cell(r, c);
                pending_cell_wrap_budget_ = column_wrap_budget[static_cast<std::size_t>(c)];
                cell->accept(*this);
                // Cell's own measured_size already folds in its padding inset (see
                // DocraftLoomTableCell's own Measure visit above) -- no extra term here.
                const auto &sz = cell->layout_box().measured_size;
                col_widths[static_cast<std::size_t>(c)] = std::max(col_widths[static_cast<std::size_t>(c)], sz.width);
                row_heights[static_cast<std::size_t>(r)] =
                        std::max(row_heights[static_cast<std::size_t>(r)], sz.height);
            }
        }

        float total_width = 0.0F;
        for (float w: col_widths)
            total_width += w;
        float total_height = 0.0F;
        for (float h: row_heights)
            total_height += h;

        auto &measured_size = table->edit_layout_box().measured_size;
        // padding() inflates the table's own measured_size around the grid, same as
        // Rectangle's padding_ does around its children -- so containers laying the
        // table out (VStack/Pagination) reserve the extra breathing room too.
        measured_size.width = total_width + (2.0F * table->padding());
        measured_size.height = total_height + (2.0F * table->padding());
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomPageNumber *node) {
        // Measured exactly like ordinary text, using its placeholder string -- the real
        // page number is substituted later, at render time, once pagination is known.
        visit(static_cast<docraft::loom::nodes::DocraftLoomText *>(node));
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomNewPage *node) {
        if (!node) return;
        auto &measured_size = node->edit_layout_box().measured_size;
        measured_size.width = 0.0F;
        measured_size.height = 0.0F;
    }
} // docraft
