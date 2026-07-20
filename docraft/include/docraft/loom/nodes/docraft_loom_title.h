#pragma once
#include "docraft/loom/nodes/docraft_loom_text.h"

namespace docraft::loom::nodes {
    /**
     * @brief Heading-level text node (`<Title>` in Craft Language). A specialized
     * DocraftLoomText whose constructor sets h1-like defaults (larger bold font, and a
     * larger margin() than plain text/paragraphs default to, so a title reads as more
     * separated from what follows it) -- any of these are still overridable via
     * explicit attributes, parsed exactly like `<Text>`'s (see DocraftTextParser).
     */
    class DOCRAFT_LIB DocraftLoomTitle : public DocraftLoomText
    {
    public:
        DocraftLoomTitle();
        ~DocraftLoomTitle() override = default;
        void accept(loom::interfaces::DocraftLoomIVisitor& visitor) override;
    };
} // docraft