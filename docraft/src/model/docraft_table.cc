#include "model/docraft_table.h"

#include <utility>
#include <nlohmann/json.hpp>
#include <stdexcept>

#include "model/docraft_clone_utils.h"
#include "renderer/docraft_pdf_renderer.h"

namespace docraft::model {
    namespace {
        std::vector<std::vector<std::string>> parse_json_matrix(const std::string &json_str) {
            nlohmann::json parsed;
            try {
                parsed = nlohmann::json::parse(json_str);
            } catch (const nlohmann::json::parse_error &e) {
                throw std::invalid_argument(
                    std::string("Table model must be a JSON matrix of strings: ") + e.what());
            }
            if (!parsed.is_array() || parsed.empty()) {
                throw std::invalid_argument("Table model must be a non-empty JSON matrix of strings");
            }
            std::vector<std::vector<std::string>> matrix;
            std::size_t expected_cols = 0;
            for (const auto &row : parsed) {
                if (!row.is_array() || row.empty()) {
                    throw std::invalid_argument("Table model must be a non-empty JSON matrix of strings");
                }
                std::vector<std::string> row_values;
                row_values.reserve(row.size());
                for (const auto &cell : row) {
                    if (!cell.is_string()) {
                        throw std::invalid_argument("Table model must contain only strings");
                    }
                    row_values.emplace_back(cell.get<std::string>());
                }
                if (expected_cols == 0) {
                    expected_cols = row_values.size();
                } else if (row_values.size() != expected_cols) {
                    throw std::invalid_argument("Table model rows must have the same number of columns");
                }
                matrix.emplace_back(std::move(row_values));
            }
            if (expected_cols == 0) {
                throw std::invalid_argument("Table model must contain at least one column");
            }
            return matrix;
        }

        std::vector<std::string> parse_json_header(const std::string &json_str) {
            nlohmann::json parsed;
            try {
                parsed = nlohmann::json::parse(json_str);
            } catch (const nlohmann::json::parse_error &e) {
                throw std::invalid_argument(
                    std::string("Table header must be a JSON array of strings: ") + e.what());
            }
            if (!parsed.is_array() || parsed.empty()) {
                throw std::invalid_argument("Table header must be a non-empty JSON array of strings");
            }
            std::vector<std::string> header;
            header.reserve(parsed.size());
            for (const auto &cell : parsed) {
                if (!cell.is_string()) {
                    throw std::invalid_argument("Table header must contain only strings");
                }
                header.emplace_back(cell.get<std::string>());
            }
            return header;
        }
    } // namespace
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

    std::shared_ptr<DocraftNode> DocraftTable::clone() const {
        auto copy = std::make_shared<DocraftTable>(*this);
        const auto &titles = title_nodes();
        for (std::size_t i = 0; i < titles.size(); ++i) {
            auto cloned = clone_node(titles[i]);
            auto text = std::dynamic_pointer_cast<DocraftText>(cloned);
            if (!text) {
                throw std::runtime_error("Title node does not clone to DocraftText");
            }
            copy->set_title_node(static_cast<int>(i), text);
        }

        const auto &htitles = htitle_nodes();
        for (std::size_t i = 0; i < htitles.size(); ++i) {
            auto cloned = clone_node(htitles[i]);
            auto text = std::dynamic_pointer_cast<DocraftText>(cloned);
            if (!text) {
                throw std::runtime_error("Header title node does not clone to DocraftText");
            }
            copy->set_htitle_node(static_cast<int>(i), text);
        }

        int content_count = static_cast<int>(content_backgrounds().size());
        auto content_rows = content_nodes();
        int current_index = 0;
        for (const auto &row : content_rows) {
            for (const auto &cell : row) {
                if (current_index >= content_count) {
                    break;
                }
                copy->set_content_node(current_index, clone_node(cell));
                ++current_index;
            }
            if (current_index >= content_count) {
                break;
            }
        }
        return copy;
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

    void DocraftTable::set_model_template(const std::string &model_template) {
        model_template_ = model_template;
    }

    void DocraftTable::set_header_template(const std::string &header_template) {
        header_template_ = header_template;
    }

    bool DocraftTable::has_model_template() const {
        return model_template_.has_value();
    }

    bool DocraftTable::has_header_template() const {
        return header_template_.has_value();
    }

    const std::string &DocraftTable::model_template() const {
        if (!model_template_.has_value()) {
            throw std::runtime_error("Model template not set");
        }
        return *model_template_;
    }

    const std::string &DocraftTable::header_template() const {
        if (!header_template_.has_value()) {
            throw std::runtime_error("Header template not set");
        }
        return *header_template_;
    }

    void DocraftTable::clear_model_template() {
        model_template_.reset();
    }

    void DocraftTable::clear_header_template() {
        header_template_.reset();
    }

    void DocraftTable::apply_json_rows(const std::string &json_str) {
        const auto matrix = parse_json_matrix(json_str);
        if (matrix.empty()) {
            throw std::invalid_argument("Table model must be a non-empty JSON matrix of strings");
        }

        const std::size_t cols = matrix.front().size();
        const std::size_t rows = matrix.size();

        if (!titles_.empty() && titles_.size() != cols) {
            throw std::invalid_argument("Table model columns do not match header size");
        }

        orientation_ = LayoutOrientation::kHorizontal;
        cols_ = static_cast<int>(cols);
        content_cols_ = static_cast<int>(cols);
        rows_ = static_cast<int>(rows);

        content_nodes_.clear();
        content_backgrounds_.clear();
        row_backgrounds_.clear();

        if (rows > 0) {
            content_nodes_.reserve(rows * cols);
            content_backgrounds_.reserve(rows * cols);
            for (const auto &row : matrix) {
                row_backgrounds_.emplace_back(std::nullopt);
                for (const auto &cell_text : row) {
                    content_nodes_.emplace_back(std::make_shared<DocraftText>(cell_text));
                    content_backgrounds_.emplace_back(std::nullopt);
                }
            }
        }
    }

    void DocraftTable::apply_json_header(const std::string &json_str) {
        const auto header = parse_json_header(json_str);
        if (header.empty()) {
            throw std::invalid_argument("Table header must be a non-empty JSON array of strings");
        }
        if (content_cols_ > 0 && static_cast<std::size_t>(content_cols_) != header.size()) {
            throw std::invalid_argument("Table header size does not match model columns");
        }

        orientation_ = LayoutOrientation::kHorizontal;
        titles_ = header;
        cols_ = static_cast<int>(header.size());
        content_cols_ = static_cast<int>(header.size());

        title_nodes_.clear();
        title_backgrounds_.clear();
        htitle_nodes_.clear();
        htitle_backgrounds_.clear();

        title_nodes_.reserve(titles_.size());
        title_backgrounds_.reserve(titles_.size());
        for (const auto &title_text : titles_) {
            auto title_node = std::make_shared<DocraftText>(title_text);
            title_node->set_alignment(TextAlignment::kCenter);
            title_node->set_style(TextStyle::kBold);
            title_nodes_.emplace_back(std::move(title_node));
            title_backgrounds_.emplace_back(std::nullopt);
        }
    }

    std::shared_ptr<DocraftTable> DocraftTable::split_after_row(std::size_t rows_to_keep, bool repeat_header) {
        if (rows_to_keep >= static_cast<std::size_t>(rows_)) {
            return nullptr;
        }

        auto remainder = std::make_shared<DocraftTable>();
        remainder->set_orientation(orientation_);
        remainder->set_baseline_offset(baseline_offset_);
        remainder->set_default_cell_background(default_cell_background_);
        remainder->set_auto_fill_height(auto_fill_height());
        remainder->set_auto_fill_width(auto_fill_width());
        if (weight() > 0.0F && weight() <= 1.0F) {
            remainder->set_weight(weight());
        }
        remainder->set_position_mode(position_mode());
        remainder->set_z_index(z_index());
        remainder->set_padding(padding());
        remainder->set_name(node_name());

        remainder->set_column_weights(column_weights_);
        remainder->set_content_cols(content_cols_);
        remainder->set_cols(cols_);

        if (repeat_header) {
            remainder->titles_ = titles_;
            remainder->title_nodes_.clear();
            remainder->title_backgrounds_.clear();
            for (std::size_t i = 0; i < title_nodes_.size(); ++i) {
                if (title_nodes_[i]) {
                    remainder->title_nodes_.emplace_back(std::make_shared<DocraftText>(*title_nodes_[i]));
                } else {
                    remainder->title_nodes_.emplace_back(nullptr);
                }
                if (i < title_backgrounds_.size()) {
                    remainder->title_backgrounds_.emplace_back(title_backgrounds_[i]);
                } else {
                    remainder->title_backgrounds_.emplace_back(std::nullopt);
                }
            }
            remainder->htitle_nodes_.clear();
            remainder->htitle_backgrounds_.clear();
            for (std::size_t i = 0; i < htitle_nodes_.size(); ++i) {
                if (htitle_nodes_[i]) {
                    remainder->htitle_nodes_.emplace_back(std::make_shared<DocraftText>(*htitle_nodes_[i]));
                } else {
                    remainder->htitle_nodes_.emplace_back(nullptr);
                }
                if (i < htitle_backgrounds_.size()) {
                    remainder->htitle_backgrounds_.emplace_back(htitle_backgrounds_[i]);
                } else {
                    remainder->htitle_backgrounds_.emplace_back(std::nullopt);
                }
            }
        }

        const std::size_t total_rows = static_cast<std::size_t>(rows_);
        const std::size_t keep_rows = std::min(rows_to_keep, total_rows);
        const std::size_t remain_rows = total_rows - keep_rows;

        if (orientation_ == LayoutOrientation::kHorizontal) {
            const std::size_t cols = static_cast<std::size_t>(content_cols());
            const std::size_t split_index = keep_rows * cols;

            std::vector<std::shared_ptr<DocraftNode>> remaining_nodes(
                content_nodes_.begin() + static_cast<std::ptrdiff_t>(split_index), content_nodes_.end());
            content_nodes_.erase(content_nodes_.begin() + static_cast<std::ptrdiff_t>(split_index), content_nodes_.end());

            std::vector<std::optional<DocraftColor>> remaining_bgs(
                content_backgrounds_.begin() + static_cast<std::ptrdiff_t>(split_index), content_backgrounds_.end());
            content_backgrounds_.erase(content_backgrounds_.begin() + static_cast<std::ptrdiff_t>(split_index),
                                       content_backgrounds_.end());

            std::vector<std::optional<DocraftColor>> remaining_row_bgs(
                row_backgrounds_.begin() + static_cast<std::ptrdiff_t>(keep_rows), row_backgrounds_.end());
            row_backgrounds_.erase(row_backgrounds_.begin() + static_cast<std::ptrdiff_t>(keep_rows),
                                   row_backgrounds_.end());

            remainder->content_nodes_ = std::move(remaining_nodes);
            remainder->content_backgrounds_ = std::move(remaining_bgs);
            remainder->row_backgrounds_ = std::move(remaining_row_bgs);

            rows_ = static_cast<int>(keep_rows);
            remainder->rows_ = static_cast<int>(remain_rows);
        } else {
            const std::size_t value_cols = static_cast<std::size_t>(content_cols());
            const std::size_t split_index = keep_rows * value_cols;

            std::vector<std::shared_ptr<DocraftNode>> remaining_nodes(
                content_nodes_.begin() + static_cast<std::ptrdiff_t>(split_index), content_nodes_.end());
            content_nodes_.erase(content_nodes_.begin() + static_cast<std::ptrdiff_t>(split_index), content_nodes_.end());

            std::vector<std::optional<DocraftColor>> remaining_bgs(
                content_backgrounds_.begin() + static_cast<std::ptrdiff_t>(split_index), content_backgrounds_.end());
            content_backgrounds_.erase(content_backgrounds_.begin() + static_cast<std::ptrdiff_t>(split_index),
                                       content_backgrounds_.end());

            std::vector<std::optional<DocraftColor>> remaining_row_bgs(
                row_backgrounds_.begin() + static_cast<std::ptrdiff_t>(keep_rows), row_backgrounds_.end());
            row_backgrounds_.erase(row_backgrounds_.begin() + static_cast<std::ptrdiff_t>(keep_rows),
                                   row_backgrounds_.end());

            std::vector<std::shared_ptr<DocraftText>> remaining_titles(
                title_nodes_.begin() + static_cast<std::ptrdiff_t>(keep_rows), title_nodes_.end());
            title_nodes_.erase(title_nodes_.begin() + static_cast<std::ptrdiff_t>(keep_rows), title_nodes_.end());

            std::vector<std::optional<DocraftColor>> remaining_title_bgs(
                title_backgrounds_.begin() + static_cast<std::ptrdiff_t>(keep_rows), title_backgrounds_.end());
            title_backgrounds_.erase(title_backgrounds_.begin() + static_cast<std::ptrdiff_t>(keep_rows),
                                     title_backgrounds_.end());

            std::vector<std::string> remaining_titles_text(
                titles_.begin() + static_cast<std::ptrdiff_t>(keep_rows), titles_.end());
            titles_.erase(titles_.begin() + static_cast<std::ptrdiff_t>(keep_rows), titles_.end());

            remainder->content_nodes_ = std::move(remaining_nodes);
            remainder->content_backgrounds_ = std::move(remaining_bgs);
            remainder->row_backgrounds_ = std::move(remaining_row_bgs);
            remainder->title_nodes_ = std::move(remaining_titles);
            remainder->title_backgrounds_ = std::move(remaining_title_bgs);
            remainder->titles_ = std::move(remaining_titles_text);

            rows_ = static_cast<int>(keep_rows);
            remainder->rows_ = static_cast<int>(remain_rows);
        }

        if (!row_weights_.empty()) {
            const std::size_t split_index = std::min(keep_rows, row_weights_.size());
            std::vector<float> remaining_weights(row_weights_.begin() + static_cast<std::ptrdiff_t>(split_index),
                                                 row_weights_.end());
            row_weights_.erase(row_weights_.begin() + static_cast<std::ptrdiff_t>(split_index), row_weights_.end());
            remainder->row_weights_ = std::move(remaining_weights);
        }

        return remainder;
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
