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
        HPDF_Page_TextOut(provider->current_page(), x, y, text.c_str());
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
        auto provider = state_->ensure_page_provider();
        HPDF_Page page = provider->current_page();
        HPDF_Page_SetTextMatrix(
            page,
            scale_x,
            skew_x,
            skew_y,
            scale_y,
            translate_x,
            translate_y);
        HPDF_Page_ShowText(page, text.c_str());
    }

    HPDF_Font DocraftHaruTextBackend::resolve_font(const std::string& font_name) const
    {
        if (font_name.empty() || !state_->pdf)
            return nullptr;
        HPDF_Font font = HPDF_GetFont(state_->pdf, font_name.c_str(), "UTF-8");
        if (!font || HPDF_GetError(state_->pdf) != HPDF_OK)
        {
            HPDF_ResetError(state_->pdf);
            font = HPDF_GetFont(state_->pdf, font_name.c_str(), nullptr);
            if (!font || HPDF_GetError(state_->pdf) != HPDF_OK)
            {
                HPDF_ResetError(state_->pdf);
                return nullptr;
            }
        }
        return font;
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
        auto* provider = state_->ensure_page_provider();
        return HPDF_Page_TextWidth(provider->current_page(), text.c_str());
    }

    float DocraftHaruTextBackend::measure_text_height(const std::string& font_name, float font_size) const
    {
        constexpr float kEM = 1000.0F;
        HPDF_Font font = resolve_font(font_name);
        if (!font || font_size <= 0.0F)
        {
            auto* provider = state_->ensure_page_provider();
            HPDF_Page page = provider->current_page();
            font = HPDF_Page_GetCurrentFont(page);
            font_size = HPDF_Page_GetCurrentFontSize(page);
        }
        const float ascent = static_cast<float>(HPDF_Font_GetAscent(font))  * font_size / kEM;
        const float descent = static_cast<float>(HPDF_Font_GetDescent(font)) * font_size / kEM;
        return ascent - descent;
    }
} // namespace docraft::backend::pdf

