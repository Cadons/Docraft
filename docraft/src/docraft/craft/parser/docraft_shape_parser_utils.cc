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

#include "docraft/craft/parser/docraft_shape_parser_utils.h"

#include <cctype>
#include <sstream>
#include <stdexcept>

namespace docraft::craft::parser::detail {
    std::vector<model::DocraftPoint> parse_points_attribute(const pugi::xml_node &node, const char *attr_name) {
        std::vector<model::DocraftPoint> points;
        if (!attr_name) {
            return points;
        }
        auto attr = node.attribute(attr_name);
        if (!attr) {
            return points;
        }

        std::string value = attr.as_string();
        if (value.empty()) {
            return points;
        }

        std::stringstream ss(value);
        std::string token;
        while (std::getline(ss, token, ' ')) {
            if (token.empty()) {
                continue;
            }
            auto comma_pos = token.find(',');
            if (comma_pos == std::string::npos) {
                throw std::invalid_argument("Invalid point token: " + token);
            }
            std::string x_str = token.substr(0, comma_pos);
            std::string y_str = token.substr(comma_pos + 1);
            if (x_str.empty() || y_str.empty()) {
                throw std::invalid_argument("Invalid point token: " + token);
            }
            float x = std::stof(x_str);
            float y = std::stof(y_str);
            points.push_back({.x = x, .y = y});
        }
        return points;
    }
} // namespace docraft::craft::parser::detail
