#pragma once
#include "abstract_docraft_layout_handler.h"
#include "generic/chain_of_responsibility_handler.h"
#include "model/docraft_table.h"

namespace docraft::layout::handler {
    class DocraftLayoutTableHandler : public generic::DocraftChainOfResponsibilityHandler<model::DocraftNode>,
                                        public AbstractDocraftLayoutHandler<model::DocraftTable> {
    public:
        using AbstractDocraftLayoutHandler<model::DocraftTable>::AbstractDocraftLayoutHandler;
        void compute(const std::shared_ptr<model::DocraftTable>& node) override;
        bool handle(std::shared_ptr<model::DocraftNode> request) override;
    };
} // docraft
