#include "docraft/loom/nodes/docraft_loom_table.h"

#include <algorithm>
#include <optional>

#include "docraft/exception/docraft_input_exceptions.h"
#include "docraft/loom/nodes/docraft_loom_image.h"
#include "docraft/loom/nodes/docraft_loom_layout_box_access.h"
#include "docraft/loom/nodes/docraft_loom_text.h"

namespace docraft::loom::nodes {
    namespace {
        std::shared_ptr<DocraftLoomTableCell> clone_cell(DocraftLoomTableCell& source)
        {
            auto clone = std::make_shared<DocraftLoomTableCell>();
            if (auto content = source.content())
            {
                if (auto text = std::dynamic_pointer_cast<DocraftLoomText>(content))
                {
                    auto text_clone = std::make_shared<DocraftLoomText>(text->text());
                    text_clone->set_font_family(text->font_family());
                    text_clone->set_font_size(text->font_size());
                    text_clone->set_bold(text->bold());
                    text_clone->set_italic(text->italic());
                    text_clone->set_underline(text->underline());
                    text_clone->set_color(text->color());
                    text_clone->set_alignment(text->alignment());
                    text_clone->set_wrap_width(text->wrap_width());
                    text_clone->set_wrapped_lines(text->wrapped_lines());
                    text_clone->set_ascent(text->ascent());
                    text_clone->set_descent(text->descent());
                    clone->set_content(text_clone);
                }
                else if (auto image = std::dynamic_pointer_cast<DocraftLoomImage>(content))
                {
                    auto image_clone = std::make_shared<DocraftLoomImage>();
                    image_clone->set_path(image->path());
                    if (image->has_raw_data())
                    {
                        image_clone->set_raw_data(image->raw_data(), image->raw_pixel_width(),
                                                  image->raw_pixel_height());
                    }
                    image_clone->set_width(image->width());
                    image_clone->set_height(image->height());
                    clone->set_content(image_clone);
                }
            }
            if (source.background())
            {
                clone->set_background(*source.background());
            }
            if (source.explicit_width())
            {
                clone->set_explicit_width(*source.explicit_width());
            }
            clone->set_is_title(source.is_title());
            // Clone must clone also the layout box
            clone->edit_layout_box() = source.layout_box();
            if (auto content = clone->content())
            {
                content->edit_layout_box() = source.content()->layout_box();
            }
            return clone;
        }

        // One cell's outcome from splitting its row at a given available_height: kept
        // stays on the current page, remainder (null if nothing carries over) becomes
        // that column's cell on the continuation.
        struct CellContentSplit {
            std::shared_ptr<DocraftLoomTableCell> kept;
            std::shared_ptr<DocraftLoomTableCell> remainder;
        };

        // Splits a single cell's content to fit within available_height. Returns
        // nullopt if the cell can't be split further and doesn't already fit on its own
        // -- the caller must then abandon splitting the whole row.
        std::optional<CellContentSplit> split_cell_content(const std::shared_ptr<DocraftLoomTableCell> &cell,
                                                           float available_height) {
            auto text = std::dynamic_pointer_cast<DocraftLoomText>(cell->content());
            const std::size_t line_count = text ? text->wrapped_lines().size() : 0;

            if (!text || line_count < 2) {
                // Not splittable content -- the whole cell must already fit on its own
                // natural height, or this row can't be split at all. measured_size (not
                // frame) is used here: frame.size.height may already be stretched to a
                // row-uniform height dominated by a taller sibling cell in the same row,
                // which isn't this cell's own requirement.
                const float natural_height = cell->layout_box().measured_size.height;
                if (natural_height > available_height + 0.01F) {
                    return std::nullopt;
                }
                // Clone rather than mutate cell in place: the caller may still abandon
                // the whole row's split (another cell returns nullopt), and the original
                // grid_ cell must be left untouched for that to be a true no-op.
                auto kept_cell = clone_cell(*cell);
                sealed_edit_frame(*kept_cell).size.height = natural_height;
                return CellContentSplit{.kept = kept_cell, .remainder = nullptr};
            }

            const float line_height = text->layout_box().measured_size.height / static_cast<float>(line_count);
            const float text_available = std::max(0.0F, available_height - (2.0F * DocraftLoomTable::kCellPaddingY));
            int keep_count = line_height > 0.0F
                                 ? static_cast<int>((text_available + 0.01F) / line_height)
                                 : static_cast<int>(line_count);
            keep_count = std::clamp(keep_count, 0, static_cast<int>(line_count));

            if (keep_count == 0) {
                // Not even one line fits -- caller falls back to accepting the row as
                // overflow rather than looping on a split that makes no progress.
                return std::nullopt;
            }
            if (keep_count >= static_cast<int>(line_count)) {
                // This cell's whole content actually fits -- it just wasn't the row's
                // own bottleneck -- so there's nothing to carry over for it. Clone
                // (same reasoning as above) and normalize its frame away from any prior
                // row-uniform stretch.
                auto kept_cell = clone_cell(*cell);
                sealed_edit_frame(*kept_cell).size.height = cell->layout_box().measured_size.height;
                return CellContentSplit{.kept = kept_cell, .remainder = nullptr};
            }

            const auto &lines = text->wrapped_lines();
            std::vector<std::string> kept_lines(lines.begin(), lines.begin() + keep_count);
            std::vector<std::string> remainder_lines(lines.begin() + keep_count, lines.end());

            auto kept_cell = clone_cell(*cell);
            auto kept_text = std::static_pointer_cast<DocraftLoomText>(kept_cell->content());
            kept_text->set_wrapped_lines(kept_lines);
            kept_text->edit_layout_box().measured_size.height = line_height * static_cast<float>(kept_lines.size());
            sealed_edit_frame(*kept_cell).size.height =
                    kept_text->layout_box().measured_size.height + (2.0F * DocraftLoomTable::kCellPaddingY);

            auto remainder_cell = clone_cell(*cell);
            auto remainder_text = std::static_pointer_cast<DocraftLoomText>(remainder_cell->content());
            remainder_text->set_wrapped_lines(remainder_lines);
            remainder_text->edit_layout_box().measured_size.height =
                    line_height * static_cast<float>(remainder_lines.size());
            sealed_edit_frame(*remainder_cell).size.height =
                    remainder_text->layout_box().measured_size.height + (2.0F * DocraftLoomTable::kCellPaddingY);

            return CellContentSplit{.kept = kept_cell, .remainder = remainder_cell};
        }

        struct RowContentSplit {
            std::vector<std::shared_ptr<DocraftLoomTableCell> > kept_row;
            std::vector<std::shared_ptr<DocraftLoomTableCell> > remainder_row;
        };

        // Assembles a row's per-cell splits into the two rows that replace it: every
        // cell in each row shares that row's own uniform height (mirrors how Layout
        // sizes a row to its tallest cell), and a cell with no remainder gets a blank
        // placeholder on the continuation instead of repeating its (already fully kept)
        // content.
        RowContentSplit assemble_split_row(const std::vector<CellContentSplit> &splits) {
            float kept_row_height = 0.0F;
            float remainder_row_height = 0.0F;
            for (const auto &split: splits) {
                kept_row_height = std::max(kept_row_height, sealed_frame(*split.kept).size.height);
                if (split.remainder) {
                    remainder_row_height =
                            std::max(remainder_row_height, sealed_frame(*split.remainder).size.height);
                }
            }

            RowContentSplit result;
            result.kept_row.reserve(splits.size());
            result.remainder_row.reserve(splits.size());
            for (const auto &split: splits) {
                sealed_edit_frame(*split.kept).size.height = kept_row_height;
                result.kept_row.push_back(split.kept);

                if (split.remainder) {
                    sealed_edit_frame(*split.remainder).size.height = remainder_row_height;
                    result.remainder_row.push_back(split.remainder);
                    continue;
                }

                // This cell's content was already exhausted by the kept fragment -- the
                // continuation shows it blank rather than repeating it. Its stale
                // position must match its row-mates' (still the original, pre-split row
                // position) so the caller's uniform per-row shift lands it correctly.
                auto blank = std::make_shared<DocraftLoomTableCell>();
                blank->set_is_title(split.kept->is_title());
                // No set_content() call -- this cell stays blank -- but background and
                // explicit_width are still styling of the cell itself and must carry
                // over, same as clone_cell does for the split.kept/remainder cells.
                if (split.kept->background()) {
                    blank->set_background(*split.kept->background());
                }
                if (auto explicit_width = split.kept->explicit_width(); explicit_width.has_value()) {
                    blank->set_explicit_width(explicit_width.value());
                }
                blank->edit_layout_box().edit_frame(layout_proof_or_throw(*split.kept)) = {
                    .position = sealed_frame(*split.kept).position,
                    .size = {sealed_frame(*split.kept).size.width, remainder_row_height}
                };
                result.remainder_row.push_back(blank);
            }
            return result;
        }
    }

    void DocraftLoomTable::accept(loom::interfaces::DocraftLoomIVisitor& visitor)
    {
        visitor.visit(this);
    }

    void DocraftLoomTable::add_row(std::vector<std::shared_ptr<DocraftLoomTableCell>> cells)
    {
        for (const auto& cell : cells)
        {
            if (!cell)
            {
                throw exception::InvalidInputException("Table row cells must not be null");
            }
        }
        if (!grid_.empty() && cells.size() != grid_.front().size())
        {
            throw exception::InvalidInputException("All table rows must have the same column count");
        }
        grid_.push_back(std::move(cells));
    }

    int DocraftLoomTable::row_count() const
    {
        return static_cast<int>(grid_.size());
    }

    int DocraftLoomTable::column_count() const
    {
        return grid_.empty() ? 0 : static_cast<int>(grid_.front().size());
    }

    std::shared_ptr<DocraftLoomTableCell> DocraftLoomTable::cell(int row, int column) const
    {
        if (row < 0 || row >= row_count() || column < 0 || column >= column_count())
        {
            return nullptr;
        }
        return grid_[static_cast<std::size_t>(row)][static_cast<std::size_t>(column)];
    }

    float DocraftLoomTable::row_height(int row) const
    {
        auto reference_cell = cell(row, 0);
        return reference_cell ? sealed_frame(*reference_cell).size.height : 0.0F;
    }

    void DocraftLoomTable::set_column_weights(std::vector<float> weights)
    {
        column_weights_ = std::move(weights);
    }

    const std::vector<float>& DocraftLoomTable::column_weights() const
    {
        return column_weights_;
    }

    void DocraftLoomTable::set_default_cell_background(const DocraftColor& color)
    {
        default_cell_background_ = color;
    }

    const std::optional<DocraftColor>& DocraftLoomTable::default_cell_background() const
    {
        return default_cell_background_;
    }

    float DocraftLoomTable::baseline_offset() const
    {
        return baseline_offset_;
    }

    void DocraftLoomTable::set_baseline_offset(float baseline_offset)
    {
        baseline_offset_ = baseline_offset;
    }

    float DocraftLoomTable::padding() const
    {
        return padding_;
    }

    void DocraftLoomTable::set_padding(float padding)
    {
        padding_ = padding;
    }

    std::shared_ptr<DocraftLoomTable> DocraftLoomTable::make_remainder_shell() const
    {
        auto remainder = std::make_shared<DocraftLoomTable>();
        remainder->column_weights_ = column_weights_;
        remainder->default_cell_background_ = default_cell_background_;
        remainder->baseline_offset_ = baseline_offset_;
        remainder->padding_ = padding_;
        return remainder;
    }

    std::shared_ptr<DocraftLoomTable> DocraftLoomTable::split_after_row(int row_index, bool repeat_header_rows)
    {
        if (row_index <= 0 || row_index >= row_count())
        {
            return nullptr;
        }

        auto remainder = make_remainder_shell();

        if (repeat_header_rows)
        {
            for (auto& cloned_row : clone_leading_header_rows(row_index))
            {
                remainder->grid_.push_back(std::move(cloned_row));
            }
        }

        for (auto r = static_cast<std::size_t>(row_index); r < grid_.size(); ++r) {
            remainder->grid_.push_back(std::move(grid_[r]));
        }
        grid_.resize(static_cast<std::size_t>(row_index));

        return remainder;
    }

    int DocraftLoomTable::leading_title_row_count() const {
        int count = 0;
        for (const auto &row: grid_) {
            const bool all_title = std::ranges::all_of(row.begin(), row.end(),
                                                       [](const std::shared_ptr<DocraftLoomTableCell> &cell) {
                                                           return cell->is_title();
                                                       });
            if (!all_title) {
                break;
            }
            ++count;
        }
        return count;
    }

    std::vector<std::vector<std::shared_ptr<DocraftLoomTableCell> > > DocraftLoomTable::clone_leading_header_rows(
        int upto_row_index) const {
        std::vector<std::vector<std::shared_ptr<DocraftLoomTableCell> > > cloned;
        const int header_rows = std::min(leading_title_row_count(), upto_row_index);
        cloned.reserve(static_cast<std::size_t>(header_rows));
        for (int r = 0; r < header_rows; ++r) {
            std::vector<std::shared_ptr<DocraftLoomTableCell> > cloned_row;
            cloned_row.reserve(grid_[static_cast<std::size_t>(r)].size());
            for (auto &cell: grid_[static_cast<std::size_t>(r)]) {
                cloned_row.push_back(clone_cell(*cell));
            }
            cloned.push_back(std::move(cloned_row));
        }
        return cloned;
    }

    std::shared_ptr<DocraftLoomTable> DocraftLoomTable::split_row_content(int row_index, float available_height,
                                                                          bool repeat_header_rows) {
        if (row_index < 0 || row_index >= row_count() || column_count() == 0) {
            return nullptr;
        }

        auto &row = grid_[static_cast<std::size_t>(row_index)];

        std::vector<CellContentSplit> splits;
        splits.reserve(row.size());
        bool made_progress = false;
        for (auto &cell: row) {
            auto split = split_cell_content(cell, available_height);
            if (!split) {
                // A cell couldn't be split and doesn't fit on its own either -- the
                // whole row can't be split; caller falls back to accepting it as
                // overflow rather than looping on a split that makes no progress.
                return nullptr;
            }
            made_progress |= (split->remainder != nullptr);
            splits.push_back(std::move(*split));
        }

        if (!made_progress) {
            // Every cell already fit whole -- there's genuinely nothing to split.
            return nullptr;
        }

        auto [kept_row, remainder_row] = assemble_split_row(splits);
        grid_[static_cast<std::size_t>(row_index)] = std::move(kept_row);

        auto remainder = make_remainder_shell();

        if (repeat_header_rows) {
            for (auto &cloned_row: clone_leading_header_rows(row_index)) {
                remainder->grid_.push_back(std::move(cloned_row));
            }
        }

        remainder->grid_.push_back(std::move(remainder_row));
        for (std::size_t r = static_cast<std::size_t>(row_index) + 1; r < grid_.size(); ++r) {
            remainder->grid_.push_back(std::move(grid_[r]));
        }
        grid_.resize(static_cast<std::size_t>(row_index) + 1);

        return remainder;
    }
} // docraft