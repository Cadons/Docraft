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

#include "docraft/craft/parser/docraft_craft_enum_parsers.h"

#include "docraft/exception/docraft_exceptions.h"
#define A3_STRING "a3"
#define A4_STRING "a4"
#define A5_STRING "a5"
#define LETTER_STRING "letter"
#define LEGAL_STRING "legal"

#define ORIENTATION_LANDSCAPE_STRING "landscape"
#define ORIENTATION_PORTRAIT_STRING "portrait"

namespace docraft::craft::parser::detail {
    docraft::backend::DocraftPageSize parse_page_size(const std::string& size_str)
    {
        std::string s = size_str;
        std::ranges::transform(s, s.begin(), [](const unsigned char ch) {
            return static_cast<char>(std::tolower(ch));
        });

        if (s == A3_STRING)
        {
            return docraft::backend::DocraftPageSize::kA3;
        }
        if (s == A5_STRING)
        {
            return docraft::backend::DocraftPageSize::kA5;
        }
        if (s == A4_STRING)
        {
            return docraft::backend::DocraftPageSize::kA4;
        }
        if (s == LETTER_STRING)
        {
            return docraft::backend::DocraftPageSize::kLetter;
        }
        if (s == LEGAL_STRING)
        {
            return docraft::backend::DocraftPageSize::kLegal;
        }
        throw docraft::exception::InvalidInputException("Invalid page_size: " + size_str);
    }

    docraft::backend::DocraftPageOrientation parse_page_orientation(const std::string& orientation_str)
    {
        std::string s = orientation_str;
        std::ranges::transform(s, s.begin(), [](const unsigned char ch) {
            return static_cast<char>(std::tolower(ch));
        });

        if (s == ORIENTATION_LANDSCAPE_STRING)
        {
            return docraft::backend::DocraftPageOrientation::kLandscape;
        }
        if (s == ORIENTATION_PORTRAIT_STRING)
        {
            return docraft::backend::DocraftPageOrientation::kPortrait;
        }
        throw docraft::exception::InvalidInputException("Invalid page_orientation: " + orientation_str);
    }
} // namespace docraft::craft::parser::detail
