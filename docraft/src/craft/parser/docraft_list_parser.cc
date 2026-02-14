#include "craft/parser/docraft_parser.h"

#include "craft/parser/docraft_parser_helpers.h"
#include "model/docraft_list.h"

namespace docraft::craft::parser {
    std::shared_ptr<model::DocraftNode> DocraftListParser::parse(const pugi::xml_node &craft_language_source) {
        auto list_node = std::make_shared<model::DocraftList>();
        list_node->set_kind(model::ListKind::kOrdered);
        if (auto style_attr = craft_language_source.attribute(elements::list::attribute::kStyle.data())) {
            std::string style_str = style_attr.as_string();
            if (style_str == std::string{elements::list::style::kNumber}) {
                list_node->set_ordered_style(model::OrderedListStyle::kNumber);
            } else if (style_str == std::string{elements::list::style::kRoman}) {
                list_node->set_ordered_style(model::OrderedListStyle::kRoman);
            } else {
                throw std::invalid_argument("Invalid list style: " + style_str);
            }
        }

        detail::configure_docraft_node_attributes(list_node, craft_language_source);
        return list_node;
    }

    std::shared_ptr<model::DocraftNode> DocraftUListParser::parse(const pugi::xml_node &craft_language_source) {
        auto list_node = std::make_shared<model::DocraftList>();
        list_node->set_kind(model::ListKind::kUnordered);
        if (auto dot_attr = craft_language_source.attribute(elements::ulist::attribute::kDot.data())) {
            std::string dot_str = dot_attr.as_string();
            if (dot_str == std::string{elements::ulist::dot::kDash}) {
                list_node->set_unordered_dot(model::UnorderedListDot::kDash);
            } else if (dot_str == std::string{elements::ulist::dot::kStar}) {
                list_node->set_unordered_dot(model::UnorderedListDot::kStar);
            } else if (dot_str == std::string{elements::ulist::dot::kCircle}) {
                list_node->set_unordered_dot(model::UnorderedListDot::kCircle);
            } else if (dot_str == std::string{elements::ulist::dot::kBox}) {
                list_node->set_unordered_dot(model::UnorderedListDot::kBox);
            } else {
                throw std::invalid_argument("Invalid unordered list dot: " + dot_str);
            }
        }

        detail::configure_docraft_node_attributes(list_node, craft_language_source);
        return list_node;
    }
}
