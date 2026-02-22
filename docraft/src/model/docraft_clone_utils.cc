#include "model/docraft_clone_utils.h"

#include <stdexcept>

namespace docraft::model {
    std::shared_ptr<DocraftNode> clone_node(const std::shared_ptr<DocraftNode> &node) {
        if (!node) {
            return nullptr;
        }
        auto clonable = std::dynamic_pointer_cast<IDocraftClonable>(node);
        if (!clonable) {
            throw std::runtime_error("Node does not implement IDocraftClonable");
        }
        return clonable->clone();
    }

    std::vector<std::shared_ptr<DocraftNode>> clone_nodes(const std::vector<std::shared_ptr<DocraftNode>> &nodes) {
        std::vector<std::shared_ptr<DocraftNode>> result;
        result.reserve(nodes.size());
        for (const auto &node : nodes) {
            result.emplace_back(clone_node(node));
        }
        return result;
    }
} // namespace docraft::model
