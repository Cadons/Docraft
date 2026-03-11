#include "docraft/craft/parser/docraft_parser.h"

#include "docraft/craft/parser/docraft_parser_helpers.h"
#include "docraft/model/docraft_blank_line.h"

namespace docraft::craft::parser {
    std::shared_ptr<model::DocraftNode> DocraftBlackLineParser::parse(const pugi::xml_node &craft_language_source) {
        auto blank = std::make_shared<model::DocraftBlankLine>();
        detail::configure_docraft_node_attributes(blank, craft_language_source);
        return blank;
    }
}
