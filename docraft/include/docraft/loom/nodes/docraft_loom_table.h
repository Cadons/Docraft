#pragma once
#include <optional>
#include <vector>

#include "docraft/docraft_color.h"
#include "docraft/loom/nodes/docraft_loom_node.h"
#include "docraft/loom/nodes/docraft_loom_table_cell.h"

namespace docraft::loom::nodes {
    /**
     * @brief Table node: a grid of DocraftLoomTableCell nodes. There is no separate
     * "header row/column" concept -- a cell may be marked is_title() (a property on the
     * cell itself), which only affects draw order in Rendering (title cells paint last,
     * over any dividers). Measure/Layout treat every cell uniformly regardless of
     * is_title -- this is what lets the same table represent both a "column headers"
     * (title row) and a "row labels" (title column) shape with one code path.
     *
     * Column-width resolution is computed by the layout engine (DocraftLoomLayoutProcessor)
     * and written into each cell's own frame.size -- the table itself holds no separate
     * width/height cache, mirroring how every other container (VStack/HStack/Rectangle)
     * derives its rendering geometry from its children's own LayoutBox rather than a
     * parallel cache.
     */
    class DOCRAFT_LIB DocraftLoomTable : public DocraftLoomNode
    {
    public:
        static constexpr float kCellPaddingX = 2.5F;
        static constexpr float kCellPaddingY = 2.5F;

        DocraftLoomTable() = default;
        ~DocraftLoomTable() override = default;
        void accept(loom::interfaces::DocraftLoomIVisitor& visitor) override;

        /**
         * @throws exception::InvalidInputException if any cell is null, or if the row's
         * column count doesn't match previously added rows.
         */
        void add_row(std::vector<std::shared_ptr<DocraftLoomTableCell>> cells);

        int row_count() const;
        int column_count() const;
        std::shared_ptr<DocraftLoomTableCell> cell(int row, int column) const;

        void set_column_weights(std::vector<float> weights);
        const std::vector<float>& column_weights() const;

        void set_default_cell_background(const DocraftColor& color);
        const std::optional<DocraftColor>& default_cell_background() const;

        float baseline_offset() const;
        void set_baseline_offset(float baseline_offset);

        /**
         * @brief Splits off rows [row_index, row_count()) into a new table (mirrors
         * legacy's DocraftTable::split_after_row), for use when a table doesn't fit on
         * one page. Column weights/default background/baseline offset are copied onto
         * the remainder. If repeat_header_rows is true, a leading contiguous run of
         * all-title rows in *this* table (before row_index) is cloned onto the front of
         * the remainder too, so a header row keeps repeating on the continuation.
         * @return The remainder table, or nullptr if row_index is 0 or >= row_count()
         * (nothing to split).
         */
        std::shared_ptr<DocraftLoomTable> split_after_row(int row_index, bool repeat_header_rows = true);

    private:
        std::vector<std::vector<std::shared_ptr<DocraftLoomTableCell>>> grid_;
        std::vector<float> column_weights_;
        std::optional<DocraftColor> default_cell_background_;
        float baseline_offset_ = 0.25F;
    };
} // docraft