#pragma once
#include "docraft_text.h"
#include "model/docraft_children_container_node.h"
#include "model/docraft_layout.h"

namespace docraft::model {
    class DocraftTable : public DocraftNode {
    public:
        DocraftTable();

        ~DocraftTable() override = default;

        void draw(const std::shared_ptr<DocraftPDFContext> &context) override;

        void set_rows(int rows);

        void set_cols(int cols);

        void set_orientation(LayoutOrientation orientation);
        void set_column_weight(int index, float weight);
        void set_row_weight(int index, float weight);
        void set_title(int index, const std::string &title);
        void set_titles(const std::vector<std::string> &titles);
        void set_column_weights(const std::vector<float> &weights);
        void set_row_weights(const std::vector<float> &weights);
        void set_title_node(int index, const std::shared_ptr<DocraftText> &node);
        void set_content_node(int index, const std::shared_ptr<DocraftNode> &node);
        void set_title_nodes(const std::vector<std::shared_ptr<DocraftText>> &nodes);
        void set_content_nodes(const std::vector<std::shared_ptr<DocraftNode>> &nodes);
        void add_title_node(const std::shared_ptr<DocraftText> &node);
        void add_content_node(const std::shared_ptr<DocraftNode> &node);

        [[nodiscard]] int rows() const;

        [[nodiscard]] int cols() const;

        [[nodiscard]] LayoutOrientation orientation() const;
        [[nodiscard]] const std::vector<std::string> &titles() const;
        [[nodiscard]] const std::vector<float> &column_weights() const;
        [[nodiscard]] const std::vector<float> &row_weights() const;
        [[nodiscard]] int last_row_weight_index() const;
        [[nodiscard]] int last_column_weight_index() const;
        [[nodiscard]] const std::vector<std::shared_ptr<DocraftText>> &title_nodes() const;
        [[nodiscard]] std::vector<std::vector<std::shared_ptr<DocraftNode>>>content_nodes() const;
        [[nodiscard]] int last_title_node_index() const;
        [[nodiscard]] int last_content_node_index() const;

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
    };
} // docraft
