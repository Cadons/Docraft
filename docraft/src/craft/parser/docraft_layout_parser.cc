#include "craft/parser/docraft_parser.h"

#include "craft/parser/docraft_parser_helpers.h"
#include "model/docraft_layout.h"

namespace docraft::craft::parser {
    std::shared_ptr<model::DocraftNode> DocraftLayoutParser::parse(const pugi::xml_node &craft_language_source) {
        auto layout_node = std::make_shared<model::DocraftLayout>();
        if (auto orientation_attr = craft_language_source.attribute(elements::layout::attribute::kOrientation.data())) {
            std::string orientation_str = orientation_attr.as_string();
            if (orientation_str == std::string{orientation::kHorizontal}) {
                layout_node->set_orientation(model::LayoutOrientation::kHorizontal);
            } else if (orientation_str == std::string{orientation::kVertical}) {
                layout_node->set_orientation(model::LayoutOrientation::kVertical);
            } else {
                throw std::invalid_argument("Invalid layout orientation: " + orientation_str);
            }
        }
        detail::configure_docraft_node_attributes(layout_node, craft_language_source);
        return layout_node;
    }
}
