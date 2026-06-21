//
// Created by Matteo on 21/06/2026.
//

#include "docraft/loom/nodes/docraft_loom_text.h"

#include "docraft/craft/docraft_craft_language_tokens.h"

namespace docraft::loom::nodes {
    DocraftLoomText::DocraftLoomText()
        : font_size_(0.0F),
          bold_(false),
          italic_(false),
          underline_(false),
          color_(DocraftColor::fromRGB(0.0F, 0.0F, 0.0F, 1.0F)) // default to black
    {
    }

    void DocraftLoomText::accept(loom::interfaces::DocraftLoomIVisitor& visitor)
    {
        visitor.visit(this);
    }

    const std::string& DocraftLoomText::text() const
    {
        return text_;
    }

    void DocraftLoomText::set_text(const std::string& text)
    {
        text_ = text;
    }

    const std::string& DocraftLoomText::font_family() const
    {
        return font_family_;
    }

    void DocraftLoomText::set_font_family(const std::string& font_family)
    {
        font_family_ = font_family;
    }

    float DocraftLoomText::font_size() const
    {
        return font_size_;
    }

    void DocraftLoomText::set_font_size(float font_size)
    {
        font_size_ = font_size;
    }

    bool DocraftLoomText::bold() const
    {
        return bold_;
    }

    void DocraftLoomText::set_bold(bool bold)
    {
        bold_ = bold;
    }

    bool DocraftLoomText::italic() const
    {
        return italic_;
    }

    void DocraftLoomText::set_italic(bool italic)
    {
        italic_ = italic;
    }

    bool DocraftLoomText::underline() const
    {
        return underline_;
    }

    void DocraftLoomText::set_underline(bool underline)
    {
        underline_ = underline;
    }

    const DocraftColor& DocraftLoomText::color() const
    {
        return color_;
    }

    void DocraftLoomText::set_color(const DocraftColor& color)
    {
        color_ = color;
    }
} // docraft
