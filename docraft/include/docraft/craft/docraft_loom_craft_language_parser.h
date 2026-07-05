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

#include <filesystem>
#include <memory>
#include <string>

#include "docraft/docraft_lib.h"

#include "docraft/loom/docraft_loom_pdf_creator.h"

namespace pugi {
    class xml_node;
} // namespace pugi

namespace docraft::craft {
    /**
     * @brief Top-level driver that turns a full Craft-language document (`<Document>`
     * with a required `<Body>` and optional `<Header>`/`<Footer>` siblings) into a fully
     * wired `docraft::loom::DocraftLoomPdfCreator`.
     *
     * This is the only component that needs to know about the `<Document>` structure
     * itself: `docraft::craft::DocraftCraftLanguageParser` has no per-tag registration for
     * `Document`/`Settings`/`Metadata`/`Foreach`/`NewPage`, so this class walks the
     * `<Document>` element directly via pugixml, picks out its `Header`/`Body`/`Footer`
     * children (each parsed generically from that point on via
     * `DocraftCraftLanguageParser::parse_node`), and ignores every other top-level child
     * (in particular `<Settings>`, whose wiring is Phase 4 -- out of scope here).
     */
    class DOCRAFT_LIB DocraftLoomCraftLanguageParser
    {
    public:
        DocraftLoomCraftLanguageParser() = default;
        ~DocraftLoomCraftLanguageParser() = default;

        /**
         * @brief Parses a full Craft-language document from a string and builds/stores
         * the resulting `DocraftLoomPdfCreator`, retrievable via `edit_creator()`.
         * @param xml_string XML source as string.
         * @throws docraft::exception::DataFormatException on malformed XML or a missing
         * required `<Body>` element.
         */
        void parse(const std::string& xml_string);

        /**
         * @brief Loads and parses a full Craft-language document from file, same as
         * `parse()`.
         * @param path Path to the `.craft` XML file.
         */
        void load_from_file(const std::filesystem::path& path);

        /**
         * @brief Returns the fully-wired creator built by the last `parse()`/
         * `load_from_file()` call. Mirrors `DocraftLoomNode::edit_layout_box()`'s
         * non-const-accessor-for-mutable-access convention.
         */
        std::shared_ptr<loom::DocraftLoomPdfCreator> edit_creator();

    private:
        void build_from_document(const pugi::xml_node& document_node);

        std::shared_ptr<loom::DocraftLoomPdfCreator> creator_;
    };
} // namespace docraft::craft
