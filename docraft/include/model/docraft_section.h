#pragma once

#include "docraft_rectangle.h"
#include "model/docraft_children_container_node.h"

namespace docraft::model {
    /**
     * @brief Rectangular document section with margin configuration.
     *
     * Used as a base for header, body, and footer containers.
     */
    class DocraftSection : public DocraftRectangle {
    public:
        using DocraftRectangle::DocraftRectangle;
        DocraftSection();
        DocraftSection(const DocraftSection& node) = default;
        /**
         * @brief Constructs from an existing node pointer (copy).
         */
        explicit DocraftSection(DocraftSection* node);
        /**
         * @brief Draws the section using the provided context.
         * @param context Document context.
         */
        void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;

        //getter
        /**
         * @brief Returns the uniform margins value.
         * @return Margin value in points.
         */
        [[nodiscard]] float margins() const;
        /**
         * @brief Returns the top margin.
         * @return Margin in points.
         */
        [[nodiscard]] float margin_top() const;
        /**
         * @brief Returns the bottom margin.
         * @return Margin in points.
         */
        [[nodiscard]] float margin_bottom() const;
        /**
         * @brief Returns the left margin.
         * @return Margin in points.
         */
        [[nodiscard]] float margin_left() const;
        /**
         * @brief Returns the right margin.
         * @return Margin in points.
         */
        [[nodiscard]] float margin_right() const;
        /**
         * @brief Sets all margins to the same value.
         * @param margins Margin in points.
         */
        void set_margins(float margins);
        /**
         * @brief Sets the top margin.
         * @param margin_top Margin in points.
         */
        void set_margin_top(float margin_top);
        /**
         * @brief Sets the bottom margin.
         * @param margin_bottom Margin in points.
         */
        void set_margin_bottom(float margin_bottom);
        /**
         * @brief Sets the left margin.
         * @param margin_left Margin in points.
         */
        void set_margin_left(float margin_left);
        /**
         * @brief Sets the right margin.
         * @param margin_right Margin in points.
         */
        void set_margin_right(float margin_right);

    private:
        float margins_ = 0.0F;
        float margin_top_ = 0.0F;
        float margin_bottom_ = 0.0F;
        float margin_left_ = 10.0F;
        float margin_right_ = 10.0F;
    };
} // Docraft
