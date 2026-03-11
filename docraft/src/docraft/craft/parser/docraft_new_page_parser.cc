#include "docraft/craft/parser/docraft_parser.h"

#include "docraft/craft/parser/docraft_parser_helpers.h"
#include "docraft/model/docraft_new_page.h"

namespace docraft::craft::parser {
    std::shared_ptr<model::DocraftNode> DocraftNewPageParser::parse(const pugi::xml_node &craft_language_source) {
        auto page_break = std::make_shared<model::DocraftNewPage>();
        detail::configure_docraft_node_attributes(page_break, craft_language_source);
        return page_break;
    }
} // namespace docraft::craft::parser
