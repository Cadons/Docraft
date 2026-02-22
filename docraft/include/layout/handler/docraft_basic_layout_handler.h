#pragma once

#include "docraft_lib.h"
#include "abstract_docraft_layout_handler.h"
#include "model/docraft_layout.h"
#include "model/docraft_node.h"

namespace docraft::layout::handler {
    /**
     * @brief Fallback layout handler for generic nodes.
     *
     * Used when no specialized handler claims the node.
     */
    class DOCRAFT_LIB DocraftBasicLayoutHandler : public AbstractDocraftLayoutHandler<model::DocraftNode> {
    public:
        using AbstractDocraftLayoutHandler::AbstractDocraftLayoutHandler;

        /**
         * @brief Computes the layout box for a basic node.
         * @param node Node to layout.
         * @param box Output transform.
         * @param cursor Layout cursor.
         */
        void compute(const std::shared_ptr<model::DocraftNode> &node, model::DocraftTransform* box, DocraftCursor& cursor) override;

        /**
         * @brief Handles a node if no specialized handler applies.
         * @param request Node to handle.
         * @param result Output transform.
         * @param cursor Layout cursor.
         * @return true if handled.
         */
        bool handle(const std::shared_ptr<model::DocraftNode> &request, model::DocraftTransform *result, DocraftCursor& cursor) override;
    };
} // docraft
