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

#include <cstddef>
#include <cstdint>

#include <hpdf.h>

#include "docraft/utils/docraft_font_registry.h"
#include "docraft/utils/docraft_logger.h"

namespace docraft::backend::pdf {
    namespace {
        // Decodes one UTF-8 codepoint starting at `text[pos]`, advancing `pos` past it.
        // Returns the Unicode replacement character for a truncated/invalid sequence,
        // still advancing `pos` by at least one byte so callers always make progress.
        char32_t decode_utf8_codepoint(const std::string& text, std::size_t& pos)
        {
            const auto lead = static_cast<unsigned char>(text[pos]);
            std::size_t extra_bytes = 0;
            char32_t codepoint = 0;
            if ((lead & 0x80U) == 0x00U)
            {
                codepoint = lead;
            }
            else if ((lead & 0xE0U) == 0xC0U)
            {
                codepoint = lead & 0x1FU;
                extra_bytes = 1;
            }
            else if ((lead & 0xF0U) == 0xE0U)
            {
                codepoint = lead & 0x0FU;
                extra_bytes = 2;
            }
            else if ((lead & 0xF8U) == 0xF0U)
            {
                codepoint = lead & 0x07U;
                extra_bytes = 3;
            }
            else
            {
                ++pos;
                return 0xFFFD;
            }

            if (pos + extra_bytes >= text.size())
            {
                pos = text.size();
                return 0xFFFD; // truncated sequence
            }
            for (std::size_t i = 1; i <= extra_bytes; ++i)
            {
                const auto continuation = static_cast<unsigned char>(text[pos + i]);
                if ((continuation & 0xC0U) != 0x80U)
                {
                    ++pos;
                    return 0xFFFD;
                }
                codepoint = (codepoint << 6U) | (continuation & 0x3FU);
            }
            pos += extra_bytes + 1;
            return codepoint;
        }

        // Maps a Unicode codepoint to its WinAnsiEncoding (Windows-1252) byte, or '?' if
        // that single-byte encoding has no glyph for it. 0x00-0x7F and 0xA0-0xFF match
        // Unicode directly (the latter covers every accented Western-European letter --
        // a, e, i, o, u, c, n with diacritics -- needed for Italian/French/Spanish/etc.);
        // 0x80-0x9F is Windows-1252's own block of typographic characters with no
        // fixed Unicode offset, so those are mapped by an explicit table.
        unsigned char codepoint_to_win_ansi(char32_t codepoint)
        {
            if (codepoint < 0x80U || (codepoint >= 0xA0U && codepoint <= 0xFFU))
            {
                return static_cast<unsigned char>(codepoint);
            }
            switch (codepoint)
            {
            case 0x20AC: return 0x80; // Euro sign
            case 0x201A: return 0x82;
            case 0x0192: return 0x83;
            case 0x201E: return 0x84;
            case 0x2026: return 0x85; // Horizontal ellipsis
            case 0x2020: return 0x86;
            case 0x2021: return 0x87;
            case 0x02C6: return 0x88;
            case 0x2030: return 0x89;
            case 0x0160: return 0x8A;
            case 0x2039: return 0x8B;
            case 0x0152: return 0x8C;
            case 0x017D: return 0x8E;
            case 0x2018: return 0x91; // Left single quote
            case 0x2019: return 0x92; // Right single quote
            case 0x201C: return 0x93; // Left double quote
            case 0x201D: return 0x94; // Right double quote
            case 0x2022: return 0x95; // Bullet
            case 0x2013: return 0x96; // En dash
            case 0x2014: return 0x97; // Em dash
            case 0x02DC: return 0x98;
            case 0x2122: return 0x99; // Trademark sign
            case 0x0161: return 0x9A;
            case 0x203A: return 0x9B;
            case 0x0153: return 0x9C;
            case 0x017E: return 0x9E;
            case 0x0178: return 0x9F;
            default: return '?';
            }
        }

        // Transcodes UTF-8 (what pugixml/JSON hand the rest of docraft) into
        // WinAnsiEncoding single-byte text, required before it reaches a base-14
        // built-in font's draw/measure call -- a WinAnsiEncoding-selected font
        // interprets each byte of the string as its own glyph, so raw UTF-8 bytes for
        // any non-ASCII character split into mojibake (e.g. "\xC3\xA0" -- UTF-8 for
        // "a" -- prints as two separate WinAnsi glyphs instead of one).
        std::string transcode_utf8_to_win_ansi(const std::string& utf8_text)
        {
            std::string result;
            result.reserve(utf8_text.size());
            std::size_t pos = 0;
            while (pos < utf8_text.size())
            {
                const char32_t codepoint = decode_utf8_codepoint(utf8_text, pos);
                result.push_back(static_cast<char>(codepoint_to_win_ansi(codepoint)));
            }
            return result;
        }
    } // namespace

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
        const std::string out_text = current_needs_win_ansi_transcode_ ? transcode_utf8_to_win_ansi(text) : text;
        HPDF_Page_TextOut(provider->current_page(), px, py, out_text.c_str());
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
        const std::string out_text = current_needs_win_ansi_transcode_ ? transcode_utf8_to_win_ansi(text) : text;
        HPDF_Page_ShowText(page, out_text.c_str());
    }

    void DocraftHaruTextBackend::set_font(const std::string& font_name, float font_size) const
    {
        bool needs_win_ansi_transcode = true;
        HPDF_Font font = resolve_font(font_name, &needs_win_ansi_transcode);
        if (font && font_size > 0.0F)
        {
            current_needs_win_ansi_transcode_ = needs_win_ansi_transcode;
            auto* provider = state_->ensure_page_provider();
            HPDF_Page_SetFontAndSize(provider->current_page(), font, font_size);
        }
    }

    HPDF_Font DocraftHaruTextBackend::resolve_font(const std::string& font_name, bool* needs_win_ansi_transcode) const
    {
        if (needs_win_ansi_transcode)
        {
            *needs_win_ansi_transcode = true;
        }
        if (font_name.empty() || !state_->pdf)
            return nullptr;

        // A craft-language family name (e.g. "OpenSans-Bold") is rarely libharu's own
        // internal name for the loaded TTF -- resolve it through the alias table
        // DocraftLoomPdfCreator::register_font() populates. Names with no registered
        // alias pass through unchanged (built-in base-14 names, or an already-internal
        // name passed directly).
        const std::string resolved_name = docraft::utils::DocraftFontRegistry::instance().resolve_font_alias(
            font_name);

        // Try UTF-8 first, but only for a name that went through the alias table --
        // that means it's a font *we* registered via DocraftLoomPdfCreator::register_font()
        // (always an embedded TrueType, which accepts UTF-8, required for any non-WinAnsi
        // character, e.g. accented letters, to render correctly instead of byte-by-byte).
        // An unaliased name is a base-14 Type1 built-in (Helvetica, Times, Courier…),
        // which always rejects UTF-8 with HPDF_INVALID_COMBINATION_BETWEEN_FONT_AND_ENCODER
        // -- probing it would print that HPDF error to stderr on every single resolution
        // for a guaranteed failure, so go straight to WinAnsiEncoding for those instead.
        const bool is_registered_alias = resolved_name != font_name;
        static constexpr const char* kEmbeddedEncodings[] = {"UTF-8", "WinAnsiEncoding"};
        static constexpr const char* kBuiltinEncodings[] = {"WinAnsiEncoding"};
        const char* const* encodings = is_registered_alias ? kEmbeddedEncodings : kBuiltinEncodings;
        const std::size_t encoding_count = is_registered_alias ? 2 : 1;
        for (std::size_t i = 0; i < encoding_count; ++i)
        {
            HPDF_Font font = HPDF_GetFont(state_->pdf, resolved_name.c_str(), encodings[i]);
            if (font && HPDF_GetError(state_->pdf) == HPDF_OK)
            {
                if (needs_win_ansi_transcode)
                {
                    *needs_win_ansi_transcode = std::string{encodings[i]} == "WinAnsiEncoding";
                }
                return font;
            }
            HPDF_ResetError(state_->pdf);
        }
        if (warned_unresolved_fonts_.insert(font_name).second)
        {
            LOG_WARNING("Font '" + font_name + "' could not be resolved; falling back to " + kDefaultFont);
        }
        return HPDF_GetFont(state_->pdf, kDefaultFont.c_str(), "WinAnsiEncoding");
    }

    float DocraftHaruTextBackend::measure_text_width(const std::string& text, const std::string& font_name,
                                                     float font_size) const
    {
        constexpr float kEM = 1000.0F;
        bool needs_win_ansi_transcode = true;
        HPDF_Font font = resolve_font(font_name, &needs_win_ansi_transcode);
        if (font && font_size > 0.0F)
        {
            const std::string out_text = needs_win_ansi_transcode ? transcode_utf8_to_win_ansi(text) : text;
            HPDF_TextWidth tw = HPDF_Font_TextWidth(font,
                                                    reinterpret_cast<const HPDF_BYTE*>(out_text.c_str()),
                                                    static_cast<HPDF_UINT>(out_text.size()));
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

