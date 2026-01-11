#pragma once
#include "abstract_docraft_layout_handler.h"
#include "generic/chain_of_responsibility_handler.h"
#include "model/docraft_layout.h"
#include "model/docraft_node.h"
#include "model/docraft_section.h"

namespace docraft::layout::handler {
    class DocraftLayoutSectionHandler : public generic::DocraftChainOfResponsibilityHandler<model::DocraftNode>,
                                        public AbstractDocraftLayoutHandler<model::DocraftSection> {
    public:
        using AbstractDocraftLayoutHandler<model::DocraftSection>::AbstractDocraftLayoutHandler;
        void compute(const std::shared_ptr<model::DocraftSection>& node) override;
        bool handle(std::shared_ptr<model::DocraftNode> request) override;

        void post_process_layouts(const std::shared_ptr<model::DocraftSection>& section) const;
    private:
        const float kFooterDefaultHeight_=20.0F;
        const float kHeaderBottomMargin_=10.0F;
    };
} // docraft
