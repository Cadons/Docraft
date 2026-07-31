//
// Created by Matteo on 29/06/2026.
//

#include "docraft/loom/pipeline/docraft_loom_rendering_processor.h"

#include <algorithm>
#include <memory>
#include <sstream>

#include "docraft/backend/pdf/docraft_haru_text_backend.h"
#include "docraft/exception/docraft_input_exceptions.h"
#include "docraft/loom/nodes/docraft_loom_circle.h"
#include "docraft/loom/nodes/docraft_loom_hstack.h"
#include "docraft/loom/nodes/docraft_loom_image.h"
#include "docraft/loom/nodes/docraft_loom_line.h"
#include "docraft/loom/nodes/docraft_loom_list.h"
#include "docraft/loom/nodes/docraft_loom_new_page.h"
#include "docraft/loom/nodes/docraft_loom_page_number.h"
#include "docraft/loom/nodes/docraft_loom_paragraph.h"
#include "docraft/loom/nodes/docraft_loom_polygon.h"
#include "docraft/loom/nodes/docraft_loom_canvas.h"
#include "docraft/loom/nodes/docraft_loom_rectangle.h"
#include "docraft/loom/nodes/docraft_loom_subtitle.h"
#include "docraft/loom/nodes/docraft_loom_table.h"
#include "docraft/loom/nodes/docraft_loom_text.h"
#include "docraft/loom/nodes/docraft_loom_title.h"
#include "docraft/loom/nodes/docraft_loom_triangle.h"
#include "docraft/loom/nodes/docraft_loom_vstack.h"

namespace docraft::loom::pipeline {
    namespace {
        struct ShapeDrawFlags
        {
            bool has_fill;
            bool has_stroke;
        };

        ShapeDrawFlags resolve_shape_draw_flags(const nodes::DocraftLoomShapeStyle& style)
        {
            return {
                .has_fill = style.background_color.toRGB().a > 0.0F,
                .has_stroke = style.border_width > 0.0F && style.border_color.toRGB().a > 0.0F,
            };
        }

        void apply_shape_paint_state(backend::IDocraftShapeRenderingBackend* shape_backend,
                                     backend::IDocraftLineRenderingBackend* line_backend,
                                     const nodes::DocraftLoomShapeStyle& style,
                                     ShapeDrawFlags flags)
        {
            if (flags.has_fill)
            {
                const auto bg = style.background_color.toRGB();
                if (bg.a < 1.0F)
                {
                    shape_backend->set_fill_alpha(bg.a);
                }
                shape_backend->set_fill_color(bg.r, bg.g, bg.b);
            }
            if (flags.has_stroke)
            {
                const auto border = style.border_color.toRGB();
                if (border.a < 1.0F)
                {
                    shape_backend->set_stroke_alpha(border.a);
                }
                line_backend->set_line_width(style.border_width);
                line_backend->set_stroke_color(border.r, border.g, border.b);
            }
        }

        void finish_shape_path(backend::IDocraftShapeRenderingBackend* shape_backend, ShapeDrawFlags flags)
        {
            if (flags.has_fill && flags.has_stroke)
            {
                shape_backend->fill_stroke();
            }
            else if (flags.has_fill)
            {
                shape_backend->fill();
            }
            else if (flags.has_stroke)
            {
                shape_backend->stroke();
            }
        }

        void draw_shape_polygon(backend::IDocraftShapeRenderingBackend* shape_backend,
                                backend::IDocraftLineRenderingBackend* line_backend,
                                const nodes::DocraftLoomShapeStyle& style,
                                const nodes::Position& origin, const std::vector<nodes::Position>& points)
        {
            if (points.size() < 3)
            {
                return;
            }
            const auto flags = resolve_shape_draw_flags(style);
            if (!(flags.has_fill || flags.has_stroke))
            {
                return;
            }
            // Points are stored Y-down relative to origin, matching loom's coordinate
            // convention throughout -- no sign flip here (see coordinate note).
            std::vector<nodes::Position> transformed;
            transformed.reserve(points.size());
            for (const auto& pt : points)
            {
                transformed.push_back({.x = origin.x + pt.x, .y = origin.y + pt.y});
            }

            shape_backend->save_state();
            apply_shape_paint_state(shape_backend, line_backend, style, flags);
            shape_backend->draw_polygon(transformed);
            finish_shape_path(shape_backend, flags);
            shape_backend->restore_state();
        }
    }

    DocraftLoomRenderingProcessor::DocraftLoomRenderingProcessor(
        docraft::backend::IDocraftRenderingCapabilityProvider* backend)
        : text_backend_(backend ? backend->edit_text_rendering() : nullptr),
          shape_backend_(backend ? backend->edit_shape_rendering() : nullptr),
          line_backend_(backend ? backend->edit_line_rendering() : nullptr),
          image_backend_(backend ? backend->edit_image_rendering() : nullptr)
    {
    }

    void DocraftLoomRenderingProcessor::set_current_page(int page_index, int total_page_count)
    {
        current_page_index_ = page_index;
        total_page_count_ = total_page_count;
    }

    bool DocraftLoomRenderingProcessor::should_render(const nodes::DocraftLoomNode& node) const
    {
        const int page_index = node.layout_box().page_index;
        return page_index < 0 || page_index == current_page_index_;
    }

    DocraftLoomRenderingProcessor::LineExtent DocraftLoomRenderingProcessor::draw_aligned_line(
        const std::string& line, float x, float y, const TextLineStyle& style)
    {
        const float actual_width = text_backend_->measure_text_width(line, style.font_name, style.font_size);
        if (style.alignment == nodes::TextAlignment::kJustified)
        {
            const auto spaces = std::count(line.begin(), line.end(), ' ');
            if (spaces == 0 || style.box_width <= actual_width)
            {
                text_backend_->draw_text(line, x, y);
                return {x, x + actual_width};
            }
            const float extra_space = (style.box_width - actual_width) / static_cast<float>(spaces);
            const float space_width = text_backend_->measure_text_width(" ", style.font_name, style.font_size);
            float word_x = x;
            std::istringstream iss(line);
            std::string word;
            bool first = true;
            while (iss >> word)
            {
                if (!first)
                {
                    word_x += space_width + extra_space;
                }
                text_backend_->draw_text(word, word_x, y);
                word_x += text_backend_->measure_text_width(word, style.font_name, style.font_size);
                first = false;
            }
            return {x, word_x};
        }

        float draw_x = x;
        if (style.alignment == nodes::TextAlignment::kCenter)
        {
            draw_x = x + (style.box_width - actual_width) / 2.0F;
        }
        else if (style.alignment == nodes::TextAlignment::kRight)
        {
            draw_x = x + (style.box_width - actual_width);
        }
        text_backend_->draw_text(line, draw_x, y);
        return {draw_x, draw_x + actual_width};
    }

    void DocraftLoomRenderingProcessor::draw_text_underline(float x_start, float x_end, float y_baseline,
                                                             float font_size, float descent, const RGB& color)
    {
        // No dedicated underline-position metric is exposed by the text backend, so the
        // offset below the baseline is derived from the font's own descent()
        // (populated by DocraftLoomMeasureProcessor) rather than a font_size guess --
        // descent is negative, so -descent is the magnitude of space below the
        // baseline, and the underline sits a third of the way into it.
        const float offset = -descent * kUnderlineDescentFraction;
        const float thickness = std::max(kTextRuleMinThickness, font_size * kTextRuleThicknessFontSizeFraction);
        draw_horizontal_text_rule(x_start, x_end, y_baseline + offset, thickness, color);
    }

    void DocraftLoomRenderingProcessor::draw_text_strikeout(float x_start, float x_end, float y_baseline,
                                                             float font_size, float ascent, const RGB& color)
    {
        // Mirrors draw_text_underline's reasoning: no x-height/strikeout-position
        // metric is exposed, so the offset above the baseline is a third of ascent()
        // (populated by DocraftLoomMeasureProcessor), which lands roughly through the
        // middle of lowercase glyphs for typical fonts.
        const float offset = ascent * kStrikeoutAscentFraction;
        const float thickness = std::max(kTextRuleMinThickness, font_size * kTextRuleThicknessFontSizeFraction);
        draw_horizontal_text_rule(x_start, x_end, y_baseline - offset, thickness, color);
    }

    void DocraftLoomRenderingProcessor::draw_horizontal_text_rule(float x_start, float x_end, float y,
                                                                   float thickness, const RGB& color)
    {
        if (x_end <= x_start)
        {
            return;
        }
        shape_backend_->save_state();
        if (color.a < 1.0F)
        {
            shape_backend_->set_stroke_alpha(color.a);
        }
        line_backend_->set_line_width(thickness);
        line_backend_->set_stroke_color(color.r, color.g, color.b);
        line_backend_->draw_line(x_start, y, x_end, y);
        shape_backend_->restore_state();
    }

    void DocraftLoomRenderingProcessor::visit(docraft::loom::nodes::DocraftLoomText* text)
    {
        if (!text || !should_render(*text))
            return;
        text_backend_->set_font(text->resolved_font_name(), text->font_size());
        const auto& lines = text->wrapped_lines();
        const float ascent = text->ascent();
        const auto rgba = text->color().toRGB();
        if (lines.empty())
        {
            const float y = text->layout_box().frame.position.y + ascent;
            text_backend_->begin_text();
            text_backend_->set_text_color(rgba.r, rgba.g, rgba.b);
            text_backend_->draw_text(text->text(), text->layout_box().frame.position.x, y);
            text_backend_->end_text();
            // Path painting is not valid inside a BT/ET text object, so the
            // underline/strikeout strokes are issued after end_text().
            if (text->underline() || text->strikeout())
            {
                const float width = text_backend_->measure_text_width(text->text(), text->resolved_font_name(),
                                                                       text->font_size());
                const float x_start = text->layout_box().frame.position.x;
                const float x_end = x_start + width;
                if (text->underline())
                {
                    draw_text_underline(x_start, x_end, y, text->font_size(), text->descent(), rgba);
                }
                if (text->strikeout())
                {
                    draw_text_strikeout(x_start, x_end, y, text->font_size(), ascent, rgba);
                }
            }
            return;
        }

        const float line_height = text->layout_box().measured_size.height / static_cast<float>(lines.size());
        const float box_x = text->layout_box().frame.position.x;
        const float box_top = text->layout_box().frame.position.y;
        TextLineStyle style{
            .box_width = text->wrap_width(),
            .alignment = text->alignment(),
            .font_name = text->resolved_font_name(),
            .font_size = text->font_size(),
            .underline = text->underline(),
            .strikeout = text->strikeout(),
            .color = rgba
        };
        for (std::size_t i = 0; i < lines.size(); ++i)
        {
            const bool is_last_line = (i + 1 == lines.size());
            // Justified text stretches every line except the last, matching the
            // conventional look (a fully-justified last line reads as stretched-looking
            // gappy text instead of a natural paragraph end).
            style.alignment = (text->alignment() == nodes::TextAlignment::kJustified && is_last_line)
                                  ? nodes::TextAlignment::kLeft
                                  : text->alignment();
            const float y = box_top + ascent + line_height * static_cast<float>(i);
            text_backend_->begin_text();
            // Unlike the single-line branch above, this loop never went through
            // set_text_color() -- wrapped text silently kept whatever color the
            // backend's graphics state last had (e.g. white from a preceding table
            // header cell), regardless of this node's own color().
            text_backend_->set_text_color(rgba.r, rgba.g, rgba.b);
            const auto extent = draw_aligned_line(lines[i], box_x, y, style);
            text_backend_->end_text();
            if (style.underline)
            {
                draw_text_underline(extent.x_start, extent.x_end, y, style.font_size, text->descent(), style.color);
            }
            if (style.strikeout)
            {
                draw_text_strikeout(extent.x_start, extent.x_end, y, style.font_size, ascent, style.color);
            }
        }
    }

    void DocraftLoomRenderingProcessor::visit(docraft::loom::nodes::DocraftLoomTitle* node)
    {
        visit(static_cast<docraft::loom::nodes::DocraftLoomText*>(node));
    }

    void DocraftLoomRenderingProcessor::visit(docraft::loom::nodes::DocraftLoomSubtitle* node)
    {
        visit(static_cast<docraft::loom::nodes::DocraftLoomText*>(node));
    }

    void DocraftLoomRenderingProcessor::draw_container_background(const nodes::DocraftLoomShapeStyle& style,
                                                                   const nodes::Position& position,
                                                                   const nodes::Size& size)
    {
        const auto flags = resolve_shape_draw_flags(style);
        if (!(flags.has_fill || flags.has_stroke))
        {
            return;
        }
        shape_backend_->save_state();
        apply_shape_paint_state(shape_backend_, line_backend_, style, flags);
        shape_backend_->draw_rectangle(position.x, position.y, size.width, size.height);
        finish_shape_path(shape_backend_, flags);
        shape_backend_->restore_state();
    }

    void DocraftLoomRenderingProcessor::visit(docraft::loom::nodes::DocraftLoomRectangle* node)
    {
        if (!node || !should_render(*node))
            return;
        draw_container_background(node->style(), node->layout_box().frame.position, node->layout_box().frame.size);
        for (int i = 0; i < node->children_count(); ++i)
            if (auto child = node->edit_child(i))
                child->accept(*this);
    }

    void DocraftLoomRenderingProcessor::visit(docraft::loom::nodes::DocraftLoomCanvas* node)
    {
        if (!node || !should_render(*node))
            return;
        const auto& frame = node->layout_box().frame;
        draw_container_background(node->style(), frame.position, frame.size);
        // Clips children to the canvas bounds, trimming anything that overflows -- see
        // visit(DocraftLoomCanvas*) in the layout processor for how children are
        // positioned relative to this origin in the first place.
        shape_backend_->save_state();
        shape_backend_->clip_rectangle(frame.position.x, frame.position.y, frame.size.width, frame.size.height);
        for (int i = 0; i < node->children_count(); ++i)
            if (auto child = node->edit_child(i))
                child->accept(*this);
        shape_backend_->restore_state();
    }

    void DocraftLoomRenderingProcessor::visit(docraft::loom::nodes::DocraftLoomParagraph* paragraph)
    {
        if (!paragraph || !should_render(*paragraph))
            return;
        for (int i = 0; i < paragraph->children_count(); ++i)
        {
            if (paragraph->edit_child(i))
                paragraph->edit_child(i)->accept(*this);
        }
    }

    void DocraftLoomRenderingProcessor::visit(docraft::loom::nodes::DocraftLoomVStack* node)
    {
        if (!node || !should_render(*node)) return;
        draw_container_background(node->style(), node->layout_box().frame.position, node->layout_box().frame.size);
        for (int i = 0; i < node->children_count(); ++i)
            if (auto child = node->edit_child(i))
                child->accept(*this);
    }

    void DocraftLoomRenderingProcessor::visit(docraft::loom::nodes::DocraftLoomHStack* node)
    {
        if (!node || !should_render(*node)) return;
        draw_container_background(node->style(), node->layout_box().frame.position, node->layout_box().frame.size);
        for (int i = 0; i < node->children_count(); ++i)
            if (auto child = node->edit_child(i))
                child->accept(*this);
    }

    void DocraftLoomRenderingProcessor::visit(docraft::loom::nodes::DocraftLoomBlankLine*)
    {
        // A blank line is a pure spacer -- it advances layout but draws nothing.
    }

    void DocraftLoomRenderingProcessor::visit(docraft::loom::nodes::DocraftLoomImage* image)
    {
        if (!image || !should_render(*image))
            return;
        // frame.position is already engine-space top-left, matching what these backend
        // calls expect internally -- do not subtract height() here (see coordinate note).
        const auto& pos = image->layout_box().frame.position;
        const auto& size = image->layout_box().frame.size;
        switch (image->format())
        {
        case nodes::ImageFormat::kPng:
            image_backend_->draw_png_image(image->path(), pos.x, pos.y, size.width, size.height);
            break;
        case nodes::ImageFormat::kJpeg:
            image_backend_->draw_jpeg_image(image->path(), pos.x, pos.y, size.width, size.height);
            break;
        case nodes::ImageFormat::kRaw:
            if (image->raw_data().empty())
            {
                throw exception::InvalidInputException("Raw image data is empty");
            }
            image_backend_->draw_raw_rgb_image_from_memory(image->raw_data().data(), image->raw_pixel_width(),
                                                           image->raw_pixel_height(), pos.x, pos.y, size.width,
                                                           size.height);
            break;
        }
    }

    void DocraftLoomRenderingProcessor::visit(docraft::loom::nodes::DocraftLoomLine* line)
    {
        if (!line || !should_render(*line))
            return;
        const auto rgba = line->border_color().toRGB();
        if (line->border_width() <= 0.0F || rgba.a <= 0.0F)
        {
            return;
        }

        const auto& pos = line->layout_box().frame.position;
        const float mid_y = line->layout_box().measured_size.height / 2.0F;

        shape_backend_->save_state();
        if (rgba.a < 1.0F)
        {
            shape_backend_->set_stroke_alpha(rgba.a);
        }
        line_backend_->set_line_width(line->border_width());
        line_backend_->set_stroke_color(rgba.r, rgba.g, rgba.b);
        line_backend_->draw_line(pos.x, pos.y + mid_y, pos.x + line->layout_box().measured_size.width, pos.y + mid_y);
        shape_backend_->restore_state();
    }

    void DocraftLoomRenderingProcessor::visit(docraft::loom::nodes::DocraftLoomCircle* node)
    {
        if (!node || !should_render(*node))
            return;
        const float radius = node->radius();
        const auto flags = resolve_shape_draw_flags(node->style());
        if (radius <= 0.0F || !(flags.has_fill || flags.has_stroke))
        {
            return;
        }
        const auto& pos = node->layout_box().frame.position;
        const nodes::Position center = {.x = pos.x + radius, .y = pos.y + radius};

        shape_backend_->save_state();
        apply_shape_paint_state(shape_backend_, line_backend_, node->style(), flags);
        shape_backend_->draw_circle(center.x, center.y, radius);
        finish_shape_path(shape_backend_, flags);
        shape_backend_->restore_state();
    }

    void DocraftLoomRenderingProcessor::visit(docraft::loom::nodes::DocraftLoomTriangle* node)
    {
        if (!node || !should_render(*node))
            return;
        draw_shape_polygon(shape_backend_, line_backend_, node->style(), node->layout_box().frame.position,
                           node->points());
    }

    void DocraftLoomRenderingProcessor::visit(docraft::loom::nodes::DocraftLoomPolygon* node)
    {
        if (!node || !should_render(*node))
            return;
        draw_shape_polygon(shape_backend_, line_backend_, node->style(), node->layout_box().frame.position,
                           node->points());
    }

    void DocraftLoomRenderingProcessor::visit(docraft::loom::nodes::DocraftLoomList* node)
    {
        if (!node || !should_render(*node))
            return;
        const auto& markers = node->markers();
        for (int i = 0; i < node->children_count(); ++i)
        {
            auto text_child = std::dynamic_pointer_cast<nodes::DocraftLoomText>(node->edit_child(i));
            if (!text_child)
            {
                continue;
            }
            const auto& marker = markers[static_cast<std::size_t>(i)];
            if (marker.kind == nodes::DocraftLoomList::Marker::Kind::kBox)
            {
                const auto rgba = text_child->color().toRGB();
                shape_backend_->save_state();
                line_backend_->set_stroke_color(rgba.r, rgba.g, rgba.b);
                line_backend_->set_line_width(1.0F);
                shape_backend_->draw_rectangle(marker.position.x, marker.position.y, marker.width, marker.width);
                shape_backend_->stroke();
                shape_backend_->restore_state();
            }
            else if (!marker.text.empty())
            {
                text_backend_->set_font(text_child->resolved_font_name(), text_child->font_size());
                text_backend_->begin_text();
                const auto marker_rgba = text_child->color().toRGB();
                text_backend_->set_text_color(marker_rgba.r, marker_rgba.g, marker_rgba.b);
                // Same baseline adjustment as DocraftLoomText's own visit(): marker.position
                // is the top of the marker's line box, not the baseline -- the baseline
                // sits `ascent` below that top, not a full line height below it.
                // ascent() was populated for text_child by the measure step.
                text_backend_->draw_text(marker.text, marker.position.x,
                                         marker.position.y + text_child->ascent());
                text_backend_->end_text();
            }
            text_child->accept(*this);
        }
    }

    void DocraftLoomRenderingProcessor::visit(docraft::loom::nodes::DocraftLoomTableCell* cell)
    {
        if (!cell)
            return;
        // Own explicit background, if set -- the table draws the default fallback for
        // cells that don't set one (see Table's visit()), so there's no precedence logic
        // to duplicate here.
        if (cell->background())
        {
            const auto rgba = cell->background()->toRGB();
            if (rgba.a > 0.0F)
            {
                const auto& pos = cell->layout_box().frame.position;
                const auto& size = cell->layout_box().frame.size;
                shape_backend_->save_state();
                if (rgba.a < 1.0F)
                {
                    shape_backend_->set_fill_alpha(rgba.a);
                }
                shape_backend_->set_fill_color(rgba.r, rgba.g, rgba.b);
                shape_backend_->draw_rectangle(pos.x, pos.y, size.width, size.height);
                shape_backend_->fill();
                shape_backend_->restore_state();
            }
        }
        if (auto content = cell->content())
        {
            content->accept(*this);
        }
    }

    // Default background: only for cells that don't already have their own explicit
    // background (which each cell draws itself in its own visit()).
    void DocraftLoomRenderingProcessor::draw_table_default_backgrounds(nodes::DocraftLoomTable& table)
    {
        if (!table.default_cell_background())
        {
            return;
        }
        const auto rgba = table.default_cell_background()->toRGB();
        if (rgba.a <= 0.0F)
        {
            return;
        }
        shape_backend_->save_state();
        if (rgba.a < 1.0F)
        {
            shape_backend_->set_fill_alpha(rgba.a);
        }
        shape_backend_->set_fill_color(rgba.r, rgba.g, rgba.b);
        for (int r = 0; r < table.row_count(); ++r)
        {
            for (int c = 0; c < table.column_count(); ++c)
            {
                auto cell = table.cell(r, c);
                if (!cell->background())
                {
                    const auto& pos = cell->layout_box().frame.position;
                    const auto& size = cell->layout_box().frame.size;
                    shape_backend_->draw_rectangle(pos.x, pos.y, size.width, size.height);
                    shape_backend_->fill();
                }
            }
        }
        shape_backend_->restore_state();
    }

    // Outer border + row/column dividers, derived directly from each cell's own resolved
    // frame (set by LayoutProcessor) -- the table itself caches no width/left geometry,
    // see the class docs on DocraftLoomTable for why.
    void DocraftLoomRenderingProcessor::draw_table_borders_and_dividers(nodes::DocraftLoomTable& table)
    {
        // Derived from the cells' own resolved frames -- not table.layout_box() directly
        // -- because that frame's position/size describe the outer padding() footprint
        // (see LayoutProcessor::visit(DocraftLoomTable*)), while the border must wrap
        // only the actual grid, inset by that padding from the footprint's edges.
        const auto table_pos = table.cell(0, 0)->layout_box().frame.position;
        nodes::Size table_size{};
        for (int c = 0; c < table.column_count(); ++c)
        {
            table_size.width += table.cell(0, c)->layout_box().frame.size.width;
        }
        for (int r = 0; r < table.row_count(); ++r)
        {
            table_size.height += table.cell(r, 0)->layout_box().frame.size.height;
        }
        shape_backend_->save_state();
        line_backend_->set_stroke_color(0.0F, 0.0F, 0.0F);
        line_backend_->set_line_width(1.0F);
        // Outer border is a single closed rectangle path, not four independent line
        // segments -- with butt-cap line ends (libharu's default), four disjoint strokes
        // leave a gap at each corner where the segments only meet at a point rather than
        // overlapping in the corner square.
        shape_backend_->draw_rectangle(table_pos.x, table_pos.y, table_size.width, table_size.height);
        shape_backend_->stroke();
        for (int c = 1; c < table.column_count(); ++c)
        {
            const float x = table.cell(0, c)->layout_box().frame.position.x;
            line_backend_->draw_line(x, table_pos.y, x, table_pos.y + table_size.height);
        }
        for (int r = 1; r < table.row_count(); ++r)
        {
            const float y = table.cell(r, 0)->layout_box().frame.position.y;
            line_backend_->draw_line(table_pos.x, y, table_pos.x + table_size.width, y);
        }
        shape_backend_->restore_state();
    }

    // Non-title cells first, title cells last, so title text paints over any divider
    // beneath it -- generalizes legacy's "titles drawn last" ordering to a grid where
    // any cell may be marked as title, not just row/column 0.
    void DocraftLoomRenderingProcessor::draw_table_content(nodes::DocraftLoomTable& table)
    {
        for (int pass = 0; pass < 2; ++pass)
        {
            const bool draw_titles = pass == 1;
            for (int r = 0; r < table.row_count(); ++r)
            {
                for (int c = 0; c < table.column_count(); ++c)
                {
                    auto cell = table.cell(r, c);
                    if (cell->is_title() == draw_titles)
                    {
                        cell->accept(*this);
                    }
                }
            }
        }
    }

    void DocraftLoomRenderingProcessor::visit(docraft::loom::nodes::DocraftLoomTable* table)
    {
        if (!table || !should_render(*table) || table->row_count() == 0 || table->column_count() == 0)
        {
            return;
        }
        draw_table_default_backgrounds(*table);
        draw_table_borders_and_dividers(*table);
        draw_table_content(*table);
    }

    void DocraftLoomRenderingProcessor::visit(docraft::loom::nodes::DocraftLoomPageNumber* node)
    {
        if (!node || !should_render(*node))
            return;
        // The only aspect DocraftLoomText's own visit() can't already handle: Measure
        // reserved space/wrap_width using the placeholder text ("99999", see the class
        // doc), so the real string is substituted here before delegating -- alignment,
        // underline/strikeout and the ascent-based baseline all then come from the
        // shared Text paint path for free.
        const std::string display = std::to_string(current_page_index_ + 1);
        node->set_text(display);
        if (node->wrap_width() > 0.0F)
        {
            node->set_wrapped_lines({display});
        }
        visit(static_cast<docraft::loom::nodes::DocraftLoomText*>(node));
    }

    void DocraftLoomRenderingProcessor::visit(docraft::loom::nodes::DocraftLoomNewPage*)
    {
        // A forced page break carries no visible content -- see
        // DocraftLoomPaginationProcessor::paginate_body() for the actual break logic.
    }
} // docraft
