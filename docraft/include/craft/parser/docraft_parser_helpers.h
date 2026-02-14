#pragma once

#include <pugixml.hpp>

#include "craft/docraft_craft_language_tokens.h"
#include "docraft_color.h"
#include "model/docraft_node.h"
#include "model/docraft_section.h"
#include "model/docraft_settings.h"

namespace docraft::craft::parser::detail {
    bool is_hex_color(const std::string &color);

    DocraftColor get_docraft_color(const pugi::xml_attribute &color_attr);

    void add_external_fonts_from_node(const pugi::xml_node &font_node,
                                      const char *child_name,
                                      model::setting::DocraftFont &docraft_font);

    void configure_docraft_node_attributes(const std::shared_ptr<model::DocraftNode> &node,
                                           const pugi::xml_node &craft_language_source);

    void configure_section_attributes(const std::shared_ptr<model::DocraftSection> &node,
                                      const pugi::xml_node &craft_language_source);
}
