#include <gtest/gtest.h>

#include "docraft/loom/nodes/docraft_loom_new_page.h"
#include "docraft/loom/nodes/docraft_loom_table.h"
#include "docraft/loom/nodes/docraft_loom_table_cell.h"
#include "docraft/loom/nodes/docraft_loom_text.h"
#include "docraft/loom/nodes/docraft_loom_vstack.h"
#include "docraft/loom/pipeline/docraft_loom_pagination_processor.h"
#include "docraft/utils/docraft_mock_rendering_backend.h"

namespace docraft::test {
    namespace {
        std::shared_ptr<loom::nodes::DocraftLoomTableCell> make_positioned_cell(float y, float height, bool is_title) {
            auto cell = std::make_shared<loom::nodes::DocraftLoomTableCell>();
            cell->set_content(std::make_shared<loom::nodes::DocraftLoomText>("x"));
            cell->set_is_title(is_title);
            cell->edit_layout_box().frame = {
                .position = {.x = 0.0F, .y = y}, .size = {.width = 50.0F, .height = height}
            };
            return cell;
        }

        // 100x100 mock page, the size every test in this file lays its content out
        // against.
        utils::MockPageBackend make_standard_page_backend() {
            auto config = utils::MockBackendSharedState::Config{};
            config.page_width = 100.0F;
            config.page_height = 100.0F;
            return utils::MockPageBackend{std::make_shared<utils::MockBackendSharedState>(config)};
        }

        // A VStack body with a single positioned DocraftLoomText child -- optionally
        // named, for tests that need to assert the overflow warning names the node.
        std::shared_ptr<loom::nodes::DocraftLoomVStack> make_single_text_body(
            const std::string& text, float y, float height, const std::string& name = "") {
            auto node = std::make_shared<loom::nodes::DocraftLoomText>(text);
            if (!name.empty()) {
                node->set_name(name);
            }
            node->edit_layout_box().frame = {
                .position = {.x = 0.0F, .y = y}, .size = {.width = 50.0F, .height = height}
            };
            auto body = std::make_shared<loom::nodes::DocraftLoomVStack>();
            body->add_child(node);
            return body;
        }
    } // namespace
    TEST(DocraftLoomPaginationProcessorTest, NewPageForcesBreakRegardlessOfRemainingSpace)
    {
        auto config = utils::MockBackendSharedState::Config{};
        config.page_width = 100.0F;
        config.page_height = 100.0F;
        auto state = std::make_shared<utils::MockBackendSharedState>(config);
        utils::MockPageBackend page_backend{state};

        auto body = std::make_shared<loom::nodes::DocraftLoomVStack>();

        auto first = std::make_shared<loom::nodes::DocraftLoomText>("first");
        first->edit_layout_box().frame = {
            .position = {.x = 0.0F, .y = 10.0F}, .size = {.width = 50.0F, .height = 10.0F}
        };
        body->add_child(first);

        // Plenty of room left on the current page -- without the forced-break special
        // case, `second` below would simply continue flowing right after `first`.
        body->add_child(std::make_shared<loom::nodes::DocraftLoomNewPage>());

        auto second = std::make_shared<loom::nodes::DocraftLoomText>("second");
        second->edit_layout_box().frame = {
            .position = {.x = 0.0F, .y = 20.0F}, .size = {.width = 50.0F, .height = 10.0F}
        };
        body->add_child(second);

        loom::pipeline::DocraftLoomPaginationProcessor processor;
        const int total_pages =
            processor.paginate_body(*body, /*body_top_y=*/10.0F, /*body_height=*/80.0F, &page_backend);

        EXPECT_EQ(total_pages, 2);
        ASSERT_EQ(body->children_count(), 3);
        EXPECT_EQ(body->child(0)->layout_box().page_index, 0);
        EXPECT_EQ(body->child(2)->layout_box().page_index, 1);
        EXPECT_EQ(page_backend.total_page_count(), 2U);
    }

    TEST(DocraftLoomPaginationProcessorTest, NoNewPageStaysOnOnePage)
    {
        auto config = utils::MockBackendSharedState::Config{};
        config.page_width = 100.0F;
        config.page_height = 100.0F;
        auto state = std::make_shared<utils::MockBackendSharedState>(config);
        utils::MockPageBackend page_backend{state};

        auto body = std::make_shared<loom::nodes::DocraftLoomVStack>();
        auto only = std::make_shared<loom::nodes::DocraftLoomText>("only");
        only->edit_layout_box().frame = {
            .position = {.x = 0.0F, .y = 10.0F}, .size = {.width = 50.0F, .height = 10.0F}
        };
        body->add_child(only);

        loom::pipeline::DocraftLoomPaginationProcessor processor;
        const int total_pages =
            processor.paginate_body(*body, /*body_top_y=*/10.0F, /*body_height=*/80.0F, &page_backend);

        EXPECT_EQ(total_pages, 1);
    }



    // Regression test for a table shaped [title row, oversized row]: before the fix,
    // try_split_table kept re-cloning the title row onto every remainder (fit_rows
    // pinned at 1, never reaching the fit_rows == 0 bail-out), so paginate_body never
    // terminated. This test must simply complete.
    TEST(DocraftLoomPaginationProcessorTest, OversizedRowAfterHeaderIsAcceptedAsOverflowInsteadOfLooping) {
        auto config = utils::MockBackendSharedState::Config{};
        config.page_width = 100.0F;
        config.page_height = 100.0F;
        auto state = std::make_shared<utils::MockBackendSharedState>(config);
        utils::MockPageBackend page_backend{state};

        auto table = std::make_shared<loom::nodes::DocraftLoomTable>();
        table->add_row({make_positioned_cell(10.0F, 10.0F, true), make_positioned_cell(10.0F, 10.0F, true)});
        table->add_row({make_positioned_cell(20.0F, 200.0F, false), make_positioned_cell(20.0F, 200.0F, false)});
        table->edit_layout_box().frame = {
            .position = {.x = 0.0F, .y = 10.0F}, .size = {.width = 100.0F, .height = 210.0F}
        };

        auto body = std::make_shared<loom::nodes::DocraftLoomVStack>();
        body->add_child(table);

        loom::pipeline::DocraftLoomPaginationProcessor processor;
        const int total_pages =
                processor.paginate_body(*body, /*body_top_y=*/10.0F, /*body_height=*/80.0F, &page_backend);

        EXPECT_EQ(total_pages, 1);
        ASSERT_EQ(body->children_count(), 1);
        EXPECT_EQ(body->child(0)->layout_box().page_index, 0);
    }

    // Non-regression: a table with a header row that genuinely needs to split across
    // pages (one full data row fits alongside the header, the next doesn't) must still
    // split correctly, with the header cloned onto the continuation.
    TEST(DocraftLoomPaginationProcessorTest, TableWithHeaderStillSplitsAcrossPagesWhenRowsIndividuallyFit) {
        auto config = utils::MockBackendSharedState::Config{};
        config.page_width = 100.0F;
        config.page_height = 100.0F;
        auto state = std::make_shared<utils::MockBackendSharedState>(config);
        utils::MockPageBackend page_backend{state};

        auto table = std::make_shared<loom::nodes::DocraftLoomTable>();
        table->add_row({make_positioned_cell(10.0F, 10.0F, true), make_positioned_cell(10.0F, 10.0F, true)});
        table->add_row({make_positioned_cell(20.0F, 40.0F, false), make_positioned_cell(20.0F, 40.0F, false)});
        table->add_row({make_positioned_cell(60.0F, 40.0F, false), make_positioned_cell(60.0F, 40.0F, false)});
        table->edit_layout_box().frame = {
            .position = {.x = 0.0F, .y = 10.0F}, .size = {.width = 100.0F, .height = 90.0F}
        };

        auto body = std::make_shared<loom::nodes::DocraftLoomVStack>();
        body->add_child(table);

        loom::pipeline::DocraftLoomPaginationProcessor processor;
        const int total_pages =
                processor.paginate_body(*body, /*body_top_y=*/10.0F, /*body_height=*/80.0F, &page_backend);

        EXPECT_EQ(total_pages, 2);
        ASSERT_EQ(body->children_count(), 2);
        EXPECT_EQ(body->child(0)->layout_box().page_index, 0);
        EXPECT_EQ(body->child(1)->layout_box().page_index, 1);

        auto continuation = std::dynamic_pointer_cast<const loom::nodes::DocraftLoomTable>(body->child(1));
        ASSERT_NE(continuation, nullptr);
        ASSERT_EQ(continuation->row_count(), 2);
        EXPECT_TRUE(continuation->cell(0, 0)->is_title());
        EXPECT_FALSE(continuation->cell(1, 0)->is_title());
    }

    // A cell whose content was actually wrapped into multiple lines (unlike the plain,
    // never-wrapped cells above) must have its text split across as many pages as
    // needed, instead of just overflowing whole on one page.
    TEST(DocraftLoomPaginationProcessorTest, OversizedWrappedCellContentSplitsAcrossPages) {
        auto config = utils::MockBackendSharedState::Config{};
        config.page_width = 100.0F;
        config.page_height = 100.0F;
        auto state = std::make_shared<utils::MockBackendSharedState>(config);
        utils::MockPageBackend page_backend{state};

        constexpr float kLineHeight = 10.0F;
        constexpr int kLineCount = 12;
        std::vector<std::string> lines;
        for (int i = 0; i < kLineCount; ++i) {
            lines.push_back("L" + std::to_string(i));
        }

        auto wrapped_text = std::make_shared<loom::nodes::DocraftLoomText>("irrelevant -- wrapped_lines wins");
        wrapped_text->set_wrap_width(50.0F);
        wrapped_text->set_wrapped_lines(lines);
        wrapped_text->edit_layout_box().measured_size = {.width = 50.0F, .height = kLineHeight * kLineCount};

        auto content_cell = std::make_shared<loom::nodes::DocraftLoomTableCell>();
        content_cell->set_content(wrapped_text);
        const float content_natural_height =
                (kLineHeight * kLineCount) + (2.0F * loom::nodes::DocraftLoomTable::kCellPaddingY);
        content_cell->edit_layout_box().measured_size = {.width = 50.0F, .height = content_natural_height};
        content_cell->edit_layout_box().frame = {
            .position = {.x = 0.0F, .y = 20.0F}, .size = {.width = 50.0F, .height = content_natural_height}
        };

        auto table = std::make_shared<loom::nodes::DocraftLoomTable>();
        table->add_row({make_positioned_cell(10.0F, 10.0F, true), make_positioned_cell(10.0F, 10.0F, true)});
        table->add_row({content_cell, make_positioned_cell(20.0F, 10.0F, false)});
        table->edit_layout_box().frame = {
            .position = {.x = 0.0F, .y = 10.0F}, .size = {.width = 100.0F, .height = 10.0F + content_natural_height}
        };

        auto body = std::make_shared<loom::nodes::DocraftLoomVStack>();
        body->add_child(table);

        loom::pipeline::DocraftLoomPaginationProcessor processor;
        const int total_pages =
                processor.paginate_body(*body, /*body_top_y=*/10.0F, /*body_height=*/80.0F, &page_backend);

        // available_height per page = page_bottom_y(90) - row_top(20) = 70; minus cell
        // padding (2*2.5) leaves 65, which fits floor(65/10) = 6 lines per page ->
        // 12 lines split into two pages of 6.
        EXPECT_EQ(total_pages, 2);
        ASSERT_EQ(body->children_count(), 2);

        auto page0 = std::dynamic_pointer_cast<const loom::nodes::DocraftLoomTable>(body->child(0));
        auto page1 = std::dynamic_pointer_cast<const loom::nodes::DocraftLoomTable>(body->child(1));
        ASSERT_NE(page0, nullptr);
        ASSERT_NE(page1, nullptr);
        EXPECT_EQ(page0->layout_box().page_index, 0);
        EXPECT_EQ(page1->layout_box().page_index, 1);

        auto page0_text = std::dynamic_pointer_cast<const loom::nodes::DocraftLoomText>(page0->cell(1, 0)->content());
        auto page1_text = std::dynamic_pointer_cast<const loom::nodes::DocraftLoomText>(page1->cell(1, 0)->content());
        ASSERT_NE(page0_text, nullptr);
        ASSERT_NE(page1_text, nullptr);

        std::vector<std::string> reconstructed = page0_text->wrapped_lines();
        reconstructed.insert(reconstructed.end(), page1_text->wrapped_lines().begin(),
                             page1_text->wrapped_lines().end());
        EXPECT_EQ(reconstructed, lines);

        EXPECT_TRUE(page1->cell(0, 0)->is_title()); // header repeated onto the continuation
    }

    // Regression test for #58: a non-table node taller than the available page height
    // used to overflow the page bottom with no diagnostic at all (exit code 0, no
    // warning). It's still accepted as overflow -- the layout behavior is unchanged --
    // but it must now be logged so the mistake is visible in tool/CI output.
    TEST(DocraftLoomPaginationProcessorTest, OversizedNonTableNodeLogsWarning) {
        auto page_backend = make_standard_page_backend();
        auto body = make_single_text_body("way too tall", /*y=*/10.0F, /*height=*/200.0F, "overflowing_text");

        loom::pipeline::DocraftLoomPaginationProcessor processor;

        testing::internal::CaptureStdout();
        const int total_pages =
                processor.paginate_body(*body, /*body_top_y=*/10.0F, /*body_height=*/80.0F, &page_backend);
        const std::string stdout_log = testing::internal::GetCapturedStdout();

        EXPECT_EQ(total_pages, 1);
        ASSERT_EQ(body->children_count(), 1);
        EXPECT_EQ(body->child(0)->layout_box().page_index, 0);
        EXPECT_NE(stdout_log.find("[WARNING]"), std::string::npos);
        EXPECT_NE(stdout_log.find("overflowing_text"), std::string::npos);
    }

    // A node that fits within the available page height must not trigger the overflow
    // warning.
    TEST(DocraftLoomPaginationProcessorTest, FittingNodeLogsNoWarning) {
        auto page_backend = make_standard_page_backend();
        auto body = make_single_text_body("only", /*y=*/10.0F, /*height=*/10.0F);

        loom::pipeline::DocraftLoomPaginationProcessor processor;

        testing::internal::CaptureStdout();
        processor.paginate_body(*body, /*body_top_y=*/10.0F, /*body_height=*/80.0F, &page_backend);
        const std::string stdout_log = testing::internal::GetCapturedStdout();

        EXPECT_EQ(stdout_log.find("[WARNING]"), std::string::npos);
    }
} // namespace docraft::test
