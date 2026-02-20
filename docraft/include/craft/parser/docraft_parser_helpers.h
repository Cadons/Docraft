#pragma once

#include <pugixml.hpp>

#include "craft/docraft_craft_language_tokens.h"
#include "docraft_color.h"
#include "model/docraft_node.h"
#include "model/docraft_section.h"
#include "model/docraft_settings.h"

namespace docraft::craft::parser::detail {
    /**
     * @brief Parses a color attribute value into a DocraftColor.
     * Supports hex colors (#RRGGBB or #RRGGBBAA) and named colors.
     * @param color_attr XML attribute containing the color value.
     * @return Parsed DocraftColor.
     */
    bool is_hex_color(const std::string &color);

    /**
     * @brief Parses a color string into a DocraftColor object.
     * Supports hex colors (e.g., #RRGGBB or #RRGGBBAA) and named colors (e.g., "red", "blue").
     * @param color_str The color string to parse.
     * @return A DocraftColor object representing the parsed color.
     * @throws std::invalid_argument if the color string is not in a valid format or is an unsupported named color.
     */
    DocraftColor get_docraft_color(const pugi::xml_attribute &color_attr);

    /**
     * @brief Adds external fonts defined in the XML node to the DocraftFont object.
     * Expects child nodes with the specified name containing font information.
     * @param font_node XML node containing external font definitions.
     * @param child_name Name of the child nodes that define external fonts.
     * @param docraft_font DocraftFont object to which the external fonts will be added.
     */
    void add_external_fonts_from_node(const pugi::xml_node &font_node,
                                      const char *child_name,
                                      model::setting::DocraftFont &docraft_font);

    /**
     * @brief Configures common attributes for a DocraftNode based on the XML node.
     * This includes attributes like id, visibility, and any other common properties.
     * @param node The DocraftNode to configure.
     * @param craft_language_source The XML node containing the attributes to apply.
     */
    void configure_docraft_node_attributes(const std::shared_ptr<model::DocraftNode> &node,
                                           const pugi::xml_node &craft_language_source);

    /**
     * @brief Configures section-specific attributes for a DocraftSection based on the XML node.
     * This includes attributes like ratio, background color, and any other section-specific properties.
     * @param node The DocraftSection to configure.
     * @param craft_language_source The XML node containing the attributes to apply.
     */
    void configure_section_attributes(const std::shared_ptr<model::DocraftSection> &node,
                                      const pugi::xml_node &craft_language_source);
}
