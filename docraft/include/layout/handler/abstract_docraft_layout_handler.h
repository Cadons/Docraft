#pragma once
#include <memory>

#include "docraft_pdf_context.h"
#include "model/docraft_node.h"
#include "generic/chain_of_responsibility_handler.h"

namespace docraft::layout::handler {
    template<typename T>
    requires std::derived_from<T, model::DocraftNode>
    class AbstractDocraftLayoutHandler :public generic::DocraftChainOfResponsibilityHandler<model::DocraftNode,model::DocraftTransform> {
    public:
        using DocraftChainOfResponsibilityHandler::DocraftChainOfResponsibilityHandler;
        ~AbstractDocraftLayoutHandler() override = default;

        explicit AbstractDocraftLayoutHandler(const std::shared_ptr<DocraftPDFContext> &context) : context_(context) {
            if (!context_) {
                throw std::invalid_argument("context is null");
            }
        }
        virtual void compute(const std::shared_ptr<T>& node, model::DocraftTransform* box) =0;
        std::shared_ptr<DocraftPDFContext> context() const {
            return context_;
        }
    protected:
        std::shared_ptr<DocraftPDFContext> context_;
    };
}
