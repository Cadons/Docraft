#pragma once
#include <pugixml.hpp>

#include "model/docraft_node.h"

namespace docraft::craft {
        /**
         * @brief Interface for Craft language node parsers.
         *
         * Implementations translate a single XML node into a Docraft model node.
         */
        class IDocraftParser {
        public:
                /**
                 * @brief Virtual destructor.
                 */
                virtual ~IDocraftParser() = default;
                /**
                 * @brief Parses an XML node into a Docraft node.
                 * @param craft_language_source XML node to parse.
                 * @return Parsed Docraft node.
                 */
                virtual std::shared_ptr<model::DocraftNode> parse(const pugi::xml_node &craft_language_source) = 0;
        };
} // docraft
