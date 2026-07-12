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

#include "docraft/backend/pdf/docraft_haru_text_backend.h"

#include <hpdf.h>

#include "docraft/utils/docraft_font_registry.h"

namespace docraft::backend::pdf {
    DocraftHaruTextBackend::DocraftHaruTextBackend(const std::shared_ptr<DocraftHaruSharedState> &state)
        : state_(state) {
    }

    void DocraftHaruTextBackend::begin_text() const {
        auto *provider = state_->ensure_page_provider();
        HPDF_Page_BeginText(provider->current_page());
    }

    void DocraftHaruTextBackend::end_text() const {
        auto *provider = state_->ensure_page_provider();
        HPDF_Page_EndText(provider->current_page());
    }

    void DocraftHaruTextBackend::draw_text(const std::string &text, float x, float y) const {
        auto *provider = state_->ensure_page_provider();
        float px, py;
        provider->compute_coordinate_system(x, y, px, py);
        HPDF_Page_TextOut(provider->current_page(), px, py, text.c_str());
    }

    void DocraftHaruTextBackend::set_text_color(float r, float g, float b) const {
        auto *provider = state_->ensure_page_provider();
        HPDF_Page_SetRGBFill(provider->current_page(), r, g, b);
    }

    void DocraftHaruTextBackend::draw_text_matrix(const std::string &text,
                                                  float scale_x,
                                                  float skew_x,
                                                  float skew_y,
                                                  float scale_y,
                                                  float translate_x,
                                                  float translate_y) const {
        auto* provider = state_->ensure_page_provider();
        float px, py;
        provider->compute_coordinate_system(translate_x, translate_y, px, py);
        HPDF_Page page = provider->current_page();
        HPDF_Page_SetTextMatrix(page, scale_x, skew_x, skew_y, scale_y, px, py);
        HPDF_Page_ShowText(page, text.c_str());
    }

    void DocraftHaruTextBackend::set_font(const std::string& font_name, float font_size) const
    {
        HPDF_Font font = resolve_font(font_name);
        if (font && font_size > 0.0F)
        {
            auto* provider = state_->ensure_page_provider();
            HPDF_Page_SetFontAndSize(provider->current_page(), font, font_size);
        }
    }

    HPDF_Font DocraftHaruTextBackend::resolve_font(const std::string& font_name) const
    {
        if (font_name.empty() || !state_->pdf)
            return nullptr;

        // A craft-language family name (e.g. "OpenSans-Bold") is rarely libharu's own
        // internal name for the loaded TTF -- resolve it through the alias table
        // DocraftLoomPdfCreator::register_font() populates. Names with no registered
        // alias pass through unchanged (built-in base-14 names, or an already-internal
        // name passed directly).
        const std::string resolved_name = docraft::utils::DocraftFontRegistry::instance().resolve_font_alias(
            font_name);

        // Try UTF-8 first: register_font() always embeds custom TrueType fonts, and an
        // embedded TTF accepts UTF-8, which is required for any non-WinAnsi character
        // (e.g. accented letters) to render correctly instead of being shown byte-by-byte.
        // UTF-8 fails with HPDF_FONT_INVALID_WIDTHS_TABLE for the Type1 built-in fonts
        // (Helvetica, Times, Courier…), so those fall back to WinAnsiEncoding below.
        static constexpr const char* kEncodings[] = {"UTF-8", "WinAnsiEncoding", nullptr};
        for (const char* enc : kEncodings)
        {
            HPDF_Font font = HPDF_GetFont(state_->pdf, resolved_name.c_str(), enc);
            if (font && HPDF_GetError(state_->pdf) == HPDF_OK)
                return font;
            HPDF_ResetError(state_->pdf);
        }
        return nullptr;
    }

    float DocraftHaruTextBackend::measure_text_width(const std::string& text, const std::string& font_name,
                                                     float font_size) const
    {
        constexpr float kEM = 1000.0F;
        HPDF_Font font = resolve_font(font_name);
        if (font && font_size > 0.0F)
        {
            HPDF_TextWidth tw = HPDF_Font_TextWidth(font,
                                                    reinterpret_cast<const HPDF_BYTE*>(text.c_str()),
                                                    static_cast<HPDF_UINT>(text.size()));
            return static_cast<float>(tw.width) * font_size / kEM;
        }
        return 0.0F;
    }

    float DocraftHaruTextBackend::measure_text_height(const std::string& font_name, float font_size) const
    {
        constexpr float kEM = 1000.0F;
        HPDF_Font font = resolve_font(font_name);
        if (!font)
            return font_size > 0.0F ? font_size : 12.0F;
        if (font_size <= 0.0F)
            font_size = 12.0F;
        const float ascent = measure_text_ascent(font_name, font_size);
        const float descent = measure_text_descent(font_name, font_size);
        return ascent - descent;
    }
    float DocraftHaruTextBackend::measure_text_descent(const std::string& font_name, float font_size) const
    {
        constexpr float kEM = 1000.0F;
        HPDF_Font font = resolve_font(font_name);
        if (!font)
            return font_size > 0.0F ? -font_size * 0.2F : -2.4F; // Default descent is ~20% of font size
        if (font_size <= 0.0F)
            font_size = 12.0F;
        return static_cast<float>(HPDF_Font_GetDescent(font)) * font_size / kEM;
    }

    float DocraftHaruTextBackend::measure_text_ascent(const std::string& font_name, float font_size) const
    {
        constexpr float kEM = 1000.0F;
        HPDF_Font font = resolve_font(font_name);
        if (!font)
            return font_size > 0.0F ? font_size : 12.0F;
        if (font_size <= 0.0F)
            font_size = 12.0F;
        return static_cast<float>(HPDF_Font_GetAscent(font)) * font_size / kEM;
    }
} // namespace docraft::backend::pdf

