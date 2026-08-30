//
// Created by Matteo on 08/08/2026.
//

#pragma once

#include <vector>

#include "docraft/backend/docraft_rendering_backend.h"
#include "docraft/docraft_color.h"
#include "docraft/loom/nodes/docraft_loom_node.h"
#include "docraft/loom/nodes/docraft_loom_shape_style.h"

namespace docraft::loom::pipeline {
    /**
     * @brief Shared shape-painting helpers used by DocraftLoomRenderingProcessor to draw
     * fillable/strokeable shapes (Rectangle, Circle, Triangle, Polygon, CurveLine) --
     * kept as free-standing static methods rather than processor members since none of
     * them touch processor state, only the backends and geometry passed in.
     */
    class DocraftLoomShapeDrawUtils {
    public:
        /**
         * @brief Whether a shape style has a visible fill and/or a visible stroke,
         * pre-resolved once per shape so callers don't repeat the alpha/width checks.
         */
        struct ShapeDrawFlags {
            bool has_fill;
            bool has_stroke;
        };

        /**
         * @brief Backends a shape is painted through -- bundled together since every
         * draw call in this class needs both.
         */
        struct ShapeRenderTarget {
            backend::IDocraftShapeRenderingBackend *shape_backend;
            backend::IDocraftLineRenderingBackend *line_backend;
        };

        /**
         * @brief Inputs for draw_shape_polygon(): a closed, fillable path (Triangle/Polygon).
         */
        struct PolygonDrawRequest {
            ShapeRenderTarget target;
            const nodes::DocraftLoomShapeStyle &style;
            nodes::Position origin;
            const std::vector<nodes::Position> &points;
        };

        /**
         * @brief Inputs for draw_shape_curve(): an open, stroke-only path (CurveLine).
         */
        struct CurveDrawRequest {
            ShapeRenderTarget target;
            const DocraftColor &border_color;
            float border_width;
            nodes::DocraftLineStyle border_style = nodes::DocraftLineStyle::kSolid;
            nodes::Position origin;
            const std::vector<nodes::Position> &points;
        };

        /**
         * @brief On/off segment length (points) of a dashed stroke's repeating unit.
         */
        static constexpr float kDashSegmentLength = 4.0F;

        /**
         * @brief Gap length (points) between dashes of a dashed stroke.
         */
        static constexpr float kDashGapLength = 2.0F;

        /**
         * @brief Resolves a DocraftLineStyle to the on/off pattern IDocraftLineRenderingBackend::
         * set_line_dash_pattern() expects -- empty (solid) for kSolid, {kDashSegmentLength,
         * kDashGapLength} for kDashed.
         */
        static std::vector<float> resolve_dash_pattern(nodes::DocraftLineStyle style);

        static ShapeDrawFlags resolve_shape_draw_flags(const nodes::DocraftLoomShapeStyle &style);

        /**
         * @brief Sets fill/stroke color and alpha on target for the flags style resolved
         * to. Caller is responsible for save_state()/restore_state() around this call.
         */
        static void apply_shape_paint_state(const ShapeRenderTarget &target, const nodes::DocraftLoomShapeStyle &style,
                                            ShapeDrawFlags flags);

        /**
         * @brief Issues the fill/stroke/fill_stroke painting operator matching flags,
         * once the shape's path has already been added to shape_backend.
         */
        static void finish_shape_path(backend::IDocraftShapeRenderingBackend *shape_backend, ShapeDrawFlags flags);

        /**
         * @brief Draws a closed polygon (request.points, at least 3) filled and/or
         * stroked per request.style. No-op if fewer than 3 points, or if the style has
         * neither a visible fill nor a visible stroke.
         */
        static void draw_shape_polygon(const PolygonDrawRequest &request);

        /**
         * @brief Draws an open stroked curve (request.points, at least 2). Always
         * stroke-only -- never filled or closed, unlike draw_shape_polygon(). No-op if
         * fewer than 2 points, or if border_width/border_color make the stroke invisible.
         */
        static void draw_shape_curve(const CurveDrawRequest &request);
    };
} // namespace docraft::loom::pipeline