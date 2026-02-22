#pragma once

#include "docraft_lib.h"
#include "i_painter.h"
#include "model/docraft_blank_line.h"

namespace docraft::renderer::painter {
    /**
     * @brief Painter that advances layout for blank lines.
     *
     * Performs no drawing but updates the cursor state.
     */
    class DOCRAFT_LIB docraft_blank_line_painter : public renderer::painter::IPainter {
    public:
        /**
         * @brief Creates a blank-line painter bound to the blank line node.
         * @param blank_line_node Blank line node.
         */
        explicit docraft_blank_line_painter(const model::DocraftBlankLine &blank_line_node);

        /**
         * @brief Draws the blank line using the provided context.
         * @param context Document context.
         */
        void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;

    private:
        const model::DocraftBlankLine &blank_line_node_;
    };
} // docraft
