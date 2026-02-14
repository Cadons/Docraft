#pragma once
#include "abstract_docraft_layout_handler.h"
#include "generic/chain_of_responsibility_handler.h"
#include "model/docraft_layout.h"
#include "model/docraft_node.h"

namespace docraft::layout::handler {
    /**
     * @brief Layout handler for DocraftLayout nodes.
     *
     * Computes child boxes based on orientation and weights.
     */
    class DocraftLayoutHandler:public AbstractDocraftLayoutHandler<model::DocraftLayout> {
    public:
        using AbstractDocraftLayoutHandler<model::DocraftLayout>::AbstractDocraftLayoutHandler;
        /**
         * @brief Computes the layout for a layout node.
         * @param node Layout node.
         * @param box Output transform.
         * @param cursor Layout cursor.
         */
        void compute(const std::shared_ptr<model::DocraftLayout> &node, model::DocraftTransform* box, DocraftCursor& cursor) override;

        /**
         * @brief Handles a node if it is a DocraftLayout.
         * @param request Node to handle.
         * @param result Output transform.
         * @param cursor Layout cursor.
         * @return true if handled.
         */
        bool handle(const std::shared_ptr<model::DocraftNode> &request, model::DocraftTransform *result, DocraftCursor& cursor) override;
    };
} // docraft
