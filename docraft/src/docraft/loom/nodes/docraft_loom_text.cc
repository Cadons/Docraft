//
// Created by Matteo on 21/06/2026.
//

#include "docraft/loom/nodes/docraft_loom_text.h"

#include "docraft/utils/docraft_font_registry.h"
#include "docraft/utils/docraft_font_resolver.h"

namespace docraft::loom::nodes {
    DocraftLoomText::DocraftLoomText()
        : DocraftLoomNode(),
    font_size_(12.0F),
          bold_(false),
          italic_(false),
          underline_(false),
          color_(DocraftColor::fromRGB(0.0F, 0.0F, 0.0F, 1.0F)), // default to black
          alignment_(TextAlignment::kLeft)
    {
    }

    DocraftLoomText::DocraftLoomText(const std::string& text) : DocraftLoomText()
    {
        text_ = text;
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

    TextAlignment DocraftLoomText::alignment() const
    {
        return alignment_;
    }

    void DocraftLoomText::set_alignment(TextAlignment alignment)
    {
        alignment_ = alignment;
    }

    float DocraftLoomText::wrap_width() const
    {
        return wrap_width_;
    }

    void DocraftLoomText::set_wrap_width(float wrap_width)
    {
        wrap_width_ = wrap_width;
    }

    const std::vector<std::string>& DocraftLoomText::wrapped_lines() const
    {
        return wrapped_lines_;
    }

    void DocraftLoomText::set_wrapped_lines(std::vector<std::string> lines)
    {
        wrapped_lines_ = std::move(lines);
    }

    std::string DocraftLoomText::resolved_font_name() const
    {
        const auto style = bold_ && italic_
                               ? utils::TextStyle::kBoldItalic
                               : bold_
                               ? utils::TextStyle::kBold
                               : italic_
                               ? utils::TextStyle::kItalic
                               : utils::TextStyle::kNormal;
        utils::DocraftFontResolver resolver;
        resolver.rebuild_index(utils::DocraftFontResolver::builtin_font_names(),
                               utils::DocraftFontRegistry::instance().registered_font_names());
        const std::string resolved = resolver.resolve(font_family_, style);
        return resolved.empty() ? font_family_ : resolved;
    }
} // docraft
