#pragma once

#include "craft/i_docraft_parser.h"

namespace docraft::craft::parser {
    /**
     * @brief Parser for triangle nodes.
     */
    class DocraftTriangleParser : public IDocraftParser {
    public:
        /**
         * @brief Parses a triangle XML node.
         * @param craft_language_source XML node.
         * @return Parsed triangle node.
         */
        std::shared_ptr<model::DocraftNode> parse(const pugi::xml_node &craft_language_source) override;
    };
} // docraft::craft::parser
