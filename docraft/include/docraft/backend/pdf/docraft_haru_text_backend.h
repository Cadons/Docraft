/*
 * Copyright 2026 Matteo Cadoni (https://github.com/cadons)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "docraft/backend/docraft_text_rendering_backend.h"
#include "docraft/backend/pdf/docraft_haru_shared_state.h"

#include <memory>
#include <unordered_set>

namespace docraft::backend::pdf {
    /**
     * @brief Haru implementation of text rendering operations.
     */
    class DocraftHaruTextBackend : public docraft::backend::IDocraftTextRenderingBackend
    {
    public:
        /**
         * @brief Creates a text backend bound to a Haru document owner.
         *
         * LIFETIME: The shared state must have a registered page operations provider.
         * This is guaranteed if the device is used through DocraftHaruBackend.
         */
        explicit DocraftHaruTextBackend(const std::shared_ptr<DocraftHaruSharedState>& state);

        /**
         * @brief Begins a text object on the current page.
         */
        void begin_text() const override;

        /**
         * @brief Ends the current text object on the current page.
         */
        void end_text() const override;

        /**
         * @brief Draws plain text at the given page coordinates.
         */
        void draw_text(const std::string& text, float x, float y) const override;

        /**
         * @brief Sets the text fill color for subsequent text drawing.
         */
        void set_text_color(float r, float g, float b) const override;

        /**
         * @brief Draws text using an explicit text transformation matrix.
         */
        void draw_text_matrix(const std::string& text,
                              float scale_x,
                              float skew_x,
                              float skew_y,
                              float scale_y,
                              float translate_x,
                              float translate_y) const override;

        /**
         * @brief Sets the active font and size for subsequent text drawing on the current page.
         */
        void set_font(const std::string& font_name, float font_size) const override;

        /**
         * @brief Measures the width of a text string using the specified font.
         * Stateless: does not modify page state.
         */
        float measure_text_width(const std::string& text, const std::string& font_name, float font_size) const override;

        /**
         * @brief Measures the line height (ascent − descent) for the specified font.
         * Stateless: does not modify page state.
         */
        float measure_text_height(const std::string& font_name, float font_size) const override;
        /**
         * @brief Measures the descent (baseline to bottom of line box) for the specified font.
         * Stateless: does not modify page state.
         * @param font_name
         * @param font_size
         * @return
         */
        float measure_text_descent(const std::string& font_name, float font_size) const override;

        /**
         * @brief Measures the ascent (baseline to top of line box) for the specified font.
         * Stateless: does not modify page state.
         */
        float measure_text_ascent(const std::string& font_name, float font_size) const override;

    private:
        /**
         * @brief Resolves `font_name` to an HPDF_Font, same as before, but also reports
         * (via `needs_win_ansi_transcode`, if non-null) whether the winning encoding was
         * WinAnsiEncoding rather than UTF-8 -- callers that draw/measure raw text need
         * this to transcode it first (see docraft_haru_text_backend.cc's anonymous
         * namespace), since a WinAnsiEncoding-selected font interprets each byte as its
         * own glyph and would otherwise split a multi-byte UTF-8 sequence into mojibake.
         */
        HPDF_Font resolve_font(const std::string& font_name, bool* needs_win_ansi_transcode = nullptr) const;
        std::shared_ptr<DocraftHaruSharedState> state_;
        static inline const std::string kDefaultFont = "Helvetica";
        // Sticky across draw_text()/draw_text_matrix() calls that don't take a font_name
        // of their own -- set by the preceding set_font() call, which does know which
        // encoding resolve_font() picked for the font it just activated.
        mutable bool current_needs_win_ansi_transcode_ = true;
        // Names already warned about falling back to kDefaultFont -- resolve_font() is
        // called per glyph run/measurement, so without this a single bad font_name would
        // spam a warning on every call instead of once.
        mutable std::unordered_set<std::string> warned_unresolved_fonts_;
    };
} // namespace docraft::backend::pdf

