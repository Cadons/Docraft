#include "craft/parser/docraft_triangle_parser.h"

#include <stdexcept>

#include "craft/docraft_craft_language_tokens.h"
#include "craft/parser/docraft_parser_helpers.h"
#include "craft/parser/docraft_shape_parser_utils.h"
#include "model/docraft_triangle.h"

namespace docraft::craft::parser {
    std::shared_ptr<model::DocraftNode> DocraftTriangleParser::parse(const pugi::xml_node &craft_language_source) {
        auto triangle = std::make_shared<model::DocraftTriangle>();
        if (auto background_color_attr = craft_language_source.attribute(
                elements::triangle::attribute::kBackgroundColor.data())) {
            triangle->set_background_color(detail::get_docraft_color(background_color_attr));
        }
        if (auto border_color_attr = craft_language_source.attribute(
                elements::triangle::attribute::kBorderColor.data())) {
            triangle->set_border_color(detail::get_docraft_color(border_color_attr));
        }
        if (auto border_width_attr = craft_language_source.attribute(
                elements::triangle::attribute::kBorderWidth.data())) {
            triangle->set_border_width(border_width_attr.as_float());
        }

        auto points = detail::parse_points_attribute(craft_language_source,
                                                     elements::triangle::attribute::kPoints.data());
        if (!points.empty()) {
            if (points.size() != 3U) {
                throw std::invalid_argument("Triangle requires exactly 3 points");
            }
            triangle->set_points(points);
        }

        if (!craft_language_source.attribute(basic::attribute::kPosition.data())) {
            triangle->set_position_mode(model::DocraftPositionType::kAbsolute);
        }
        detail::configure_docraft_node_attributes(triangle, craft_language_source);
        return triangle;
    }
} // namespace docraft::craft::parser
