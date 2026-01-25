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
        /**
         * @brief Computes the layout for a single node tree.
         * @param node A shared pointer to a DocraftNode object.
         * @return A DocraftTransform representing the computed layout.
         */
        model::DocraftTransform compute_layout(const std::shared_ptr<model::DocraftNode>& node);
        /**
         * @brief Computes the layout for a full document represented by a vector of nodes.
         * @param nodes A vector of shared pointers to DocraftNode objects.
         */
        void compute_document_layout(const std::vector<std::shared_ptr<model::DocraftNode>>& nodes);
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
        const float kHeaderHeightRatio_ = 0.15F;
        const float kBodyHeightRatio_ = 0.75F;
        const float kFooterHeightRatio_ = 0.10F;

        /**
         * @brief Execute the correct handler to compute the layout for the given node.
         * @param node
         * @param box
         * @return
         */
        bool compute_node(const std::shared_ptr<model::DocraftNode>& node, model::DocraftTransform* box) const;
    };
} // layout
