#pragma once
#include "abstract_docraft_layout_handler.h"
#include "generic/chain_of_responsibility_handler.h"
#include "model/docraft_table.h"

namespace docraft::layout::handler {
    class DocraftLayoutTableHandler : public AbstractDocraftLayoutHandler<model::DocraftTable> {
    public:
        using AbstractDocraftLayoutHandler<model::DocraftTable>::AbstractDocraftLayoutHandler;
        void compute(const std::shared_ptr<model::DocraftTable>& node, model::DocraftTransform* box) override;

        bool handle(const std::shared_ptr<model::DocraftNode> &request, model::DocraftTransform *result) override;
    };
} // docraft
