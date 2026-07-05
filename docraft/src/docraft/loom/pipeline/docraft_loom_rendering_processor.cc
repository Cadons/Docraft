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
#include "docraft/loom/nodes/docraft_loom_page_number.h"
#include "docraft/loom/nodes/docraft_loom_paragraph.h"
#include "docraft/loom/nodes/docraft_loom_polygon.h"
#include "docraft/loom/nodes/docraft_loom_rectangle.h"
#include "docraft/loom/nodes/docraft_loom_table.h"
#include "docraft/loom/nodes/docraft_loom_text.h"
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
                                const nodes::Position& origin, const std::vector<model::DocraftPoint>& points)
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
            std::vector<model::DocraftPoint> transformed;
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

    void DocraftLoomRenderingProcessor::draw_aligned_line(const std::string& line, float x, float y,
                                                          const TextLineStyle& style)
    {
        const float actual_width = text_backend_->measure_text_width(line, style.font_name, style.font_size);
        if (style.alignment == model::TextAlignment::kJustified)
        {
            const auto spaces = std::count(line.begin(), line.end(), ' ');
            if (spaces == 0 || style.box_width <= actual_width)
            {
                text_backend_->draw_text(line, x, y);
                return;
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
            return;
        }

        float draw_x = x;
        if (style.alignment == model::TextAlignment::kCenter)
        {
            draw_x = x + (style.box_width - actual_width) / 2.0F;
        }
        else if (style.alignment == model::TextAlignment::kRight)
        {
            draw_x = x + (style.box_width - actual_width);
        }
        text_backend_->draw_text(line, draw_x, y);
    }

    void DocraftLoomRenderingProcessor::visit(docraft::loom::nodes::DocraftLoomText* text)
    {
        if (!text || !should_render(*text))
            return;
        text_backend_->set_font(text->resolved_font_name(), text->font_size());
        const auto& lines = text->wrapped_lines();

        // draw_text's y is the baseline, but frame.position.y is the top of the text's
        // own line box (as measured by measure_text_height) -- shift down by the full
        // line height so the glyph sits inside [position.y, position.y + height]
        // instead of poking above it.
        if (lines.empty())
        {
            text_backend_->begin_text();
            text_backend_->draw_text(text->text(), text->layout_box().frame.position.x,
                                     text->layout_box().frame.position.y + text->layout_box().measured_size.height);
            text_backend_->end_text();
            return;
        }

        const float line_height = text->layout_box().measured_size.height / static_cast<float>(lines.size());
        const float box_x = text->layout_box().frame.position.x;
        const float box_top = text->layout_box().frame.position.y;
        TextLineStyle style{
            .box_width = text->wrap_width(),
            .alignment = text->alignment(),
            .font_name = text->resolved_font_name(),
            .font_size = text->font_size()
        };
        for (std::size_t i = 0; i < lines.size(); ++i)
        {
            const bool is_last_line = (i + 1 == lines.size());
            // Justified text stretches every line except the last, matching the
            // conventional look (a fully-justified last line reads as stretched-looking
            // gappy text instead of a natural paragraph end).
            style.alignment = (text->alignment() == model::TextAlignment::kJustified && is_last_line)
                                  ? model::TextAlignment::kLeft
                                  : text->alignment();
            const float y = box_top + line_height * static_cast<float>(i + 1);
            text_backend_->begin_text();
            draw_aligned_line(lines[i], box_x, y, style);
            text_backend_->end_text();
        }
    }

    void DocraftLoomRenderingProcessor::visit(docraft::loom::nodes::DocraftLoomRectangle* node)
    {
        if (!node || !should_render(*node))
            return;
        const auto flags = resolve_shape_draw_flags(node->style());
        if (flags.has_fill || flags.has_stroke)
        {
            shape_backend_->save_state();
            apply_shape_paint_state(shape_backend_, line_backend_, node->style(), flags);
            const auto& pos = node->layout_box().frame.position;
            const auto& size = node->layout_box().frame.size;
            shape_backend_->draw_rectangle(pos.x, pos.y, size.width, size.height);
            finish_shape_path(shape_backend_, flags);
            shape_backend_->restore_state();
        }
        for (int i = 0; i < node->children_count(); ++i)
            if (auto child = node->edit_child(i))
                child->accept(*this);
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
        for (int i = 0; i < node->children_count(); ++i)
            if (auto child = node->edit_child(i))
                child->accept(*this);
    }

    void DocraftLoomRenderingProcessor::visit(docraft::loom::nodes::DocraftLoomHStack* node)
    {
        if (!node || !should_render(*node)) return;
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
        case model::ImageFormat::kPng:
            image_backend_->draw_png_image(image->path(), pos.x, pos.y, size.width, size.height);
            break;
        case model::ImageFormat::kJpeg:
            image_backend_->draw_jpeg_image(image->path(), pos.x, pos.y, size.width, size.height);
            break;
        case model::ImageFormat::kRaw:
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
                // Same baseline adjustment as DocraftLoomText's own visit(): marker.position
                // is the top of the marker's line box, not the baseline.
                text_backend_->draw_text(marker.text, marker.position.x,
                                         marker.position.y + text_child->layout_box().measured_size.height);
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
        const auto& table_pos = table.layout_box().frame.position;
        const auto& table_size = table.layout_box().frame.size;
        shape_backend_->save_state();
        line_backend_->set_stroke_color(0.0F, 0.0F, 0.0F);
        line_backend_->set_line_width(1.0F);
        line_backend_->draw_line(table_pos.x, table_pos.y, table_pos.x + table_size.width, table_pos.y);
        line_backend_->draw_line(table_pos.x, table_pos.y + table_size.height, table_pos.x + table_size.width,
                                 table_pos.y + table_size.height);
        line_backend_->draw_line(table_pos.x, table_pos.y, table_pos.x, table_pos.y + table_size.height);
        line_backend_->draw_line(table_pos.x + table_size.width, table_pos.y, table_pos.x + table_size.width,
                                 table_pos.y + table_size.height);
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
        // Unlike a plain DocraftLoomText, the drawn string is computed fresh from the
        // current page context rather than read from the node's own (placeholder) text.
        const std::string display = std::to_string(current_page_index_ + 1);
        text_backend_->set_font(node->resolved_font_name(), node->font_size());
        text_backend_->begin_text();
        text_backend_->draw_text(display, node->layout_box().frame.position.x,
                                 node->layout_box().frame.position.y + node->layout_box().measured_size.height);
        text_backend_->end_text();
    }
} // docraft
