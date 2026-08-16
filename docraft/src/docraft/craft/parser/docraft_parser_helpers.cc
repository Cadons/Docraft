/*
 * Copyright 2026 Matteo Cadoni (https://github.com/cadons)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "docraft/craft/parser/docraft_parser_helpers.h"
#include <vector>
#include <string_view>
#include <array>
#include <algorithm>
#include <format>
#include <ranges>

#include <cctype>

#include "docraft/exception/docraft_exceptions.h"

namespace {
    // These nodes derive their extent from their contents or coordinates. Although
    // width/height are common syntactically, their current layout implementations
    // have no explicit-size semantics, so accepting them would be a silent no-op.
    constexpr std::array<std::string_view, 5> kContentOrCoordinateSized = {
        docraft::craft::elements::kList, docraft::craft::elements::kParagraph,
        docraft::craft::elements::kLine, docraft::craft::elements::kCurveLine,
        docraft::craft::elements::kTable};

    void reject_unsupported_explicit_size(std::string_view name, const std::string& tag_name)
    {
        const bool is_explicit_size = name == docraft::craft::basic::attribute::kWidth ||
            name == docraft::craft::basic::attribute::kHeight;
        if (!is_explicit_size ||
            std::ranges::find(kContentOrCoordinateSized, tag_name) == kContentOrCoordinateSized.end())
        {
            return;
        }
        throw docraft::exception::InvalidInputException(std::format(
            "Attribute '{}' is not supported on <{}>; that element's size is determined by {}",
            name, tag_name,
            tag_name == docraft::craft::elements::kLine || tag_name == docraft::craft::elements::kCurveLine
                ? "its coordinates"
                : "its content"));
    }

    // `weight` is deliberately not part of kCommon: unlike x/y/padding/etc. it only
    // means anything on a direct child of <Layout> (DocraftLoomTreeBuilder collects it
    // there to build the parent HStack/VStack's weights()) -- on any other tag it would
    // silently no-op, so it's rejected outright instead. Returns true once `name` has
    // been fully handled as the `weight` attribute (accepted or rejected), so the caller
    // knows to move on to the next attribute without falling through to the generic
    // accepted/common checks.
    bool handle_weight_attribute(const pugi::xml_node& craft_language_source, std::string_view name,
                                  const std::string& tag_name)
    {
        if (name != docraft::craft::basic::attribute::kWeight)
        {
            return false;
        }
        const pugi::xml_node parent = craft_language_source.parent();
        const bool parent_is_layout =
            parent && std::string_view{parent.name()} == docraft::craft::elements::kLayout;
        if (!parent_is_layout)
        {
            throw docraft::exception::InvalidInputException(std::format(
                "Attribute 'weight' is not supported on <{}>; weight only applies to a "
                "direct child of <Layout>", tag_name));
        }
        return true;
    }

    [[noreturn]] void throw_unknown_attribute(std::string_view name, const std::string& tag_name,
                                               const std::vector<std::string_view>& accepted, bool allow_common)
    {
        std::string known;
        for (const std::string_view candidate : accepted)
        {
            known += known.empty() ? "" : ", ";
            known += candidate;
        }
        throw docraft::exception::InvalidInputException(std::format(
            "Unknown attribute '{}' on <{}>. That element accepts {}{}", name, tag_name,
            known.empty() ? std::string{"only the common attributes"} : std::format("'{}'", known),
            allow_common ? ", plus the common attributes (name, x, y, width, height, padding, margin*, "
                           "position, z_index, visible)"
                         : " and nothing else"));
    }
} // namespace

namespace docraft::craft::parser::detail {
    bool is_hex_color(const std::string &color) {
        if (color.size() != 7 && color.size() != 9) {
            return false;
        }
        if (color[0] != '#') {
            return false;
        }
        for (size_t i = 1; i < color.size(); ++i) {
            if (!std::isxdigit(static_cast<unsigned char>(color[i]))) {
                return false;
            }
        }
        return true;
    }

    std::string get_color_attribute_raw(const pugi::xml_attribute &color_attr) {
        std::string color_name_str = color_attr.as_string();
        if (color_name_str.empty()) {
            throw docraft::exception::InvalidInputException("Color attribute cannot be empty");
        }
        return color_name_str;
    }

    DocraftColor parse_docraft_color(const std::string &color_name_str) {
        if (color_name_str[0] == '#') {
            if (!is_hex_color(color_name_str)) {
                throw docraft::exception::InvalidInputException("Invalid hex color: " + color_name_str);
            }
            return DocraftColor(color_name_str);
        }

        if (color_name_str == std::string{color::kBlack}) {
            return DocraftColor(ColorName::kBlack);
        } else if (color_name_str == std::string{color::kWhite}) {
            return DocraftColor(ColorName::kWhite);
        } else if (color_name_str == std::string{color::kRed}) {
            return DocraftColor(ColorName::kRed);
        } else if (color_name_str == std::string{color::kGreen}) {
            return DocraftColor(ColorName::kGreen);
        } else if (color_name_str == std::string{color::kBlue}) {
            return DocraftColor(ColorName::kBlue);
        } else if (color_name_str == std::string{color::kYellow}) {
            return DocraftColor(ColorName::kYellow);
        } else if (color_name_str == std::string{color::kCyan}) {
            return DocraftColor(ColorName::kCyan);
        } else if (color_name_str == std::string{color::kMagenta}) {
            return DocraftColor(ColorName::kMagenta);
        } else if (color_name_str == std::string{color::kPurple}) {
            return DocraftColor(ColorName::kPurple);
        }

        // Naming the accepted set matters here: the palette is deliberately small, so a
        // plausible-looking name ("orange", "grey") fails, and a bare "unknown color"
        // gives the author no way to tell whether they misspelled a name or asked for
        // one that never existed.
        throw docraft::exception::InvalidInputException(
            "Unknown color: '" + color_name_str + "'. Use a #RRGGBB hex value, or one of: "
            "black, white, red, green, blue, yellow, cyan, magenta, purple");
    }

    void validate_attributes(const pugi::xml_node& craft_language_source, const std::string& tag_name,
                             const std::vector<std::string_view>& accepted, bool allow_common)
    {
        // Exactly the names parse_common_node_attributes() below reads -- deliberately
        // not "everything in basic::attribute", which also holds `color` (only some tags
        // read it) and the position *values*, neither of which every element accepts.
        static constexpr std::array<std::string_view, 14> kCommon = {
            basic::attribute::kNodeName, basic::attribute::kX, basic::attribute::kY,
            basic::attribute::kWidth, basic::attribute::kHeight, basic::attribute::kPadding,
            basic::attribute::kMargin, basic::attribute::kMarginTop, basic::attribute::kMarginRight,
            basic::attribute::kMarginBottom, basic::attribute::kMarginLeft,
            basic::attribute::kPosition, basic::attribute::kZIndex, basic::attribute::kVisible};

        for (const auto& attribute : craft_language_source.attributes())
        {
            const std::string_view name = attribute.name();
            reject_unsupported_explicit_size(name, tag_name);

            if (handle_weight_attribute(craft_language_source, name, tag_name))
            {
                continue;
            }
            // Table sub-elements (Row/Cell/HTitle/VTitle) are not laid out as nodes of
            // their own, so the common positioning attributes mean nothing on them and
            // are not silently tolerated either.
            if (allow_common && std::ranges::find(kCommon, name) != kCommon.end())
            {
                continue;
            }
            if (std::ranges::find(accepted, name) != accepted.end())
            {
                continue;
            }
            throw_unknown_attribute(name, tag_name, accepted, allow_common);
        }
    }

    DocraftCommonAttributes parse_common_node_attributes(const pugi::xml_node& craft_language_source)
    {
        DocraftCommonAttributes common;
        bool has_position_attr = false;
        bool has_xy = false;

        if (auto name_attr = craft_language_source.attribute(basic::attribute::kNodeName.data()))
        {
            common.name = name_attr.as_string();
        }
        if (auto x_attr = craft_language_source.attribute(basic::attribute::kX.data()))
        {
            common.x = x_attr.as_float();
            has_xy = true;
        }
        if (auto y_attr = craft_language_source.attribute(basic::attribute::kY.data()))
        {
            common.y = y_attr.as_float();
            has_xy = true;
        }
        if (auto width_attr = craft_language_source.attribute(basic::attribute::kWidth.data()))
        {
            common.width = width_attr.as_float();
        }
        if (auto height_attr = craft_language_source.attribute(basic::attribute::kHeight.data()))
        {
            common.height = height_attr.as_float();
        }
        if (auto padding_attr = craft_language_source.attribute(basic::attribute::kPadding.data()))
        {
            common.padding = padding_attr.as_float();
        }
        if (auto margin_attr = craft_language_source.attribute(basic::attribute::kMargin.data()))
        {
            common.margin = margin_attr.as_float();
        }
        if (auto margin_top_attr = craft_language_source.attribute(basic::attribute::kMarginTop.data()))
        {
            common.margin_top = margin_top_attr.as_float();
        }
        if (auto margin_right_attr = craft_language_source.attribute(basic::attribute::kMarginRight.data()))
        {
            common.margin_right = margin_right_attr.as_float();
        }
        if (auto margin_bottom_attr = craft_language_source.attribute(basic::attribute::kMarginBottom.data()))
        {
            common.margin_bottom = margin_bottom_attr.as_float();
        }
        if (auto margin_left_attr = craft_language_source.attribute(basic::attribute::kMarginLeft.data()))
        {
            common.margin_left = margin_left_attr.as_float();
        }
        if (auto position_attr = craft_language_source.attribute(basic::attribute::kPosition.data()))
        {
            const std::string position_str = position_attr.as_string();
            has_position_attr = true;
            if (position_str == std::string{basic::attribute::position_type::kBlock})
            {
                common.position_mode = PositionMode::kBlock;
            } else if (position_str == std::string{basic::attribute::position_type::kAbsolute})
            {
                common.position_mode = PositionMode::kAbsolute;
            } else {
                throw docraft::exception::InvalidInputException("Invalid position value: " + position_str);
            }
        }
        if (has_xy && !has_position_attr)
        {
            // Explicit coordinates with no explicit "position" attribute imply absolute
            // placement, mirroring the legacy parser's behavior.
            common.position_mode = PositionMode::kAbsolute;
        }
        if (auto weight_attr = craft_language_source.attribute(basic::attribute::kWeight.data())) {
            common.weight = weight_attr.as_float();
        }
        if (auto z_index_attr = craft_language_source.attribute(basic::attribute::kZIndex.data())) {
            common.z_index = z_index_attr.as_int();
        }
        if (auto visible_attr = craft_language_source.attribute(basic::attribute::kVisible.data()))
        {
            common.visible = visible_attr.as_bool();
        }

        return common;
    }

    ParsedTextStyle parse_text_style(const std::string& style_str) {
        if (style_str == std::string{style::kBold}) {
            return ParsedTextStyle::kBold;
        }
        if (style_str == std::string{style::kItalic}) {
            return ParsedTextStyle::kItalic;
        }
        if (style_str == std::string{style::kBoldItalic}) {
            return ParsedTextStyle::kBoldItalic;
        }
        if (style_str == std::string{style::kNormal}) {
            return ParsedTextStyle::kNormal;
        }
        throw docraft::exception::InvalidInputException("Invalid text style: " + style_str);
    }

    ParsedTextAlignment parse_text_alignment(const std::string& alignment_str) {
        if (alignment_str == std::string{alignment::kCenter}) {
            return ParsedTextAlignment::kCenter;
        }
        if (alignment_str == std::string{alignment::kRight}) {
            return ParsedTextAlignment::kRight;
        }
        if (alignment_str == std::string{alignment::kJustified}) {
            return ParsedTextAlignment::kJustified;
        }
        if (alignment_str == std::string{alignment::kLeft}) {
            return ParsedTextAlignment::kLeft;
        }
        throw docraft::exception::InvalidInputException("Invalid text alignment: " + alignment_str);
    }

    std::string trim_whitespace(const std::string& text) {
        constexpr auto whitespace = " \t\n\r\f\v";

        const auto first = text.find_first_not_of(whitespace);
        if (first == std::string::npos) {
            return {};
        }

        const auto last = text.find_last_not_of(whitespace);
        return text.substr(first, last - first + 1);
    }
} // namespace docraft::craft::parser::detail
