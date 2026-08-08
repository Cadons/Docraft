//
// Created by Matteo on 08/08/2026.
//

#include "docraft/loom/pipeline/docraft_loom_shape_draw_utils.h"

namespace docraft::loom::pipeline {
    DocraftLoomShapeDrawUtils::ShapeDrawFlags DocraftLoomShapeDrawUtils::resolve_shape_draw_flags(
        const nodes::DocraftLoomShapeStyle &style) {
        return {
            .has_fill = style.background_color.toRGB().a > 0.0F,
            .has_stroke = style.border_width > 0.0F && style.border_color.toRGB().a > 0.0F,
        };
    }

    void DocraftLoomShapeDrawUtils::apply_shape_paint_state(const ShapeRenderTarget &target,
                                                            const nodes::DocraftLoomShapeStyle &style,
                                                            ShapeDrawFlags flags) {
        if (flags.has_fill) {
            const auto bg = style.background_color.toRGB();
            if (bg.a < 1.0F) {
                target.shape_backend->set_fill_alpha(bg.a);
            }
            target.shape_backend->set_fill_color(bg.r, bg.g, bg.b);
        }
        if (flags.has_stroke) {
            const auto border = style.border_color.toRGB();
            if (border.a < 1.0F) {
                target.shape_backend->set_stroke_alpha(border.a);
            }
            target.line_backend->set_line_width(style.border_width);
            target.line_backend->set_stroke_color(border.r, border.g, border.b);
        }
    }

    void DocraftLoomShapeDrawUtils::finish_shape_path(backend::IDocraftShapeRenderingBackend *shape_backend,
                                                      ShapeDrawFlags flags) {
        if (flags.has_fill && flags.has_stroke) {
            shape_backend->fill_stroke();
        } else if (flags.has_fill) {
            shape_backend->fill();
        } else if (flags.has_stroke) {
            shape_backend->stroke();
        }
    }

    void DocraftLoomShapeDrawUtils::draw_shape_polygon(const PolygonDrawRequest &request) {
        if (request.points.size() < 3) {
            return;
        }
        const auto flags = resolve_shape_draw_flags(request.style);
        if (!(flags.has_fill || flags.has_stroke)) {
            return;
        }
        // Points are stored Y-down relative to origin, matching loom's coordinate
        // convention throughout -- no sign flip here (see coordinate note).
        std::vector<nodes::Position> transformed;
        transformed.reserve(request.points.size());
        for (const auto &pt: request.points) {
            transformed.push_back({.x = request.origin.x + pt.x, .y = request.origin.y + pt.y});
        }

        request.target.shape_backend->save_state();
        apply_shape_paint_state(request.target, request.style, flags);
        request.target.shape_backend->draw_polygon(transformed);
        finish_shape_path(request.target.shape_backend, flags);
        request.target.shape_backend->restore_state();
    }

    // Mirrors draw_shape_polygon() above, but for an open stroked curve (Spline)
    // rather than a closed fillable path -- draw_curve() only ever strokes, never
    // fills, so there is no ShapeDrawFlags/fill branch to consider here.
    void DocraftLoomShapeDrawUtils::draw_shape_curve(const CurveDrawRequest &request) {
        if (request.points.size() < 2) {
            return;
        }
        const auto rgba = request.border_color.toRGB();
        if (request.border_width <= 0.0F || rgba.a <= 0.0F) {
            return;
        }
        std::vector<nodes::Position> transformed;
        transformed.reserve(request.points.size());
        for (const auto &pt: request.points) {
            transformed.push_back({.x = request.origin.x + pt.x, .y = request.origin.y + pt.y});
        }

        request.target.shape_backend->save_state();
        if (rgba.a < 1.0F) {
            request.target.shape_backend->set_stroke_alpha(rgba.a);
        }
        request.target.line_backend->set_line_width(request.border_width);
        request.target.line_backend->set_stroke_color(rgba.r, rgba.g, rgba.b);
        request.target.line_backend->draw_curve(transformed);
        request.target.shape_backend->restore_state();
    }
} // namespace docraft::loom::pipeline