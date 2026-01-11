#pragma once
#include "docraft_children_container_node.h"
#include "docraft_node.h"

namespace docraft::model {
    enum class LayoutOrientation {
        kHorizontal,
        kVertical
    };

    class DocraftLayout : public DocraftChildrenContainerNode {
    public:
        using DocraftChildrenContainerNode::DocraftChildrenContainerNode;
        DocraftLayout(const DocraftLayout& node) = default;
        ~DocraftLayout() override = default;
        explicit DocraftLayout(DocraftLayout* node);
        LayoutOrientation orientation() const;

        void set_orientation(LayoutOrientation orientation);
        void draw(const std::shared_ptr<DocraftPDFContext> &context) override;
        void set_weight_for_child(int index, float weight) const;

    protected:
        void update_weights();

        void on_child_added() override;

        void on_child_removed(int index) override;

    public:
        std::vector<float> weights() const;

    private:
        LayoutOrientation orientation_;
    };
} // docraft
