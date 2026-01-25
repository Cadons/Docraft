// C++
#pragma once
#include <memory>
#include <vector>

#include "generic/chain_of_responsibility_handler.h"
#include "model/docraft_node.h"

namespace docraft::layout {
    using Handlers = std::vector<std::unique_ptr<generic::DocraftChainOfResponsibilityHandler<model::DocraftNode, model::DocraftTransform>>>;
    class DocraftLayoutEngine {
    public:
        explicit DocraftLayoutEngine(const std::shared_ptr<DocraftPDFContext>& context);
        ~DocraftLayoutEngine() = default;
        [[deprecated]]
        static void layout(const std::shared_ptr<model::DocraftNode>& node, const std::shared_ptr<DocraftPDFContext> &context);
        model::DocraftTransform compute_layout(const std::shared_ptr<model::DocraftNode>& node);
    protected:
        const std::shared_ptr<DocraftPDFContext>& context() const;
        /**
         * @brief Computes the maximum rectangle that can contain all the given boxes.
         * @param boxes A vector of DocraftTransform representing the boxes.
         * @return A DocraftTransform representing the maximum rectangle.
         */
        static model::DocraftTransform compute_max_rect(const std::vector<model::DocraftTransform>& boxes) ;

    private:
        void configure_handlers(const std::shared_ptr<DocraftPDFContext>& context);
        Handlers handlers_;
        std::shared_ptr<DocraftPDFContext> context_;

        /**
         * @brief Execute the correct handler to compute the layout for the given node.
         * @param node
         * @param box
         * @return
         */
        bool compute_node(const std::shared_ptr<model::DocraftNode>& node, model::DocraftTransform* box) const;
    };
} // layout
