#pragma once
#include <string>

#include "docraft_pdf_context.h"
#include "docraft_position.h"

namespace docraft::model {

    /**
     * @brief Base class that represent a node in the document
     */
    class DocraftNode : public DocraftTransform{
    public:
        explicit DocraftNode();

        DocraftNode(const DocraftNode &node) = default;

        explicit DocraftNode(const DocraftNode *node);

        virtual ~DocraftNode() = default;

        virtual void draw(const std::shared_ptr<DocraftPDFContext> &context) = 0;

        //getter
        [[nodiscard]] int id() const;

        [[nodiscard]] const std::string &node_name() const;


        [[nodiscard]] bool auto_fill_height() const;
        [[nodiscard]] bool auto_fill_width() const;
        [[nodiscard]] float padding() const;
        [[nodiscard]] float weight() const;
        [[nodiscard]] DocraftPositionType position_mode() const;
        //setter
        void set_name(const std::string &name);
        void set_auto_fill_height(bool auto_fill_height);
        void set_auto_fill_width(bool auto_fill_width);

        virtual void set_x_for_children(float x);
        virtual void set_y_for_children(float y);

        void set_padding(float padding);
        void set_weight(float weight);

    private:
        int id_ = 0;
        static int next_id_;
        std::string node_name_;
        float x_ = 0.0F;
        float y_ = 0.0F;
        float width_ = 0.0F;
        float height_ = 0.0F;
        bool auto_fill_height_ = true;
        bool auto_fill_width_ = true;
        float padding_ = 1.0F;
        float weight_ = 1.0F;
        DocraftTransform transform_box_;
        DocraftPositionType position_mode_ = DocraftPositionType::kBlock;
    };
} // Docraft
