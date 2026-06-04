#include <gtest/gtest.h>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <memory>
#include <string>

#include "docraft/backend/pdf/docraft_haru_backend_providers_factory.h"
#include "docraft/docraft_document_context.h"
#include "docraft/craft/docraft_craft_language_tokens.h"
#include "docraft/generic/docraft_font_applier.h"
#include "docraft/layout/docraft_layout_engine.h"
#include "docraft/renderer/docraft_pdf_renderer.h"

#include "docraft/model/docraft_blank_line.h"
#include "docraft/model/docraft_body.h"
#include "docraft/model/docraft_circle.h"
#include "docraft/model/docraft_footer.h"
#include "docraft/model/docraft_foreach.h"
#include "docraft/model/docraft_header.h"
#include "docraft/model/docraft_image.h"
#include "docraft/model/docraft_layout.h"
#include "docraft/model/docraft_line.h"
#include "docraft/model/docraft_list.h"
#include "docraft/model/docraft_new_page.h"
#include "docraft/model/docraft_page_number.h"
#include "docraft/model/docraft_polygon.h"
#include "docraft/model/docraft_rectangle.h"
#include "docraft/model/docraft_table.h"
#include "docraft/model/docraft_text.h"
#include "docraft/model/docraft_triangle.h"

#include "docraft/utils/docraft_mock_rendering_backend.h"

namespace docraft::test::layout {
    class DocraftPaginationStressTest : public ::testing::Test {
    protected:
        void SetUp() override {
            const std::string backend_mode = requested_backend_mode();
            if (backend_mode == "mock") {
                setup_mock_backend();
            } else if (backend_mode == "haru") {
                setup_haru_backend();
            } else if (!try_setup_haru_backend()) {
                setup_mock_backend();
            }

            context_->set_renderer(std::make_shared<renderer::DocraftPDFRenderer>(context_));
            context_->edit_typography().set_font_applier(std::make_shared<generic::DocraftFontApplier>(context_));
            engine_ = std::make_unique<docraft::layout::DocraftLayoutEngine>(context_);
        }

        void TearDown() override {
            if (!use_haru_ || !context_) {
                return;
            }

            const auto *test_info = ::testing::UnitTest::GetInstance()->current_test_info();
            if (!test_info) {
                return;
            }

            const auto output = context_->rendering().output_backend();
            if (!output) {
                return;
            }

            std::string file_name = std::string(test_info->test_suite_name()) + "_" + test_info->name() + ".pdf";
            for (char &ch: file_name) {
                const auto value = static_cast<unsigned char>(ch);
                if (!(std::isalnum(value) || ch == '_' || ch == '-' || ch == '.')) {
                    ch = '_';
                }
            }

            std::filesystem::create_directories("stress_artifacts");
            output->save_to_file("stress_artifacts/" + file_name);
        }

        [[nodiscard]] std::size_t total_page_count() const {
            if (!context_) {
                return 0;
            }
            const auto page_backend = context_->rendering().page_rendering();
            if (!page_backend) {
                return 0;
            }
            return page_backend->total_page_count();
        }

        [[nodiscard]] std::size_t page_threshold(std::size_t mock_pages, std::size_t haru_pages) const {
            return use_haru_ ? haru_pages : mock_pages;
        }

        [[nodiscard]] std::size_t fragment_threshold(std::size_t mock_fragments, std::size_t haru_fragments) const {
            return use_haru_ ? haru_fragments : mock_fragments;
        }

        void layout_and_render(const std::vector<std::shared_ptr<model::DocraftNode> > &nodes) {
            engine_->compute_document_layout(nodes);
            auto &rendering_service = context_->edit_rendering();
            const auto page_backend = rendering_service.edit_page_rendering();
            if (page_backend) {
                page_backend->go_to_first_page();
            }

            for (const auto &node: nodes) {
                if (!node || !node->visible()) {
                    continue;
                }

                if (node->page_owner() == -1 && page_backend) {
                    const auto page_count = page_backend->total_page_count();
                    for (std::size_t i = 0; i < page_count; ++i) {
                        page_backend->go_to_page(i);
                        node->draw(context_);
                    }
                    continue;
                }

                if (page_backend && node->page_owner() > 0) {
                    page_backend->go_to_page(static_cast<std::size_t>(node->page_owner() - 1));
                }
                if (node->should_render(context_)) {
                    node->draw(context_);
                }
            }
        }

        [[nodiscard]] static std::string requested_backend_mode() {
            const char *value = std::getenv("DOCRAFT_STRESS_BACKEND");
            if (!value || std::string(value).empty()) {
                return "auto";
            }
            std::string mode(value);
            std::ranges::transform(mode, mode.begin(),
                                   [](const unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
            if (mode == "mock" || mode == "haru") {
                return mode;
            }
            return "auto";
        }

        void setup_mock_backend() {
            backend_ = std::make_shared<docraft::test::utils::MockRenderingBackend>(
                docraft::test::utils::MockRenderingBackend::Config{
                    .page_width = 100.0F,
                    .page_height = 100.0F,
                    .text_width_factor = 5.0F,
                    .initial_pages = 1,
                    .extension = ".pdf",
                    .can_use_font = true
                });
            context_ = std::make_shared<DocraftDocumentContext>(
                std::make_shared<docraft::test::utils::MockBackendProvidersFactory>(backend_));
            context_->edit_layout().set_page_dimensions(100.0F, 100.0F);
            use_haru_ = false;
        }

        void setup_haru_backend() {
            context_ = std::make_shared<DocraftDocumentContext>(
                std::make_shared<backend::pdf::DocraftHaruCapabilityProvidersFactory>());

            const auto page_backend = context_->edit_rendering().edit_page_rendering();
            if (!page_backend) {
                throw std::runtime_error("Haru page backend is not available");
            }

            page_backend->set_page_format(model::DocraftPageSize::kA4, model::DocraftPageOrientation::kPortrait);
            context_->edit_layout().set_page_dimensions(page_backend->page_width(), page_backend->page_height());
            use_haru_ = true;
        }

        [[nodiscard]] bool try_setup_haru_backend() {
            try {
                setup_haru_backend();
                return true;
            } catch (...) {
                return false;
            }
        }

        std::shared_ptr<docraft::test::utils::MockRenderingBackend> backend_;
        std::shared_ptr<DocraftDocumentContext> context_;
        std::unique_ptr<docraft::layout::DocraftLayoutEngine> engine_;
        bool use_haru_ = false;
    };

    // Stress sequential block flow: ensures repeated overflow keeps deterministic owner ordering.
    TEST_F(DocraftPaginationStressTest, ManyBlocksPaginateAcrossMultiplePagesKeepingOrder) {
        auto body = std::make_shared<model::DocraftBody>();
        body->set_margin_left(0.0F);
        body->set_margin_right(0.0F);

        std::vector<std::shared_ptr<model::DocraftRectangle> > blocks;
        blocks.reserve(24);
        for (int i = 0; i < 24; ++i) {
            auto rect = std::make_shared<model::DocraftRectangle>();
            rect->set_height(40.0F);
            body->add_child(rect);
            blocks.push_back(rect);
        }

        std::vector<std::shared_ptr<model::DocraftNode> > nodes{body};
        layout_and_render(nodes);

        EXPECT_GE(total_page_count(), page_threshold(6U, 2U));

        int previous_owner = 1;
        for (const auto &block: blocks) {
            EXPECT_GE(block->page_owner(), 1);
            EXPECT_GE(block->page_owner(), previous_owner);
            previous_owner = block->page_owner();
        }
    }

    // Stress row-fit logic with asymmetric cell heights and validate no row is lost after splitting.
    TEST_F(DocraftPaginationStressTest, MixedHeightTableRowsSplitWithoutRowLoss) {
        auto body = std::make_shared<model::DocraftBody>();
        body->set_margin_left(0.0F);
        body->set_margin_right(0.0F);

        auto table = std::make_shared<model::DocraftTable>();
        table->set_titles({"A", "B"});
        table->set_column_weights({0.5F, 0.5F});
        table->set_auto_fill_width(true);

        constexpr std::size_t expected_rows = 8;
        for (std::size_t i = 0; i < expected_rows; ++i) {
            auto c_left = std::make_shared<model::DocraftRectangle>();
            c_left->set_height(6.0F);
            auto c_right = std::make_shared<model::DocraftRectangle>();
            c_right->set_height((i % 2 == 0) ? 34.0F : 44.0F);
            table->add_content_node(c_left);
            table->add_content_node(c_right);
        }

        body->add_child(table);

        std::vector<std::shared_ptr<model::DocraftNode> > nodes{body};
        layout_and_render(nodes);

        EXPECT_GE(total_page_count(), page_threshold(2U, 1U));

        std::size_t accumulated_rows = 0;
        std::size_t table_fragments = 0;
        for (const auto &node: body->children()) {
            auto table_fragment = std::dynamic_pointer_cast<model::DocraftTable>(node);
            if (!table_fragment) {
                continue;
            }
            ++table_fragments;
            accumulated_rows += static_cast<std::size_t>(table_fragment->rows());
            EXPECT_GT(table_fragment->rows(), 0);
        }

        EXPECT_GE(table_fragments, fragment_threshold(2U, 1U));
        EXPECT_EQ(accumulated_rows, expected_rows);
    }

    // Stress recursive layout traversal depth and verify geometry remains finite and non-degenerate.
    TEST_F(DocraftPaginationStressTest, DeepNestedLayoutsWithManyNodesRemainStable) {
        auto body = std::make_shared<model::DocraftBody>();
        body->set_margin_left(0.0F);
        body->set_margin_right(0.0F);

        auto root = std::make_shared<model::DocraftLayout>();
        root->set_orientation(model::LayoutOrientation::kVertical);
        body->add_child(root);

        auto current = root;
        for (int depth = 0; depth < 30; ++depth) {
            auto nested = std::make_shared<model::DocraftLayout>();
            nested->set_orientation(depth % 2 == 0
                                        ? model::LayoutOrientation::kVertical
                                        : model::LayoutOrientation::kHorizontal);

            for (int j = 0; j < 8; ++j) {
                auto rect = std::make_shared<model::DocraftRectangle>();
                rect->set_height(6.0F + static_cast<float>((depth + j) % 5));
                rect->set_weight(1.0F / 8.0F);
                nested->add_child(rect);
            }

            current->add_child(nested);
            current = nested;
        }

        std::vector<std::shared_ptr<model::DocraftNode> > nodes{body};
        layout_and_render(nodes);

        EXPECT_TRUE(std::isfinite(root->width()));
        EXPECT_TRUE(std::isfinite(root->height()));
        EXPECT_GT(root->width(), 0.0F);
        EXPECT_GT(root->height(), 0.0F);
        EXPECT_GE(total_page_count(), 1U);
    }

    // Stress explicit page-break churn and verify page ownership remains monotonic.
    TEST_F(DocraftPaginationStressTest, ManyExplicitNewPagesDoNotCorruptOwnership) {
        auto body = std::make_shared<model::DocraftBody>();
        body->set_margin_left(0.0F);
        body->set_margin_right(0.0F);

        std::vector<std::shared_ptr<model::DocraftRectangle> > payload_nodes;
        payload_nodes.reserve(12);

        for (int i = 0; i < 12; ++i) {
            auto rect = std::make_shared<model::DocraftRectangle>();
            rect->set_height(8.0F);
            body->add_child(rect);
            payload_nodes.push_back(rect);

            auto page_break = std::make_shared<model::DocraftNewPage>();
            body->add_child(page_break);
        }

        std::vector<std::shared_ptr<model::DocraftNode> > nodes{body};
        layout_and_render(nodes);

        EXPECT_GE(total_page_count(), 12U);

        int previous_owner = 1;
        for (const auto &node: payload_nodes) {
            EXPECT_GE(node->page_owner(), previous_owner);
            previous_owner = node->page_owner();
        }
    }

    // Stress large table fragmentation over many pages and assert split integrity by row count.
    TEST_F(DocraftPaginationStressTest, VeryLargeTableSplitPreservesAllRows) {
        auto body = std::make_shared<model::DocraftBody>();
        body->set_margin_left(0.0F);
        body->set_margin_right(0.0F);

        auto table = std::make_shared<model::DocraftTable>();
        table->set_titles({"C1", "C2", "C3"});
        table->set_column_weights({0.34F, 0.33F, 0.33F});
        table->set_auto_fill_width(true);

        constexpr std::size_t kExpectedRows = 120;
        for (std::size_t r = 0; r < kExpectedRows; ++r) {
            for (int c = 0; c < 3; ++c) {
                auto cell = std::make_shared<model::DocraftRectangle>();
                cell->set_height(6.0F + static_cast<float>((r + static_cast<std::size_t>(c)) % 7));
                table->add_content_node(cell);
            }
        }

        body->add_child(table);

        std::vector<std::shared_ptr<model::DocraftNode> > nodes{body};
        layout_and_render(nodes);

        std::size_t fragments = 0;
        std::size_t accumulated_rows = 0;
        int max_owner = 1;

        for (const auto &node: body->children()) {
            auto fragment = std::dynamic_pointer_cast<model::DocraftTable>(node);
            if (!fragment) {
                continue;
            }
            ++fragments;
            accumulated_rows += static_cast<std::size_t>(fragment->rows());
            EXPECT_GT(fragment->rows(), 0);
            EXPECT_GE(fragment->page_owner(), 1);
            max_owner = std::max(max_owner, fragment->page_owner());
        }

        EXPECT_GE(fragments, fragment_threshold(5U, 2U));
        EXPECT_EQ(accumulated_rows, kExpectedRows);
        EXPECT_GE(static_cast<std::size_t>(max_owner), 2U);
    }

    // Brutal mixed scenario: deep nesting, forced page breaks, and huge table in one run.
    TEST_F(DocraftPaginationStressTest, BrutalCompositePaginationStressKeepsIntegrity) {
        auto body = std::make_shared<model::DocraftBody>();
        body->set_margin_left(0.0F);
        body->set_margin_right(0.0F);

        // Deep nested layout chain with many children per level.
        auto root = std::make_shared<model::DocraftLayout>();
        root->set_orientation(model::LayoutOrientation::kVertical);
        body->add_child(root);

        auto current = root;
        for (int depth = 0; depth < 40; ++depth) {
            auto nested = std::make_shared<model::DocraftLayout>();
            nested->set_orientation(depth % 2 == 0
                                        ? model::LayoutOrientation::kHorizontal
                                        : model::LayoutOrientation::kVertical);
            for (int j = 0; j < 10; ++j) {
                auto rect = std::make_shared<model::DocraftRectangle>();
                rect->set_height(5.0F + static_cast<float>((depth + j) % 9));
                rect->set_weight(0.1F);
                nested->add_child(rect);
            }
            current->add_child(nested);
            current = nested;
        }

        // Add explicit hard page breaks with payload nodes.
        std::vector<std::shared_ptr<model::DocraftRectangle> > payload_nodes;
        payload_nodes.reserve(30);
        for (int i = 0; i < 30; ++i) {
            auto payload = std::make_shared<model::DocraftRectangle>();
            payload->set_height(9.0F + static_cast<float>(i % 4));
            body->add_child(payload);
            payload_nodes.push_back(payload);
            body->add_child(std::make_shared<model::DocraftNewPage>());
        }

        // Massive table that must split many times.
        auto huge_table = std::make_shared<model::DocraftTable>();
        huge_table->set_titles({"A", "B", "C", "D"});
        huge_table->set_column_weights({0.25F, 0.25F, 0.25F, 0.25F});
        huge_table->set_auto_fill_width(true);

        constexpr std::size_t kExpectedRows = 300;
        for (std::size_t r = 0; r < kExpectedRows; ++r) {
            for (int c = 0; c < 4; ++c) {
                auto cell = std::make_shared<model::DocraftRectangle>();
                cell->set_height(4.0F + static_cast<float>((r + static_cast<std::size_t>(c)) % 11));
                huge_table->add_content_node(cell);
            }
        }
        body->add_child(huge_table);

        std::vector<std::shared_ptr<model::DocraftNode> > nodes{body};
        layout_and_render(nodes);

        EXPECT_GE(total_page_count(), page_threshold(20U, 20U));
        EXPECT_TRUE(std::isfinite(root->width()));
        EXPECT_TRUE(std::isfinite(root->height()));
        EXPECT_GT(root->width(), 0.0F);
        EXPECT_GT(root->height(), 0.0F);

        int previous_owner = 1;
        for (const auto &node: payload_nodes) {
            EXPECT_GE(node->page_owner(), previous_owner);
            previous_owner = node->page_owner();
        }

        std::size_t fragments = 0;
        std::size_t accumulated_rows = 0;
        for (const auto &node: body->children()) {
            auto fragment = std::dynamic_pointer_cast<model::DocraftTable>(node);
            if (!fragment) {
                continue;
            }
            ++fragments;
            accumulated_rows += static_cast<std::size_t>(fragment->rows());
            EXPECT_GT(fragment->rows(), 0);
            EXPECT_GE(fragment->page_owner(), 1);
        }

        EXPECT_GE(fragments, fragment_threshold(8U, 2U));
        EXPECT_EQ(accumulated_rows, kExpectedRows);
    }

    // All-elements scenario: combines section nodes plus heterogeneous content and pagination pressure.
    TEST_F(DocraftPaginationStressTest, AllElementsCompositeStressScenarioRemainsConsistent) {
        auto header = std::make_shared<model::DocraftHeader>();
        header->set_margin_left(0.0F);
        header->set_margin_right(0.0F);
        auto header_title = std::make_shared<model::DocraftText>("Stress Header");
        header->add_child(header_title);

        auto body = std::make_shared<model::DocraftBody>();
        body->set_margin_left(0.0F);
        body->set_margin_right(0.0F);

        auto footer = std::make_shared<model::DocraftFooter>();
        footer->set_margin_left(0.0F);
        footer->set_margin_right(0.0F);
        auto page_number = std::make_shared<model::DocraftPageNumber>();
        footer->add_child(page_number);

        // Main flow layout containing heterogeneous node types.
        auto root = std::make_shared<model::DocraftLayout>();
        root->set_orientation(model::LayoutOrientation::kVertical);

        auto text = std::make_shared<model::DocraftText>("Long stress paragraph for layout engine");
        text->set_font_size(9.0F);
        root->add_child(text);

        auto blank = std::make_shared<model::DocraftBlankLine>();
        root->add_child(blank);

        auto list = std::make_shared<model::DocraftList>();
        list->set_kind(model::ListKind::kOrdered);
        list->set_ordered_style(model::OrderedListStyle::kRoman);
        for (int i = 0; i < 6; ++i) {
            auto item = std::make_shared<model::DocraftText>("Item " + std::to_string(i));
            list->add_child(item);
        }
        root->add_child(list);

        auto image = std::make_shared<model::DocraftImage>();
        image->set_raw_data({255, 0, 0}, 1, 1);
        image->set_width(100.0F);
        image->set_height(100.0F);
        root->add_child(image);

        auto circle = std::make_shared<model::DocraftCircle>();
        circle->set_width(40.0F);
        circle->set_height(40.0F);
        circle->set_position_mode(model::DocraftPositionType::kAbsolute);
        circle->set_background_color(DocraftColor::fromColorName(ColorName::kBlue));
        circle->set_position({.x = 500.0F, .y = 500.0F});
        root->add_child(circle);

        auto line = std::make_shared<model::DocraftLine>();
        line->set_start({.x = 100.0F, .y = 0.0F});
        line->set_end({.x = 500.0F, .y = 500.0F});
        line->set_border_color(DocraftColor::fromColorName(ColorName::kRed));
        line->set_border_width(2.0F);

        root->add_child(line);

        auto triangle = std::make_shared<model::DocraftTriangle>();
        triangle->set_points({
            {.x = 0.0F, .y = 0.0F},
            {.x = 14.0F, .y = 0.0F},
            {.x = 7.0F, .y = 12.0F}
        });
        triangle->set_width(14.0F);
        triangle->set_height(12.0F);
        triangle->set_background_color(DocraftColor::fromColorName(ColorName::kGreen));
        root->add_child(triangle);

        auto polygon = std::make_shared<model::DocraftPolygon>();
        polygon->set_points({
            {.x = 0.0F, .y = 0.0F},
            {.x = 8.0F, .y = 0.0F},
            {.x = 12.0F, .y = 6.0F},
            {.x = 6.0F, .y = 12.0F},
            {.x = 0.0F, .y = 6.0F}
        });
        polygon->set_width(12.0F);
        polygon->set_height(12.0F);
        polygon->set_background_color(DocraftColor::fromColorName(ColorName::kYellow));
        root->add_child(polygon);

        // Foreach with rendered children stress-path.
        auto foreach_node = std::make_shared<model::DocraftForeach>();
        foreach_node->set_model("${items}");
        for (int i = 0; i < 8; ++i) {
            auto foreach_text = std::make_shared<model::DocraftText>("Foreach child " + std::to_string(i));
            foreach_node->add_child(foreach_text);
        }
        root->add_child(foreach_node);

        // Heavy table to trigger splits.
        auto table = std::make_shared<model::DocraftTable>();
        table->set_titles({"C1", "C2", "C3"});
        table->set_column_weights({0.34F, 0.33F, 0.33F});
        table->set_auto_fill_width(true);
        constexpr std::size_t kRows = 140;
        for (std::size_t r = 0; r < kRows; ++r) {
            for (int c = 0; c < 3; ++c) {
                auto cell = std::make_shared<model::DocraftRectangle>();
                cell->set_height(6.0F + static_cast<float>((r + static_cast<std::size_t>(c)) % 9));
                table->add_content_node(cell);
            }
        }
        body->add_child(root);
        body->add_child(std::make_shared<model::DocraftNewPage>());
        body->add_child(table);

        // Add explicit page breaks intermixed with payload to stress owner propagation.
        std::vector<std::shared_ptr<model::DocraftRectangle> > payload_nodes;
        for (int i = 0; i < 12; ++i) {
            auto payload = std::make_shared<model::DocraftRectangle>();
            payload->set_height(10.0F + static_cast<float>(i % 3));
            body->add_child(payload);
            payload_nodes.push_back(payload);
            payload->set_background_color(DocraftColor::fromColorName(ColorName::kCyan));
            body->add_child(std::make_shared<model::DocraftNewPage>());
        }

        std::vector<std::shared_ptr<model::DocraftNode> > nodes{header, body, footer};
        layout_and_render(nodes);

        EXPECT_GE(total_page_count(), 8U);
        EXPECT_EQ(header->page_owner(), -1);
        EXPECT_EQ(footer->page_owner(), -1);
        EXPECT_TRUE(std::isfinite(root->width()));
        EXPECT_TRUE(std::isfinite(root->height()));
        EXPECT_GT(root->width(), 0.0F);
        EXPECT_GT(root->height(), 0.0F);

        int previous_owner = 1;
        for (const auto &payload: payload_nodes) {
            EXPECT_GE(payload->page_owner(), previous_owner);
            previous_owner = payload->page_owner();
        }

        std::size_t table_fragments = 0;
        std::size_t accumulated_rows = 0;
        for (const auto &node: body->children()) {
            auto table_fragment = std::dynamic_pointer_cast<model::DocraftTable>(node);
            if (!table_fragment) {
                continue;
            }
            ++table_fragments;
            accumulated_rows += static_cast<std::size_t>(table_fragment->rows());
            EXPECT_GT(table_fragment->rows(), 0);
        }

        EXPECT_GE(table_fragments, 2U);
        EXPECT_EQ(accumulated_rows, kRows);
    }
} // namespace docraft::test::layout
