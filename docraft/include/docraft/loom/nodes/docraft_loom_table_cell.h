#pragma once
#include <optional>

#include "docraft/docraft_color.h"
#include "docraft/loom/nodes/docraft_loom_node.h"

namespace docraft::loom::nodes {
    /**
     * @brief A single table cell -- a node like any other. Its content is just its child
     * (set via set_content(), which enforces the same Text/Image-only restriction as
     * legacy tables). background/explicit_width/is_title are properties of the cell
     * itself, not of the table.
     */
    class DOCRAFT_LIB DocraftLoomTableCell : public DocraftLoomNode
    {
    public:
        DocraftLoomTableCell() = default;
        ~DocraftLoomTableCell() override = default;
        void accept(loom::interfaces::DocraftLoomIVisitor& visitor) override;

        /**
         * @brief Sets the cell's content, replacing any existing child.
         * @throws exception::InvalidInputException if content is neither a
         * DocraftLoomText nor a DocraftLoomImage.
         */
        void set_content(std::shared_ptr<DocraftLoomNode> content);
        std::shared_ptr<DocraftLoomNode> content();

        const std::optional<DocraftColor>& background() const;
        void set_background(const DocraftColor& color);

        std::optional<float> explicit_width() const;
        void set_explicit_width(float width);

        bool is_title() const;
        void set_is_title(bool is_title);

    private:
        std::optional<DocraftColor> background_;
        std::optional<float> explicit_width_;
        bool is_title_ = false;
    };
} // docraft