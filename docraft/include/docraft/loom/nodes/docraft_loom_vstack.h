#pragma once

#include "docraft/docraft_lib.h"
#include "docraft/loom/nodes/docraft_loom_layout_container.h"

namespace docraft::loom::nodes {
    /**
     * @brief Loom node that stacks its children vertically (top-to-bottom).
     *
     * During the measure pass, width = max child width and height = sum of child heights
     * plus spacing between each pair of adjacent children (both `spacing()` and the
     * optional background/border `style()` are inherited from
     * DocraftLoomLayoutContainer -- this lets `<Header>`/`<Body>`/`<Footer>` sections,
     * built as a DocraftLoomVStack by DocraftLoomTreeBuilder::build_section, keep
     * supporting background_color/border_color/border_width without needing
     * DocraftLoomRectangle's unrelated width_/height_/padding_ fields).
     * During the layout pass, each child is placed below the previous one.
     */
    class DOCRAFT_LIB DocraftLoomVStack : public DocraftLoomLayoutContainer
    {
    public:
        DocraftLoomVStack();
        ~DocraftLoomVStack() override = default;

        void accept(interfaces::DocraftLoomIVisitor& visitor) override;
    };
} // namespace docraft::loom::nodes