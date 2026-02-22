#pragma once

#include "docraft_lib.h"

#include <functional>

#include "abstract_docraft_layout_handler.h"
#include "model/docraft_list.h"

namespace docraft::layout::handler {
    /**
     * @brief Layout handler for list nodes.
     *
     * Computes marker positions and lays out list item content.
     */
    class DOCRAFT_LIB DocraftLayoutListHandler : public AbstractDocraftLayoutHandler<model::DocraftList> {
    public:
        using AbstractDocraftLayoutHandler<model::DocraftList>::AbstractDocraftLayoutHandler;

        /**
         * @brief Computes the layout box for a list node.
         * @param node List node.
         * @param box Output transform.
         * @param cursor Layout cursor.
         */
        void compute(const std::shared_ptr<model::DocraftList> &node,
                     model::DocraftTransform *box,
                     DocraftCursor& cursor) override;

        /**
         * @brief Computes layout for list children using a provided layout function.
         * @param node List node.
         * @param cursor Layout cursor.
         * @param child_boxes Output transforms for children.
         * @param layout_child Layout function for a child node.
         * @param max_width Available width in points.
         */
        void compute_children(const std::shared_ptr<model::DocraftList> &node,
                              DocraftCursor& cursor,
                              std::vector<model::DocraftTransform>& child_boxes,
                              const std::function<model::DocraftTransform(
                                  const std::shared_ptr<model::DocraftNode>&, DocraftCursor&)>& layout_child,
                              float max_width) const;

        /**
         * @brief Handles a node if it is a DocraftList.
         * @param request Node to handle.
         * @param result Output transform.
         * @param cursor Layout cursor.
         * @return true if handled.
         */
        bool handle(const std::shared_ptr<model::DocraftNode> &request,
                    model::DocraftTransform *result,
                    DocraftCursor& cursor) override;
    };
} // docraft::layout::handler
