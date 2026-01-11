#include "model/docraft_table.h"

#include <utility>

#include "renderer/docraft_pdf_renderer.h"

namespace docraft::model {
    DocraftTable::DocraftTable() {
        set_auto_fill_height(false);
    }

    void DocraftTable::draw(const std::shared_ptr<DocraftPDFContext> &context) {
        if (column_weights_.empty()) {
            float default_weight = 1.0F;
            column_weights_.resize(cols_, default_weight);
        }
        if (row_weights_.empty()) {
            float default_weight = 1.0F;
            row_weights_.resize(rows_, default_weight);
        }
        context->renderer()->render_table(*this);
    }

#pragma region setter
    void DocraftTable::set_rows(int rows) {
        rows_ = rows;
    }

    void DocraftTable::set_cols(int cols) {
        cols_ = cols;
    }

    void DocraftTable::set_orientation(LayoutOrientation orientation) {
        orientation_ = orientation;
    }

    void DocraftTable::set_column_weight(int index, float weight) {
        if (index < 0 || std::cmp_greater_equal(index, column_weights_.size())) {
            throw std::out_of_range("Column weight index out of range");
        }
        column_weights_[index] = weight;
    }

    void DocraftTable::set_row_weight(int index, float weight) {
        if (index < 0 || std::cmp_greater_equal(index, row_weights_.size())) {
            throw std::out_of_range("Row weight index out of range");
        }
        row_weights_[index] = weight;
    }

    void DocraftTable::set_title(int index, const std::string &title) {
        if (index < 0 || std::cmp_greater_equal(index, titles_.size())) {
            throw std::out_of_range("Title index out of range");
        }
        titles_[index] = title;
    }

    void DocraftTable::set_titles(const std::vector<std::string> &titles) {
        titles_ = titles;
        set_cols(titles_.size());
    }

    void DocraftTable::set_column_weights(const std::vector<float> &column_weights) {
        column_weights_ = column_weights;
    }

    void DocraftTable::set_row_weights(const std::vector<float> &row_weights) {
        row_weights_ = row_weights;
    }

    void DocraftTable::add_title_node(const std::shared_ptr<DocraftText> &node) {
        title_nodes_.emplace_back(node);
    }

    void DocraftTable::add_content_node(const std::shared_ptr<DocraftNode> &node) {
        content_nodes_.emplace_back(node);
        set_rows(static_cast<int>(content_nodes_.size() / cols_));
    }

    void DocraftTable::set_content_node(int index, const std::shared_ptr<DocraftNode> &node) {
        if (index < 0 || std::cmp_greater_equal(index, content_nodes_.size())) {
            throw std::out_of_range("Content node index out of range");
        }
        content_nodes_[index] = node;
    }

    void DocraftTable::set_content_nodes(const std::vector<std::shared_ptr<DocraftNode> > &nodes) {
        content_nodes_ = nodes;
        set_rows(static_cast<int>(nodes.size() / cols_));
    }

    void DocraftTable::set_title_node(int index, const std::shared_ptr<DocraftText> &node) {
        if (index < 0 || std::cmp_greater_equal(index, title_nodes_.size())) {
            throw std::out_of_range("Title node index out of range");
        }
        if (!node) {
            throw std::invalid_argument("Title node cannot be null");
        }
        title_nodes_[index] = node;
    }

    void DocraftTable::set_title_nodes(const std::vector<std::shared_ptr<DocraftText>> &nodes) {
        if (nodes.size() != titles_.size()) {
            throw std::invalid_argument("Number of title nodes must be equal to number of titles");
        }
        title_nodes_ = nodes;
    }
#pragma endregion
#pragma region getter
    int DocraftTable::rows() const {
        return rows_;
    }

    int DocraftTable::cols() const {
        return cols_;
    }

    LayoutOrientation DocraftTable::orientation() const {
        return orientation_;
    }

    const std::vector<std::string> &DocraftTable::titles() const {
        return titles_;
    }

    const std::vector<float> &DocraftTable::column_weights() const {
        return column_weights_;
    }

    const std::vector<float> &DocraftTable::row_weights() const {
        return row_weights_;
    }

    int DocraftTable::last_row_weight_index() const {
        if (row_weights_.empty()) {
            return -1;
        }
        return static_cast<int>(row_weights_.size()) - 1;
    }

    int DocraftTable::last_column_weight_index() const {
        if (column_weights_.empty()) {
            return -1;
        }
        return static_cast<int>(column_weights_.size()) - 1;
    }

    const std::vector<std::shared_ptr<DocraftText>> &DocraftTable::title_nodes() const {
        return title_nodes_;
    }

    std::vector<std::vector<std::shared_ptr<DocraftNode>>> DocraftTable::content_nodes() const {
        std::vector<std::vector<std::shared_ptr<DocraftNode>>> result;
        for (int r = 0; r < rows_; ++r) {
            std::vector<std::shared_ptr<DocraftNode>> row;
            for (int c = 0; c < cols_; ++c) {
                int index = r * cols_ + c;
                if (index < content_nodes_.size()) {
                    row.emplace_back(content_nodes_[index]);
                } else {
                    row.emplace_back(nullptr);
                }
            }
            result.emplace_back(row);
        }
        return result;
    }

    int DocraftTable::last_title_node_index() const {
        return title_nodes_.size() - 1;
    }

    int DocraftTable::last_content_node_index() const {
        return content_nodes_.size() - 1;
    }

#pragma endregion
} // docraft
