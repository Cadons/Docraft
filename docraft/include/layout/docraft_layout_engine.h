// C++
#pragma once
#include <memory>
#include <vector>

#include "generic/chain_of_responsibility_handler.h"
#include "model/docraft_node.h"

namespace docraft::layout {
    using Handlers = std::vector<std::unique_ptr<generic::DocraftChainOfResponsibilityHandler<model::DocraftNode> > >;
    class DocraftLayoutEngine {
    public:
        static void layout(std::shared_ptr<model::DocraftNode> node, const std::shared_ptr<DocraftPDFContext> &context);
    private:

        static void configure_handlers(const std::shared_ptr<DocraftPDFContext>& context);
        static Handlers handlers_;

    };
} // layout
