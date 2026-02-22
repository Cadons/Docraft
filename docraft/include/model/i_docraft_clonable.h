#pragma once

#include <memory>

namespace docraft::model {
    class DocraftNode;

    /**
     * @brief Interface for clonable Docraft nodes.
     * Nodes that implement this interface can be cloned, which is useful for templating and other operations that require copying nodes.
     */
    class IDocraftClonable {
    public:
        virtual ~IDocraftClonable() = default;
        /**
         * @brief Creates a deep copy of the node.
         * @return A shared pointer to the cloned node.
         */
        virtual std::shared_ptr<DocraftNode> clone() const = 0;
    };
} // namespace docraft::model
