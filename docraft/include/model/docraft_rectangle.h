#pragma once
#include "docraft_children_container_node.h"
#include "docraft_color.h"
#include "docraft_node.h"

namespace docraft::model {
    class DocraftRectangle : public DocraftChildrenContainerNode {
    public:
        using DocraftChildrenContainerNode::DocraftChildrenContainerNode;
        void draw(const std::shared_ptr<DocraftPDFContext> &context) override;
        void set_background_color(const DocraftColor &color);
        void set_border_color(const DocraftColor &color);
        void set_border_width(float width);
        const DocraftColor &background_color() const;
        const DocraftColor &border_color() const;
        float border_width() const;


    private:
        DocraftColor background_color_= DocraftColor(0,0,0,0); //transparent;
        DocraftColor border_color_= DocraftColor(0,0,0,0);
        float border_width_= 1.0F;
    };
} // docraft
