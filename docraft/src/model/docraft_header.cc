#include "model/docraft_header.h"
#include <iostream>

#include "model/docraft_clone_utils.h"

namespace docraft::model {
    DocraftHeader::DocraftHeader() {
        set_name("header");
    }

    std::shared_ptr<DocraftNode> DocraftHeader::clone() const {
        auto copy = std::make_shared<DocraftHeader>(*this);
        copy->clear_children();
        for (const auto &child : children()) {
            copy->add_child(clone_node(child));
        }
        return copy;
    }
}
