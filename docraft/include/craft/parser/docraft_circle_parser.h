#pragma once

#include "craft/i_docraft_parser.h"

namespace docraft::craft::parser {
    /**
     * @brief Parser for circle nodes.
     */
    class DocraftCircleParser : public IDocraftParser {
    public:
        /**
         * @brief Parses a circle XML node.
         * @param craft_language_source XML node.
         * @return Parsed circle node.
         */
        std::shared_ptr<model::DocraftNode> parse(const pugi::xml_node &craft_language_source) override;
    };
} // docraft::craft::parser
