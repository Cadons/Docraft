#pragma once
#include <cstddef>
#include <vector>

#include "docraft/loom/nodes/docraft_loom_node.h"

namespace docraft::loom {
    /**
     * @brief Plain C++ repeater: invokes fn(item, index) once per element of items, in
     * order. No templating/XML binding -- just a named loop, useful when building
     * repeated content (table rows, list items, HStack columns) from a data collection
     * instead of hand-writing one call per item.
     */
    template <typename T, typename Fn>
    void foreach_item(const std::vector<T>& items, Fn&& fn)
    {
        for (std::size_t i = 0; i < items.size(); ++i)
        {
            fn(items[i], static_cast<int>(i));
        }
    }

    /**
     * @brief Convenience form of foreach_item() for the common case of appending one
     * child per item to a container (e.g. a VStack/HStack/List): build(item, index) must
     * return the child node to append.
     */
    template <typename T, typename Build>
    void foreach_append(nodes::DocraftLoomNode& container, const std::vector<T>& items, Build&& build)
    {
        foreach_item(items, [&](const T& item, int index)
        {
            container.add_child(build(item, index));
        });
    }
} // docraft::loom