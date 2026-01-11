#pragma once
#include "abstract_docraft_layout_handler.h"
#include "generic/chain_of_responsibility_handler.h"
#include "model/docraft_blank_line.h"

namespace docraft::layout::handler {
    class DocraftLayoutBlankLine :public generic::DocraftChainOfResponsibilityHandler<model::DocraftNode>,
                                        public AbstractDocraftLayoutHandler<model::DocraftBlankLine> {
    public:
        using AbstractDocraftLayoutHandler<model::DocraftBlankLine>::AbstractDocraftLayoutHandler;
        void compute(const std::shared_ptr<model::DocraftBlankLine> &node) override;

        bool handle(std::shared_ptr<model::DocraftNode> request) override;
    };
} // docraft
