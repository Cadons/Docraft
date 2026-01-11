#pragma once
#include "abstract_docraft_layout_handler.h"
#include "generic/chain_of_responsibility_handler.h"
#include "model/docraft_layout.h"
#include "model/docraft_text.h"

namespace docraft::layout::handler {
    class DocraftLayoutTextHandler : public generic::DocraftChainOfResponsibilityHandler<model::DocraftNode>,
                                     public AbstractDocraftLayoutHandler<model::DocraftText> {
    public:

        void compute(const std::shared_ptr<model::DocraftText>& node) override;

        using AbstractDocraftLayoutHandler::AbstractDocraftLayoutHandler;


        bool handle(std::shared_ptr<model::DocraftNode> request) override;
    protected:
        static void filter_text(std::shared_ptr<model::DocraftText> node);

    };
} // docraft
