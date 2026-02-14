#pragma once
#include <memory>

#include "docraft_document_context.h"
#include "model/docraft_node.h"
#include "generic/chain_of_responsibility_handler.h"

namespace docraft::layout::handler {
    template<typename T>
    requires std::derived_from<T, model::DocraftNode>
    /**
     * @brief Base class for layout handlers in the chain of responsibility.
     *
     * Each handler is responsible for computing layout boxes for a specific
     * node type and is given access to the shared document context.
     *
     * @tparam T Node type handled by this handler.
     */
    class AbstractDocraftLayoutHandler :public generic::DocraftChainOfResponsibilityHandler<model::DocraftNode,model::DocraftTransform> {
    public:
        using DocraftChainOfResponsibilityHandler::DocraftChainOfResponsibilityHandler;
        ~AbstractDocraftLayoutHandler() override = default;

        /**
         * @brief Creates a handler bound to a document context.
         * @throws std::invalid_argument if context is null.
         */
        explicit AbstractDocraftLayoutHandler(const std::shared_ptr<DocraftDocumentContext> &context) : context_(context) {
            if (!context_) {
                throw std::invalid_argument("context is null");
            }
        }
        /**
         * @brief Computes the layout box for a concrete node type.
         * @param node Node to layout.
         * @param box Output transform.
         * @param cursor Layout cursor.
         */
        virtual void compute(const std::shared_ptr<T>& node, model::DocraftTransform* box, DocraftCursor& cursor) =0;
        /**
         * @brief Returns the bound document context.
         * @return Document context.
         */
        std::shared_ptr<DocraftDocumentContext> context() const {
            return context_;
        }
    protected:
        std::shared_ptr<DocraftDocumentContext> context_;
    };
}
