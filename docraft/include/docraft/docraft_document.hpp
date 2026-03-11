#pragma once

#include <vector>
#include <memory>

namespace docraft {
    template <typename T>
    std::vector<std::shared_ptr<T>> DocraftDocument::get_by_type() const {
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
