#pragma once
#include <pugixml.hpp>

#include "model/docraft_node.h"

namespace docraft::craft {
        class IDocraftParser {
        public:
                virtual ~IDocraftParser() = default;
                virtual std::shared_ptr<model::DocraftNode> parse(const pugi::xml_node &craft_language_source) = 0;
        };
} // docraft
