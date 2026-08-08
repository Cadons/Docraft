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

#include "docraft/craft/parser/docraft_parser.h"

#include "docraft/craft/parser/docraft_parser_helpers.h"

namespace docraft::craft::parser {
    std::any DocraftSeriesParser::parse(const pugi::xml_node& craft_language_source)
    {
        ParsedSeriesData data;
        if (auto color_attr = craft_language_source.attribute(elements::series::attribute::kColor.data()))
        {
            data.color = detail::get_color_attribute_raw(color_attr);
        }
        if (auto model_attr = craft_language_source.attribute(elements::series::attribute::kModel.data()))
        {
            data.model = model_attr.as_string();
        }
        return data;
    }

    std::vector<std::string_view> DocraftSeriesParser::accepted_attributes() const
    {
        return {
            elements::series::attribute::kColor,
            elements::series::attribute::kModel
        };
    }
} // namespace docraft::craft::parser
