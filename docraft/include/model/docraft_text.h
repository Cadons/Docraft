#pragma once
#include <list>
#include <memory>
#include <vector>

#include "docraft_children_container_node.h"
#include "docraft_color.h"
#include "docraft_node.h"

namespace docraft::model {
    enum class TextStyle {
        kNormal,
        kBold,
        kItalic,
        kBoldItalic
    };
    enum class TextAlignment {
        kLeft,
        kCenter,
        kRight,
        kJustified
    };
    class DocraftText : public DocraftChildrenContainerNode {
    public:
        using DocraftChildrenContainerNode::DocraftChildrenContainerNode;
        explicit DocraftText(const std::string& text);
        DocraftText(const DocraftText& node) = default;
        ~DocraftText() override = default;
        void draw(const std::shared_ptr<DocraftPDFContext>& context) override;

        //getter
        [[nodiscard]] const std::string &text() const;
        [[nodiscard]] float font_size() const;
        [[nodiscard]] const std::string& font_name() const;
        [[nodiscard]] const DocraftColor& color() const;
        [[nodiscard]] TextStyle style() const;
        [[nodiscard]] TextAlignment alignment() const;
        [[nodiscard]] bool underline() const;
        //setter
        void set_text(const std::string &text);
        void set_font_size(float font_size);
        void set_font_name(const std::string &font_name);
        void set_color(const DocraftColor &color);
        void set_style(TextStyle style);
        void set_alignment(TextAlignment alignment);
        void set_underline(bool underline);
        void add_line(const std::shared_ptr<DocraftText>& line);
        std::vector<std::shared_ptr<DocraftText>>lines() const;
        void clear_lines();
        void set_x_for_children(float x) override;
        void set_y_for_children(float y) override;

    private:
        std::string text_;
        float font_size_ = 12.0F;
        std::string font_name_ = "OpenSans";
        DocraftColor color_;
        TextStyle style_ = TextStyle::kNormal;
        TextAlignment alignment_ = TextAlignment::kLeft;
        bool underline_ = false;



    };
} // docraft
