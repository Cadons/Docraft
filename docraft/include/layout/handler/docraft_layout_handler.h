#pragma once
#include "abstract_docraft_layout_handler.h"
#include "generic/chain_of_responsibility_handler.h"
#include "model/docraft_layout.h"
#include "model/docraft_node.h"

namespace docraft::layout::handler {
    class DocraftLayoutHandler: public generic::DocraftChainOfResponsibilityHandler<model::DocraftNode>,
                                        public AbstractDocraftLayoutHandler<model::DocraftLayout> {
    public:
        using AbstractDocraftLayoutHandler<model::DocraftLayout>::AbstractDocraftLayoutHandler;
        void compute(const std::shared_ptr<model::DocraftLayout> &node) override;

        bool handle(std::shared_ptr<model::DocraftNode> request) override;
    };
} // docraft
