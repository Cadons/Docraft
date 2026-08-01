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

#include "docraft/utils/docraft_utf8.h"

namespace docraft::utils {
    std::size_t utf8_codepoint_byte_length(unsigned char lead_byte)
    {
        if ((lead_byte & 0x80U) == 0x00U) return 1; // ASCII
        if ((lead_byte & 0xE0U) == 0xC0U) return 2;
        if ((lead_byte & 0xF0U) == 0xE0U) return 3;
        if ((lead_byte & 0xF8U) == 0xF0U) return 4;
        return 1;
    }

    std::size_t utf8_codepoint_count(const std::string& text)
    {
        std::size_t count = 0;
        std::size_t i = 0;
        while (i < text.length())
        {
            i += utf8_codepoint_byte_length(static_cast<unsigned char>(text[i]));
            ++count;
        }
        return count;
    }
}
