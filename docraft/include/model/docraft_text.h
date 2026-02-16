#pragma once
#include <list>
#include <memory>
#include <vector>

#include "docraft_children_container_node.h"
#include "docraft_color.h"
#include "docraft_node.h"

namespace docraft::model {
    /**
     * @brief Text styles supported by Docraft.
     */
    enum class TextStyle {
        kNormal,
        kBold,
        kItalic,
        kBoldItalic
    };
    /**
     * @brief Text alignment options.
     */
    enum class TextAlignment {
        kLeft,
        kCenter,
        kRight,
        kJustified
    };
    /**
     * @brief Text node with styling, alignment, and optional line splits.
     *
     * Text nodes can generate internal line children for wrapping/justification.
     */
    class DocraftText : public DocraftChildrenContainerNode {
    public:
        using DocraftChildrenContainerNode::DocraftChildrenContainerNode;
        /**
         * @brief Creates an empty text node.
         */
        DocraftText();
        /**
         * @brief Creates a text node with initial text content.
         * @param text Initial text string.
         */
        explicit DocraftText(const std::string& text);
        DocraftText(const DocraftText& node) = default;
        ~DocraftText() override = default;
        /**
         * @brief Draws the text node using the provided context.
         * @param context Document context.
         */
        void draw(const std::shared_ptr<DocraftDocumentContext>& context) override;

        //getter
        /**
         * @brief Returns the text content.
         * @return Text string.
         */
        [[nodiscard]] const std::string &text() const;
        /**
         * @brief Returns the font size in points.
         * @return Font size in points.
         */
        [[nodiscard]] float font_size() const;
        /**
         * @brief Returns the font family name.
         * @return Font family name.
         */
        [[nodiscard]] const std::string& font_name() const;
        /**
         * @brief Returns the text color.
         * @return Text color.
         */
        [[nodiscard]] const DocraftColor& color() const;
        /**
         * @brief Returns the text style.
         * @return Text style enum.
         */
        [[nodiscard]] TextStyle style() const;
        /**
         * @brief Returns the text alignment.
         * @return Text alignment enum.
         */
        [[nodiscard]] TextAlignment alignment() const;
        /**
         * @brief Returns whether underline is enabled.
         * @return true if underline is enabled.
         */
        [[nodiscard]] bool underline() const;
        //setter
        /**
         * @brief Sets the text content.
         * @param text New text string.
         */
        void set_text(const std::string &text);
        /**
         * @brief Sets the font size in points.
         * @param font_size Font size in points.
         */
        void set_font_size(float font_size);
        /**
         * @brief Sets the font family name.
         * @param font_name Font family name.
         */
        void set_font_name(const std::string &font_name);
        /**
         * @brief Sets the text color.
         * @param color Text color.
         */
        void set_color(const DocraftColor &color);
        /**
         * @brief Sets the text style.
         * @param style Text style enum.
         */
        void set_style(TextStyle style);
        /**
         * @brief Sets the text alignment.
         * @param alignment Text alignment enum.
         */
        void set_alignment(TextAlignment alignment);
        /**
         * @brief Enables or disables underline.
         * @param underline true to enable underline.
         */
        void set_underline(bool underline);
        /**
         * @brief Adds a line node for multi-line layout.
         * @param line Line node to add.
         */
        void add_line(const std::shared_ptr<DocraftText>& line);
        /**
         * @brief Returns the list of line nodes.
         * @return Vector of line nodes.
         */
        std::vector<std::shared_ptr<DocraftText>>lines() const;
        /**
         * @brief Clears all line nodes.
         */
        void clear_lines();
        /**
         * @brief Propagates x position to child line nodes.
         * @param x New x coordinate.
         */
        void set_x_for_children(float x) override;
        /**
         * @brief Propagates y position to child line nodes.
         * @param y New y coordinate.
         */
        void set_y_for_children(float y) override;

    private:
        std::string text_;
        float font_size_ = 12.0F;
        std::string font_name_ = "OpenSans";
        DocraftColor color_;
        TextStyle style_ = TextStyle::kNormal;
        TextAlignment alignment_ = TextAlignment::kLeft;
        bool underline_ = false;
        constexpr static float kDefaultTextPadding = 5.0F; // Line spacing multiplier



    };
} // docraft
