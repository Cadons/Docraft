#pragma once

#include "docraft_lib.h"
#include "abstract_docraft_layout_handler.h"
#include "generic/chain_of_responsibility_handler.h"
#include "model/docraft_blank_line.h"

namespace docraft::layout::handler {
    /**
     * @brief Layout handler for blank line nodes.
     *
     * Advances the cursor to create vertical spacing.
     */
    class DOCRAFT_LIB DocraftLayoutBlankLine :public AbstractDocraftLayoutHandler<model::DocraftBlankLine> {
    public:
        using AbstractDocraftLayoutHandler<model::DocraftBlankLine>::AbstractDocraftLayoutHandler;
        /**
         * @brief Computes the layout box for a blank line.
         * @param node Blank line node.
         * @param box Output transform.
         * @param cursor Layout cursor.
         */
        void compute(const std::shared_ptr<model::DocraftBlankLine> &node, model::DocraftTransform* box, DocraftCursor& cursor) override;

        /**
         * @brief Handles a node if it is a DocraftBlankLine.
         * @param request Node to handle.
         * @param result Output transform.
         * @param cursor Layout cursor.
         * @return true if handled.
         */
        bool handle(const std::shared_ptr<model::DocraftNode> &request, model::DocraftTransform *result, DocraftCursor& cursor) override;
    };
} // docraft
