#include "docraft/model/docraft_body.h"

#include "docraft/model/docraft_clone_utils.h"

namespace docraft::model{

    std::shared_ptr<DocraftNode> DocraftBody::clone() const {
        auto copy = std::make_shared<DocraftBody>(*this);
        copy->clear_children();
        for (const auto &child : children()) {
            copy->add_child(clone_node(child));
        }
        return copy;
    }
} // docraft
