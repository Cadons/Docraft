#pragma once

#include "backend/docraft_line_rendering_backend.h"

namespace docraft::backend {
    /**
     * @brief Interface for shape rendering backends used by Docraft.
     */
    class IDocraftShapeRenderingBackend : public virtual IDocraftLineRenderingBackend {
    public:
        ~IDocraftShapeRenderingBackend() override = default;
        /**
         * @brief Saves the current graphics state.
         */
        virtual void save_state() const = 0;
        /**
         * @brief Restores the previous graphics state.
         */
        virtual void restore_state() const = 0;
        /**
         * @brief Sets the fill color used for subsequent fills.
         * @param r Red component in [0,1].
         * @param g Green component in [0,1].
         * @param b Blue component in [0,1].
         */
        virtual void set_fill_color(float r, float g, float b) const = 0;
        /**
         * @brief Sets the alpha used for subsequent fills.
         * @param alpha Alpha in [0,1].
         */
        virtual void set_fill_alpha(float alpha) const = 0;
        /**
         * @brief Sets the alpha used for subsequent strokes.
         * @param alpha Alpha in [0,1].
         */
        virtual void set_stroke_alpha(float alpha) const = 0;
        /**
         * @brief Defines a rectangle path with the given dimensions.
         * @param x The x-coordinate of the bottom-left corner.
         * @param y The y-coordinate of the bottom-left corner.
         * @param width The rectangle width.
         * @param height The rectangle height.
         */
        virtual void draw_rectangle(float x, float y, float width, float height) const = 0;
        /**
         * @brief Fills the current path.
         */
        virtual void fill() const = 0;
        /**
         * @brief Strokes the current path.
         */
        virtual void stroke() const = 0;
        /**
         * @brief Fills and strokes the current path.
         */
        virtual void fill_stroke() const = 0;
    };
} // docraft::backend
