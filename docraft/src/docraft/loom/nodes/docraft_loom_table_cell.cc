#include "docraft/loom/nodes/docraft_loom_table_cell.h"

#include "docraft/exception/docraft_input_exceptions.h"
#include "docraft/loom/nodes/docraft_loom_image.h"
#include "docraft/loom/nodes/docraft_loom_text.h"

namespace docraft::loom::nodes {
    void DocraftLoomTableCell::accept(loom::interfaces::DocraftLoomIVisitor& visitor)
    {
        visitor.visit(this);
    }

    void DocraftLoomTableCell::set_content(std::shared_ptr<DocraftLoomNode> content)
    {
        if (content && !std::dynamic_pointer_cast<DocraftLoomText>(content)
            && !std::dynamic_pointer_cast<DocraftLoomImage>(content))
        {
            throw exception::InvalidInputException("Table cell content must be Text or Image");
        }
        clear_children();
        if (content)
        {
            add_child(std::move(content));
        }
    }

    std::shared_ptr<DocraftLoomNode> DocraftLoomTableCell::content()
    {
        return children_count() > 0 ? edit_child(0) : nullptr;
    }

    const std::optional<DocraftColor>& DocraftLoomTableCell::background() const
    {
        return background_;
    }

    void DocraftLoomTableCell::set_background(const DocraftColor& color)
    {
        background_ = color;
    }

    std::optional<float> DocraftLoomTableCell::explicit_width() const
    {
        return explicit_width_;
    }

    void DocraftLoomTableCell::set_explicit_width(float width)
    {
        explicit_width_ = width;
    }

    bool DocraftLoomTableCell::is_title() const
    {
        return is_title_;
    }

    void DocraftLoomTableCell::set_is_title(bool is_title)
    {
        is_title_ = is_title;
    }
} // docraft