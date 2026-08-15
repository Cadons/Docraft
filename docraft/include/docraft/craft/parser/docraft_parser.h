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

#include <any>
#include <optional>
#include <string>
#include <vector>

#include "docraft/docraft_lib.h"

#include "docraft/craft/docraft_craft_parsed_element.h"
#include "docraft/craft/i_docraft_parser.h"

namespace docraft::craft::parser {
    /**
     * @brief Tag-specific payload parsed from a `<Rectangle>` element.
     */
    struct ParsedRectangleData
    {
        std::optional<std::string> background_color;
        std::optional<std::string> border_color;
        std::optional<float> border_width;
    };

    /**
     * @brief Parser for rectangle nodes.
     */
    class DOCRAFT_LIB DocraftRectangleParser : public IDocraftParser {
    public:
        /**
         * @brief Parses a rectangle XML node into a ParsedRectangleData.
         * @param craft_language_source XML node.
         * @return `ParsedRectangleData`.
         */
        std::any parse(const pugi::xml_node& craft_language_source) override;

        /**
         * @brief Attribute names this parser reads, beyond the common set.
         */
        std::vector<std::string_view> accepted_attributes() const override;
    };

    /**
     * @brief Text style, duplicated from (rather than shared with) loom's own text style
     * fields -- see `docraft::craft::PositionMode` for the same "duplicate a tiny enum
     * and translate" rationale. `docraft::loom::nodes::DocraftLoomText` exposes bold()/
     * italic() as independent booleans rather than a single style enum; the loom tree
     * builder does that translation.
     */
    enum class ParsedTextStyle
    {
        kNormal,
        kBold,
        kItalic,
        kBoldItalic
    };

    /**
     * @brief Text alignment, duplicated for the same reason as `ParsedTextStyle`.
     */
    enum class ParsedTextAlignment
    {
        kLeft,
        kCenter,
        kRight,
        kJustified
    };

    /**
     * @brief Tag-specific payload parsed from `<Text>`/`<Title>`/`<Subtitle>` elements.
     * `Title`/`Subtitle` apply their heading-like defaults (font size/style) at parse
     * time, exactly like the legacy parser did, before any explicit attribute overrides
     * are applied.
     */
    struct ParsedTextData
    {
        std::string text;
        std::optional<float> font_size;
        std::optional<std::string> font_name;
        std::optional<std::string> color;
        std::optional<ParsedTextStyle> style;
        std::optional<ParsedTextAlignment> alignment;
        std::optional<bool> underline;
        std::optional<bool> strikeout;
    };

    /**
     * @brief Parser for `<Text>`, `<Title>` and `<Subtitle>` elements (all produce a
     * `ParsedTextData`, differing only in the tag-based defaults applied).
     */
    class DOCRAFT_LIB DocraftTextParser : public IDocraftParser {
    public:
        /**
         * @brief Parses a text-like XML node.
         * @param craft_language_source XML node.
         * @return `ParsedTextData`.
         */
        std::any parse(const pugi::xml_node& craft_language_source) override;

        /**
         * @brief Attribute names this parser reads, beyond the common set.
         */
        std::vector<std::string_view> accepted_attributes() const override;
    };

    /**
     * @brief Tag-specific payload parsed from a `<PageNumber>` element. Shares every
     * field with `ParsedTextData` except `text` -- the page number's text is generated
     * at render time, not parsed from the source.
     */
    struct ParsedPageNumberData
    {
        std::optional<float> font_size;
        std::optional<std::string> font_name;
        std::optional<std::string> color;
        std::optional<ParsedTextStyle> style;
        std::optional<ParsedTextAlignment> alignment;
        std::optional<bool> underline;
        std::optional<bool> strikeout;
        /**
         * @brief Display format, with `{page}`/`{total}` placeholders substituted at
         * render time. Defaults to `"{page}"` (bare current page number) when unset.
         */
        std::optional<std::string> format;
    };

    /**
     * @brief Parser for page number nodes.
     */
    class DOCRAFT_LIB DocraftPageNumberParser : public IDocraftParser {
    public:
        /**
         * @brief Parses a page number XML node.
         * @param craft_language_source XML node.
         * @return `ParsedPageNumberData`.
         */
        std::any parse(const pugi::xml_node& craft_language_source) override;

        /**
         * @brief Attribute names this parser reads, beyond the common set.
         */
        std::vector<std::string_view> accepted_attributes() const override;
    };

    /**
     * @brief Tag-specific payload parsed from an `<Image>` element.
     */
    struct ParsedImageData
    {
        std::optional<std::string> path; // from `src`, or from `data` when not base64-prefixed
        std::optional<std::vector<unsigned char>> raw_data; // decoded base64 RGB pixels
        std::optional<int> raw_pixel_width;
        std::optional<int> raw_pixel_height;
    };

    /**
     * @brief Parser for image nodes.
     */
    class DOCRAFT_LIB DocraftImageParser : public IDocraftParser {
    public:
        /**
         * @brief Parses an image XML node.
         * @param craft_language_source XML node.
         * @return `ParsedImageData`.
         * @throws docraft::exception::InvalidInputException if both `src` and `data` are
         * present, or if base64 `data` is missing/invalid dimensions.
         */
        std::any parse(const pugi::xml_node& craft_language_source) override;

        /**
         * @brief Attribute names this parser reads, beyond the common set.
         */
        std::vector<std::string_view> accepted_attributes() const override;
    };

    /**
     * @brief Tag-specific payload parsed from a `<Table>` element's title/header cells
     * (`HTitle`/`VTitle`), mirroring the legacy parser's shared title-node handling.
     */
    struct ParsedTableTitleData
    {
        std::string text;
        ParsedTextAlignment alignment = ParsedTextAlignment::kCenter;
        ParsedTextStyle style = ParsedTextStyle::kBold;
        std::optional<std::string> color;
        std::optional<std::string> background;
        /**
         * @brief Header/row-title font size, unset meaning "keep the node default".
         * Body cells honour `font_size` through their own `<Text>`, so without this a
         * table's headers could not be sized at all.
         */
        std::optional<float> font_size;
    };

    /**
     * @brief A single `<Cell>`'s content -- either a `ParsedTextData` or a
     * `ParsedImageData`, selected by `content_tag_name`.
     */
    struct ParsedTableCellData
    {
        std::string content_tag_name; // "Text" or "Image"
        std::any content;
        docraft::craft::DocraftCommonAttributes content_common;
        std::optional<std::string> background;
        std::optional<float> width;
    };

    /**
     * @brief A single `<Row>`.
     */
    struct ParsedTableRowData
    {
        std::vector<ParsedTableCellData> cells;
        std::optional<std::string> background;
        std::optional<ParsedTableTitleData> row_title; // vertical orientation's <VTitle>
    };

    enum class ParsedTableOrientation
    {
        kHorizontal,
        kVertical
    };

    /**
     * @brief Tag-specific payload parsed from a `<Table>` element.
     *
     * Covers explicit `<THead>`/`<TBody>` markup (both orientations) as well as the
     * JSON-matrix/template form of `model`/`header` (horizontal only): when `model` isn't
     * the literal `horizontal`/`vertical` keyword, it is stored verbatim in
     * `model_data_template` for the loom builder to resolve (`${...}` substitution, then
     * JSON-parse as a rectangular string matrix) at build time, mirroring how `<Foreach
     * model="...">` defers resolution. Loom's own `DocraftLoomTable` is a plain grid with
     * no separate header/title bookkeeping, so unlike the legacy struct this carries no
     * `cols`/`content_cols` counters -- the builder derives geometry from the grid itself.
     */
    struct ParsedTableData
    {
        ParsedTableOrientation orientation = ParsedTableOrientation::kHorizontal;
        std::optional<float> baseline_offset;
        std::optional<std::string> default_cell_background;
        std::vector<ParsedTableTitleData> header_titles; // <THead><HTitle>
        std::vector<ParsedTableRowData> rows; // <TBody><Row>
        std::optional<std::string> model_data_template; // `model`, when a JSON matrix/`${...}` rather than a keyword
        std::optional<std::string> header_data_template; // `header`, JSON string array/`${...}`
    };

    /**
     * @brief Parser for table nodes.
     */
    class DOCRAFT_LIB DocraftTableParser : public IDocraftParser {
    public:
        /**
         * @brief Parses a table XML node.
         * @param craft_language_source XML node.
         * @return `ParsedTableData`.
         */
        std::any parse(const pugi::xml_node& craft_language_source) override;

        /**
         * @brief Attribute names this parser reads, beyond the common set.
         */
        std::vector<std::string_view> accepted_attributes() const override;
    };

    /**
     * @brief Ordered/unordered list kind, duplicated for the same reason as
     * `ParsedTextStyle`.
     */
    enum class ParsedListKind
    {
        kOrdered,
        kUnordered
    };

    enum class ParsedOrderedListStyle
    {
        kNumber,
        kRoman
    };

    enum class ParsedUnorderedListDot
    {
        kDash,
        kStar,
        kCircle,
        kBox
    };

    /**
     * @brief Tag-specific payload parsed from `<List>`/`<UList>` elements.
     */
    struct ParsedListData
    {
        ParsedListKind kind = ParsedListKind::kUnordered;
        std::optional<ParsedOrderedListStyle> ordered_style;
        std::optional<ParsedUnorderedListDot> unordered_dot;
    };

    /**
     * @brief Parser for ordered list nodes.
     */
    class DOCRAFT_LIB DocraftListParser : public IDocraftParser {
    public:
        /**
         * @brief Parses a list XML node.
         * @param craft_language_source XML node.
         * @return `ParsedListData` with kind = kOrdered.
         */
        std::any parse(const pugi::xml_node& craft_language_source) override;

        /**
         * @brief Attribute names this parser reads, beyond the common set.
         */
        std::vector<std::string_view> accepted_attributes() const override;
    };

    /**
     * @brief Parser for unordered list nodes.
     */
    class DOCRAFT_LIB DocraftUListParser : public IDocraftParser {
    public:
        /**
         * @brief Parses an unordered list XML node.
         * @param craft_language_source XML node.
         * @return `ParsedListData` with kind = kUnordered.
         */
        std::any parse(const pugi::xml_node& craft_language_source) override;

        /**
         * @brief Attribute names this parser reads, beyond the common set.
         */
        std::vector<std::string_view> accepted_attributes() const override;
    };

    /**
     * @brief Tag-specific payload parsed from a `<Blank>` element. Empty: the blank
     * line's height is carried entirely by the generic `common.height` attribute.
     */
    struct ParsedBlankLineData
    {
    };

    /**
     * @brief Parser for blank line nodes.
     */
    class DOCRAFT_LIB DocraftBlackLineParser : public IDocraftParser {
    public:
        /**
         * @brief Parses a blank line XML node.
         * @param craft_language_source XML node.
         * @return `ParsedBlankLineData`.
         */
        std::any parse(const pugi::xml_node& craft_language_source) override;

        /**
         * @brief Attribute names this parser reads, beyond the common set.
         */
        std::vector<std::string_view> accepted_attributes() const override;
    };

    /**
     * @brief Tag-specific payload parsed from a `<NewPage>` element. Empty: `<NewPage>` is
     * a pure forced-page-break marker with no attributes of its own.
     */
    struct ParsedNewPageData
    {
    };

    /**
     * @brief Parser for forced-page-break nodes.
     */
    class DOCRAFT_LIB DocraftNewPageParser : public IDocraftParser
    {
    public:
        /**
         * @brief Parses a `<NewPage>` XML node.
         * @param craft_language_source XML node.
         * @return `ParsedNewPageData`.
         */
        std::any parse(const pugi::xml_node& craft_language_source) override;

        /**
         * @brief Attribute names this parser reads, beyond the common set.
         */
        std::vector<std::string_view> accepted_attributes() const override;
    };

    /**
     * @brief Layout orientation, duplicated for the same reason as `ParsedTextStyle`.
     */
    enum class ParsedLayoutOrientation
    {
        kHorizontal,
        kVertical
    };

    /**
     * @brief Tag-specific payload parsed from a `<layout>` element.
     *
     * `weights` carries the single comma-separated `weights="1,2,1"` attribute on
     * `<layout>` itself, if present -- the only way to specify per-child weights;
     * there is no per-child `weight="..."` attribute.
     */
    struct ParsedLayoutData
    {
        ParsedLayoutOrientation orientation = ParsedLayoutOrientation::kVertical;
        std::optional<float> spacing;
        std::optional<std::vector<float>> weights;
    };

    /**
     * @brief Parser for layout nodes.
     */
    class DOCRAFT_LIB DocraftLayoutParser : public IDocraftParser
    {
    public:
        /**
         * @brief Parses a layout XML node.
         * @param craft_language_source XML node.
         * @return `ParsedLayoutData`.
         */
        std::any parse(const pugi::xml_node& craft_language_source) override;

        /**
         * @brief Attribute names this parser reads, beyond the common set.
         */
        std::vector<std::string_view> accepted_attributes() const override;
    };

    /**
     * @brief Tag-specific payload parsed from a `<Chart>` element. `style` selects which
     * registered `docraft::loom::charts::DocraftChartBuilderFn` builds the chart's
     * content (e.g. "scatter") -- required, validated by the loom builder, not here.
     * `axis_position` defaults to "left" when absent. background/border mirror
     * `ParsedRectangleData` so the loom builder can reuse `apply_shape_style()` as-is.
     * `show_percentage` only affects the "pie" style (each slice's centered percentage
     * label) and defaults to shown when absent.
     */
    struct ParsedChartData
    {
        std::optional<std::string> style;
        std::optional<std::string> axis_position;
        std::optional<std::string> title;
        std::optional<std::string> x_label;
        std::optional<std::string> y_label;
        std::optional<std::string> background_color;
        std::optional<std::string> border_color;
        std::optional<float> border_width;
        std::optional<bool> show_percentage;
    };

    /**
     * @brief Parser for chart nodes.
     */
    class DOCRAFT_LIB DocraftChartParser : public IDocraftParser
    {
    public:
        /**
         * @brief Parses a chart XML node.
         * @param craft_language_source XML node.
         * @return `ParsedChartData`.
         */
        std::any parse(const pugi::xml_node& craft_language_source) override;

        /**
         * @brief Attribute names this parser reads, beyond the common set.
         */
        std::vector<std::string_view> accepted_attributes() const override;
    };

    /**
     * @brief Tag-specific payload parsed from a `<Series>` element (a `<Chart>` child).
     * `model` is carried raw/unresolved -- like `<Table model="...">` -- and resolved
     * into numeric points by the loom builder (`${...}` substitution, then JSON-parse
     * as an array of `[x,y]` pairs or `{"x":..,"y":..}` objects). The series' display
     * `name` is not parsed here -- it's already available generically via
     * `DocraftCommonAttributes::name`, since every tag's common attributes are parsed
     * unconditionally by `DocraftCraftLanguageParser`.
     */
    struct ParsedSeriesData
    {
        std::optional<std::string> color;
        std::optional<std::string> model;
    };

    /**
     * @brief Parser for chart series nodes.
     */
    class DOCRAFT_LIB DocraftSeriesParser : public IDocraftParser
    {
    public:
        /**
         * @brief Parses a series XML node.
         * @param craft_language_source XML node.
         * @return `ParsedSeriesData`.
         */
        std::any parse(const pugi::xml_node& craft_language_source) override;

        /**
         * @brief Attribute names this parser reads, beyond the common set.
         */
        std::vector<std::string_view> accepted_attributes() const override;
    };
} // namespace docraft::craft::parser
