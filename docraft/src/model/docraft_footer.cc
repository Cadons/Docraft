#include "model/docraft_footer.h"

#include <iostream>

#include "model/docraft_clone_utils.h"

namespace docraft::model {
    DocraftFooter::DocraftFooter() {
        set_name("footer");
    }

    std::shared_ptr<DocraftNode> DocraftFooter::clone() const {
        auto copy = std::make_shared<DocraftFooter>(*this);
        copy->clear_children();
        for (const auto &child : children()) {
            copy->add_child(clone_node(child));
        }
        return copy;
    }
} // docraft
