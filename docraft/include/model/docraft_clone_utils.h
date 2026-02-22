#pragma once

#include <memory>
#include <vector>

#include "model/docraft_node.h"
#include "model/i_docraft_clonable.h"

namespace docraft::model {
    std::shared_ptr<DocraftNode> clone_node(const std::shared_ptr<DocraftNode> &node);
    std::vector<std::shared_ptr<DocraftNode>> clone_nodes(const std::vector<std::shared_ptr<DocraftNode>> &nodes);
} // namespace docraft::model
