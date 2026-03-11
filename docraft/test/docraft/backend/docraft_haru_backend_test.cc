#include <gtest/gtest.h>

#include <filesystem>
#include <string>

#include "docraft/backend/pdf/docraft_haru_backend.h"

#include "docraft/docraft_document_metadata.h"
#include "docraft/model/docraft_page_format.h"

namespace docraft::test::backend {

class DocraftHaruBackendTest : public ::testing::Test {
protected:
    void SetUp() override {
        backend_ = std::make_unique<docraft::backend::pdf::DocraftHaruBackend>();
    }

    docraft::backend::pdf::DocraftHaruBackend& backend() {
        return *backend_;
    }

private:
    std::unique_ptr<docraft::backend::pdf::DocraftHaruBackend> backend_;
};

TEST_F(DocraftHaruBackendTest, StartsWithSinglePageAndValidDimensions) {
    EXPECT_EQ(backend().total_page_count(), 1U);
    EXPECT_EQ(backend().current_page_number(), 1U);
    EXPECT_GT(backend().page_width(), 0.0F);
    EXPECT_GT(backend().page_height(), 0.0F);
}
TEST_F(DocraftHaruBackendTest, MoveToNextPage) {
    backend().add_new_page();
    EXPECT_EQ(backend().total_page_count(), 2U);
    EXPECT_EQ(backend().current_page_number(), 2U);
    backend().go_to_page(0);// Go back to first page
    backend().move_to_next_page();
    EXPECT_EQ(backend().current_page_number(), 2U);
}

TEST_F(DocraftHaruBackendTest, AddsAndNavigatesPages) {
    backend().add_new_page();
    backend().add_new_page();

    EXPECT_EQ(backend().total_page_count(), 3U);
    EXPECT_EQ(backend().current_page_number(), 3U);

    backend().go_to_page(0);
    EXPECT_EQ(backend().current_page_number(), 1U);

    backend().move_to_next_page();
    EXPECT_EQ(backend().current_page_number(), 2U);

    backend().go_to_page(2);
    EXPECT_EQ(backend().current_page_number(), 3U);
}

TEST_F(DocraftHaruBackendTest, NavigatesFirstPreviousLastPages) {
    backend().add_new_page();
    backend().add_new_page();

    backend().go_to_first_page();
    EXPECT_EQ(backend().current_page_number(), 1U);

    backend().move_to_next_page();
    EXPECT_EQ(backend().current_page_number(), 2U);

    backend().go_to_last_page();
    EXPECT_EQ(backend().current_page_number(), 3U);

    backend().go_to_previous_page();
    EXPECT_EQ(backend().current_page_number(), 2U);
}

TEST_F(DocraftHaruBackendTest, ThrowsOnPreviousAtFirstPage) {
    backend().go_to_first_page();
    EXPECT_THROW(backend().go_to_previous_page(), std::runtime_error);
}

TEST_F(DocraftHaruBackendTest, SetsPageFormat) {
    EXPECT_NO_THROW(backend().set_page_format(model::DocraftPageSize::kA3,
                                              model::DocraftPageOrientation::kLandscape));
    EXPECT_GT(backend().page_width(), 0.0F);
    EXPECT_GT(backend().page_height(), 0.0F);
}

TEST_F(DocraftHaruBackendTest, ThrowsWhenMovingPastLastPage) {
    EXPECT_THROW(backend().move_to_next_page(), std::runtime_error);
}

TEST_F(DocraftHaruBackendTest, ThrowsOnInvalidPageNavigation) {
    EXPECT_THROW(backend().go_to_page(1U), std::runtime_error);
    EXPECT_THROW(backend().go_to_page(2U), std::runtime_error);
}

TEST_F(DocraftHaruBackendTest, SupportsBuiltInFontAndTextMeasure) {
    EXPECT_TRUE(backend().can_use_font("Helvetica", nullptr));
    EXPECT_NO_THROW(backend().set_font("Helvetica", 12.0F, nullptr));

    backend().begin_text();
    backend().draw_text("Hello backend", 20.0F, 20.0F);
    backend().end_text();

    EXPECT_GT(backend().measure_text_width("Hello backend"), 0.0F);
}

TEST_F(DocraftHaruBackendTest, ReportsPdfFileExtension) {
    EXPECT_EQ(backend().file_extension(), ".pdf");
}

TEST_F(DocraftHaruBackendTest, ThrowsWhenSettingUnknownFont) {
    EXPECT_THROW(backend().set_font("__missing_font__", 12.0F, nullptr), std::runtime_error);
    EXPECT_FALSE(backend().can_use_font("__missing_font__", nullptr));
}

TEST_F(DocraftHaruBackendTest, SavesPdfToFile) {
    const auto output_path = std::filesystem::temp_directory_path() / "docraft_haru_backend_test_output.pdf";

    backend().save_to_file(output_path.string());

    ASSERT_TRUE(std::filesystem::exists(output_path));
    EXPECT_GT(std::filesystem::file_size(output_path), 0U);

    std::filesystem::remove(output_path);
}

TEST_F(DocraftHaruBackendTest, SavesPdfWithMetadataInfo) {
    DocraftDocumentMetadata metadata;
    metadata.set_title("Docraft Metadata Title");
    metadata.set_author("Docraft Metadata Author");
    metadata.set_creator("Docraft Metadata Creator");
    metadata.set_producer("Docraft Metadata Producer");
    metadata.set_subject("Docraft Metadata Subject");
    metadata.set_keywords("docraft,metadata,test");
    metadata.set_trapped("False");
    metadata.set_gts_pdfx("PDF/X-3:2002");
    metadata.set_creation_date({2026, 2, 20, 8, 30, 15, '+', 0, 0});
    metadata.set_modification_date({2026, 2, 20, 9, 45, 10, '+', 0, 0});

    EXPECT_NO_THROW(backend().set_document_metadata(metadata));

    const auto output_path = std::filesystem::temp_directory_path() / "docraft_haru_backend_test_metadata_output.pdf";
    backend().save_to_file(output_path.string());

    ASSERT_TRUE(std::filesystem::exists(output_path));
    EXPECT_GT(std::filesystem::file_size(output_path), 0U);

    std::filesystem::remove(output_path);
}

} // namespace docraft::test::backend
