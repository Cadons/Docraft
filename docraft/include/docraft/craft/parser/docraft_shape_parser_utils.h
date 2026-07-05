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
#include <vector>

#include <pugixml.hpp>

#include "docraft/docraft_position.h"

namespace docraft::craft::parser::detail {
    /**
     * @brief Parses a "x,y x,y ..." points attribute (the convention shared by Triangle
     * and Polygon) into a list of points.
     */
    std::vector<docraft::Position> parse_points_attribute(const pugi::xml_node& node, const char* attr_name);
}
