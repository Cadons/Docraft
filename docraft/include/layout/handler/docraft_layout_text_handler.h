#pragma once
#include "abstract_docraft_layout_handler.h"
#include "generic/chain_of_responsibility_handler.h"
#include "model/docraft_layout.h"
#include "model/docraft_text.h"

namespace docraft::layout::handler {
    class DocraftLayoutTextHandler : public AbstractDocraftLayoutHandler<model::DocraftText> {
    public:

        void compute(const std::shared_ptr<model::DocraftText>& node, model::DocraftTransform* box) override;

        using AbstractDocraftLayoutHandler::AbstractDocraftLayoutHandler;

        bool handle(const std::shared_ptr<model::DocraftNode> &request, model::DocraftTransform *result) override;

    protected:
        static void filter_text(const std::shared_ptr<model::DocraftText>& node);
        float measure_text_width(const std::shared_ptr<model::DocraftText>& node) const;
        float measure_test_width(const std::string& text) const;

    };
} // docraft
