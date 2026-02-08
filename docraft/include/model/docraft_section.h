#pragma once

#include "docraft_rectangle.h"
#include "model/docraft_children_container_node.h"

namespace docraft::model {
    class DocraftSection : public DocraftRectangle {
    public:
        using DocraftRectangle::DocraftRectangle;
        DocraftSection(const DocraftSection& node) = default;
        explicit DocraftSection(DocraftSection* node);

        //getter
        [[nodiscard]] float margins() const;
        [[nodiscard]] float margin_top() const;
        [[nodiscard]] float margin_bottom() const;
        [[nodiscard]] float margin_left() const;
        [[nodiscard]] float margin_right() const;
        void set_margins(float margins);
        void set_margin_top(float margin_top);
        void set_margin_bottom(float margin_bottom);
        void set_margin_left(float margin_left);
        void set_margin_right(float margin_right);

    private:
        float margins_ = 0.0F;
        float margin_top_ = 0.0F;
        float margin_bottom_ = 0.0F;
        float margin_left_ = 10.0F;
        float margin_right_ = 10.0F;
    };
} // Docraft
