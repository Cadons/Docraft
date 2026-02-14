#include "craft/parser/docraft_parser.h"

#include "craft/parser/docraft_parser_helpers.h"
#include "model/docraft_image.h"

namespace docraft::craft::parser {
    std::shared_ptr<model::DocraftNode> DocraftImageParser::parse(const pugi::xml_node &craft_language_source) {
        auto image_node = std::make_shared<model::DocraftImage>();
        if (auto src_attr = craft_language_source.attribute(elements::image::attribute::kSrc.data())) {
            image_node->set_path(src_attr.as_string());
        }
        detail::configure_docraft_node_attributes(image_node, craft_language_source);
        return image_node;
    }
}
