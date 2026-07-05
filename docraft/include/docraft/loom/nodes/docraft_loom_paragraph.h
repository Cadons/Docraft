#pragma once

#include "docraft/loom/nodes/docraft_loom_node.h"
#include "docraft/loom/nodes/docraft_loom_text.h"

namespace docraft::loom::nodes {
    /**
     * @brief Loom node representing a paragraph — a block-level container of text runs.
     *
     * A `DocraftLoomParagraph` groups one or more `DocraftLoomText` children into a single
     * vertical block. During the measure pass, the paragraph accumulates the heights of its
     * children (scaled by `line_spacing`) and adds `space_before` / `space_after` margins.
     * Width is the maximum measured width among its children.
     *
     * Children are added via `DocraftLoomNode::add_child()` and are visited recursively by
     * the measure processor before the paragraph's own size is computed.
     */
    class DOCRAFT_LIB DocraftLoomParagraph : public DocraftLoomNode
    {
    public:
        /**
         * @brief Constructs a paragraph with default typographic values.
         *
         * Defaults: `line_spacing = 1.2`, `space_before = 0`, `space_after = 0`,
         * `alignment = TextAlignment::kLeft`.
         */
        DocraftLoomParagraph();
        ~DocraftLoomParagraph() override = default;

        /**
         * @brief Dispatches this node to the visitor's `visit(DocraftLoomParagraph*)` overload.
         * @param visitor Visitor that will process this node.
         */
        void accept(loom::interfaces::DocraftLoomIVisitor& visitor) override;

        /**
         * @brief Returns the line spacing multiplier applied to each child's height.
         *
         * A value of `1.0` means tight (no extra space between lines); `1.2` is the
         * typographic convention for comfortable reading.
         * @return Line spacing multiplier.
         */
        float line_spacing() const;

        /**
         * @brief Sets the line spacing multiplier.
         * @param line_spacing Multiplier applied to each child's height. Must be > 0.
         */
        void set_line_spacing(float line_spacing);

        /**
         * @brief Returns the vertical space added above the paragraph (in points).
         * @return Space before the paragraph in points.
         */
        float space_before() const;

        /**
         * @brief Sets the vertical space above the paragraph.
         * @param space_before Space in points. Use 0 for no extra margin.
         */
        void set_space_before(float space_before);

        /**
         * @brief Returns the vertical space added below the paragraph (in points).
         * @return Space after the paragraph in points.
         */
        float space_after() const;

        /**
         * @brief Sets the vertical space below the paragraph.
         * @param space_after Space in points. Use 0 for no extra margin.
         */
        void set_space_after(float space_after);

        /**
         * @brief Returns the default text alignment for this paragraph's content.
         * @return Text alignment enum value.
         */
        TextAlignment alignment() const;

        /**
         * @brief Sets the default text alignment for this paragraph's content.
         *
         * Individual `DocraftLoomText` children may override this with their own alignment.
         * @param alignment Text alignment to apply.
         */
        void set_alignment(TextAlignment alignment);

    private:
        float line_spacing_;
        float space_before_;
        float space_after_;
        TextAlignment alignment_;
    };
} // docraft::loom::nodes
