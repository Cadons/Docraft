#include <gtest/gtest.h>

#include "docraft/loom/nodes/docraft_loom_page_number.h"
#include "docraft/loom/pipeline/docraft_loom_rendering_processor.h"
#include "docraft/utils/docraft_mock_rendering_backend.h"

namespace docraft::test {
    TEST(DocraftLoomPageNumberTest, DefaultFormatRendersBareCurrentPageNumber)
    {
        utils::MockRenderingBackend backend;
        loom::pipeline::DocraftLoomRenderingProcessor rendering(&backend);
        rendering.set_current_page(0, 1); // 0-based page index 0 -> "1"

        loom::nodes::DocraftLoomPageNumber page_number;
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
        page_number.edit_layout_box().page_index = -1;
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
        page_number.set_format("{page}/{total} (page {page})");
        page_number.accept(rendering);

        EXPECT_EQ(page_number.text(), "1/3 (page 1)");
    }
} // namespace docraft::test
