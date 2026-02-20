#pragma once

#include <functional>
#include <string>
#include <vector>

#include "model/docraft_children_container_node.h"
#include "model/docraft_text.h"

namespace docraft::model {
    /**
     * @brief List kinds supported by Docraft.
     */
    enum class ListKind {
        kOrdered,
        kUnordered
    };

    /**
     * @brief Ordered list style variants.
     */
    enum class OrderedListStyle {
        kNumber,
        kRoman
    };

    /**
     * @brief Unordered list marker shapes.
     */
    enum class UnorderedListDot {
        kDash,
        kStar,
        kCircle,
        kBox
    };

    /**
     * @brief List node with markers and child items.
     *
     * Markers are computed from the list style and stored for layout and rendering.
     */
    class DocraftList : public DocraftChildrenContainerNode {
    public:
        using DocraftChildrenContainerNode::DocraftChildrenContainerNode;
        DocraftList(const DocraftList& node) = default;
        ~DocraftList() override = default;

        /**
         * @brief Draws the list using the provided context.
         * @param context Document context.
         */
        void draw(const std::shared_ptr<DocraftDocumentContext> &context) override;

        /**
         * @brief Returns the list kind.
         * @return List kind enum.
         */
        ListKind kind() const;
        /**
         * @brief Returns the ordered list style.
         * @return Ordered list style enum.
         */
        OrderedListStyle ordered_style() const;
        /**
         * @brief Returns the unordered list marker type.
         * @return Unordered list marker enum.
         */
        UnorderedListDot unordered_dot() const;

        /**
         * @brief Sets the list kind.
         * @param kind List kind enum.
         */
        void set_kind(ListKind kind);
        /**
         * @brief Sets the ordered list style.
         * @param style Ordered list style enum.
         */
        void set_ordered_style(OrderedListStyle style);
        /**
         * @brief Sets the unordered list marker type.
         * @param dot Unordered list marker enum.
         */
        void set_unordered_dot(UnorderedListDot dot);

        /**
         * @brief Rebuilds item markers based on children.
         */
        void update_items();
        /**
         * @brief Applies a text transform to all items and updates cached raw values.
         * @param transform Text transform function.
         */
        void apply_text_transform(const std::function<std::string(const std::string&)> &transform);
        /**
         * @brief Returns the marker text for a given index.
         * @param index Item index.
         * @return Marker string.
         */
        std::string marker_for_index(int index) const;
        /**
         * @brief Returns whether the marker is rendered as a box.
         * @return true if markers should be drawn as boxes.
         */
        bool marker_is_box() const;

        /**
         * @brief Marker metadata for layout and rendering.
         */
        struct Marker {
            /**
             * @brief Marker rendering type.
             */
            enum class Kind {
                kText,
                kBox
            };
            std::string text;
            DocraftPoint position;
            Kind kind = Kind::kText;
            float size = 0.0F;
        };
        /**
         * @brief Clears all markers.
         */
        void clear_markers();
        /**
         * @brief Sets a marker at the given index.
         * @param index Marker index.
         * @param text Marker text.
         * @param position Marker position.
         * @param kind Marker kind.
         * @param size Marker size in points.
         */
        void set_marker(int index, const std::string& text, const DocraftPoint& position,
                        Marker::Kind kind = Marker::Kind::kText, float size = 0.0F);
        /**
         * @brief Returns all markers.
         * @return Vector of markers.
         */
        const std::vector<Marker>& markers() const;

    protected:
        /**
         * @brief Updates markers when a child is added.
         */
        void on_child_added() override;
        /**
         * @brief Updates markers when a child is removed.
         * @param index Index of removed child.
         */
        void on_child_removed(int index) override;

    private:
        static std::shared_ptr<DocraftText> as_text_node(const std::shared_ptr<DocraftNode> &node);
        void sync_raw_texts_with_children();
        /**
         * @brief Returns the ordered list prefix for the index.
         * @param index Item index.
         * @return Prefix string.
         */
        std::string prefix_for_index(int index) const;
        /**
         * @brief Converts a number to a roman numeral.
         * @param value Number to convert.
         * @return Roman numeral string.
         */
        static std::string roman_for(int value);

        ListKind kind_ = ListKind::kUnordered;
        OrderedListStyle ordered_style_ = OrderedListStyle::kNumber;
        UnorderedListDot unordered_dot_ = UnorderedListDot::kCircle;
        std::vector<std::string> raw_texts_;
        std::vector<Marker> markers_;
    };
} // docraft::model
