/*
 * Copyright 2026 Matteo Cadoni (https://github.com/cadons)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "docraft/docraft_lib.h"
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
