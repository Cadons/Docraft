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

#include <memory>
#include <string>
#include <unordered_map>

#include "docraft/docraft_lib.h"
#include "docraft/loom/craft/handlers/i_docraft_loom_tag_handler.h"

namespace docraft::loom::craft {
    /**
     * @brief Maps a Craft-language tag name (e.g. "Rectangle", "Layout") to the
     * `IDocraftLoomTagHandler` that builds it.
     * @details Mirrors `docraft::craft::DocraftCraftLanguageParser`'s own tag ->
     * `IDocraftParser` map one stage earlier, and `charts::DocraftChartBuilderRegistry`'s
     * registry shape. Built-in tags register once via `register_builtin_tag_handlers()`,
     * called from `DocraftLoomTreeBuilder`'s constructor.
     */
    class DOCRAFT_LIB DocraftLoomTagHandlerRegistry
    {
    public:
        static DocraftLoomTagHandlerRegistry& instance();

        DocraftLoomTagHandlerRegistry(const DocraftLoomTagHandlerRegistry&) = delete;
        DocraftLoomTagHandlerRegistry& operator=(const DocraftLoomTagHandlerRegistry&) = delete;

        void register_handler(const std::string& tag, std::unique_ptr<IDocraftLoomTagHandler> handler);

        /**
         * @brief Returns the handler registered for `tag`, or nullptr if none is.
         */
        IDocraftLoomTagHandler* find(const std::string& tag) const;

    private:
        DocraftLoomTagHandlerRegistry() = default;

        std::unordered_map<std::string, std::unique_ptr<IDocraftLoomTagHandler>> handlers_;
    };

    /**
     * @brief Registers every Craft-language tag Docraft's loom pipeline ships out of the
     * box into `DocraftLoomTagHandlerRegistry::instance()`.
     * @details Safe to call more than once (re-registering a tag just overwrites its
     * handler).
     */
    DOCRAFT_LIB void register_builtin_tag_handlers();
} // namespace docraft::loom::craft
