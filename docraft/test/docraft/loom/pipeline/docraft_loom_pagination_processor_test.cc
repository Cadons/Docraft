#include <gtest/gtest.h>

#include "docraft/loom/nodes/docraft_loom_new_page.h"
#include "docraft/loom/nodes/docraft_loom_text.h"
#include "docraft/loom/nodes/docraft_loom_vstack.h"
#include "docraft/loom/pipeline/docraft_loom_pagination_processor.h"
#include "docraft/utils/docraft_mock_rendering_backend.h"

namespace docraft::test {
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
} // namespace docraft::test
