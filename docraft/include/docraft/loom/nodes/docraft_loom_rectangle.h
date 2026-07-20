//
// Created by Matteo on 21/06/2026.
//

#pragma once
#include "docraft_loom_node.h"
#include "docraft/loom/nodes/docraft_loom_layout_container.h"

namespace docraft::loom::nodes {
    /**
     * @brief Represents a rectangular node in the Docraft Loom structure.
     *
     * `DocraftLoomRectangle` is a specific implementation of `DocraftLoomLayoutContainer`
     * that encapsulates properties and behavior for a rectangular shape. It supports
     * visitor-based operations, background/border styling and inter-child spacing
     * (both inherited from DocraftLoomLayoutContainer), and can hold children (inherited
     * from DocraftLoomNode) that are laid out inside it, padded by padding() (also
     * inherited from DocraftLoomNode -- every node type has it, not just Rectangle).
     */
    class DocraftLoomRectangle : public DocraftLoomLayoutContainer
    {
    public:
        DocraftLoomRectangle();
        ~DocraftLoomRectangle() override = default;

        void accept(loom::interfaces::DocraftLoomIVisitor& visitor) override;

        float width() const;
        void set_width(float width);

        float height() const;
        void set_height(float height);

    private:
        float width_ = 0.0F;
        float height_ = 0.0F;
    };
} // docraft
