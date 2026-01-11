#pragma once
#include "docraft_node.h"
#include <vector>
namespace docraft::model {
    class DocraftChildrenContainerNode : public DocraftNode {
    public:
        using DocraftNode::DocraftNode;

        DocraftChildrenContainerNode(const DocraftChildrenContainerNode& node) = default;
        explicit DocraftChildrenContainerNode(DocraftChildrenContainerNode* node);

        void add_child(const std::shared_ptr<DocraftNode>& child);
        void remove_child(const std::shared_ptr<DocraftNode>& child);
        [[nodiscard]]const std::vector<std::shared_ptr<DocraftNode>>& children() const;
        void set_x_for_children(float x) override;
        void set_y_for_children(float y) override;
    protected:
        virtual void draw_children(const std::shared_ptr<DocraftPDFContext> &context) const;

        /**
         * @brief Called when a child is added to the container
         */
        virtual void on_child_added();

        /**
         * @brief Called when a child is removed from the container
         */
        virtual void on_child_removed(int index);
        void clear_children();
    private:
        std::vector<std::shared_ptr<DocraftNode>> children_;
    };
} // docraft
