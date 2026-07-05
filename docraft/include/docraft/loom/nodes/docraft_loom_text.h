#pragma once
#include <string>
#include <vector>

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

        /**
         * @brief Resolves the backend-registered font name for font_family() + bold() +
         * italic() (e.g. "Helvetica" + bold -> "Helvetica-Bold"), falling back to the
         * plain constructed name if it isn't registered with the backend.
         */
        std::string resolved_font_name() const;

        /**
         * @brief Sets the width (in points) this text wraps to. 0 (the default) disables
         * wrapping -- text() is measured/drawn as a single line, today's behavior.
         * Setting a positive value makes DocraftLoomMeasureProcessor word-wrap text()
         * into wrapped_lines() at measure time, and alignment() (including kJustified)
         * is applied per line within this width at render time.
         */
        float wrap_width() const;
        void set_wrap_width(float wrap_width);

        /**
         * @brief The lines text() was wrapped into, populated by
         * DocraftLoomMeasureProcessor when wrap_width() > 0. Empty when wrapping is
         * disabled.
         */
        const std::vector<std::string>& wrapped_lines() const;
        void set_wrapped_lines(std::vector<std::string> lines);

    private:
        std::string text_;
        std::string font_family_ = "Helvetica";
        float font_size_;
        bool bold_;
        bool italic_;
        bool underline_;
        DocraftColor color_;
        model::TextAlignment alignment_;
        float wrap_width_ = 0.0F;
        std::vector<std::string> wrapped_lines_;
    };
} // docraft
