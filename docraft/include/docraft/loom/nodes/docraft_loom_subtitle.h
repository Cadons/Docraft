#pragma once
#include "docraft/loom/nodes/docraft_loom_text.h"

namespace docraft::loom::nodes {
    /**
     * @brief Sub-heading text node (`<Subtitle>` in Craft Language). A specialized
     * DocraftLoomText whose constructor sets h2-like defaults (bold font, and a larger
     * margin() than plain text/paragraphs default to -- though smaller than
     * DocraftLoomTitle's -- so it reads as separated from what follows it) -- any of
     * these are still overridable via explicit attributes, parsed exactly like
     * `<Text>`'s (see DocraftTextParser).
     */
    class DOCRAFT_LIB DocraftLoomSubtitle : public DocraftLoomText
    {
    public:
        DocraftLoomSubtitle();
        ~DocraftLoomSubtitle() override = default;
        void accept(loom::interfaces::DocraftLoomIVisitor& visitor) override;
    };
} // docraft