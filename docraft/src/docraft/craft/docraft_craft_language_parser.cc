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

// cpp
#include "docraft/craft/docraft_craft_language_parser.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

#include "docraft/craft/docraft_craft_language_tokens.h"
#include "docraft/craft/parser/docraft_parser.h"
#include "docraft/model/docraft_header.h"
#include "docraft/model/docraft_body.h"
#include "docraft/model/docraft_footer.h"
#include "docraft/model/docraft_children_container_node.h"
#include "docraft/model/docraft_foreach.h"
#include "docraft/model/docraft_list.h"
#include "docraft/model/docraft_new_page.h"
#include "docraft/model/docraft_text.h"
#include "docraft/utils/docraft_keyword_extractor.h"
#include "docraft/utils/docraft_logger.h"

namespace {
    struct DocraftAutoKeywordConfig {
        bool enabled = false;
        std::size_t max_keywords = 10;
        std::size_t min_length = 4;
        std::vector<std::string> stop_word_languages = {"it", "en", "fr", "de", "es"};
    };

    struct DocraftMetadataParseOutcome {
        docraft::DocraftDocumentMetadata metadata;
        DocraftAutoKeywordConfig auto_keyword_config;
        bool has_metadata = false;
    };

    std::string trim_copy(std::string_view source) {
        std::size_t start = 0;
        std::size_t end = source.size();
        while (start < end && std::isspace(static_cast<unsigned char>(source[start]))) {
            ++start;
        }
        while (end > start && std::isspace(static_cast<unsigned char>(source[end - 1]))) {
            --end;
        }
        return std::string(source.substr(start, end - start));
    }

    std::string normalize_tag_name(const std::string &tag_name) {
        if (tag_name.empty()) {
            return tag_name;
        }
        bool has_upper = false;
        for (const char c: tag_name) {
            if (std::isupper(static_cast<unsigned char>(c))) {
                has_upper = true;
                break;
            }
        }
        if (has_upper) {
            return tag_name;
        }
        std::string normalized = tag_name;
        for (char &c: normalized) {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
        normalized[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(normalized[0])));
        return normalized;
    }

    std::optional<std::string> read_child_text(const pugi::xml_node &parent, const std::string &child_tag_name) {
        if (!parent) {
            return std::nullopt;
        }
        if (const pugi::xml_node child = parent.child(child_tag_name.c_str())) {
            const std::string value = trim_copy(child.child_value());
            if (!value.empty()) {
                return value;
            }
        }
        return std::nullopt;
    }

    std::optional<std::string> read_attr_or_child_text(
        const pugi::xml_node &parent,
        const std::string &attribute_name,
        const std::string &child_tag_name) {
        if (!parent) {
            return std::nullopt;
        }
        if (const pugi::xml_attribute attr = parent.attribute(attribute_name.c_str())) {
            const std::string value = trim_copy(attr.as_string());
            if (!value.empty()) {
                return value;
            }
        }
        return read_child_text(parent, child_tag_name);
    }

    bool parse_bool_string(const std::string &raw_value, const std::string &error_context) {
        std::string value = raw_value;
        for (char &ch: value) {
            ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
        }
        if (value == "true" || value == "1" || value == "yes" || value == "on") {
            return true;
        }
        if (value == "false" || value == "0" || value == "no" || value == "off") {
            return false;
        }
        throw std::invalid_argument("Invalid boolean value '" + raw_value + "' for " + error_context);
    }

    int parse_int_in_range(const std::optional<std::string> &value,
                           const std::string &field_name,
                           int min_value,
                           int max_value,
                           bool required,
                           int default_value = 0) {
        if (!value) {
            if (required) {
                throw std::invalid_argument("Missing required metadata date field '" + field_name + "'");
            }
            return default_value;
        }
        try {
            const int parsed = std::stoi(*value);
            if (parsed < min_value || parsed > max_value) {
                throw std::invalid_argument("Metadata date field '" + field_name + "' out of range");
            }
            return parsed;
        } catch (const std::invalid_argument &) {
            throw std::invalid_argument("Invalid integer metadata date field '" + field_name + "': " + *value);
        } catch (const std::out_of_range &) {
            throw std::invalid_argument("Out-of-range metadata date field '" + field_name + "': " + *value);
        }
    }

    docraft::DocraftDocumentMetadata::DateTime parse_metadata_date(const pugi::xml_node &date_node,
                                                                   const std::string &tag_name) {
        if (!date_node) {
            throw std::invalid_argument("Missing metadata date node '" + tag_name + "'");
        }

        const std::optional<std::string> year_value = read_attr_or_child_text(
            date_node,
            std::string{docraft::craft::elements::metadata::date::attribute::kYear},
            std::string{docraft::craft::elements::metadata::date_component::kYear});
        const std::optional<std::string> month_value = read_attr_or_child_text(
            date_node,
            std::string{docraft::craft::elements::metadata::date::attribute::kMonth},
            std::string{docraft::craft::elements::metadata::date_component::kMonth});
        const std::optional<std::string> day_value = read_attr_or_child_text(
            date_node,
            std::string{docraft::craft::elements::metadata::date::attribute::kDay},
            std::string{docraft::craft::elements::metadata::date_component::kDay});
        const std::optional<std::string> hour_value = read_attr_or_child_text(
            date_node,
            std::string{docraft::craft::elements::metadata::date::attribute::kHour},
            std::string{docraft::craft::elements::metadata::date_component::kHour});
        const std::optional<std::string> minutes_value = read_attr_or_child_text(
            date_node,
            std::string{docraft::craft::elements::metadata::date::attribute::kMinutes},
            std::string{docraft::craft::elements::metadata::date_component::kMinutes});
        const std::optional<std::string> seconds_value = read_attr_or_child_text(
            date_node,
            std::string{docraft::craft::elements::metadata::date::attribute::kSeconds},
            std::string{docraft::craft::elements::metadata::date_component::kSeconds});
        const std::optional<std::string> ind_value = read_attr_or_child_text(
            date_node,
            std::string{docraft::craft::elements::metadata::date::attribute::kInd},
            std::string{docraft::craft::elements::metadata::date_component::kInd});
        const std::optional<std::string> off_hour_value = read_attr_or_child_text(
            date_node,
            std::string{docraft::craft::elements::metadata::date::attribute::kOffHour},
            std::string{docraft::craft::elements::metadata::date_component::kOffHour});
        const std::optional<std::string> off_minutes_value = read_attr_or_child_text(
            date_node,
            std::string{docraft::craft::elements::metadata::date::attribute::kOffMinutes},
            std::string{docraft::craft::elements::metadata::date_component::kOffMinutes});

        docraft::DocraftDocumentMetadata::DateTime date_time{};
        date_time.year = parse_int_in_range(year_value, tag_name + ".year", 1, 9999, true);
        date_time.month = parse_int_in_range(month_value, tag_name + ".month", 1, 12, true);
        date_time.day = parse_int_in_range(day_value, tag_name + ".day", 1, 31, true);
        date_time.hour = parse_int_in_range(hour_value, tag_name + ".hour", 0, 23, false, 0);
        date_time.minutes = parse_int_in_range(minutes_value, tag_name + ".minutes", 0, 59, false, 0);
        date_time.seconds = parse_int_in_range(seconds_value, tag_name + ".seconds", 0, 59, false, 0);

        char timezone_indicator = '+';
        if (ind_value) {
            if (ind_value->size() != 1U) {
                throw std::invalid_argument("Metadata date field '" + tag_name + ".ind' must be a single character");
            }
            timezone_indicator = (*ind_value)[0];
        }

        if (timezone_indicator == 'Z' || timezone_indicator == 'z') {
            date_time.ind = '+';
            date_time.off_hour = 0;
            date_time.off_minutes = 0;
            return date_time;
        }
        if (timezone_indicator != '+' && timezone_indicator != '-') {
            throw std::invalid_argument("Metadata date field '" + tag_name + ".ind' must be '+' or '-'");
        }
        date_time.ind = timezone_indicator;
        date_time.off_hour = parse_int_in_range(off_hour_value, tag_name + ".off_hour", 0, 23, false, 0);
        date_time.off_minutes = parse_int_in_range(off_minutes_value, tag_name + ".off_minutes", 0, 59, false, 0);

        return date_time;
    }

    std::vector<std::string> split_keywords(const std::string &value) {
        std::vector<std::string> keywords;
        std::string current;
        for (const char ch: value) {
            if (ch == ',' || ch == ';') {
                const std::string candidate = trim_copy(current);
                if (!candidate.empty()) {
                    keywords.push_back(candidate);
                }
                current.clear();
            } else {
                current.push_back(ch);
            }
        }
        const std::string candidate = trim_copy(current);
        if (!candidate.empty()) {
            keywords.push_back(candidate);
        }
        return keywords;
    }

    std::string normalize_token(const std::string &token) {
        std::string normalized = trim_copy(token);
        for (char &ch: normalized) {
            ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
        }
        return normalized;
    }

    std::string merge_keywords(const std::optional<std::string> &existing_keywords,
                               const std::vector<std::string> &generated_keywords) {
        std::vector<std::string> merged;
        std::unordered_set<std::string> seen;

        if (existing_keywords && !existing_keywords->empty()) {
            for (const std::string &candidate: split_keywords(*existing_keywords)) {
                const std::string normalized = normalize_token(candidate);
                if (normalized.empty()) {
                    continue;
                }
                if (seen.insert(normalized).second) {
                    merged.push_back(candidate);
                }
            }
        }

        for (const std::string &generated: generated_keywords) {
            const std::string normalized = normalize_token(generated);
            if (normalized.empty()) {
                continue;
            }
            if (seen.insert(normalized).second) {
                merged.push_back(generated);
            }
        }

        std::string result;
        for (std::size_t i = 0; i < merged.size(); ++i) {
            if (i > 0) {
                result += ", ";
            }
            result += merged[i];
        }
        return result;
    }

    std::vector<std::string> split_languages(const std::string &value) {
        std::vector<std::string> languages;
        std::string token;
        for (const char ch: value) {
            if (ch == ',' || ch == ';' || std::isspace(static_cast<unsigned char>(ch))) {
                const std::string trimmed = trim_copy(token);
                if (!trimmed.empty()) {
                    languages.push_back(trimmed);
                }
                token.clear();
            } else {
                token.push_back(ch);
            }
        }
        const std::string trimmed = trim_copy(token);
        if (!trimmed.empty()) {
            languages.push_back(trimmed);
        }
        return languages;
    }

    bool is_supported_stopword_language(const std::string &language) {
        static const std::unordered_set<std::string> supported_languages = {"it", "en", "fr", "de", "es"};
        std::string normalized = language;
        for (char &ch: normalized) {
            ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
        }
        return supported_languages.find(normalized) != supported_languages.end();
    }

    DocraftMetadataParseOutcome parse_metadata_node(const pugi::xml_node &document_node) {
        DocraftMetadataParseOutcome outcome;
        const std::string metadata_tag = normalize_tag_name(std::string{docraft::craft::section::kMetadata});
        const pugi::xml_node metadata_node = document_node.child(metadata_tag.c_str());
        if (!metadata_node) {
            return outcome;
        }
        outcome.has_metadata = true;

        const std::string title_tag = normalize_tag_name(std::string{docraft::craft::elements::metadata::kDocumentTitle});
        const std::string author_tag = normalize_tag_name(std::string{docraft::craft::elements::metadata::kAuthor});
        const std::string creator_tag = normalize_tag_name(std::string{docraft::craft::elements::metadata::kCreator});
        const std::string producer_tag = normalize_tag_name(std::string{docraft::craft::elements::metadata::kProducer});
        const std::string subject_tag = normalize_tag_name(std::string{docraft::craft::elements::metadata::kSubject});
        const std::string keywords_tag = normalize_tag_name(std::string{docraft::craft::elements::metadata::kKeywords});
        const std::string trapped_tag = normalize_tag_name(std::string{docraft::craft::elements::metadata::kTrapped});
        const std::string gts_pdfx_tag = normalize_tag_name(std::string{docraft::craft::elements::metadata::kGtsPdfx});
        const std::string creation_date_tag = normalize_tag_name(
            std::string{docraft::craft::elements::metadata::kCreationDate});
        const std::string modification_date_tag = normalize_tag_name(
            std::string{docraft::craft::elements::metadata::kModificationDate});
        const std::string auto_keywords_tag = normalize_tag_name(
            std::string{docraft::craft::elements::metadata::kAutoKeywords});

        if (const auto value = read_child_text(metadata_node, title_tag)) {
            outcome.metadata.set_title(*value);
        }
        if (const auto value = read_child_text(metadata_node, author_tag)) {
            outcome.metadata.set_author(*value);
        }
        if (const auto value = read_child_text(metadata_node, creator_tag)) {
            outcome.metadata.set_creator(*value);
        }
        if (const auto value = read_child_text(metadata_node, producer_tag)) {
            outcome.metadata.set_producer(*value);
        }
        if (const auto value = read_child_text(metadata_node, subject_tag)) {
            outcome.metadata.set_subject(*value);
        }
        if (const auto value = read_child_text(metadata_node, keywords_tag)) {
            outcome.metadata.set_keywords(*value);
        }
        if (const auto value = read_child_text(metadata_node, trapped_tag)) {
            outcome.metadata.set_trapped(*value);
        }
        if (const auto value = read_child_text(metadata_node, gts_pdfx_tag)) {
            outcome.metadata.set_gts_pdfx(*value);
        }
        if (const pugi::xml_node creation_date_node = metadata_node.child(creation_date_tag.c_str())) {
            outcome.metadata.set_creation_date(parse_metadata_date(creation_date_node, creation_date_tag));
        }
        if (const pugi::xml_node modification_date_node = metadata_node.child(modification_date_tag.c_str())) {
            outcome.metadata.set_modification_date(parse_metadata_date(modification_date_node, modification_date_tag));
        }

        if (const pugi::xml_node auto_keywords_node = metadata_node.child(auto_keywords_tag.c_str())) {
            outcome.auto_keyword_config.enabled = true;
            if (const pugi::xml_attribute enabled_attr = auto_keywords_node.attribute(
                    std::string{docraft::craft::elements::metadata::auto_keywords::attribute::kEnabled}.c_str())) {
                outcome.auto_keyword_config.enabled = parse_bool_string(
                    enabled_attr.as_string(), auto_keywords_tag + ".enabled");
            } else {
                const std::string node_value = trim_copy(auto_keywords_node.child_value());
                if (!node_value.empty()) {
                    outcome.auto_keyword_config.enabled = parse_bool_string(
                        node_value, auto_keywords_tag + " body value");
                }
            }

            if (const pugi::xml_attribute max_keywords_attr = auto_keywords_node.attribute(
                    std::string{
                        docraft::craft::elements::metadata::auto_keywords::attribute::kMaxKeywords}.c_str())) {
                const int max_keywords = max_keywords_attr.as_int();
                if (max_keywords <= 0) {
                    throw std::invalid_argument("Metadata AutoKeywords max_keywords must be greater than 0");
                }
                outcome.auto_keyword_config.max_keywords = static_cast<std::size_t>(max_keywords);
            }

            if (const pugi::xml_attribute min_length_attr = auto_keywords_node.attribute(
                    std::string{
                        docraft::craft::elements::metadata::auto_keywords::attribute::kMinLength}.c_str())) {
                const int min_length = min_length_attr.as_int();
                if (min_length <= 0) {
                    throw std::invalid_argument("Metadata AutoKeywords min_length must be greater than 0");
                }
                outcome.auto_keyword_config.min_length = static_cast<std::size_t>(min_length);
            }

            if (const pugi::xml_attribute language_attr = auto_keywords_node.attribute(
                    std::string{
                        docraft::craft::elements::metadata::auto_keywords::attribute::kLanguage}.c_str())) {
                const auto parsed_languages = split_languages(language_attr.as_string());
                if (parsed_languages.empty()) {
                    throw std::invalid_argument("Metadata AutoKeywords language cannot be empty");
                }
                std::vector<std::string> validated_languages;
                for (const auto &language: parsed_languages) {
                    std::string normalized = language;
                    for (char &ch: normalized) {
                        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
                    }
                    if (!is_supported_stopword_language(normalized)) {
                        throw std::invalid_argument(
                            "Unsupported AutoKeywords language '" + language + "'. Supported: it,en,fr,de,es");
                    }
                    validated_languages.push_back(normalized);
                }
                outcome.auto_keyword_config.stop_word_languages = std::move(validated_languages);
            }
        }

        return outcome;
    }
} // namespace

namespace docraft::craft {

DocraftCraftLanguageParser::DocraftCraftLanguageParser() {
    // Register parsers for different node types
    parsers_[tag_formatter(elements::kSettings.data())] = std::make_unique<parser::DocraftSettingsParser>(); // settings should be parsed before any other node, so it is registered first
    parsers_[tag_formatter(elements::templating::kForeach.data())] = std::make_unique<parser::DocraftForeachParser>(this);
    parsers_[tag_formatter(section::kHeader.data())] = std::make_unique<parser::DocraftHeaderParser>();
    parsers_[tag_formatter(section::kBody.data())] = std::make_unique<parser::DocraftBodyParser>();
    parsers_[tag_formatter(section::kFooter.data())] = std::make_unique<parser::DocraftFooterParser>();
    parsers_[tag_formatter(elements::kText.data())] = std::make_unique<parser::DocraftTextParser>();
    parsers_[tag_formatter(elements::kTitle.data())] = std::make_unique<parser::DocraftTextParser>();
    parsers_[tag_formatter(elements::kSubtitle.data())] = std::make_unique<parser::DocraftTextParser>();
    parsers_[tag_formatter(elements::kPageNumber.data())] = std::make_unique<parser::DocraftPageNumberParser>();
    parsers_[tag_formatter(elements::kImage.data())] = std::make_unique<parser::DocraftImageParser>();
    parsers_[tag_formatter(elements::kTable.data())] = std::make_unique<parser::DocraftTableParser>();
    parsers_[tag_formatter(elements::kList.data())] = std::make_unique<parser::DocraftListParser>();
    parsers_[tag_formatter(elements::kUList.data())] = std::make_unique<parser::DocraftUListParser>();
    parsers_[tag_formatter(elements::kLayout.data())] = std::make_unique<parser::DocraftLayoutParser>();
    parsers_[tag_formatter(elements::kBlankLine.data())] = std::make_unique<parser::DocraftBlackLineParser>();
    parsers_[tag_formatter(elements::kNewPage.data())] = std::make_unique<parser::DocraftNewPageParser>();
    parsers_[tag_formatter(elements::kRectangle.data())] = std::make_unique<parser::DocraftRectangleParser>();
    parsers_[tag_formatter(elements::kCircle.data())] = std::make_unique<parser::DocraftCircleParser>();
    parsers_[tag_formatter(elements::kTriangle.data())] = std::make_unique<parser::DocraftTriangleParser>();
    parsers_[tag_formatter(elements::kLine.data())] = std::make_unique<parser::DocraftLineParser>();
    parsers_[tag_formatter(elements::kPolygon.data())] = std::make_unique<parser::DocraftPolygonParser>();
    // Add more parsers as needed
}

void DocraftCraftLanguageParser::parse(const std::string &craft_language_source) {
    xml_doc_ = pugi::xml_document();
    pugi::xml_parse_result result = xml_doc_.load_string(craft_language_source.c_str());
    if (!result) {
        throw std::runtime_error("Error parsing .craft content: " + std::string(result.description()));
    }
    load_document();
}

void DocraftCraftLanguageParser::load_from_file(const std::string &file_path) {
    xml_doc_ = pugi::xml_document();
    pugi::xml_parse_result result = xml_doc_.load_file(file_path.c_str());
    if (!result) {
        throw std::runtime_error("Error loading .craft file: " + std::string(result.description()));
    }
    load_document();
}

void DocraftCraftLanguageParser::print_xml_tree(const pugi::xml_node &node, int /*indent*/) {
    node.print(std::cout);
}

void DocraftCraftLanguageParser::debug_node(const pugi::xml_node &node, int indent) {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "<" << node.name();
    for (pugi::xml_attribute attr: node.attributes()) {
        std::cout << " " << attr.name() << "=\"" << attr.value() << "\"";
    }
    std::cout << ">" << std::endl;
    for (pugi::xml_node child: node.children()) {
        debug_node(child, indent + 1);
    }
}

std::string DocraftCraftLanguageParser::tag_formatter(const std::string &tag_name) {
    // document -> Document, header -> Header, body -> Body, footer -> Footer
    if (tag_name.empty()) {
        return tag_name;
    }
    bool has_upper = false;
    for (char c : tag_name) {
        if (std::isupper(static_cast<unsigned char>(c))) {
            has_upper = true;
            break;
        }
    }
    if (has_upper) {
        return tag_name;
    }
    std::string formatted_tag = tag_name;
    // make all lowercase, then uppercase first letter — avoid UB by casting to unsigned char
    for (char &c: formatted_tag) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    formatted_tag[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(formatted_tag[0])));
    return formatted_tag;
}

std::shared_ptr<model::DocraftNode> DocraftCraftLanguageParser::parse_node(const pugi::xml_node &xml_node) {
    std::string node_name = xml_node.name();
    auto it = parsers_.find(node_name);
    if (it == parsers_.end()) {
        throw std::runtime_error("No parser registered for node: " + node_name);
    }

    auto result = it->second->parse(xml_node);
    if (std::dynamic_pointer_cast<model::DocraftForeach>(result)) {
        // Foreach parser already captures template nodes; don't add them as children.
        return result;
    }
    if (auto parent = std::dynamic_pointer_cast<model::DocraftChildrenContainerNode>(result)) {
        // if the node can have children parse them
        for (pugi::xml_node child: xml_node.children()) {
            if (child.type() != pugi::node_element) {
                continue; // Skip non-element nodes
            }
            if (auto list_node = std::dynamic_pointer_cast<model::DocraftList>(result)) {
                if (child.name() != std::string{elements::kText}) {
                    throw std::invalid_argument(std::string(child.name()) + " cannot be placed in a list");
                }
            }

            // Validate that Text nodes cannot contain Text-like child nodes
            if (std::dynamic_pointer_cast<model::DocraftText>(result)) {
                const std::string child_name = child.name();
                if (child_name == std::string{elements::kText} ||
                    child_name == std::string{elements::kTitle} ||
                    child_name == std::string{elements::kSubtitle} ||
                    child_name == std::string{elements::kPageNumber}) {
                    throw std::invalid_argument(
                        "Text nodes cannot contain child <" + child_name + "> nodes. " +
                        "Text is a leaf node and only accepts text content. " +
                        "Use <Layout> as a container for multiple text elements instead.");
                }
            }

            parent->add_child(parse_node(child));
        }
    }
    return result;
}

void DocraftCraftLanguageParser::load_document() {
    document_ = std::make_shared<DocraftDocument>();

    // root
    const std::string root_tag = tag_formatter(section::kDocument.data());
    pugi::xml_node root = xml_doc_.child(root_tag.c_str());
    if (!root) {
        throw std::runtime_error("Invalid .craft file: missing <Document> root element");
    }
    pugi::xml_node document = root;
    if (const pugi::xml_attribute path_attr = document.attribute(section::attribute::kPath.data())) {
        const std::string output_path = trim_copy(path_attr.as_string());
        if (!output_path.empty()) {
            document_->set_document_path(output_path);
        }
    }

    const DocraftMetadataParseOutcome metadata_parse_outcome = parse_metadata_node(document);
    if (metadata_parse_outcome.has_metadata) {
        document_->set_document_metadata(metadata_parse_outcome.metadata);
    }

    //Settings
    const std::string settings_tag = tag_formatter(elements::kSettings.data());
    if (pugi::xml_node settings_node = document.child(settings_tag.c_str())) {
       LOG_DEBUG("Settings found.");
        auto it = parsers_.find(settings_tag);
        if (it != parsers_.end()) {
            if (auto settings = std::dynamic_pointer_cast<model::DocraftSettings>(it->second->parse(settings_node))) {
                document_->set_settings(settings);
            }
        }
    }
    // Header (optional)
    const std::string header_tag = tag_formatter(section::kHeader.data());
    if (pugi::xml_node header_node = document.child(header_tag.c_str())) {
        LOG_DEBUG("Header found.");
        auto it = parsers_.find(header_tag);
        if (it != parsers_.end()) {
            if (auto header = std::dynamic_pointer_cast<model::DocraftHeader>(it->second->parse(header_node))) {
                document_->add_node(parse_node(header_node));
            }
        }
    }

    // Body (required)
    const std::string body_tag = tag_formatter(section::kBody.data());
    if (pugi::xml_node body_node = document.child(body_tag.c_str())) {
        LOG_DEBUG("Body found.");
        auto it = parsers_.find(body_tag);
        if (it != parsers_.end()) {
            if (auto body = std::dynamic_pointer_cast<model::DocraftBody>(it->second->parse(body_node))) {
                document_->add_node(parse_node(body_node));
            }
        }
    } else {
        throw std::runtime_error("Invalid .craft file: missing <body> element");
    }

    // Footer (optional)
    const std::string footer_tag = tag_formatter(section::kFooter.data());
    if (pugi::xml_node footer_node = document.child(footer_tag.c_str())) {
        LOG_DEBUG("Footer found.");
        auto it = parsers_.find(footer_tag);
        if (it != parsers_.end()) {
            if (auto footer = std::dynamic_pointer_cast<model::DocraftFooter>(it->second->parse(footer_node))) {
                document_->add_node(parse_node(footer_node));
            }
        }
    }

    if (metadata_parse_outcome.auto_keyword_config.enabled) {
        DocraftDocumentMetadata metadata = document_->document_metadata();
        utils::DocraftKeywordExtractor extractor({
            .max_keywords = metadata_parse_outcome.auto_keyword_config.max_keywords,
            .min_length = metadata_parse_outcome.auto_keyword_config.min_length,
            .stop_word_languages = metadata_parse_outcome.auto_keyword_config.stop_word_languages
        });
        const std::vector<std::string> extracted_keywords = extractor.extract(*document_);
        if (!extracted_keywords.empty()) {
            metadata.set_keywords(merge_keywords(metadata.keywords(), extracted_keywords));
            document_->set_document_metadata(metadata);
        }
    }

    LOG_INFO("Document loaded successfully with title: " + document_->document_title());
}

std::shared_ptr<DocraftDocument> DocraftCraftLanguageParser::get_document() const {
    return document_;
}

} // namespace docraft::craft
