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

#include "docraft/loom/craft/docraft_loom_tag_handler_registry.h"

namespace docraft::loom::craft {
    DocraftLoomTagHandlerRegistry& DocraftLoomTagHandlerRegistry::instance()
    {
        static DocraftLoomTagHandlerRegistry registry;
        return registry;
    }

    void DocraftLoomTagHandlerRegistry::register_handler(const std::string& tag,
                                                          std::unique_ptr<IDocraftLoomTagHandler> handler)
    {
        handlers_[tag] = std::move(handler);
    }

    IDocraftLoomTagHandler* DocraftLoomTagHandlerRegistry::find(const std::string& tag) const
    {
        const auto it = handlers_.find(tag);
        return it != handlers_.end() ? it->second.get() : nullptr;
    }
} // namespace docraft::loom::craft
