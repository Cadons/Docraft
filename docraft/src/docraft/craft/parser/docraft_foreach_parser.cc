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

#include "docraft/craft/parser/docraft_foreach_parser.h"

#include "docraft/craft/docraft_craft_language_tokens.h"
#include "docraft/exception/docraft_exceptions.h"

namespace docraft::craft::parser {
    std::any DocraftForeachParser::parse(const pugi::xml_node& craft_language_source)
    {
        const auto model_attr = craft_language_source.attribute(
            elements::templating::foreach::attribute::kModel.data());
        const auto n_attr = craft_language_source.attribute(elements::templating::foreach::attribute::kN.data());

        if (model_attr && n_attr)
        {
            throw docraft::exception::InvalidInputException("<Foreach> cannot have both 'model' and 'n' attributes");
        }
        if (!model_attr && !n_attr)
        {
            throw docraft::exception::InvalidInputException("<Foreach> requires either a 'model' or an 'n' attribute");
        }

        ParsedForeachData data;
        if (model_attr)
        {
            // Stored verbatim: `model` may be a `${variable}`/`${data(...)}` expression
            // instead of a JSON literal, and may use single-quoted JSON instead of
            // double-quoted -- both are resolved later, at build time, by
            // DocraftLoomTreeBuilder::expand_foreach() (which has access to the template
            // engine and, for a nested <Foreach>, the outer iteration's item).
            data.model = model_attr.as_string();
        }
        else
        {
            const int n = n_attr.as_int();
            if (n < 0)
            {
                throw docraft::exception::InvalidInputException(
                    "<Foreach> 'n' attribute must be a non-negative integer");
            }
            data.count = n;
        }
        return data;
    }

    std::vector<std::string_view> DocraftForeachParser::accepted_attributes() const
    {
        return {
            elements::templating::foreach::attribute::kModel,
            elements::templating::foreach::attribute::kN
        };
    }
} // namespace docraft::craft::parser
