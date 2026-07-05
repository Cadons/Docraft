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

    private:
        std::vector<std::shared_ptr<DocraftLoomNode>> children_;
        LayoutBox layout_box_ = {};
        DocraftPositionType position_mode_ = DocraftPositionType::kBlock;
        Position explicit_position_{0.0F, 0.0F};
        int z_index_ = 0;
        std::string name_;
    };
} // docraft
