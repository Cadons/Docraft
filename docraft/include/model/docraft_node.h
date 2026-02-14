#pragma once
#include <string>

#include "docraft_document_context.h"
#include "docraft_position.h"

namespace docraft::model {
    /**
     * @brief Base class for all document nodes.
     *
     * A node carries layout properties (position, size, weight, autofill flags)
     * and exposes a draw hook used by renderers/painters.
     */
    class DocraftNode : public DocraftTransform {
    public:
        /**
         * @brief Creates a node with a unique id.
         */
        explicit DocraftNode();

        /**
         * @brief Copy-constructs a node.
         */
        DocraftNode(const DocraftNode &node);

        /**
         * @brief Constructs a node from an existing pointer (copy).
         */
        explicit DocraftNode(const DocraftNode *node);

        virtual ~DocraftNode() = default;

        /**
         * @brief Draws the node using the provided context.
         */
        virtual void draw(const std::shared_ptr<DocraftDocumentContext> &context) = 0;//probably unuseful

        //getter
        /**
         * @brief Returns the node id.
         * @return Unique node id.
         */
        [[nodiscard]] int id() const;

        /**
         * @brief Returns the node name.
         * @return Node name string.
         */
        [[nodiscard]] const std::string &node_name() const;


        /**
         * @brief Returns whether height is automatically filled.
         * @return true if height auto-fill is enabled.
         */
        [[nodiscard]] bool auto_fill_height() const;

        /**
         * @brief Returns whether width is automatically filled.
         * @return true if width auto-fill is enabled.
         */
        [[nodiscard]] bool auto_fill_width() const;

        /**
         * @brief Returns the layout weight.
         * @return Weight value, -1.0 if not set.
         */
        [[nodiscard]] float weight() const;

        /**
         * @brief Returns the positioning mode.
         * @return Position mode enum.
         */
        [[nodiscard]] DocraftPositionType position_mode() const;

        //setter
        /**
         * @brief Sets the node name.
         * @param name New name.
         */
        void set_name(const std::string &name);

        /**
         * @brief Sets whether height is auto-filled.
         * @param auto_fill_height true to enable auto-fill.
         */
        void set_auto_fill_height(bool auto_fill_height);

        /**
         * @brief Sets whether width is auto-filled.
         * @param auto_fill_width true to enable auto-fill.
         */
        void set_auto_fill_width(bool auto_fill_width);

        /**
         * @brief Propagates x position to children.
         * @param x New x coordinate for children.
         */
        virtual void set_x_for_children(float x);

        /**
         * @brief Propagates y position to children.
         * @param y New y coordinate for children.
         */
        virtual void set_y_for_children(float y);
        
        /**
         * @brief Sets the layout weight.
         * @param weight Weight value, -1 disables weighting.
         */
        void set_weight(float weight);

        /**
         * @brief Sets the positioning mode.
         * @param position_mode Positioning mode enum.
         */
        void set_position_mode(DocraftPositionType position_mode);

    private:
        int id_ = 0;
        static int next_id_;
        std::string node_name_;
        bool auto_fill_height_ = true;
        bool auto_fill_width_ = true;
        float weight_ = -1.0F;// -1 means no weight assigned
        DocraftTransform transform_box_;
        DocraftPositionType position_mode_ = DocraftPositionType::kBlock;
    };
} // Docraft
