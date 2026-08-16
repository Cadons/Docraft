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

        /**
         * @brief Row row's height, once Layout has resolved it: every cell in a row
         * shares the same frame.size.height (place_table_cells stretches each row to
         * its tallest cell), so cell(row, 0) alone is always representative -- this is
         * the single named accessor for that convention, instead of every call site
         * re-deriving it from cell(row, 0)->layout_box().frame.size.height by hand.
         * @return 0 if row is out of range or the table has no columns.
         */
        float row_height(int row) const;

        /**
         * @brief Invokes fn(DocraftLoomTableCell&) for every cell in the grid, in
         * row-major order. Cells live in the table's own grid_, not the inherited
         * children_ vector -- generic tree walks over a node's children (page-index
         * assignment, position shifts, ...) must use this instead of children_count()/
         * edit_child() to reach a table's cells.
         */
        template <typename Fn>
        void for_each_cell(Fn&& fn) const
        {
            for (int r = 0; r < row_count(); ++r)
            {
                for (int c = 0; c < column_count(); ++c)
                {
                    if (auto cell_ptr = cell(r, c))
                    {
                        fn(*cell_ptr);
                    }
                }
            }
        }

        void set_column_weights(std::vector<float> weights);
        const std::vector<float>& column_weights() const;

        void set_default_cell_background(const DocraftColor& color);
        const std::optional<DocraftColor>& default_cell_background() const;

        float baseline_offset() const;
        void set_baseline_offset(float baseline_offset);

        /**
         * @brief Outer spacing around the whole table (all four sides), mirroring how
         * DocraftLoomRectangle's own padding() insets its children from its border --
         * here there's no border to speak of, so it simply keeps the grid from sitting
         * flush against whatever precedes/follows the table (or the page edge). Wired to
         * the common `padding` craft-language attribute like any other node that exposes
         * a setter (see DocraftLoomTreeBuilder::apply_common_attributes).
         */
        float padding() const;
        void set_padding(float padding);

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

        /**
         * @brief Length of the leading contiguous run of all-title rows (0 if row 0
         * isn't a title row, or the table has no rows). This is exactly what
         * split_after_row(..., repeat_header_rows=true) clones onto a remainder's front
         * as its repeating header, so callers can tell whether a given number of
         * "fitting" rows covers only repeated header rows or actual content.
         */
        int leading_title_row_count() const;

        /**
         * @brief Splits row row_index's own cell content by wrapped-line count, for the
         * case where the row alone is taller than available_height (typically a whole
         * page) and can't be moved elsewhere as a unit. For each cell whose content is a
         * DocraftLoomText with 2+ wrapped_lines(), as many leading lines as fit within
         * available_height stay in *this (row_index shrinks to just that fragment); the
         * rest become that column's cell in a new one-row remainder table (repeated
         * header rows cloned onto its front first, mirroring split_after_row), the same
         * way this table's own leftover rows (if any, after row_index) do. A cell whose
         * content isn't a splittable Text -- or whose sole line still exceeds
         * available_height -- must already fit within available_height on its own, or
         * nothing is split at all.
         * @return The remainder table, or nullptr if row_index is out of range, the
         * table has no columns, no cell's content could be split, or every cell already
         * fit whole (nothing to split).
         */
        std::shared_ptr<DocraftLoomTable> split_row_content(int row_index, float available_height,
                                                            bool repeat_header_rows = true);

    private:
        /**
         * @brief Clones the leading contiguous run of all-title rows before
         * upto_row_index (clamped to leading_title_row_count()) -- the shared "repeating
         * header" prefix that both split_after_row and split_row_content prepend onto
         * their remainder.
         */
        std::vector<std::vector<std::shared_ptr<DocraftLoomTableCell> > > clone_leading_header_rows(
            int upto_row_index) const;

        /**
         * @brief A fresh, empty table carrying over this table's own column
         * weights/default background/baseline offset/padding -- the shared starting
         * point both split_after_row and split_row_content build their remainder on top
         * of, before either fills in its own rows.
         */
        std::shared_ptr<DocraftLoomTable> make_remainder_shell() const;

        std::vector<std::vector<std::shared_ptr<DocraftLoomTableCell>>> grid_;
        std::vector<float> column_weights_;
        std::optional<DocraftColor> default_cell_background_;
        float baseline_offset_ = 0.25F;
        // No padding by default -- a Table's grid/border sits flush with whatever width
        // its container hands it (e.g. the body's margins), matching every other node's
        // default. Still fully overridable via the `padding` attribute.
        float padding_ = 0.0F;
    };
} // docraft