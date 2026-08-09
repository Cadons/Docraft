//
// Created by Matteo on 21/06/2026.
//

#pragma once
#include <string>
#include <vector>
#include <memory>
#include "docraft/docraft_lib.h"
#include "docraft/docraft_position.h"
#include "docraft/loom/interfaces/docraft_loom_visitor.h"

namespace docraft::loom::nodes {
    /**
     * @brief Struct representing a size with width and height.
     */
    struct Size
    {
        float width;
        float height;
    };

    /**
     * @brief A position with x and y coordinates -- an alias for the shared
     * `docraft::Position`, so loom nodes, the cursor, and backend rendering
     * interfaces all share one point type.
     */
    using Position = docraft::Position;

    /**
     * @brief Struct representing a rectangle with a position and size.
     */
    struct Rect
    {
        Position position;
        Size size;
    };

    /**
     * @brief Positioning modes for loom layout.
     */
    enum class DocraftPositionType
    {
        kBlock,
        kAbsolute
    };

    /**
     * @brief Per-edge outer spacing (points), CSS-margin-shorthand style.
     */
    struct DocraftLoomMargin
    {
        float top = 0.0F;
        float right = 0.0F;
        float bottom = 0.0F;
        float left = 0.0F;
    };

    /**
     * @brief Struct representing a layout box, which encapsulates layout-related data for nodes.
     *
     * The `LayoutBox` struct is used to store the results of key steps in the layout process:
     * - Measurement step (`measured_size`): The calculated size of the node.
     * - Pagination step (`page_index`): The index of the page where the node is located.
     * - Layout step (`frame`): The final position and size of the node to be applied in the layout.
     */
    struct LayoutBox
    {
        Size measured_size; // measure step result
        int page_index; // pagination step result
        Rect frame; // layout step result, this is the final result to apply in the layout
        LayoutBox() = default;
    };

    /**
     * @brief Represents a node in the Docraft Loom structure, supporting child-node management
     *        and layout features.
     *
     * This class provides the ability to manage child nodes and store layout information through
     * the use of `LayoutBox`. It also inherits from `interfaces::DocraftLoomIVisitorNode`, enabling
     * visitor-based operations.
     */
    class DOCRAFT_LIB DocraftLoomNode : public interfaces::DocraftLoomIVisitorNode
    {
    public:
        using DocraftLoomIVisitorNode::DocraftLoomIVisitorNode;
        ~DocraftLoomNode() override = default;
        //children management
        int children_count() const;
        void add_child(std::shared_ptr<DocraftLoomNode> child);
        std::shared_ptr<DocraftLoomNode> remove_child(int index);
        std::shared_ptr<const DocraftLoomNode> child(int index) const;
        std::shared_ptr<DocraftLoomNode> edit_child(int index);
        void clear_children();
        /**
         * @brief Returns this node's direct-child indices (0..children_count()-1),
         * stable-sorted ascending by each child's z_index() -- the order in which
         * this node's own children should be painted (sibling-scoped only, CSS
         * stacking-context style; does not look into grandchildren or any other
         * container's children). Stability preserves declaration order among
         * children that share the same z_index.
         */
        std::vector<int> child_indices_in_paint_order() const;
        //layout box
        const LayoutBox& layout_box() const;
        LayoutBox& edit_layout_box();
        //absolute positioning
        /**
         * @brief Returns the positioning mode (block flow or absolute).
         */
        DocraftPositionType position_mode() const;
        /**
         * @brief Sets the positioning mode. Absolute nodes are placed at explicit_position()
         * during layout and do not advance the parent's cursor.
         */
        void set_position_mode(DocraftPositionType position_mode);
        /**
         * @brief Returns the explicit position used when position_mode() is kAbsolute.
         */
        const Position& explicit_position() const;
        /**
         * @brief Sets the explicit position used when position_mode() is kAbsolute.
         */
        void set_explicit_position(const Position& position);

        /**
         * @brief Returns the paint-order index. Higher values paint later (on top);
         * default 0 makes every existing node a no-op for z-index sorting.
         */
        int z_index() const;
        void set_z_index(int z_index);

        /**
         * @brief Returns the node's optional name (empty by default), settable from
         * Craft-language's `name` attribute for later lookup/debugging.
         */
        const std::string& name() const;
        void set_name(const std::string& name);

        /**
         * @brief Inset (in points, uniform on every side) between this node's own box
         * edge and where it lays out its children -- e.g. DocraftLoomRectangle offsets
         * its children's start position by padding() and grows its own measured size
         * by 2x padding() around them. A no-op default for node types that don't read
         * it (leaves like Text/Image/Circle).
         */
        float padding() const;
        void set_padding(float padding);

        /**
         * @brief Extra space (in points, per edge) this node asks its parent's
         * stacking layout to reserve around it, on top of whatever gap the parent's
         * own spacing()/spacing-like property already adds between children (see
         * DocraftLoomLayoutContainer::spacing()) -- e.g. a `<Title>` can set a larger
         * margin() than a plain `<Text>`/`<Paragraph>` so it reads as more separated
         * from what follows it, regardless of which container it's stacked in.
         * Containers combine the touching edges of two adjacent children via max(),
         * not sum (mirrors CSS margin collapsing), so two neighbors both asking for
         * breathing room don't double up -- vertical stacks (Rectangle/VStack) combine
         * bottom/top, horizontal stacks (HStack) combine right/left.
         */
        const DocraftLoomMargin& margin() const;
        void set_margin(float margin);
        void set_margin(float top, float right, float bottom, float left);

    private:
        std::vector<std::shared_ptr<DocraftLoomNode>> children_;
        LayoutBox layout_box_ = {};
        DocraftPositionType position_mode_ = DocraftPositionType::kBlock;
        Position explicit_position_{0.0F, 0.0F};
        int z_index_ = 0;
        std::string name_;
        float padding_ = 0.0F;
        DocraftLoomMargin margin_;
    };
} // docraft
