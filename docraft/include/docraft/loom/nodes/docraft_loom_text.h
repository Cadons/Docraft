#pragma once
#include <string>

#include "docraft/loom/nodes/docraft_loom_node.h"
#include "docraft/docraft_color.h"
#include "docraft/model/docraft_text.h"

namespace docraft::loom::nodes {
    class DOCRAFT_LIB DocraftLoomText : public DocraftLoomNode
    {
    public:
        DocraftLoomText();
        DocraftLoomText(const std::string& text);
        ~DocraftLoomText() override = default;
        void accept(loom::interfaces::DocraftLoomIVisitor& visitor) override;
        //setter
        const std::string& text() const;
        void set_text(const std::string& text);
        const std::string& font_family() const;
        void set_font_family(const std::string& font_family);
        float font_size() const;
        void set_font_size(float font_size);
        //getter
        bool bold() const;
        void set_bold(bool bold);
        bool italic() const;
        void set_italic(bool italic);
        bool underline() const;
        void set_underline(bool underline);
        const DocraftColor& color() const;
        void set_color(const DocraftColor& color);
        model::TextAlignment alignment() const;
        void set_alignment(model::TextAlignment alignment);

    private:
        std::string text_;
        std::string font_family_ = "Helvetica";
        float font_size_;
        bool bold_;
        bool italic_;
        bool underline_;
        DocraftColor color_;
        model::TextAlignment alignment_;
    };
} // docraft
