//
// Created by Matteo on 21/06/2026.
//

#include "docraft/loom/pipeline/docraft_loom_measure_processor.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <sstream>

#include "docraft/exception/docraft_input_exceptions.h"
#include "docraft/loom/nodes/docraft_loom_blank_line.h"
#include "docraft/loom/nodes/docraft_loom_hstack.h"
#include "docraft/loom/nodes/docraft_loom_circle.h"
#include "docraft/loom/nodes/docraft_loom_image.h"
#include "docraft/loom/nodes/docraft_loom_line.h"
#include "docraft/loom/nodes/docraft_loom_list.h"
#include "docraft/loom/nodes/docraft_loom_paragraph.h"
#include "docraft/loom/nodes/docraft_loom_polygon.h"
#include "docraft/loom/nodes/docraft_loom_triangle.h"
#include "docraft/loom/nodes/docraft_loom_page_number.h"
#include "docraft/loom/nodes/docraft_loom_rectangle.h"
#include "docraft/loom/nodes/docraft_loom_table.h"
#include "docraft/loom/nodes/docraft_loom_text.h"
#include "docraft/loom/nodes/docraft_loom_vstack.h"

namespace docraft::loom::pipeline {
    DocraftLoomMeasureProcessor::DocraftLoomMeasureProcessor(
        const std::shared_ptr<backend::IDocraftTextRenderingBackend>& text_backend)
        : text_backend_(text_backend)
    {
    }

    void DocraftLoomMeasureProcessor::set_content_width(float width)
    {
        content_width_ = width;
    }

    std::vector<std::string> DocraftLoomMeasureProcessor::wrap_text(const std::string& text, float max_width,
                                                                    const std::string& font_name,
                                                                    float font_size) const
    {
        std::vector<std::string> lines;

        auto add_char_split_word = [&](const std::string& word)
        {
            if (word.empty())
            {
                return;
            }
            std::size_t start = 0;
            while (start < word.length())
            {
                std::size_t probe_end = start + 1;
                std::size_t last_fit_end = start;
                while (probe_end <= word.length())
                {
                    const std::string candidate = word.substr(start, probe_end - start);
                    if (text_backend_->measure_text_width(candidate, font_name, font_size) > max_width)
                    {
                        break;
                    }
                    last_fit_end = probe_end;
                    ++probe_end;
                }
                if (last_fit_end == start)
                {
                    // Not even one character fits -- take one anyway to guarantee progress.
                    last_fit_end = std::min(start + 1, word.length());
                }
                lines.push_back(word.substr(start, last_fit_end - start));
                start = last_fit_end;
            }
        };

        auto wrap_paragraph = [&](const std::string& paragraph)
        {
            std::istringstream iss(paragraph);
            std::string word;
            std::string current_line;
            while (iss >> word)
            {
                if (current_line.empty())
                {
                    if (text_backend_->measure_text_width(word, font_name, font_size) <= max_width)
                    {
                        current_line = word;
                    }
                    else
                    {
                        add_char_split_word(word);
                    }
                    continue;
                }
                const std::string candidate = current_line + " " + word;
                if (text_backend_->measure_text_width(candidate, font_name, font_size) <= max_width)
                {
                    current_line = candidate;
                }
                else
                {
                    lines.push_back(current_line);
                    current_line.clear();
                    if (text_backend_->measure_text_width(word, font_name, font_size) <= max_width)
                    {
                        current_line = word;
                    }
                    else
                    {
                        add_char_split_word(word);
                    }
                }
            }
            if (!current_line.empty())
            {
                lines.push_back(current_line);
            }
        };

        // Split on explicit newlines first, then word-wrap each paragraph.
        for (std::size_t start = 0; start < text.length();)
        {
            std::size_t end = text.find('\n', start);
            if (end == std::string::npos)
            {
                end = text.length();
            }
            wrap_paragraph(text.substr(start, end - start));
            start = end + 1;
        }
        if (lines.empty())
        {
            lines.emplace_back();
        }
        return lines;
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomText* text)
    {
        if (!text)
        {
            return;
        }
        const std::string reg_font = text->resolved_font_name();
        const float font_size = text->font_size();
        auto& measure_size = text->edit_layout_box().measured_size;

        // An explicit wrap_width() always wins; otherwise fall back to whatever width a
        // weighted ancestor (HStack column, ...) pushed down for this text to
        // auto-wrap into, consumed here so it doesn't leak to whatever gets measured
        // next.
        const float effective_wrap_width = text->wrap_width() > 0.0F ? text->wrap_width() : inherited_wrap_width_;
        inherited_wrap_width_ = 0.0F;

        if (effective_wrap_width > 0.0F)
        {
            auto lines = wrap_text(text->text(), effective_wrap_width, reg_font, font_size);
            const float line_height = text_backend_->measure_text_height(reg_font, font_size);
            measure_size.width = effective_wrap_width;
            measure_size.height = line_height * static_cast<float>(lines.size());
            text->set_wrap_width(effective_wrap_width); // so Rendering knows the box width too
            text->set_wrapped_lines(std::move(lines));
        }
        else
        {
            measure_size.width = text_backend_->measure_text_width(text->text(), reg_font, font_size);
            measure_size.height = text_backend_->measure_text_height(reg_font, font_size);
            text->set_wrapped_lines({});
        }
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomRectangle* node)
    {
        if (!node)
            return;
        float child_width = 0.0F;
        float child_height = 0.0F;
        const int n = node->children_count();
        for (int i = 0; i < n; ++i)
        {
            auto child = node->edit_child(i);
            child->accept(*this);
            const auto& sz = child->layout_box().measured_size;
            child_height += sz.height;
            child_width = std::max(child_width, sz.width);
        }
        const bool has_children = n > 0;
        auto& measured_size = node->edit_layout_box().measured_size;
        measured_size.height = has_children ? (child_height + (2.0F * node->padding())) : node->height();
        measured_size.width = node->width() > 0.0F
                                  ? node->width()
                                  : (has_children ? child_width + (2.0F * node->padding()) : 0.0F);
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomParagraph* paragraph)
    {
        if (!paragraph)
            return;

        float total_height = paragraph->space_before() + paragraph->space_after(); // Start with space before and after
        float max_width = 0.0f;

        // A width pushed down by a weighted ancestor applies to every child of this
        // paragraph in turn -- each DocraftLoomText consumes (clears) it for itself, so
        // it must be re-armed here before every child, not just the first.
        const float incoming_wrap_width = inherited_wrap_width_;

        for (int i = 0; i < paragraph->children_count(); ++i)
        {
            inherited_wrap_width_ = incoming_wrap_width;
            auto child = paragraph->edit_child(i);
            child->accept(*this);
            const auto& child_size = child->layout_box().measured_size;
            total_height += child_size.height * paragraph->line_spacing();
            // Update max_width if the child's width is greater
            if (child_size.width > max_width)
                max_width = child_size.width;
        }
        // Set the measured size of the paragraph node
        auto& measured_size = paragraph->edit_layout_box().measured_size;
        measured_size.width = max_width;
        measured_size.height = total_height;
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomVStack* node)
    {
        if (!node) return;
        float total_height = 0.0F;
        float max_width = 0.0F;
        const int n = node->children_count();
        for (int i = 0; i < n; ++i)
        {
            auto child = node->edit_child(i);
            child->accept(*this);
            const auto& sz = child->layout_box().measured_size;
            total_height += sz.height;
            if (i < n - 1)
            {
                total_height += node->spacing();
            }
            max_width = std::max(max_width, sz.width);
        }
        auto& ms = node->edit_layout_box().measured_size;
        ms.width = max_width;
        ms.height = total_height;
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomHStack* node)
    {
        if (!node) return;
        float total_width = 0.0F;
        float max_height = 0.0F;
        const int n = node->children_count();
        const auto& weights = node->weights();

        // Opt-in, mirrors DocraftLoomLayoutProcessor's weighted branch: resolve each
        // column's width from weights alone (no natural-width floor here, unlike
        // Layout's) and push it down as a wrap constraint before measuring that child,
        // so e.g. a Paragraph's Text auto-wraps to fit its column instead of the column
        // having to grow to fit unwrapped text. Layout's own floor becomes a no-op once
        // that child's natural width already equals its resolved share.
        std::vector<float> resolved_widths;
        if (!weights.empty() && n > 0 && content_width_ > 0.0F)
        {
            float total_weight = 0.0F;
            std::vector<float> effective_weights(static_cast<std::size_t>(n));
            for (int i = 0; i < n; ++i)
            {
                const float w = i < static_cast<int>(weights.size()) && weights[static_cast<std::size_t>(i)] > 0.0F
                                    ? weights[static_cast<std::size_t>(i)]
                                    : 1.0F;
                effective_weights[static_cast<std::size_t>(i)] = w;
                total_weight += w;
            }
            const float spacing_total = n > 1 ? node->spacing() * static_cast<float>(n - 1) : 0.0F;
            const float available_width = content_width_ - spacing_total;
            resolved_widths.resize(static_cast<std::size_t>(n));
            for (int i = 0; i < n; ++i)
            {
                resolved_widths[static_cast<std::size_t>(i)] =
                    available_width * effective_weights[static_cast<std::size_t>(i)] / total_weight;
            }
        }

        for (int i = 0; i < n; ++i)
        {
            if (!resolved_widths.empty())
            {
                inherited_wrap_width_ = resolved_widths[static_cast<std::size_t>(i)];
            }
            auto child = node->edit_child(i);
            child->accept(*this);
            const auto& sz = child->layout_box().measured_size;
            total_width += sz.width;
            if (i < n - 1)
            {
                total_width += node->spacing();
            }
            max_height = std::max(max_height, sz.height);
        }
        auto& ms = node->edit_layout_box().measured_size;
        ms.width = total_width;
        ms.height = max_height;
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomBlankLine* node)
    {
        if (!node) return;
        // Width depends on the container (a blank line stretches to the available width),
        // so it's left to 0 here and resolved by the layout stage, same as legacy's blank line.
        auto& measured_size = node->edit_layout_box().measured_size;
        measured_size.width = 0.0F;
        measured_size.height = node->height();
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomImage* node)
    {
        if (!node)
            return;
        auto& measured_size = node->edit_layout_box().measured_size;
        measured_size.width = node->width();
        measured_size.height = node->height();
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomLine* node)
    {
        if (!node)
            return;
        auto& measured_size = node->edit_layout_box().measured_size;
        measured_size.width = std::abs(node->end().x - node->start().x);
        measured_size.height = std::max(node->border_width(), 4.0F);
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomCircle* node)
    {
        if (!node)
            return;
        auto& measured_size = node->edit_layout_box().measured_size;
        measured_size.width = node->radius() * 2.0F;
        measured_size.height = node->radius() * 2.0F;
    }

    namespace {
        void measure_points_bounding_box(const std::vector<nodes::Position>& points, nodes::Size& measured_size)
        {
            if (points.size() < 2)
            {
                measured_size = {};
                return;
            }
            float min_x = points[0].x, max_x = points[0].x;
            float min_y = points[0].y, max_y = points[0].y;
            for (const auto& pt : points)
            {
                min_x = std::min(min_x, pt.x);
                max_x = std::max(max_x, pt.x);
                min_y = std::min(min_y, pt.y);
                max_y = std::max(max_y, pt.y);
            }
            measured_size.width = max_x - min_x;
            measured_size.height = max_y - min_y;
        }
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomTriangle* node)
    {
        if (!node)
            return;
        measure_points_bounding_box(node->points(), node->edit_layout_box().measured_size);
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomPolygon* node)
    {
        if (!node)
            return;
        measure_points_bounding_box(node->points(), node->edit_layout_box().measured_size);
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomList* node)
    {
        if (!node)
            return;
        const int n = node->children_count();
        auto& markers = node->edit_markers();
        markers.resize(static_cast<std::size_t>(n));

        float max_width = 0.0F;
        float total_height = 0.0F;
        for (int i = 0; i < n; ++i)
        {
            auto text_child = std::dynamic_pointer_cast<nodes::DocraftLoomText>(node->edit_child(i));
            if (!text_child)
            {
                throw exception::InvalidInputException("List items must be Text nodes");
            }
            text_child->accept(*this);

            auto& marker = markers[static_cast<std::size_t>(i)];
            marker.text = node->marker_text_for_index(i);
            if (node->marker_is_box())
            {
                marker.kind = nodes::DocraftLoomList::Marker::Kind::kBox;
                marker.width = text_child->font_size() * 0.6F;
            }
            else
            {
                marker.kind = nodes::DocraftLoomList::Marker::Kind::kText;
                const std::string reg_font = text_child->resolved_font_name();
                marker.width = marker.text.empty()
                                   ? 0.0F
                                   : text_backend_->measure_text_width(marker.text, reg_font, text_child->font_size());
            }

            const float gap = marker.width > 0.0F ? node->marker_gap() : 0.0F;
            const auto& child_size = text_child->layout_box().measured_size;
            max_width = std::max(max_width, marker.width + gap + child_size.width);
            total_height += child_size.height;
        }

        auto& measured_size = node->edit_layout_box().measured_size;
        measured_size.width = max_width;
        measured_size.height = total_height;
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomTableCell* cell)
    {
        if (!cell)
            return;
        auto& measured_size = cell->edit_layout_box().measured_size;
        if (auto content = cell->content())
        {
            content->accept(*this);
            // Cell size includes a small automatic inset around content, so text never
            // sits flush against the cell's own border -- mirrors how Rectangle's own
            // padding_ inflates its measured_size around its children.
            const auto& content_size = content->layout_box().measured_size;
            measured_size.width = content_size.width + (2.0F * nodes::DocraftLoomTable::kCellPaddingX);
            measured_size.height = content_size.height + (2.0F * nodes::DocraftLoomTable::kCellPaddingY);
        }
        else
        {
            measured_size = {};
        }
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomTable* table)
    {
        if (!table)
            return;
        const int rows = table->row_count();
        const int cols = table->column_count();

        std::vector<float> col_widths(static_cast<std::size_t>(cols), 0.0F);
        std::vector<float> row_heights(static_cast<std::size_t>(rows), 0.0F);
        for (int r = 0; r < rows; ++r)
        {
            for (int c = 0; c < cols; ++c)
            {
                auto cell = table->cell(r, c);
                cell->accept(*this);
                // Cell's own measured_size already folds in its padding inset (see
                // DocraftLoomTableCell's own Measure visit above) -- no extra term here.
                const auto& sz = cell->layout_box().measured_size;
                col_widths[static_cast<std::size_t>(c)] = std::max(col_widths[static_cast<std::size_t>(c)], sz.width);
                row_heights[static_cast<std::size_t>(r)] =
                    std::max(row_heights[static_cast<std::size_t>(r)], sz.height);
            }
        }

        float total_width = 0.0F;
        for (float w : col_widths)
            total_width += w;
        float total_height = 0.0F;
        for (float h : row_heights)
            total_height += h;

        auto& measured_size = table->edit_layout_box().measured_size;
        measured_size.width = total_width;
        measured_size.height = total_height;
    }

    void DocraftLoomMeasureProcessor::visit(docraft::loom::nodes::DocraftLoomPageNumber* node)
    {
        // Measured exactly like ordinary text, using its placeholder string -- the real
        // page number is substituted later, at render time, once pagination is known.
        visit(static_cast<docraft::loom::nodes::DocraftLoomText*>(node));
    }
} // docraft
