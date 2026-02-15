#include "craft/parser/docraft_circle_parser.h"

#include "craft/docraft_craft_language_tokens.h"
#include "craft/parser/docraft_parser_helpers.h"
#include "model/docraft_circle.h"

namespace docraft::craft::parser {
    std::shared_ptr<model::DocraftNode> DocraftCircleParser::parse(const pugi::xml_node &craft_language_source) {
        auto circle = std::make_shared<model::DocraftCircle>();
        if (auto background_color_attr = craft_language_source.attribute(
                elements::circle::attribute::kBackgroundColor.data())) {
            circle->set_background_color(detail::get_docraft_color(background_color_attr));
        }
        if (auto border_color_attr = craft_language_source.attribute(
                elements::circle::attribute::kBorderColor.data())) {
            circle->set_border_color(detail::get_docraft_color(border_color_attr));
        }
        if (auto border_width_attr = craft_language_source.attribute(
                elements::circle::attribute::kBorderWidth.data())) {
            circle->set_border_width(border_width_attr.as_float());
        }
        if (!craft_language_source.attribute(basic::attribute::kPosition.data())) {
            circle->set_position_mode(model::DocraftPositionType::kAbsolute);
        }
        detail::configure_docraft_node_attributes(circle, craft_language_source);
        return circle;
    }
} // namespace docraft::craft::parser
