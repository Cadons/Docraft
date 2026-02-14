// cpp
#include "craft/docraft_craft_language_parser.h"

#include <iostream>
#include <memory>
#include <cctype>

#include "craft/docraft_craft_language_tokens.h"
#include "craft/parser/docraft_parser.h"
#include "model/docraft_text.h"
#include "model/docraft_header.h"
#include "model/docraft_body.h"
#include "model/docraft_footer.h"
#include "model/docraft_children_container_node.h"
#include "utils/docraft_logger.h"

namespace docraft::craft {

DocraftCraftLanguageParser::DocraftCraftLanguageParser() {
    // Register parsers for different node types
    parsers_[tag_formatter(elements::kSettings.data())] = std::make_unique<parser::DocraftSettingsParser>(); // settings should be parsed before any other node, so it is registered first
    parsers_[tag_formatter(section::kHeader.data())] = std::make_unique<parser::DocraftHeaderParser>();
    parsers_[tag_formatter(section::kBody.data())] = std::make_unique<parser::DocraftBodyParser>();
    parsers_[tag_formatter(section::kFooter.data())] = std::make_unique<parser::DocraftFooterParser>();
    parsers_[tag_formatter(elements::kText.data())] = std::make_unique<parser::DocraftTextParser>();
    parsers_[tag_formatter(elements::kImage.data())] = std::make_unique<parser::DocraftImageParser>();
    parsers_[tag_formatter(elements::kTable.data())] = std::make_unique<parser::DocraftTableParser>();
    parsers_[tag_formatter(elements::kLayout.data())] = std::make_unique<parser::DocraftLayoutParser>();
    parsers_[tag_formatter(elements::kBlankLine.data())] = std::make_unique<parser::DocraftBlackLineParser>();
    parsers_[tag_formatter(elements::kRectangle.data())] = std::make_unique<parser::DocraftRectangleParser>();
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
    if (auto parent = std::dynamic_pointer_cast<model::DocraftChildrenContainerNode>(result)) {
        // if the node can have children parse them
        for (pugi::xml_node child: xml_node.children()) {
            if (child.type() != pugi::node_element) {
                continue; // Skip non-element nodes
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

    LOG_INFO("Document loaded successfully with title: " + document_->document_title());
}

std::shared_ptr<DocraftDocument> DocraftCraftLanguageParser::get_document() const {
    return document_;
}

} // namespace docraft::craft
