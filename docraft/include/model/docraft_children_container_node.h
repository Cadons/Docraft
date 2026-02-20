#pragma once
#include "docraft_node.h"
#include <vector>
namespace docraft::model {
    /**
     * @brief Node that owns and manages a list of child nodes.
     *
     * Used by composite nodes such as layout, rectangles, lists, and text.
     */
    class DocraftChildrenContainerNode : public DocraftNode {
    public:
        using DocraftNode::DocraftNode;

        DocraftChildrenContainerNode(const DocraftChildrenContainerNode& node) = default;
        /**
         * @brief Constructs from an existing node pointer (copy).
         */
        explicit DocraftChildrenContainerNode(DocraftChildrenContainerNode* node);

        /**
         * @brief Adds a child node.
         * @param child Child node to add.
         */
        void add_child(const std::shared_ptr<DocraftNode>& child);
        /**
         * @brief Inserts a child node at the given index.
         * @param index Index to insert at (clamped to size).
         * @param child Child node to insert.
         */
        void insert_child(std::size_t index, const std::shared_ptr<DocraftNode>& child);
        /**
         * @brief Removes a child node.
         * @param child Child node to remove.
         */
        void remove_child(const std::shared_ptr<DocraftNode>& child);
        /**
         * @brief Returns all child nodes.
         * @return Vector of child nodes.
         */
        [[nodiscard]]const std::vector<std::shared_ptr<DocraftNode>>& children() const;
        /**
         * @brief Propagates x position to children.
         * @param x New x coordinate.
         */
        void set_x_for_children(float x) override;
        /**
         * @brief Propagates y position to children.
         * @param y New y coordinate.
         */
        void set_y_for_children(float y) override;
    protected:
        /**
         * @brief Draws all children using the provided context.
         * @param context Document context.
         */
        virtual void draw_children(const std::shared_ptr<DocraftDocumentContext> &context) const;

        /**
         * @brief Called when a child is added to the container
         */
        virtual void on_child_added();

        /**
         * @brief Called when a child is removed from the container
         * @param index Index of the removed child.
         */
        virtual void on_child_removed(int index);
        /**
         * @brief Removes all child nodes.
         */
        void clear_children();
    private:
        std::vector<std::shared_ptr<DocraftNode>> children_;
    };
} // docraft
