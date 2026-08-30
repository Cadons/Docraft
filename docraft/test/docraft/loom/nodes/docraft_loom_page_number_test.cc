#include <gtest/gtest.h>

#include "docraft/loom/nodes/docraft_loom_page_number.h"
#include "docraft/loom/pipeline/docraft_loom_rendering_processor.h"
#include "docraft/utils/docraft_mock_rendering_backend.h"
#include "docraft/utils/docraft_loom_layout_box_test_access.h"

namespace docraft::test {
    TEST(DocraftLoomPageNumberTest, DefaultFormatRendersBareCurrentPageNumber)
    {
        utils::MockRenderingBackend backend;
        loom::pipeline::DocraftLoomRenderingProcessor rendering(&backend);
        rendering.set_current_page(0, 1); // 0-based page index 0 -> "1"

        loom::nodes::DocraftLoomPageNumber page_number;
        // visit(Text*) (which visit(PageNumber*) delegates to) reads the node's frame
        // to position the draw -- irrelevant to this test's actual assertion (just the
        // formatted text), but still required to have been sealed by Layout.
        page_number.edit_layout_box().edit_frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof());
        page_number.accept(rendering);

        EXPECT_EQ(page_number.text(), "1");
    }

    TEST(DocraftLoomPageNumberTest, FormatSubstitutesPageAndTotalPlaceholders)
    {
        utils::MockRenderingBackend backend;
        loom::pipeline::DocraftLoomRenderingProcessor rendering(&backend);
        rendering.set_current_page(2, 5); // 0-based page index 2 -> "3"

        loom::nodes::DocraftLoomPageNumber page_number;
        // A PageNumber (typically in a Header/Footer) renders on every page, which
        // DocraftLoomPaginationProcessor marks with page_index == -1; the default
        // (unpaginated) page_index of 0 would otherwise only match page 1.
        page_number.edit_layout_box().set_page_index(-1, docraft::test::utils::LayoutBoxTestAccess::make_page_index_proof());
        page_number.edit_layout_box().edit_frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof());
        page_number.set_format("Page {page} of {total}");
        page_number.accept(rendering);

        EXPECT_EQ(page_number.text(), "Page 3 of 5");
    }

    TEST(DocraftLoomPageNumberTest, FormatWithRepeatedPlaceholderSubstitutesEveryOccurrence)
    {
        utils::MockRenderingBackend backend;
        loom::pipeline::DocraftLoomRenderingProcessor rendering(&backend);
        rendering.set_current_page(0, 3);

        loom::nodes::DocraftLoomPageNumber page_number;
        page_number.edit_layout_box().edit_frame(docraft::test::utils::LayoutBoxTestAccess::make_layout_proof());
        page_number.set_format("{page}/{total} (page {page})");
        page_number.accept(rendering);

        EXPECT_EQ(page_number.text(), "1/3 (page 1)");
    }
} // namespace docraft::test
