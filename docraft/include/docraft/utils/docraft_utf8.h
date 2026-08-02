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
#include <cstddef>
#include <string>

namespace docraft::utils {
    /**
     * @brief Returns the byte length of the UTF-8 codepoint starting with `lead_byte`.
     * A stray continuation byte or invalid lead falls back to 1 so callers advancing
     * through a string always make forward progress.
     * @param lead_byte The first byte of a (possibly multi-byte) UTF-8 codepoint.
     * @return The codepoint's byte length (1-4).
     */
    std::size_t utf8_codepoint_byte_length(unsigned char lead_byte);

    /**
     * @brief Counts the number of UTF-8 codepoints (glyphs) in `text`, as opposed to
     * its raw byte length -- a multi-byte character (e.g. an accented letter) counts
     * as one codepoint, not one per encoded byte.
     * @param text UTF-8 encoded string.
     * @return The number of codepoints in `text`.
     */
    std::size_t utf8_codepoint_count(const std::string& text);
}
