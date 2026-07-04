//
// Created by Matteo on 21/06/2026.
//

#pragma once
#include "docraft_loom_node.h"
#include "docraft/docraft_color.h"

namespace docraft::loom::nodes {
    /**
     * @brief Represents a rectangular node in the Docraft Loom structure.
     *
     * `DocraftLoomRectangle` is a specific implementation of `DocraftLoomNode` that encapsulates
     * properties and behavior for a rectangular shape. It supports visitor-based operations,
     * color management, and size dimensions.
     */
    class DocraftLoomRectangle : public docraft::loom::nodes::DocraftLoomNode
    {
    public:
        DocraftLoomRectangle() = default;
        ~DocraftLoomRectangle() override = default;

        void accept(loom::interfaces::DocraftLoomIVisitor& visitor) override;

        const DocraftColor& color() const;
        void set_color(const DocraftColor& color);

        float width() const;
        void set_width(float width);

        float height() const;
        void set_height(float height);

    private:
        DocraftColor color_;
        float width_ = 0.0f;
        float height_ = 0.0f;
    };
} // docraft
