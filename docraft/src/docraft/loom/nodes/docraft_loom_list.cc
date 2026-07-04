#include "docraft/loom/nodes/docraft_loom_list.h"

namespace docraft::loom::nodes {
    namespace {
        std::string roman_for(int value)
        {
            if (value <= 0)
            {
                return "I";
            }
            struct Roman
            {
                int value;
                const char* numeral;
            };
            const Roman numerals[] = {
                {.value = 1000, .numeral = "M"},
                {.value = 900, .numeral = "CM"},
                {.value = 500, .numeral = "D"},
                {.value = 400, .numeral = "CD"},
                {.value = 100, .numeral = "C"},
                {.value = 90, .numeral = "XC"},
                {.value = 50, .numeral = "L"},
                {.value = 40, .numeral = "XL"},
                {.value = 10, .numeral = "X"},
                {.value = 9, .numeral = "IX"},
                {.value = 5, .numeral = "V"},
                {.value = 4, .numeral = "IV"},
                {.value = 1, .numeral = "I"},
            };
            std::string result;
            int remaining = value;
            for (const auto& item : numerals)
            {
                while (remaining >= item.value)
                {
                    result += item.numeral;
                    remaining -= item.value;
                }
            }
            return result;
        }
    }

    void DocraftLoomList::accept(loom::interfaces::DocraftLoomIVisitor& visitor)
    {
        visitor.visit(this);
    }

    ListKind DocraftLoomList::kind() const
    {
        return kind_;
    }

    void DocraftLoomList::set_kind(ListKind kind)
    {
        kind_ = kind;
    }

    OrderedListStyle DocraftLoomList::ordered_style() const
    {
        return ordered_style_;
    }

    void DocraftLoomList::set_ordered_style(OrderedListStyle style)
    {
        ordered_style_ = style;
    }

    UnorderedListDot DocraftLoomList::unordered_dot() const
    {
        return unordered_dot_;
    }

    void DocraftLoomList::set_unordered_dot(UnorderedListDot dot)
    {
        unordered_dot_ = dot;
    }

    bool DocraftLoomList::marker_is_box() const
    {
        return kind_ == ListKind::kUnordered && unordered_dot_ == UnorderedListDot::kBox;
    }

    std::string DocraftLoomList::marker_text_for_index(int index) const
    {
        if (kind_ == ListKind::kOrdered)
        {
            const int one_based = index + 1;
            if (ordered_style_ == OrderedListStyle::kRoman)
            {
                return roman_for(one_based) + ".";
            }
            return std::to_string(one_based) + ".";
        }

        switch (unordered_dot_)
        {
        case UnorderedListDot::kDash:
            return "-";
        case UnorderedListDot::kStar:
            return "*";
        case UnorderedListDot::kCircle:
            return "o";
        case UnorderedListDot::kBox:
            return "";
        default:
            return "o";
        }
    }

    const std::vector<DocraftLoomList::Marker>& DocraftLoomList::markers() const
    {
        return markers_;
    }

    std::vector<DocraftLoomList::Marker>& DocraftLoomList::edit_markers()
    {
        return markers_;
    }

    float DocraftLoomList::marker_gap() const
    {
        return marker_gap_;
    }
} // docraft
