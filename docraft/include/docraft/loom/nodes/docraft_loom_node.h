//
// Created by Matteo on 21/06/2026.
//

#pragma once
#include <vector>
#include <memory>
#include "docraft/docraft_lib.h"
#include "docraft/loom/interfaces/docraft_loom_visitor.h"
#include "docraft/model/docraft_position.h"

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
     * @brief Struct representing a position with x and y coordinates.
     */
    struct Position
    {
        float x;
        float y;
    };

    /**
     * @brief Struct representing a rectangle with a position and size.
     */
    struct Rect
    {
        Position position;
        Size size;
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
        model::DocraftPositionType position_mode() const;
        /**
         * @brief Sets the positioning mode. Absolute nodes are placed at explicit_position()
         * during layout and do not advance the parent's cursor.
         */
        void set_position_mode(model::DocraftPositionType position_mode);
        /**
         * @brief Returns the explicit position used when position_mode() is kAbsolute.
         */
        const Position& explicit_position() const;
        /**
         * @brief Sets the explicit position used when position_mode() is kAbsolute.
         */
        void set_explicit_position(const Position& position);

    private:
        std::vector<std::shared_ptr<DocraftLoomNode>> children_;
        LayoutBox layout_box_ = {};
        model::DocraftPositionType position_mode_ = model::DocraftPositionType::kBlock;
        Position explicit_position_{0.0F, 0.0F};
    };
} // docraft
