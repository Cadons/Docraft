#pragma once
#include "docraft/loom/nodes/docraft_loom_node.h"

namespace docraft::loom::nodes {
    /**
     * @brief Zero-size marker node for a forced page break. Carries no visible content --
     * DocraftLoomPaginationProcessor::paginate_body() recognizes it and unconditionally
     * advances to a fresh page whenever it's encountered among the body's top-level
     * children, instead of running the normal fits-on-this-page check.
     */
    class DOCRAFT_LIB DocraftLoomNewPage : public DocraftLoomNode
    {
    public:
        DocraftLoomNewPage() = default;
        ~DocraftLoomNewPage() override = default;
        void accept(loom::interfaces::DocraftLoomIVisitor& visitor) override;
    };
} // docraft
