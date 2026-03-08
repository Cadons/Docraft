#include "craft/parser/docraft_parser.h"

#include "craft/parser/docraft_parser_helpers.h"
#include "model/docraft_rectangle.h"

namespace docraft::craft::parser {
    void DocraftRectangleParser::apply_attributes_to(const std::shared_ptr<model::DocraftRectangle> &from,
                                                     const std::shared_ptr<model::DocraftRectangle> &to) {
        to->set_background_color(from->background_color());
        to->set_border_color(from->border_color());
        to->set_border_width(from->border_width());
    }

    std::shared_ptr<model::DocraftNode> DocraftRectangleParser::parse(const pugi::xml_node &craft_language_source) {
        auto rectangle = std::make_shared<model::DocraftRectangle>();
        if (auto background_color_attr = craft_language_source.attribute(
                elements::rectangle::attribute::kBackgroundColor.data())) {
            //Refactoring: The color handling could be refactored to avoid specific parsing and handling for each node that has a color attribute,
            //but for now this is the most straightforward way to handle template expressions for colors
            rectangle->set_background_color(detail::get_docraft_color(background_color_attr));
            if (rectangle->background_color().is_template_expression()) {
                rectangle->set_background_color_template_expression(rectangle->background_color().template_expression());
            }
        }
        if (auto border_color_attr = craft_language_source.attribute(
                elements::rectangle::attribute::kBorderColor.data())) {
            rectangle->set_border_color(detail::get_docraft_color(border_color_attr));
            if (rectangle->border_color().is_template_expression()) {
                rectangle->set_border_color_template_expression(rectangle->border_color().template_expression());
            }

        }
        if (auto border_width_attr = craft_language_source.attribute(
                elements::rectangle::attribute::kBorderWidth.data())) {
            rectangle->set_border_width(border_width_attr.as_float());
        }
        detail::configure_docraft_node_attributes(rectangle, craft_language_source);
        return rectangle;
    }
}
