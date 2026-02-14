#include "craft/parser/docraft_parser.h"

#include "craft/parser/docraft_parser_helpers.h"
#include "model/docraft_body.h"
#include "model/docraft_footer.h"
#include "model/docraft_header.h"

namespace docraft::craft::parser {
    std::shared_ptr<model::DocraftNode> DocraftHeaderParser::parse(const pugi::xml_node &craft_language_source) {
        auto header_node = std::make_shared<model::DocraftHeader>();
        const auto rect = std::dynamic_pointer_cast<model::DocraftRectangle>(
                DocraftRectangleParser::parse(craft_language_source));
        apply_attributes_to(rect, header_node);
        detail::configure_section_attributes(header_node, craft_language_source);
        return header_node;
    }

    std::shared_ptr<model::DocraftNode> DocraftBodyParser::parse(const pugi::xml_node &craft_language_source) {
        auto body_node = std::make_shared<model::DocraftBody>();
        const auto rect = std::dynamic_pointer_cast<model::DocraftRectangle>(
                DocraftRectangleParser::parse(craft_language_source));
        apply_attributes_to(rect, body_node);
        detail::configure_section_attributes(body_node, craft_language_source);
        return body_node;
    }

    std::shared_ptr<model::DocraftNode> DocraftFooterParser::parse(const pugi::xml_node &craft_language_source) {
        auto footer_node = std::make_shared<model::DocraftFooter>();
        const auto rect = std::dynamic_pointer_cast<model::DocraftRectangle>(
                DocraftRectangleParser::parse(craft_language_source));
        apply_attributes_to(rect, footer_node);
        detail::configure_section_attributes(footer_node, craft_language_source);
        return footer_node;
    }
}
