#pragma once

#include "docraft_lib.h"
#include <memory>

namespace docraft {
    class DocraftCursor;
}

namespace docraft::generic{
    /**
     * @brief Generic chain-of-responsibility handler interface.
     * @tparam T Request type.
     * @tparam K Result type.
     */
    template<class T,class K>
    class DocraftChainOfResponsibilityHandler {
    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~DocraftChainOfResponsibilityHandler() = default;
        /**
         * @brief Attempts to handle the request and write the result.
         * @param request Input request.
         * @param result Output result pointer.
         * @param cursor Layout cursor used by handlers.
         * @return true if handled, false otherwise.
         */
        virtual bool handle(const std::shared_ptr<T>& request, K* result, docraft::DocraftCursor& cursor) =0;
    };
}
