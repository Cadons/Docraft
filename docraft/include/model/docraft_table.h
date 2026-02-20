#pragma once
#include <optional>
#include "docraft_text.h"
#include "docraft_color.h"
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
         * @brief Sets the number of content columns (data cells per row).
         * @param cols Content column count.
         */
        void set_content_cols(int cols);
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
         * @brief Sets a header title node by index (vertical tables only).
         * @param index Header title index.
         * @param node Header title node.
         */
        void set_htitle_node(int index, const std::shared_ptr<DocraftText> &node);
        /**
         * @brief Sets a content node by flat index.
         * @param index Content index (row-major).
         * @param node Content node.
         */
        void set_content_node(int index, const std::shared_ptr<DocraftNode> &node);
        /**
         * @brief Sets a content node background by flat index.
         * @param index Content index (row-major).
         * @param color Background color.
         */
        void set_content_node_background(int index, const DocraftColor &color);
        /**
         * @brief Sets all title nodes.
         * @param nodes Vector of title nodes.
         */
        void set_title_nodes(const std::vector<std::shared_ptr<DocraftText>> &nodes);
        /**
         * @brief Sets all header title nodes (vertical tables only).
         * @param nodes Vector of header title nodes.
         */
        void set_htitle_nodes(const std::vector<std::shared_ptr<DocraftText>> &nodes);
        /**
         * @brief Sets all content nodes.
         * @param nodes Vector of content nodes.
         */
        void set_content_nodes(const std::vector<std::shared_ptr<DocraftNode>> &nodes);
        /**
         * @brief Adds a title node.
         * @param node Title node.
         */
        void add_title_node(const std::shared_ptr<DocraftText> &node,
                            std::optional<DocraftColor> background = std::nullopt);
        /**
         * @brief Adds a header title node (vertical tables only).
         * @param node Header title node.
         */
        void add_htitle_node(const std::shared_ptr<DocraftText> &node,
                             std::optional<DocraftColor> background = std::nullopt);
        /**
         * @brief Adds a content node.
         * @param node Content node.
         */
        void add_content_node(const std::shared_ptr<DocraftNode> &node,
                              std::optional<DocraftColor> background = std::nullopt);
        /**
         * @brief Adds a row background.
         * @param background Optional background color.
         */
        void add_row_background(std::optional<DocraftColor> background);
        /**
         * @brief Sets a row background.
         * @param index Row index.
         * @param background Background color.
         */
        void set_row_background(int index, const DocraftColor &background);
        /**
         * @brief Sets the default background for data cells.
         * @param background Optional background color.
         */
        void set_default_cell_background(std::optional<DocraftColor> background);
        /**
         * @brief Sets a template placeholder for JSON model data.
         * @param model_template Template string (e.g. "${prodotti}").
         */
        void set_model_template(const std::string &model_template);
        /**
         * @brief Sets a template placeholder for JSON header data.
         * @param header_template Template string (e.g. "${prodotti_header}").
         */
        void set_header_template(const std::string &header_template);
        /**
         * @brief Returns whether a model template is set.
         * @return true if template is set.
         */
        [[nodiscard]] bool has_model_template() const;
        /**
         * @brief Returns whether a header template is set.
         * @return true if template is set.
         */
        [[nodiscard]] bool has_header_template() const;
        /**
         * @brief Returns the model template string.
         * @return Template string.
         */
        [[nodiscard]] const std::string &model_template() const;
        /**
         * @brief Returns the header template string.
         * @return Template string.
         */
        [[nodiscard]] const std::string &header_template() const;
        /**
         * @brief Clears the model template string.
         */
        void clear_model_template();
        /**
         * @brief Clears the header template string.
         */
        void clear_header_template();
        /**
         * @brief Applies JSON matrix model to the table (horizontal only).
         * @param json_str JSON matrix string.
         */
        void apply_json_rows(const std::string &json_str);
        /**
         * @brief Applies JSON header array to the table (horizontal only).
         * @param json_str JSON array string.
         */
        void apply_json_header(const std::string &json_str);
        /**
         * @brief Splits the table after the given number of rows.
         * @param rows_to_keep Number of rows to keep in the current table.
         * @param repeat_header Whether to repeat titles/header in the remainder table.
         * @return New table containing the remaining rows, or nullptr if no split.
         */
        std::shared_ptr<DocraftTable> split_after_row(std::size_t rows_to_keep, bool repeat_header);

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
         * @brief Returns the number of content columns.
         * @return Content column count.
         */
        [[nodiscard]] int content_cols() const;

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
         * @brief Returns header title nodes (vertical tables only).
         * @return Vector of header title nodes.
         */
        [[nodiscard]] const std::vector<std::shared_ptr<DocraftText>> &htitle_nodes() const;
        /**
         * @brief Returns content nodes as a 2D array.
         * @return 2D vector of content nodes.
         */
        [[nodiscard]] std::vector<std::vector<std::shared_ptr<DocraftNode>>>content_nodes() const;
        /**
         * @brief Returns title cell background colors.
         * @return Vector of optional title background colors.
         */
        [[nodiscard]] const std::vector<std::optional<DocraftColor>> &title_backgrounds() const;
        /**
         * @brief Returns header title cell background colors.
         * @return Vector of optional header title background colors.
         */
        [[nodiscard]] const std::vector<std::optional<DocraftColor>> &htitle_backgrounds() const;
        /**
         * @brief Returns content cell background colors.
         * @return Vector of optional content background colors.
         */
        [[nodiscard]] const std::vector<std::optional<DocraftColor>> &content_backgrounds() const;
        /**
         * @brief Returns row background colors.
         * @return Vector of optional row background colors.
         */
        [[nodiscard]] const std::vector<std::optional<DocraftColor>> &row_backgrounds() const;
        /**
         * @brief Returns the default background for data cells.
         * @return Optional background color.
         */
        [[nodiscard]] const std::optional<DocraftColor> &default_cell_background() const;
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
        int content_cols_ = 0;
        LayoutOrientation orientation_ = LayoutOrientation::kHorizontal;
        //Horizontal means that the title are on the top, vertical creates rows with titles and items are in n columns (generally usde for nx2 tables)
        std::vector<float> column_weights_;
        std::vector<float> row_weights_;
        std::vector<std::string> titles_;
        std::vector<std::shared_ptr<DocraftText>> title_nodes_;
        std::vector<std::shared_ptr<DocraftText>> htitle_nodes_;
        std::vector<std::shared_ptr<DocraftNode>> content_nodes_;
        std::vector<std::optional<DocraftColor>> title_backgrounds_;
        std::vector<std::optional<DocraftColor>> htitle_backgrounds_;
        std::vector<std::optional<DocraftColor>> content_backgrounds_;
        std::vector<std::optional<DocraftColor>> row_backgrounds_;
        std::optional<DocraftColor> default_cell_background_;
        std::optional<std::string> model_template_;
        std::optional<std::string> header_template_;
        float baseline_offset_ = 0.25F;
    };
} // docraft
