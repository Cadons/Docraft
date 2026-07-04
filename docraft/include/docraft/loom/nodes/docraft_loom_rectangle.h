//
// Created by Matteo on 21/06/2026.
//

#pragma once
#include "docraft_loom_node.h"
#include "docraft/loom/nodes/docraft_loom_shape.h"

namespace docraft::loom::nodes {
    /**
     * @brief Represents a rectangular node in the Docraft Loom structure.
     *
     * `DocraftLoomRectangle` is a specific implementation of `DocraftLoomShape` that encapsulates
     * properties and behavior for a rectangular shape. It supports visitor-based operations,
     * background/border styling (inherited from DocraftLoomShape), and can hold children
     * (inherited from DocraftLoomNode) that are laid out inside it, padded by padding().
     */
    class DocraftLoomRectangle : public DocraftLoomShape
    {
    public:
        DocraftLoomRectangle() = default;
        ~DocraftLoomRectangle() override = default;

        void accept(loom::interfaces::DocraftLoomIVisitor& visitor) override;

        float width() const;
        void set_width(float width);

        float height() const;
        void set_height(float height);

        float padding() const;
        void set_padding(float padding);

    private:
        float width_ = 0.0F;
        float height_ = 0.0F;
        float padding_ = 0.0F;
    };
} // docraft
