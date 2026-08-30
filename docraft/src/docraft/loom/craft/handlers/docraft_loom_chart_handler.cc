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

#include "docraft/loom/craft/handlers/docraft_loom_chart_handler.h"

#include <any>
#include <optional>
#include <utility>
#include <vector>

#include <nlohmann/json.hpp>

#include "docraft/craft/docraft_craft_language_tokens.h"
#include "docraft/craft/parser/docraft_parser.h"
#include "docraft/exception/docraft_exceptions.h"
#include "docraft/loom/charts/docraft_chart_types.h"
#include "docraft/loom/craft/docraft_loom_tree_builder_utils.h"
#include "docraft/loom/nodes/docraft_loom_node.h"

namespace docraft::loom::craft {
    namespace tokens = docraft::craft;
    namespace parser = docraft::craft::parser;

    struct DocraftLoomChartHandler::ResolvedSeriesPoints
    {
        std::vector<nodes::Position> points;
        std::vector<std::optional<std::string>> labels;
    };

    DocraftLoomChartHandler::ResolvedSeriesPoints DocraftLoomChartHandler::resolve_series_points(
        const std::string& raw, DocraftLoomTableHandlerContext& context)
    {
        if (raw.empty())
        {
            return {};
        }
        const std::string normalized = normalize_single_quoted_json(context.render_template_text(raw));

        nlohmann::json parsed;
        try
        {
            parsed = nlohmann::json::parse(normalized);
        }
        catch (const nlohmann::json::exception& e)
        {
            throw docraft::exception::DataFormatException(
                std::string{"<Series> 'model' is not valid JSON: "} + e.what());
        }
        if (!parsed.is_array())
        {
            throw docraft::exception::DataFormatException("<Series> 'model' must resolve to a JSON array");
        }

        ResolvedSeriesPoints resolved;
        resolved.points.reserve(parsed.size());
        resolved.labels.reserve(parsed.size());
        for (const auto& item : parsed)
        {
            float x, y;
            std::optional<std::string> label;
            if (item.is_array() && item.size() == 2 && item[0].is_number() && item[1].is_number())
            {
                x = item[0].get<float>();
                y = item[1].get<float>();
            }
            else if (item.is_object() && item.contains("x") && item.contains("y")
                     && item["x"].is_number() && item["y"].is_number())
            {
                x = item["x"].get<float>();
                y = item["y"].get<float>();
            }
            else if (item.is_object() && item.size() == 1 && item.begin().value().is_number())
            {
                // The shape pie/histogram model data always uses: a single "label":
                // value entry. There's no explicit x, so its ordinal position in the
                // array becomes x (a category index) -- histogram/pie map that back to
                // an X-axis tick label / slice label via the parallel `label` below.
                x = static_cast<float>(resolved.points.size());
                y = item.begin().value().get<float>();
                label = item.begin().key();
            }
            else
            {
                throw docraft::exception::DataFormatException(
                    "<Series> 'model' entries must be [x,y] pairs, {\"x\":..,\"y\":..} objects, or "
                    "{\"label\":value} objects");
            }
            resolved.points.push_back({.x = x, .y = y});
            resolved.labels.push_back(std::move(label));
        }
        return resolved;
    }

    std::shared_ptr<nodes::DocraftLoomNode> DocraftLoomChartHandler::build(
        const docraft::craft::DocraftParsedElement& element, DocraftLoomTableHandlerContext& context)
    {
        const auto& data = std::any_cast<const parser::ParsedChartData&>(element.data);

        if (!data.style.has_value())
        {
            throw docraft::exception::InvalidInputException("<Chart> requires a 'style' attribute");
        }
        if (!element.common.width.has_value() || !element.common.height.has_value())
        {
            throw docraft::exception::InvalidInputException(
                "<Chart> requires explicit 'width' and 'height' attributes");
        }

        const auto* builder = charts::DocraftChartBuilderRegistry::instance().find(*data.style);
        if (!builder)
        {
            throw docraft::exception::InvalidInputException("<Chart> unknown style: '" + *data.style + "'");
        }

        charts::DocraftChartBuildContext build_context;
        build_context.width = *element.common.width;
        build_context.height = *element.common.height;
        build_context.axis_position = charts::parse_chart_axis_position(data.axis_position.value_or("left"));
        if (data.title.has_value())
        {
            build_context.title = context.render_template_text(*data.title);
        }
        if (data.x_label.has_value())
        {
            build_context.x_label = context.render_template_text(*data.x_label);
        }
        if (data.y_label.has_value())
        {
            build_context.y_label = context.render_template_text(*data.y_label);
        }
        build_context.show_percentage = data.show_percentage.value_or(true);
        build_context.font_family = context.default_font_family();

        build_context.series.reserve(element.children.size());
        for (const auto& child : element.children)
        {
            if (child->tag_name != std::string{tokens::elements::kSeries})
            {
                throw docraft::exception::InvalidInputException("<Chart> only accepts <Series> children, got <"
                                                                  + child->tag_name + ">");
            }
            const auto& series_data = std::any_cast<const parser::ParsedSeriesData&>(child->data);
            charts::DocraftChartSeries series;
            series.name = child->common.name.value_or("");
            series.color = series_data.color ? context.resolve_color(*series_data.color)
                                              : charts::default_series_color(build_context.series.size());
            ResolvedSeriesPoints resolved = resolve_series_points(series_data.model.value_or(""), context);
            series.points = std::move(resolved.points);
            series.point_labels = std::move(resolved.labels);
            build_context.series.push_back(std::move(series));
        }

        auto canvas = (*builder)(build_context);
        apply_shape_style(*canvas, data, [&context](const std::string& c) { return context.resolve_color(c); });
        apply_common_attributes(*canvas, element.common);
        return canvas;
    }
} // namespace docraft::loom::craft
