#pragma once
#include <memory>

#include "docraft_pdf_context.h"
#include "model/docraft_node.h"

namespace docraft::layout::handler {
    template<typename T>
    requires std::derived_from<T, model::DocraftNode>
    class AbstractDocraftLayoutHandler {
    public:
        virtual ~AbstractDocraftLayoutHandler() = default;

        explicit AbstractDocraftLayoutHandler(const std::shared_ptr<DocraftPDFContext> &context) : context_(context) {
        }

        virtual void compute(const std::shared_ptr<T>& node) =0;
        std::shared_ptr<DocraftPDFContext> context() const {
            return context_;
        }
        template<typename U>
        requires std::derived_from<U, model::DocraftNode>
        static void set_node_transform_box(const std::shared_ptr<U>& node) {
            model::DocraftTransform box;
            box.top_left_ = {node->x(), node->y()};
            box.top_right_ = {node->x() + node->width(), node->y()};
            box.bottom_left_ = {node->x(), node->y() - node->height()};
            box.bottom_right_ = {node->x() + node->width(), node->y() - node->height()};
            box.top_center_ = {node->x() + (node->width() / 2), node->y()};
            box.bottom_center_ = {node->x() + (node->width() / 2), node->y() - node->height()};
            box.left_center_ = {node->x(), node->y() - (node->height() / 2)};
            box.right_center_ = {node->x() + node->width(), node->y() - (node->height() / 2)};
            node->set_transform_box(box);
        }

    protected:
        template<typename U>
        requires std::derived_from<U, model::DocraftNode>
        void set_node_position(const std::shared_ptr<U>& node) {
            if (!node)
                return;
            // if (node->position() != model::DocraftPositionType::kAbsolute) {
            //     node->set_x(context()->cursor().x());
            //     node->set_y(context()->cursor().y());
            // }
        }
        std::shared_ptr<DocraftPDFContext> context_;
    };
}
