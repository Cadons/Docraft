#include "docraft/model/docraft_list.h"

#include <stdexcept>

#include "docraft/model/docraft_clone_utils.h"
#include "docraft/renderer/docraft_renderer.h"

namespace docraft::model {
    void DocraftList::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
        const auto &items = children();
        for (size_t i = 0; i < items.size(); ++i) {
            auto text_node = std::dynamic_pointer_cast<DocraftText>(items[i]);
            if (!text_node) {
                continue;
            }
            if (i >= markers_.size()) {
                continue;
            }
            const auto &marker = markers_[i];
            if (marker.kind == Marker::Kind::kBox) {
                if (context && context->shape_backend()) {
                    auto rgb = text_node->color().toRGB();
                    const float size = marker.size > 0.0F ? marker.size : (text_node->font_size() * 0.6F);
                    const float x = marker.position.x;
                    const float y = marker.position.y - (size * 0.2F);
                    auto shape = context->shape_backend();
                    shape->save_state();
                    shape->set_stroke_color(rgb.r, rgb.g, rgb.b);
                    shape->set_line_width(1.0F);
                    shape->draw_rectangle(x, y, size, size);
                    shape->stroke();
                    shape->restore_state();
                }
                continue;
            }
            DocraftText marker_text;
            auto line = std::make_shared<DocraftText>(marker.text);
            line->set_font_name(text_node->font_name());
            line->set_font_size(text_node->font_size());
            line->set_color(text_node->color());
            line->set_style(text_node->style());
            line->set_alignment(TextAlignment::kLeft);
            line->set_underline(false);
            line->set_position(marker.position);
            if (context && context->text_backend()) {
                line->set_width(context->text_backend()->measure_text_width(marker.text));
            }
            line->set_height(text_node->font_size() * 1.2F);
            marker_text.add_line(line);
            context->renderer()->render_text(marker_text);
        }
        draw_children(context);
    }

    std::shared_ptr<DocraftNode> DocraftList::clone() const {
        auto copy = std::make_shared<DocraftList>(*this);
        copy->clear_children();
        for (const auto &child : children()) {
            copy->add_child(clone_node(child));
        }
        copy->update_items();
        return copy;
    }

    ListKind DocraftList::kind() const {
        return kind_;
    }

    OrderedListStyle DocraftList::ordered_style() const {
        return ordered_style_;
    }

    UnorderedListDot DocraftList::unordered_dot() const {
        return unordered_dot_;
    }

    void DocraftList::set_kind(ListKind kind) {
        kind_ = kind;
        update_items();
    }

    void DocraftList::set_ordered_style(OrderedListStyle style) {
        ordered_style_ = style;
        update_items();
    }

    void DocraftList::set_unordered_dot(UnorderedListDot dot) {
        unordered_dot_ = dot;
        update_items();
    }

    void DocraftList::on_child_added() {
        if (children().empty()) {
            return;
        }
        const auto &child = children().back();
        auto text_node = as_text_node(child);
        raw_texts_.emplace_back(text_node->text());
        update_items();
    }

    void DocraftList::on_child_removed(int index) {
        if (index >= 0 && index < static_cast<int>(raw_texts_.size())) {
            raw_texts_.erase(raw_texts_.begin() + index);
        }
        update_items();
    }

    void DocraftList::update_items() {
        const auto &items = children();
        sync_raw_texts_with_children();

        for (size_t i = 0; i < items.size(); ++i) {
            auto text_node = as_text_node(items[i]);
            const std::string &raw = raw_texts_[i];
            text_node->set_text(raw);
        }
    }

    void DocraftList::apply_text_transform(const std::function<std::string(const std::string&)> &transform) {
        if (!transform) {
            return;
        }
        const auto &items = children();
        sync_raw_texts_with_children();

        for (size_t i = 0; i < items.size(); ++i) {
            auto text_node = as_text_node(items[i]);
            const std::string transformed = transform(raw_texts_[i]);
            raw_texts_[i] = transformed;
            text_node->set_text(transformed);
        }
    }

    std::shared_ptr<DocraftText> DocraftList::as_text_node(const std::shared_ptr<DocraftNode> &node) {
        auto text_node = std::dynamic_pointer_cast<DocraftText>(node);
        if (!text_node) {
            throw std::invalid_argument("List items must be Text nodes");
        }
        return text_node;
    }

    void DocraftList::sync_raw_texts_with_children() {
        const auto &items = children();
        if (raw_texts_.size() == items.size()) {
            return;
        }
        raw_texts_.clear();
        raw_texts_.reserve(items.size());
        for (const auto &child : items) {
            auto text_node = as_text_node(child);
            raw_texts_.emplace_back(text_node->text());
        }
    }

    std::string DocraftList::marker_for_index(int index) const {
        return prefix_for_index(index);
    }

    bool DocraftList::marker_is_box() const {
        return kind_ == ListKind::kUnordered && unordered_dot_ == UnorderedListDot::kBox;
    }

    void DocraftList::clear_markers() {
        markers_.clear();
    }

    void DocraftList::set_marker(int index, const std::string &text, const DocraftPoint &position,
                                 Marker::Kind kind, float size) {
        if (index < 0) {
            return;
        }
        const auto idx = static_cast<size_t>(index);
        if (markers_.size() <= idx) {
            markers_.resize(idx + 1);
        }
        markers_[idx] = Marker{.text = text, .position = position, .kind = kind, .size = size};
    }

    const std::vector<DocraftList::Marker> &DocraftList::markers() const {
        return markers_;
    }

    std::string DocraftList::prefix_for_index(int index) const {
        if (kind_ == ListKind::kOrdered) {
            const int one_based = index + 1;
            if (ordered_style_ == OrderedListStyle::kRoman) {
                return roman_for(one_based) + ".";
            }
            return std::to_string(one_based) + ".";
        }

        switch (unordered_dot_) {
            case UnorderedListDot::kDash:
                return "-";
            case UnorderedListDot::kStar:
                return "*";
            case UnorderedListDot::kCircle:
                return "o";
            case UnorderedListDot::kBox:
                return "";
            default:
                return "o";
        }
    }

    std::string DocraftList::roman_for(int value) {
        if (value <= 0) {
            return "I";
        }
        struct Roman {
            int value;
            const char *numeral;
        };
        const Roman numerals[] = {
            {1000, "M"},
            {900, "CM"},
            {500, "D"},
            {400, "CD"},
            {100, "C"},
            {90, "XC"},
            {50, "L"},
            {40, "XL"},
            {10, "X"},
            {9, "IX"},
            {5, "V"},
            {4, "IV"},
            {1, "I"},
        };

        std::string result;
        int remaining = value;
        for (const auto &item : numerals) {
            while (remaining >= item.value) {
                result += item.numeral;
                remaining -= item.value;
            }
        }
        return result;
    }
} // docraft::model
