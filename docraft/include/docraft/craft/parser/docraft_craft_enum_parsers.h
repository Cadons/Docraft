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

#include <string>

#include "docraft/backend/docraft_page_format.h"
#include "docraft/docraft_lib.h"

namespace docraft::craft::parser::detail {
    /**
     * @brief Parses a `<Page size="...">` attribute value (case-insensitive
     * A3/A4/A5/Letter/Legal).
     * @throws docraft::exception::InvalidInputException if `size_str` isn't recognized.
     */
    DOCRAFT_LIB docraft::backend::DocraftPageSize parse_page_size(const std::string& size_str);

    /**
     * @brief Parses a `<Page orientation="...">` attribute value (landscape/portrait).
     * @throws docraft::exception::InvalidInputException if `orientation_str` isn't
     * recognized.
     */
    DOCRAFT_LIB docraft::backend::DocraftPageOrientation parse_page_orientation(const std::string& orientation_str);
} // namespace docraft::craft::parser::detail
