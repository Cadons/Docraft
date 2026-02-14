#include "model/docraft_table.h"

#include <utility>

#include "renderer/docraft_pdf_renderer.h"

namespace docraft::model {
    DocraftTable::DocraftTable() {
        set_auto_fill_height(false);
    }

    void DocraftTable::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
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

    void DocraftTable::set_baseline_offset(const float offset) {
        baseline_offset_ = offset;
    }

    void DocraftTable::set_title(int index, const std::string &title) {
        if (index < 0 || std::cmp_greater_equal(index, titles_.size())) {
            throw std::out_of_range("Title index out of range");
        }
        titles_[index] = title;
    }

    void DocraftTable::set_titles(const std::vector<std::string> &titles) {
        titles_ = titles;
        if (orientation_ == LayoutOrientation::kHorizontal) {
            set_cols(static_cast<int>(titles_.size()));
        } else {
            set_rows(static_cast<int>(titles_.size()));
        }
    }

    void DocraftTable::set_content_cols(int cols) {
        content_cols_ = cols;
    }

    void DocraftTable::set_column_weights(const std::vector<float> &column_weights) {
        column_weights_ = column_weights;
    }

    void DocraftTable::set_row_weights(const std::vector<float> &row_weights) {
        row_weights_ = row_weights;
    }

    void DocraftTable::add_title_node(const std::shared_ptr<DocraftText> &node,
                                      std::optional<DocraftColor> background) {
        title_nodes_.emplace_back(node);
        title_backgrounds_.emplace_back(std::move(background));
    }

    void DocraftTable::add_htitle_node(const std::shared_ptr<DocraftText> &node,
                                       std::optional<DocraftColor> background) {
        htitle_nodes_.emplace_back(node);
        htitle_backgrounds_.emplace_back(std::move(background));
    }

    void DocraftTable::add_content_node(const std::shared_ptr<DocraftNode> &node,
                                        std::optional<DocraftColor> background) {
        content_nodes_.emplace_back(node);
        content_backgrounds_.emplace_back(std::move(background));
        if (orientation_ == LayoutOrientation::kHorizontal && content_cols() > 0) {
            const int cols = content_cols();
            const int inferred_rows = static_cast<int>((content_nodes_.size() + cols - 1) / cols);//inferred rows is the number of rows needed to fit all content nodes given the number of columns
            set_rows(inferred_rows);
        }
    }

    void DocraftTable::set_content_node(int index, const std::shared_ptr<DocraftNode> &node) {
        if (index < 0 || std::cmp_greater_equal(index, content_nodes_.size())) {
            throw std::out_of_range("Content node index out of range");
        }
        content_nodes_[index] = node;
    }

    void DocraftTable::set_content_node_background(int index, const DocraftColor &color) {
        if (index < 0 || std::cmp_greater_equal(index, content_backgrounds_.size())) {
            throw std::out_of_range("Content background index out of range");
        }
        content_backgrounds_[index] = color;
    }

    void DocraftTable::set_content_nodes(const std::vector<std::shared_ptr<DocraftNode> > &nodes) {
        content_nodes_ = nodes;
        content_backgrounds_.resize(nodes.size());
        if (orientation_ == LayoutOrientation::kHorizontal && content_cols() > 0) {
            const int cols = content_cols();
            const int inferred_rows = static_cast<int>((nodes.size() + cols - 1) / cols);
            set_rows(inferred_rows);
        }
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

    void DocraftTable::set_htitle_node(int index, const std::shared_ptr<DocraftText> &node) {
        if (index < 0 || std::cmp_greater_equal(index, htitle_nodes_.size())) {
            throw std::out_of_range("Header title node index out of range");
        }
        if (!node) {
            throw std::invalid_argument("Header title node cannot be null");
        }
        htitle_nodes_[index] = node;
    }

    void DocraftTable::set_title_nodes(const std::vector<std::shared_ptr<DocraftText>> &nodes) {
        if (nodes.size() != titles_.size()) {
            throw std::invalid_argument("Number of title nodes must be equal to number of titles");
        }
        title_nodes_ = nodes;
        title_backgrounds_.resize(nodes.size());
    }

    void DocraftTable::set_htitle_nodes(const std::vector<std::shared_ptr<DocraftText>> &nodes) {
        htitle_nodes_ = nodes;
        htitle_backgrounds_.resize(nodes.size());
    }

    void DocraftTable::add_row_background(std::optional<DocraftColor> background) {
        row_backgrounds_.emplace_back(std::move(background));
    }

    void DocraftTable::set_row_background(int index, const DocraftColor &background) {
        if (index < 0 || std::cmp_greater_equal(index, row_backgrounds_.size())) {
            throw std::out_of_range("Row background index out of range");
        }
        row_backgrounds_[index] = background;
    }

    void DocraftTable::set_default_cell_background(std::optional<DocraftColor> background) {
        default_cell_background_ = std::move(background);
    }
#pragma endregion
#pragma region getter
    int DocraftTable::rows() const {
        return rows_;
    }

    int DocraftTable::cols() const {
        return cols_;
    }

    int DocraftTable::content_cols() const {
        if (content_cols_ > 0) {
            return content_cols_;
        }
        if (orientation_ == LayoutOrientation::kVertical) {
            return std::max(1, cols_ - 1);
        }
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

    const std::vector<std::shared_ptr<DocraftText>> &DocraftTable::htitle_nodes() const {
        return htitle_nodes_;
    }

    std::vector<std::vector<std::shared_ptr<DocraftNode>>> DocraftTable::content_nodes() const {
        std::vector<std::vector<std::shared_ptr<DocraftNode>>> result;
        const int content_cols = this->content_cols();
        for (int r = 0; r < rows_; ++r) {
            std::vector<std::shared_ptr<DocraftNode>> row;
            for (int c = 0; c < content_cols; ++c) {
                int index = r * content_cols + c;
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

    float DocraftTable::baseline_offset() const {
        return baseline_offset_;
    }

    const std::vector<std::optional<DocraftColor>> &DocraftTable::title_backgrounds() const {
        return title_backgrounds_;
    }

    const std::vector<std::optional<DocraftColor>> &DocraftTable::htitle_backgrounds() const {
        return htitle_backgrounds_;
    }

    const std::vector<std::optional<DocraftColor>> &DocraftTable::content_backgrounds() const {
        return content_backgrounds_;
    }

    const std::vector<std::optional<DocraftColor>> &DocraftTable::row_backgrounds() const {
        return row_backgrounds_;
    }

    const std::optional<DocraftColor> &DocraftTable::default_cell_background() const {
        return default_cell_background_;
    }

#pragma endregion
} // docraft
