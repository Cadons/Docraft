#pragma once
#include "docraft_text.h"
#include "model/docraft_children_container_node.h"
#include "model/docraft_layout.h"

namespace docraft::model {
    /**
     * @brief Table node with titles and cell content.
     *
     * Supports horizontal or vertical models, with per-row/column weights
     * and title/content nodes stored separately.
     */
    class DocraftTable : public DocraftNode {
    public:
        /**
         * @brief Creates an empty table.
         */
        DocraftTable();

        ~DocraftTable() override = default;

        /**
         * @brief Draws the table using the provided context.
         * @param context Document context.
         */
        void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;

        /**
         * @brief Sets the number of rows.
         * @param rows Row count.
         */
        void set_rows(int rows);

        /**
         * @brief Sets the number of columns.
         * @param cols Column count.
         */
        void set_cols(int cols);

        /**
         * @brief Sets the table orientation.
         * @param orientation Layout orientation.
         */
        void set_orientation(LayoutOrientation orientation);
        /**
         * @brief Sets the weight for a column.
         * @param index Column index.
         * @param weight Weight value.
         */
        void set_column_weight(int index, float weight);
        /**
         * @brief Sets the weight for a row.
         * @param index Row index.
         * @param weight Weight value.
         */
        void set_row_weight(int index, float weight);
        /**
         * @brief Sets the baseline offset for text alignment.
         * @param offset Offset value.
         */
        void set_baseline_offset(float offset);
        /**
         * @brief Sets a column title string by index.
         * @param index Column index.
         * @param title Title text.
         */
        void set_title(int index, const std::string &title);
        /**
         * @brief Sets all column titles.
         * @param titles Vector of titles.
         */
        void set_titles(const std::vector<std::string> &titles);
        /**
         * @brief Sets all column weights.
         * @param weights Vector of weights.
         */
        void set_column_weights(const std::vector<float> &weights);
        /**
         * @brief Sets all row weights.
         * @param weights Vector of weights.
         */
        void set_row_weights(const std::vector<float> &weights);
        /**
         * @brief Sets a title node by index.
         * @param index Column index.
         * @param node Title node.
         */
        void set_title_node(int index, const std::shared_ptr<DocraftText> &node);
        /**
         * @brief Sets a content node by flat index.
         * @param index Content index (row-major).
         * @param node Content node.
         */
        void set_content_node(int index, const std::shared_ptr<DocraftNode> &node);
        /**
         * @brief Sets all title nodes.
         * @param nodes Vector of title nodes.
         */
        void set_title_nodes(const std::vector<std::shared_ptr<DocraftText>> &nodes);
        /**
         * @brief Sets all content nodes.
         * @param nodes Vector of content nodes.
         */
        void set_content_nodes(const std::vector<std::shared_ptr<DocraftNode>> &nodes);
        /**
         * @brief Adds a title node.
         * @param node Title node.
         */
        void add_title_node(const std::shared_ptr<DocraftText> &node);
        /**
         * @brief Adds a content node.
         * @param node Content node.
         */
        void add_content_node(const std::shared_ptr<DocraftNode> &node);

        /**
         * @brief Returns the number of rows.
         * @return Row count.
         */
        [[nodiscard]] int rows() const;

        /**
         * @brief Returns the number of columns.
         * @return Column count.
         */
        [[nodiscard]] int cols() const;

        /**
         * @brief Returns the table orientation.
         * @return Orientation enum.
         */
        [[nodiscard]] LayoutOrientation orientation() const;
        /**
         * @brief Returns title strings.
         * @return Vector of titles.
         */
        [[nodiscard]] const std::vector<std::string> &titles() const;
        /**
         * @brief Returns column weights.
         * @return Vector of column weights.
         */
        [[nodiscard]] const std::vector<float> &column_weights() const;
        /**
         * @brief Returns row weights.
         * @return Vector of row weights.
         */
        [[nodiscard]] const std::vector<float> &row_weights() const;
        /**
         * @brief Returns the last row-weight index used.
         * @return Index of last row weight.
         */
        [[nodiscard]] int last_row_weight_index() const;
        /**
         * @brief Returns the last column-weight index used.
         * @return Index of last column weight.
         */
        [[nodiscard]] int last_column_weight_index() const;
        /**
         * @brief Returns title nodes.
         * @return Vector of title nodes.
         */
        [[nodiscard]] const std::vector<std::shared_ptr<DocraftText>> &title_nodes() const;
        /**
         * @brief Returns content nodes as a 2D array.
         * @return 2D vector of content nodes.
         */
        [[nodiscard]] std::vector<std::vector<std::shared_ptr<DocraftNode>>>content_nodes() const;
        /**
         * @brief Returns the last title node index.
         * @return Index of last title node.
         */
        [[nodiscard]] int last_title_node_index() const;
        /**
         * @brief Returns the last content node index.
         * @return Index of last content node.
         */
        [[nodiscard]] int last_content_node_index() const;
        /**
         * @brief Returns the baseline offset.
         * @return Baseline offset.
         */
        [[nodiscard]] float baseline_offset() const;

    private:
        int rows_;
        int cols_;
        LayoutOrientation orientation_ = LayoutOrientation::kHorizontal;
        //Horizontal means that the title are on the top, vertical creates rows with titles and items are in n columns (generally usde for nx2 tables)
        std::vector<float> column_weights_;
        std::vector<float> row_weights_;
        std::vector<std::string> titles_;
        std::vector<std::shared_ptr<DocraftText>> title_nodes_;
        std::vector<std::shared_ptr<DocraftNode>> content_nodes_;
        float baseline_offset_ = 0.25F;
    };
} // docraft
