#pragma once

namespace docraft::backend {
    /**
     * @brief Interface for line rendering backends used by Docraft.
     */
    class IDocraftLineRenderingBackend {
    public:
        virtual ~IDocraftLineRenderingBackend() = default;
        /**
         * @brief Sets the stroke color used for subsequent line drawing.
         * @param r Red component in [0,1].
         * @param g Green component in [0,1].
         * @param b Blue component in [0,1].
         */
        virtual void set_stroke_color(float r, float g, float b) const = 0;
        /**
         * @brief Sets the line width used for subsequent line drawing.
         * @param thickness Line width in points.
         */
        virtual void set_line_width(float thickness) const = 0;
        /**
         * @brief Draws a line between two points using the current stroke settings.
         * @param x1 The x-coordinate of the line start.
         * @param y1 The y-coordinate of the line start.
         * @param x2 The x-coordinate of the line end.
         * @param y2 The y-coordinate of the line end.
         */
        virtual void draw_line(float x1, float y1, float x2, float y2) const = 0;
    };
} // docraft::backend
