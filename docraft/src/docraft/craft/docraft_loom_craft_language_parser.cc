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

#include "docraft/craft/docraft_loom_craft_language_parser.h"

#include <any>
#include <optional>
#include <string>

#include <pugixml.hpp>

#include "docraft/craft/docraft_craft_language_parser.h"
#include "docraft/craft/docraft_craft_language_tokens.h"
#include "docraft/craft/parser/docraft_section_parsers.h"
#include "docraft/exception/docraft_exceptions.h"
#include "docraft/loom/craft/docraft_loom_tree_builder.h"

namespace docraft::craft {
    namespace {
        loom::DocraftLoomPdfCreator::Margins to_margins(const parser::ParsedSectionData& data)
        {
            loom::DocraftLoomPdfCreator::Margins margins;
            if (data.margin_top)
            {
                margins.top = *data.margin_top;
            }
            if (data.margin_bottom)
            {
                margins.bottom = *data.margin_bottom;
            }
            if (data.margin_left)
            {
                margins.left = *data.margin_left;
            }
            if (data.margin_right)
            {
                margins.right = *data.margin_right;
            }
            return margins;
        }

        docraft::backend::DocraftPageSize parse_page_size(const std::string& size_str)
        {
            if (size_str == "A3" || size_str == "a3")
            {
                return docraft::backend::DocraftPageSize::kA3;
            }
            if (size_str == "A5" || size_str == "a5")
            {
                return docraft::backend::DocraftPageSize::kA5;
            }
            if (size_str == "Letter" || size_str == "letter")
            {
                return docraft::backend::DocraftPageSize::kLetter;
            }
            if (size_str == "Legal" || size_str == "legal")
            {
                return docraft::backend::DocraftPageSize::kLegal;
            }
            if (size_str == "A4" || size_str == "a4")
            {
                return docraft::backend::DocraftPageSize::kA4;
            }
            throw docraft::exception::InvalidInputException("Invalid page_size: " + size_str);
        }

        docraft::backend::DocraftPageOrientation parse_page_orientation(const std::string& orientation_str)
        {
            if (orientation_str == "landscape")
            {
                return docraft::backend::DocraftPageOrientation::kLandscape;
            }
            if (orientation_str == "portrait")
            {
                return docraft::backend::DocraftPageOrientation::kPortrait;
            }
            throw docraft::exception::InvalidInputException("Invalid page_orientation: " + orientation_str);
        }

        void apply_page(const pugi::xml_node& page_node, loom::DocraftLoomPdfCreator& creator)
        {
            const auto size_attr = page_node.attribute(elements::settings::page::attribute::kSize.data());
            const auto orientation_attr =
                page_node.attribute(elements::settings::page::attribute::kOrientation.data());
            const auto size = size_attr
                                  ? parse_page_size(size_attr.as_string())
                                  : docraft::backend::DocraftPageSize::kA4;
            const auto orientation = orientation_attr
                                         ? parse_page_orientation(orientation_attr.as_string())
                                         : docraft::backend::DocraftPageOrientation::kPortrait;
            creator.set_page_format(size, orientation);
        }

        void apply_section_ratios(const pugi::xml_node& ratios_node, loom::DocraftLoomPdfCreator& creator)
        {
            const auto header_attr =
                ratios_node.attribute(elements::settings::section_ratios::attribute::kHeaderRatio.data());
            const auto body_attr =
                ratios_node.attribute(elements::settings::section_ratios::attribute::kBodyRatio.data());
            const auto footer_attr =
                ratios_node.attribute(elements::settings::section_ratios::attribute::kFooterRatio.data());

            const float header_ratio = header_attr ? header_attr.as_float() : creator.header_ratio();
            const float body_ratio = body_attr ? body_attr.as_float() : creator.body_ratio();
            const float footer_ratio = footer_attr ? footer_attr.as_float() : creator.footer_ratio();
            if (header_ratio < 0.0F || body_ratio < 0.0F || footer_ratio < 0.0F)
            {
                throw docraft::exception::InvalidInputException("Section ratios must be non-negative");
            }
            if (header_ratio + body_ratio + footer_ratio > 1.0F + 1e-6F)
            {
                throw docraft::exception::InvalidInputException("Section ratios must sum to 1.0 or less");
            }
            creator.set_section_ratios(header_ratio, body_ratio, footer_ratio);
        }

        // Returns the `src` of a `<Font>`'s variant child (FontNormal/FontBold/
        // FontItalic/FontBoldItalic), or nullopt if that variant isn't present.
        std::optional<std::string> font_variant_src(const pugi::xml_node& font_node, const std::string& variant_tag)
        {
            const auto variant_node = font_node.child(variant_tag.c_str());
            if (!variant_node)
            {
                return std::nullopt;
            }
            const auto src_attr = variant_node.attribute(
                elements::settings::fonts::font_type::attribute::kSrc.data());
            if (!src_attr)
            {
                throw docraft::exception::InvalidInputException(
                    "<" + variant_tag + "> requires a '" +
                    std::string{elements::settings::fonts::font_type::attribute::kSrc} + "' attribute");
            }
            return std::string{src_attr.as_string()};
        }

        void apply_fonts(const pugi::xml_node& fonts_node, loom::DocraftLoomPdfCreator& creator)
        {
            for (const pugi::xml_node font_node : fonts_node.children())
            {
                if (std::string{font_node.name()} != std::string{elements::settings::fonts::kFont})
                {
                    throw docraft::exception::InvalidInputException(
                        "<" + std::string{font_node.name()} + "> cannot be placed in <Fonts>");
                }
                const auto name_attr = font_node.attribute(elements::settings::fonts::attribute::kName.data());
                if (!name_attr)
                {
                    throw docraft::exception::InvalidInputException(
                        "<" + std::string{elements::settings::fonts::kFont} + "> requires a '" +
                        std::string{elements::settings::fonts::attribute::kName} + "' attribute");
                }

                const auto normal_src = font_variant_src(font_node, std::string{
                                                             elements::settings::fonts::font_type::kFontNormal
                                                         });
                const auto bold_src = font_variant_src(font_node, std::string{
                                                           elements::settings::fonts::font_type::kFontBold
                                                       });
                const auto italic_src = font_variant_src(font_node, std::string{
                                                             elements::settings::fonts::font_type::kFontItalic
                                                         });
                const auto bold_italic_src = font_variant_src(
                    font_node, std::string{elements::settings::fonts::font_type::kFontBoldItalic});
                if (!normal_src && !bold_src && !italic_src && !bold_italic_src)
                {
                    throw docraft::exception::InvalidInputException(
                        "<" + std::string{elements::settings::fonts::kFont} + "> requires at least one of "
                        "FontNormal/FontBold/FontItalic/FontBoldItalic");
                }

                creator.register_font(name_attr.as_string(), normal_src, bold_src, italic_src, bold_italic_src);
            }
        }

        // Reads <Settings><Fonts default="..."> (if present), the family name applied to
        // Text/Title/Subtitle/PageNumber/table-cell nodes that don't specify their own
        // `font_name`. Must run -- and its result be handed to the tree builder -- before
        // build_from_document() builds any of Header/Body/Footer, since that's the only
        // point where each node's font_family is decided.
        std::optional<std::string> extract_default_font(const pugi::xml_node& settings_node)
        {
            if (!settings_node)
            {
                return std::nullopt;
            }
            const auto fonts_node = settings_node.child(elements::settings::kFonts.data());
            if (!fonts_node)
            {
                return std::nullopt;
            }
            const auto default_attr = fonts_node.attribute(
                elements::settings::fonts::attribute::kDefault.data());
            if (!default_attr)
            {
                return std::nullopt;
            }
            const std::string default_font = default_attr.as_string();
            if (default_font.empty())
            {
                throw docraft::exception::InvalidInputException(
                    "<" + std::string{elements::settings::kFonts} + "> 'default' attribute must not be empty");
            }
            return default_font;
        }

        // Applies a <Metadata> element's simple string subtags to the creator. Only the
        // plain text fields are wired -- CreationDate/ModificationDate/Trapped/GtsPdfx/
        // AutoKeywords are left unrecognized (silently skipped) for now.
        void apply_metadata(const pugi::xml_node& metadata_node, loom::DocraftLoomPdfCreator& creator)
        {
            DocraftDocumentMetadata metadata;
            for (const pugi::xml_node child : metadata_node.children())
            {
                const std::string tag = child.name();
                const std::string value = child.child_value();
                if (tag == std::string{elements::metadata::kDocumentTitle})
                {
                    metadata.set_title(value);
                }
                else if (tag == std::string{elements::metadata::kAuthor})
                {
                    metadata.set_author(value);
                }
                else if (tag == std::string{elements::metadata::kCreator})
                {
                    metadata.set_creator(value);
                }
                else if (tag == std::string{elements::metadata::kProducer})
                {
                    metadata.set_producer(value);
                }
                else if (tag == std::string{elements::metadata::kSubject})
                {
                    metadata.set_subject(value);
                }
                else if (tag == std::string{elements::metadata::kKeywords})
                {
                    metadata.set_keywords(value);
                }
            }
            creator.set_metadata(metadata);
        }

        // Applies a <Settings> element's <Page>/<SectionRatios>/<Fonts> children (if
        // present) to the creator.
        void apply_settings(const pugi::xml_node& settings_node, loom::DocraftLoomPdfCreator& creator)
        {
            if (settings_node.first_attribute())
            {
                throw docraft::exception::InvalidInputException(
                    "<Settings> does not accept attributes directly -- use its <Page>/<SectionRatios>/<Fonts> "
                    "sub-tags instead");
            }

            bool seen_page = false;
            bool seen_section_ratios = false;
            bool seen_fonts = false;
            for (const pugi::xml_node child : settings_node.children())
            {
                const std::string tag = child.name();
                if (tag == std::string{elements::settings::kPage})
                {
                    if (seen_page)
                    {
                        throw docraft::exception::InvalidInputException("<Page> may only appear once in <Settings>");
                    }
                    seen_page = true;
                    apply_page(child, creator);
                }
                else if (tag == std::string{elements::settings::kSectionRatios})
                {
                    if (seen_section_ratios)
                    {
                        throw docraft::exception::InvalidInputException(
                            "<SectionRatios> may only appear once in <Settings>");
                    }
                    seen_section_ratios = true;
                    apply_section_ratios(child, creator);
                }
                else if (tag == std::string{elements::settings::kFonts})
                {
                    if (seen_fonts)
                    {
                        throw docraft::exception::InvalidInputException("<Fonts> may only appear once in <Settings>");
                    }
                    seen_fonts = true;
                    apply_fonts(child, creator);
                }
                else
                {
                    throw docraft::exception::InvalidInputException("<" + tag + "> cannot be placed in <Settings>");
                }
            }
        }
    } // namespace

    void DocraftLoomCraftLanguageParser::parse(const std::string& xml_string)
    {
        pugi::xml_document doc;
        const pugi::xml_parse_result result = doc.load_string(xml_string.c_str());
        if (!result)
        {
            throw docraft::exception::DataFormatException(
                "Error parsing .craft content: " + std::string(result.description()));
        }
        build_from_document(doc.first_child());
    }

    void DocraftLoomCraftLanguageParser::load_from_file(const std::filesystem::path& path)
    {
        pugi::xml_document doc;
        const pugi::xml_parse_result result = doc.load_file(path.c_str());
        if (!result)
        {
            throw docraft::exception::DataFormatException(
                "Error loading .craft file: " + std::string(result.description()));
        }
        build_from_document(doc.first_child());
    }

    std::shared_ptr<loom::DocraftLoomPdfCreator> DocraftLoomCraftLanguageParser::edit_creator()
    {
        return creator_;
    }

    void DocraftLoomCraftLanguageParser::set_template_engine(
        std::shared_ptr<docraft::templating::DocraftTemplateEngine> template_engine)
    {
        template_engine_ = std::move(template_engine);
    }

    void DocraftLoomCraftLanguageParser::build_from_document(const pugi::xml_node& document_node)
    {
        if (!document_node || std::string{document_node.name()} != std::string{section::kDocument})
        {
            throw docraft::exception::DataFormatException(
                "Invalid .craft content: missing required <" + std::string{section::kDocument} + "> root element");
        }

        DocraftCraftLanguageParser craft_parser;
        loom::craft::DocraftLoomTreeBuilder tree_builder(template_engine_);

        std::shared_ptr<DocraftParsedElement> header_element;
        std::shared_ptr<DocraftParsedElement> body_element;
        std::shared_ptr<DocraftParsedElement> footer_element;
        pugi::xml_node settings_node;
        pugi::xml_node metadata_node;

        // Only Header/Body/Footer/Settings/Metadata are recognized at this top level --
        // anything else is deliberately skipped rather than parsed (out of scope for this
        // phase; see the class-level doc comment). Foreach/NewPage are recognized inside
        // Header/Body/Footer's subtree (registered per-tag parsers), not here.
         std::unordered_map<std::string_view, bool> seen_tags={
            {section::kHeader, false},
            {section::kBody, false},
            {section::kFooter, false},
            {elements::kSettings, false},
            {section::kMetadata, false}
        };//These elements can be place one time only

        for (pugi::xml_node child : document_node.children())
        {
            if (child.type() != pugi::node_element)
            {
                continue;
            }
            const std::string tag = child.name();
            for (const auto& [seen_tag, seen] : seen_tags)
            {
                if (tag == std::string{seen_tag} && seen)
                {
                    throw docraft::exception::InvalidInputException(
                        "Duplicate top-level element <" + tag + "> in <" + std::string{section::kDocument} + ">, only one is allowed");
                }
            }
            if (tag == std::string{section::kHeader})
            {
                header_element = craft_parser.parse_node(child);
                seen_tags.at(section::kHeader) = true;
            }
            else if (tag == std::string{section::kBody})
            {
                body_element = craft_parser.parse_node(child);
                seen_tags.at(section::kBody) = true;
            }
            else if (tag == std::string{section::kFooter})
            {
                footer_element = craft_parser.parse_node(child);
                seen_tags.at(section::kFooter) = true;
            }
            else if (tag == std::string{elements::kSettings})
            {
                settings_node = child;
                seen_tags.at(elements::kSettings) = true;
            }
            else if (tag == std::string{section::kMetadata})
            {
                metadata_node = child;
                seen_tags.at(section::kMetadata) = true;
            }else {
                throw docraft::exception::InvalidInputException(std::format("Unknown top-level element <{}> in <{}>", tag, section::kDocument));
            }
        }

        if (!body_element)
        {
            throw docraft::exception::InvalidInputException(
                "Invalid .craft document: missing required <" + std::string{section::kBody} + "> element");
        }

        if (const auto default_font = extract_default_font(settings_node))
        {
            tree_builder.set_default_font_family(*default_font);
        }

        auto body_node = tree_builder.build(body_element);
        creator_ = std::make_shared<loom::DocraftLoomPdfCreator>(body_node);
        creator_->set_body_margins(
            to_margins(std::any_cast<const parser::ParsedSectionData&>(body_element->data)));

        if (header_element)
        {
            creator_->set_header(tree_builder.build(header_element));
            creator_->set_header_margins(
                to_margins(std::any_cast<const parser::ParsedSectionData&>(header_element->data)));
        }
        if (footer_element)
        {
            creator_->set_footer(tree_builder.build(footer_element));
            creator_->set_footer_margins(
                to_margins(std::any_cast<const parser::ParsedSectionData&>(footer_element->data)));
        }

        if (settings_node)
        {
            apply_settings(settings_node, *creator_);
        }
        if (metadata_node)
        {
            apply_metadata(metadata_node, *creator_);
        }
    }
} // namespace docraft::craft
