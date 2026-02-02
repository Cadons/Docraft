#include "model/docraft_text.h"

#include <filesystem>
#include <hpdf.h>
#include <ostream>
#include <memory>

#include "renderer/docraft_renderer.h"

namespace docraft::model {
    DocraftText::DocraftText(const std::string &text) : text_(text) {
    }

    void DocraftText::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
        context->renderer()->render_text(*this);

    }
#pragma region Getters
    const std::string &DocraftText::text() const {
        return text_;
    }

    const std::string &DocraftText::font_name() const {
        return font_name_;
    }

    float DocraftText::font_size() const {
        return font_size_;
    }

    const DocraftColor &DocraftText::color() const {
        return color_;
    }

    TextStyle DocraftText::style() const {
        return style_;
    }

    TextAlignment DocraftText::alignment() const {
        return alignment_;
    }
    bool DocraftText::underline() const {
        return underline_;
    }
    std::vector<std::shared_ptr<DocraftText>>DocraftText::lines() const {
        std::vector<std::shared_ptr<DocraftText>> result;
        for (const auto &line: children()) {
            if (auto text_line = std::dynamic_pointer_cast<DocraftText>(line)) {
                result.emplace_back(text_line);
            }
        }
        return result;
    }
#pragma endregion
#pragma region Setters
    inline std::string strip(std::string_view s) {
        size_t start = 0;
        size_t end = s.size();
        while (start < end && std::isspace(static_cast<unsigned char>(s[start]))) ++start;
        while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) --end;
        return std::string(s.substr(start, end - start));
    }
    void DocraftText::set_text(const std::string &text) {
        text_ = strip(text);
    }

    void DocraftText::set_font_name(const std::string &font_name) {
        font_name_ = font_name;
    }

    void DocraftText::set_font_size(float font_size) {
        font_size_ = font_size;
    }

    void DocraftText::set_color(const DocraftColor &color) {
        color_ = color;
    }
    void DocraftText::set_style(TextStyle style) {
        style_ = style;
    }

    void DocraftText::set_alignment(TextAlignment alignment) {
        alignment_ = alignment;
    }

    void DocraftText::set_underline(bool underline) {
        underline_ = underline;
    }

    void DocraftText::add_line(const std::shared_ptr<DocraftText>& line) {
        add_child(line);
    }
    void DocraftText::clear_lines() {
        clear_children();
    }

    void DocraftText::set_x_for_children(float x) {
        for (const auto &line: lines()) {
            if (alignment()==TextAlignment::kCenter||alignment()==TextAlignment::kRight) {
                line->set_position({.x=line->position().x + x,.y=line->position().y});//translate the current x coordinate of x units
            }
            else {
                line->set_position({.x= x,.y=line->position().y});//translate the current x coordinate of x units
            }
        }
    }
    void DocraftText::set_y_for_children(float y) {
        for (const auto &line: lines()) {
            line->set_position({.x=line->position().x,.y=line->position().y-y});
        }
    }
#pragma endregion

} // docraft
