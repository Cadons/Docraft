#pragma once
#include "abstract_docraft_layout_handler.h"
#include "generic/chain_of_responsibility_handler.h"
#include "model/docraft_blank_line.h"

namespace docraft::layout::handler {
    class DocraftLayoutBlankLine :public AbstractDocraftLayoutHandler<model::DocraftBlankLine> {
    public:
        using AbstractDocraftLayoutHandler<model::DocraftBlankLine>::AbstractDocraftLayoutHandler;
        void compute(const std::shared_ptr<model::DocraftBlankLine> &node, model::DocraftTransform* box, DocraftCursor& cursor) override;

        bool handle(const std::shared_ptr<model::DocraftNode> &request, model::DocraftTransform *result, DocraftCursor& cursor) override;
    };
} // docraft
