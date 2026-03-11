#include "docraft/craft/parser/docraft_polygon_parser.h"

#include "docraft/craft/docraft_craft_language_tokens.h"
#include "docraft/craft/parser/docraft_parser_helpers.h"
#include "docraft/craft/parser/docraft_shape_parser_utils.h"
#include "docraft/model/docraft_polygon.h"

namespace docraft::craft::parser {
    std::shared_ptr<model::DocraftNode> DocraftPolygonParser::parse(const pugi::xml_node &craft_language_source) {
        auto polygon = std::make_shared<model::DocraftPolygon>();
        if (auto background_color_attr = craft_language_source.attribute(
                elements::polygon::attribute::kBackgroundColor.data())) {
            polygon->set_background_color(detail::get_docraft_color(background_color_attr));
        }
        if (auto border_color_attr = craft_language_source.attribute(
                elements::polygon::attribute::kBorderColor.data())) {
            polygon->set_border_color(detail::get_docraft_color(border_color_attr));
        }
        if (auto border_width_attr = craft_language_source.attribute(
                elements::polygon::attribute::kBorderWidth.data())) {
            polygon->set_border_width(border_width_attr.as_float());
        }

        auto points = detail::parse_points_attribute(craft_language_source,
                                                     elements::polygon::attribute::kPoints.data());
        if (!points.empty()) {
            polygon->set_points(points);
        }

        detail::configure_docraft_node_attributes(polygon, craft_language_source);
        return polygon;
    }
} // namespace docraft::craft::parser
