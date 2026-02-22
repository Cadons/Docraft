#pragma once

#include "docraft_lib.h"
#include "i_painter.h"
#include "model/docraft_table.h"

namespace docraft::renderer::painter {
    /**
     * @brief Painter that draws DocraftTable nodes.
     *
     * Draws headers, grid lines, and cell content using the current context.
     */
    class DOCRAFT_LIB DocraftTablePainter : public renderer::painter::IPainter {
    public:
        /**
         * @brief Creates a table painter bound to the table node.
         * @param table_node Table node.
         */
        DocraftTablePainter(const model::DocraftTable &table_node);

        /**
         * @brief Draws the table using the provided context.
         * @param context Document context.
         */
        void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;
    private:
        const model::DocraftTable &table_node_;
    };
} // docraft
