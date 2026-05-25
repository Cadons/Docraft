#pragma once

#include <vector>
#include <memory>

namespace docraft {
    template <typename T>
    std::vector<std::shared_ptr<const T>> DocraftDocument::find_by_type() const {
        std::vector<std::shared_ptr<const T>> result;
        traverse_dom([&](const std::shared_ptr<model::DocraftNode> &node, DocraftDomTraverseOp op) {
            if (op != DocraftDomTraverseOp::kEnter) {
                return;
            }
            if (auto casted = std::dynamic_pointer_cast<const T>(node)) {
                result.push_back(casted);
            }
        });
        return result;
    }

    template <typename T>
    std::vector<std::shared_ptr<T>> DocraftDocument::edit_find_by_type() {
        std::vector<std::shared_ptr<T>> result;
        traverse_dom([&](const std::shared_ptr<model::DocraftNode> &node, DocraftDomTraverseOp op) {
            if (op != DocraftDomTraverseOp::kEnter) {
                return;
            }
            if (auto casted = std::dynamic_pointer_cast<T>(node)) {
                result.push_back(casted);
            }
        });
        return result;
    }
}
