#pragma once
#include <string>
#include <vector>

#include "docraft/loom/nodes/docraft_loom_node.h"

namespace docraft::loom::nodes {
    enum class ListKind
    {
        kOrdered,
        kUnordered
    };

    enum class OrderedListStyle
    {
        kNumber,
        kRoman
    };

    enum class UnorderedListDot
    {
        kDash,
        kStar,
        kCircle,
        kBox
    };

    /**
     * @brief Loom node representing a list. Items are DocraftLoomText children
     * (added via the inherited add_child()); each item gets a Marker computed during
     * Measure (text/kind/width) and positioned during Layout.
     */
    class DOCRAFT_LIB DocraftLoomList : public DocraftLoomNode
    {
    public:
        struct Marker
        {
            enum class Kind
            {
                kText,
                kBox
            };

            std::string text;
            Position position;
            Kind kind = Kind::kText;
            float width = 0.0F;
        };

        DocraftLoomList() = default;
        ~DocraftLoomList() override = default;
        void accept(loom::interfaces::DocraftLoomIVisitor& visitor) override;

        ListKind kind() const;
        void set_kind(ListKind kind);
        OrderedListStyle ordered_style() const;
        void set_ordered_style(OrderedListStyle style);
        UnorderedListDot unordered_dot() const;
        void set_unordered_dot(UnorderedListDot dot);

        bool marker_is_box() const;
        /**
         * @brief Returns the marker text for a 0-based item index (e.g. "1.", "IV.", "*"),
         * mirroring legacy's DocraftList::prefix_for_index/roman_for. Box markers have no
         * text (an empty string), the box itself is drawn instead.
         */
        std::string marker_text_for_index(int index) const;

        const std::vector<Marker>& markers() const;
        std::vector<Marker>& edit_markers();

        float marker_gap() const;

    private:
        ListKind kind_ = ListKind::kUnordered;
        OrderedListStyle ordered_style_ = OrderedListStyle::kNumber;
        UnorderedListDot unordered_dot_ = UnorderedListDot::kCircle;
        std::vector<Marker> markers_;
        const float marker_gap_ = 6.0F; // horizontal gap between marker and text
    };
} // docraft
