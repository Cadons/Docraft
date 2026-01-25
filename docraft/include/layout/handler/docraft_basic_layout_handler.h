#pragma once
#include "abstract_docraft_layout_handler.h"
#include "model/docraft_layout.h"
#include "model/docraft_node.h"

namespace docraft::layout::handler {
    class DocraftBasicLayoutHandler : public AbstractDocraftLayoutHandler<model::DocraftNode> {
    public:
        using AbstractDocraftLayoutHandler::AbstractDocraftLayoutHandler;

        void compute(const std::shared_ptr<model::DocraftNode> &node, model::DocraftTransform* box) override;

        bool handle(const std::shared_ptr<model::DocraftNode> &request, model::DocraftTransform *result) override;
    };
} // docraft
