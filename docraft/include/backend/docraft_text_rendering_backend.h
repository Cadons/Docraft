#pragma once
#include <string>

#include "backend/docraft_line_rendering_backend.h"

namespace docraft::backend {
    /**
     * @brief Interface for text rendering backends used by Docraft.
     */
    class IDocraftTextRenderingBackend : public virtual IDocraftLineRenderingBackend {
    public:
        ~IDocraftTextRenderingBackend() override = default;
        /**
         * @brief Initializes the text rendering context. Must be called before any text drawing operations.
         */
        virtual void begin_text() const = 0;
        /**
         * @brief Finalizes the text rendering context. Must be called after all text drawing operations are completed.
         */
        virtual void end_text() const = 0;
        /**
         * @brief Draws the specified text at the given coordinates on the PDF page.
         * @param text The text to be drawn.
         * @param x The x-coordinate where the text should start.
         * @param y The y-coordinate where the text should start.
         */
        virtual void draw_text(const std::string& text, float x, float y) const = 0;
        /**
         * @brief Sets the fill color used for subsequent text drawing.
         * @param r Red component in [0,1].
         * @param g Green component in [0,1].
         * @param b Blue component in [0,1].
         */
        virtual void set_text_color(float r, float g, float b) const = 0;
        /**
         * @brief Draws the specified text using a custom transformation matrix.
         * @param text The text to be drawn.
         * @param scale_x The horizontal scaling factor.
         * @param skew_x The horizontal skewing factor.(skew=tan(angle),represents the tangent of the skew angle)
         * @param skew_y The vertical skewing factor.(skew=tan(angle),represents the tangent of the skew angle)
         * @param scale_y The vertical scaling factor.
         * @param translate_x The horizontal translation (movement) in points.
         * @param translate_y The vertical translation (movement) in points.
         */
        virtual void draw_text_matrix(
            const std::string& text,
            float scale_x,
            float skew_x,
            float skew_y,
            float scale_y,
            float translate_x,
            float translate_y) const = 0;
        /**
         * @brief Measures the width of the specified text using the current font settings.
         * @param text The text to be measured.
         * @return The width of the text in points.
         */
        virtual float measure_text_width(const std::string& text) const = 0;
    };
} // docraft::backend
