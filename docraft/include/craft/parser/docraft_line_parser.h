#pragma once

#include "docraft_lib.h"

#include "craft/i_docraft_parser.h"

namespace docraft::craft::parser {
    /**
     * @brief Parser for line nodes.
     */
    class DOCRAFT_LIB DocraftLineParser : public IDocraftParser {
    public:
        /**
         * @brief Parses a line XML node.
         * @param craft_language_source XML node.
         * @return Parsed line node.
         */
        std::shared_ptr<model::DocraftNode> parse(const pugi::xml_node &craft_language_source) override;
    };
} // docraft::craft::parser
