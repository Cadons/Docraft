#include "docraft/loom/nodes/docraft_loom_table.h"

#include "docraft/exception/docraft_input_exceptions.h"
#include "docraft/loom/nodes/docraft_loom_image.h"
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

    std::shared_ptr<DocraftLoomTable> DocraftLoomTable::split_after_row(int row_index, bool repeat_header_rows)
    {
        if (row_index <= 0 || row_index >= row_count())
        {
            return nullptr;
        }

        auto remainder = std::make_shared<DocraftLoomTable>();
        remainder->column_weights_ = column_weights_;
        remainder->default_cell_background_ = default_cell_background_;
        remainder->baseline_offset_ = baseline_offset_;
        remainder->padding_ = padding_;

        if (repeat_header_rows)
        {
            for (int r = 0; r < row_index; ++r)
            {
                bool all_title = true;
                for (auto& cell : grid_[static_cast<std::size_t>(r)])
                {
                    if (!cell->is_title())
                    {
                        all_title = false;
                        break;
                    }
                }
                if (!all_title)
                {
                    break;
                }
                std::vector<std::shared_ptr<DocraftLoomTableCell>> cloned_row;
                cloned_row.reserve(grid_[static_cast<std::size_t>(r)].size());
                for (auto& cell : grid_[static_cast<std::size_t>(r)])
                {
                    cloned_row.push_back(clone_cell(*cell));
                }
                remainder->grid_.push_back(std::move(cloned_row));
            }
        }

        for (std::size_t r = static_cast<std::size_t>(row_index); r < grid_.size(); ++r)
        {
            remainder->grid_.push_back(std::move(grid_[r]));
        }
        grid_.resize(static_cast<std::size_t>(row_index));

        return remainder;
    }
} // docraft