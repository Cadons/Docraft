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

#include <memory>

#include "docraft/craft/docraft_craft_language_tokens.h"
#include "docraft/loom/craft/docraft_loom_tag_handler_registry.h"
#include "docraft/loom/craft/handlers/docraft_loom_blank_line_handler.h"
#include "docraft/loom/craft/handlers/docraft_loom_canvas_handler.h"
#include "docraft/loom/craft/handlers/docraft_loom_chart_handler.h"
#include "docraft/loom/craft/handlers/docraft_loom_circle_handler.h"
#include "docraft/loom/craft/handlers/docraft_loom_curve_line_handler.h"
#include "docraft/loom/craft/handlers/docraft_loom_image_handler.h"
#include "docraft/loom/craft/handlers/docraft_loom_layout_handler.h"
#include "docraft/loom/craft/handlers/docraft_loom_line_handler.h"
#include "docraft/loom/craft/handlers/docraft_loom_list_handler.h"
#include "docraft/loom/craft/handlers/docraft_loom_new_page_handler.h"
#include "docraft/loom/craft/handlers/docraft_loom_page_number_handler.h"
#include "docraft/loom/craft/handlers/docraft_loom_paragraph_handler.h"
#include "docraft/loom/craft/handlers/docraft_loom_polygon_handler.h"
#include "docraft/loom/craft/handlers/docraft_loom_rectangle_handler.h"
#include "docraft/loom/craft/handlers/docraft_loom_section_handler.h"
#include "docraft/loom/craft/handlers/docraft_loom_table_handler.h"
#include "docraft/loom/craft/handlers/docraft_loom_text_handler.h"
#include "docraft/loom/craft/handlers/docraft_loom_triangle_handler.h"

namespace docraft::loom::craft {
    namespace tokens = docraft::craft;

    void register_builtin_tag_handlers()
    {
        auto& registry = DocraftLoomTagHandlerRegistry::instance();
        registry.register_handler(std::string{tokens::elements::kRectangle},
                                  std::make_unique<DocraftLoomRectangleHandler>());
        registry.register_handler(std::string{tokens::elements::kCircle},
                                  std::make_unique<DocraftLoomCircleHandler>());
        registry.register_handler(std::string{tokens::elements::kTriangle},
                                  std::make_unique<DocraftLoomTriangleHandler>());
        registry.register_handler(std::string{tokens::elements::kPolygon},
                                  std::make_unique<DocraftLoomPolygonHandler>());
        registry.register_handler(std::string{tokens::elements::kLine},
                                  std::make_unique<DocraftLoomLineHandler>());
        registry.register_handler(std::string{tokens::elements::kCurveLine},
                                  std::make_unique<DocraftLoomCurveLineHandler>());
        registry.register_handler(std::string{tokens::elements::kCanvas},
                                  std::make_unique<DocraftLoomCanvasHandler>());
        registry.register_handler(std::string{tokens::elements::kChart},
                                  std::make_unique<DocraftLoomChartHandler>());
        registry.register_handler(std::string{tokens::elements::kText},
                                  std::make_unique<DocraftLoomTextHandler>());
        registry.register_handler(std::string{tokens::elements::kTitle},
                                  std::make_unique<DocraftLoomTitleHandler>());
        registry.register_handler(std::string{tokens::elements::kSubtitle},
                                  std::make_unique<DocraftLoomSubtitleHandler>());
        registry.register_handler(std::string{tokens::elements::kPageNumber},
                                  std::make_unique<DocraftLoomPageNumberHandler>());
        registry.register_handler(std::string{tokens::elements::kImage},
                                  std::make_unique<DocraftLoomImageHandler>());
        registry.register_handler(std::string{tokens::elements::kBlankLine},
                                  std::make_unique<DocraftLoomBlankLineHandler>());
        registry.register_handler(std::string{tokens::elements::kNewPage},
                                  std::make_unique<DocraftLoomNewPageHandler>());
        registry.register_handler(std::string{tokens::elements::kList},
                                  std::make_unique<DocraftLoomListHandler>());
        registry.register_handler(std::string{tokens::elements::kUList},
                                  std::make_unique<DocraftLoomListHandler>());
        registry.register_handler(std::string{tokens::elements::kParagraph},
                                  std::make_unique<DocraftLoomParagraphHandler>());
        registry.register_handler(std::string{tokens::elements::kLayout},
                                  std::make_unique<DocraftLoomLayoutHandler>());
        registry.register_handler(std::string{tokens::section::kHeader},
                                  std::make_unique<DocraftLoomSectionHandler>());
        registry.register_handler(std::string{tokens::section::kBody},
                                  std::make_unique<DocraftLoomSectionHandler>());
        registry.register_handler(std::string{tokens::section::kFooter},
                                  std::make_unique<DocraftLoomSectionHandler>());
        registry.register_handler(std::string{tokens::elements::kTable},
                                  std::make_unique<DocraftLoomTableHandler>());
    }
} // namespace docraft::loom::craft
