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

#include <cstddef>
#include <string>
#include <vector>

#include <nlohmann/json_fwd.hpp>

#include "docraft/docraft_lib.h"
#include "docraft/loom/craft/handlers/i_docraft_loom_tag_handler.h"

namespace docraft::craft::parser {
    struct ParsedTableTitleData;
    struct ParsedTableCellData;
    struct ParsedTableData;
} // namespace docraft::craft::parser

namespace docraft::loom::nodes {
    class DocraftLoomTable;
    class DocraftLoomTableCell;
} // namespace docraft::loom::nodes

namespace docraft::loom::craft {
    /**
     * @brief `<Table>`: by far the widest-surface tag (model-as-matrix, model-as-templated-
     * object-rows, explicit `<THead>`/`<TBody>`, horizontal/vertical orientation) --
     * intentionally kept as one substantial handler rather than decomposed further, per
     * `.local/TABELLE_LOOM.md`'s catalogue of Table's cross-cutting complexity.
     */
    class DOCRAFT_LIB DocraftLoomTableHandler : public IDocraftLoomTagHandler
    {
    public:
        std::shared_ptr<nodes::DocraftLoomNode> build(const docraft::craft::DocraftParsedElement& element,
                                                        DocraftLoomTableHandlerContext& context) override;

    private:
        /**
         * @brief Builds a `<HTitle>`/`<VTitle>` table title cell. Deliberately resolves only
         * plain `${variable}` expressions, not `${data("field")}` -- unlike model/header/cell
         * content, a title's text has no per-row or per-object data of its own to bind to.
         */
        static std::shared_ptr<nodes::DocraftLoomTableCell> build_title_cell(
            const docraft::craft::parser::ParsedTableTitleData& title, DocraftLoomTableHandlerContext& context);

        static std::shared_ptr<nodes::DocraftLoomTableCell> build_content_cell(
            const docraft::craft::parser::ParsedTableCellData& cell_data, DocraftLoomTableHandlerContext& context);

        /**
         * @brief Resolves a `<Table model="...">` attribute (already known not to be the
         * "horizontal"/"vertical" keyword) into its parsed JSON form: `${...}` substitution
         * (against the ambient current Foreach item, if this Table is itself nested inside
         * an outer `<Foreach>`), then single-quote-JSON normalization and parsing.
         * @throws docraft::exception::DataFormatException if `raw` doesn't resolve to a
         * valid, non-empty JSON array.
         */
        static nlohmann::json resolve_table_model_json(const std::string& raw, DocraftLoomTableHandlerContext& context);

        /**
         * @brief Converts an already-resolved `<Table model="...">` JSON array into a
         * rectangular, string-only matrix (the "array of arrays" model shape).
         */
        static std::vector<std::vector<std::string>> to_string_matrix(const nlohmann::json& parsed);

        /**
         * @brief Resolves a `<Table header="...">` attribute into a non-empty, string-only
         * array.
         */
        static std::vector<std::string> resolve_table_header(const std::string& raw,
                                                              DocraftLoomTableHandlerContext& context);

        /**
         * @brief Builds and appends the header row for a JSON/template `model` table
         * (either shape), from the `header` attribute or an explicit `<THead>` -- whichever
         * `data` carries -- validated against `column_count`. Does nothing if neither is
         * present.
         */
        static void add_table_model_header_row(nodes::DocraftLoomTable& table,
                                               const docraft::craft::parser::ParsedTableData& data,
                                               std::size_t column_count, DocraftLoomTableHandlerContext& context);

        /**
         * @brief Builds rows for a `<Table model="...">` that resolves to a JSON array of
         * objects: the explicit `<TBody>` (already parsed into `data.rows`) is used as a
         * per-object row template, repeated once per array element with the context's
         * current Foreach item scoped to that element so `build_content_cell()`'s
         * `${data("field")}` substitution resolves against it.
         */
        static void build_templated_model_rows(nodes::DocraftLoomTable& table, const nlohmann::json& model_json,
                                               const docraft::craft::parser::ParsedTableData& data,
                                               DocraftLoomTableHandlerContext& context);
    };
} // namespace docraft::loom::craft
