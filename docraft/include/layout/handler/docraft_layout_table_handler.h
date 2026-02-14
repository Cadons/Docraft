#pragma once
#include "abstract_docraft_layout_handler.h"
#include "generic/chain_of_responsibility_handler.h"
#include "model/docraft_table.h"

namespace docraft::layout::handler {
    /**
     * @brief Layout handler for table nodes.
     *
     * Calculates cell boxes and header/content areas based on weights.
     */
    class DocraftLayoutTableHandler : public AbstractDocraftLayoutHandler<model::DocraftTable> {
    public:
        using AbstractDocraftLayoutHandler<model::DocraftTable>::AbstractDocraftLayoutHandler;
        /**
         * @brief Computes the layout box for a table node.
         * @param node Table node.
         * @param box Output transform.
         * @param cursor Layout cursor.
         */
        void compute(const std::shared_ptr<model::DocraftTable>& node, model::DocraftTransform* box, DocraftCursor& cursor) override;

        /**
         * @brief Handles a node if it is a DocraftTable.
         * @param request Node to handle.
         * @param result Output transform.
         * @param cursor Layout cursor.
         * @return true if handled.
         */
        bool handle(const std::shared_ptr<model::DocraftNode> &request, model::DocraftTransform *result, DocraftCursor& cursor) override;
    };
} // docraft
