#include <gtest/gtest.h>

#include <filesystem>
#include <string>

#include "docraft/backend/pdf/docraft_haru_backend.h"
#include "docraft/backend/pdf/docraft_haru_page_backend.h"

#include "docraft/docraft_document_metadata.h"
#include "docraft/exception/docraft_exceptions.h"
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

    const docraft::backend::IDocraftPageRenderingBackend& page_backend() const {
        return *backend_->page_rendering();
    }

    docraft::backend::IDocraftPageRenderingBackend& edit_page_backend() {
        return *backend_->edit_page_rendering();
    }

    const docraft::backend::IDocraftTextRenderingBackend& text_backend() const {
        return *backend_->text_rendering();
    }

    docraft::backend::IDocraftTextRenderingBackend& edit_text_backend() {
        return *backend_->edit_text_rendering();
    }

private:
    std::unique_ptr<docraft::backend::pdf::DocraftHaruBackend> backend_;
};

TEST_F(DocraftHaruBackendTest, ExposesStableCapabilityAccessors) {
    const auto& const_backend = backend();

    ASSERT_NE(const_backend.line_rendering(), nullptr);
    ASSERT_NE(const_backend.text_rendering(), nullptr);
    ASSERT_NE(const_backend.shape_rendering(), nullptr);
    ASSERT_NE(const_backend.image_rendering(), nullptr);
    ASSERT_NE(const_backend.page_rendering(), nullptr);
    ASSERT_NE(const_backend.output_backend(), nullptr);
    ASSERT_NE(const_backend.font_backend(), nullptr);
    ASSERT_NE(const_backend.metadata_backend(), nullptr);

    EXPECT_EQ(const_backend.line_rendering(), backend().edit_line_rendering());
    EXPECT_EQ(const_backend.text_rendering(), backend().edit_text_rendering());
    EXPECT_EQ(const_backend.shape_rendering(), backend().edit_shape_rendering());
    EXPECT_EQ(const_backend.image_rendering(), backend().edit_image_rendering());
    EXPECT_EQ(const_backend.page_rendering(), backend().edit_page_rendering());
    EXPECT_EQ(const_backend.output_backend(), backend().edit_output_backend());
    EXPECT_EQ(const_backend.font_backend(), backend().edit_font_backend());
    EXPECT_EQ(const_backend.metadata_backend(), backend().edit_metadata_backend());
}

TEST_F(DocraftHaruBackendTest, StartsWithSinglePageAndValidDimensions) {
    EXPECT_EQ(page_backend().total_page_count(), 1U);
    EXPECT_EQ(page_backend().current_page_number(), 1U);
    EXPECT_GT(page_backend().page_width(), 0.0F);
    EXPECT_GT(page_backend().page_height(), 0.0F);
}
TEST_F(DocraftHaruBackendTest, MoveToNextPage) {
    edit_page_backend().add_new_page();
    EXPECT_EQ(page_backend().total_page_count(), 2U);
    EXPECT_EQ(page_backend().current_page_number(), 2U);
    edit_page_backend().go_to_page(0);// Go back to first page
    edit_page_backend().move_to_next_page();
    EXPECT_EQ(page_backend().current_page_number(), 2U);
}

TEST_F(DocraftHaruBackendTest, AddsAndNavigatesPages) {
    edit_page_backend().add_new_page();
    edit_page_backend().add_new_page();

    EXPECT_EQ(page_backend().total_page_count(), 3U);
    EXPECT_EQ(page_backend().current_page_number(), 3U);

    edit_page_backend().go_to_page(0);
    EXPECT_EQ(page_backend().current_page_number(), 1U);

    edit_page_backend().move_to_next_page();
    EXPECT_EQ(page_backend().current_page_number(), 2U);

    edit_page_backend().go_to_page(2);
    EXPECT_EQ(page_backend().current_page_number(), 3U);
}

TEST_F(DocraftHaruBackendTest, NavigatesFirstPreviousLastPages) {
    edit_page_backend().add_new_page();
    edit_page_backend().add_new_page();

    edit_page_backend().go_to_first_page();
    EXPECT_EQ(page_backend().current_page_number(), 1U);

    edit_page_backend().move_to_next_page();
    EXPECT_EQ(page_backend().current_page_number(), 2U);

    edit_page_backend().go_to_last_page();
    EXPECT_EQ(page_backend().current_page_number(), 3U);

    edit_page_backend().go_to_previous_page();
    EXPECT_EQ(page_backend().current_page_number(), 2U);
}

TEST_F(DocraftHaruBackendTest, ThrowsOnPreviousAtFirstPage) {
    edit_page_backend().go_to_first_page();
    EXPECT_THROW(edit_page_backend().go_to_previous_page(), docraft::exception::PageStateException);
}

TEST_F(DocraftHaruBackendTest, SetsPageFormat) {
    EXPECT_NO_THROW(edit_page_backend().set_page_format(model::DocraftPageSize::kA3,
                                                        model::DocraftPageOrientation::kLandscape));
    EXPECT_GT(page_backend().page_width(), 0.0F);
    EXPECT_GT(page_backend().page_height(), 0.0F);
}

TEST_F(DocraftHaruBackendTest, ThrowsWhenMovingPastLastPage) {
    EXPECT_THROW(edit_page_backend().move_to_next_page(), docraft::exception::PageStateException);
}

TEST_F(DocraftHaruBackendTest, ThrowsOnInvalidPageNavigation) {
    EXPECT_THROW(edit_page_backend().go_to_page(1U), docraft::exception::PageStateException);
    EXPECT_THROW(edit_page_backend().go_to_page(2U), docraft::exception::PageStateException);
}

TEST_F(DocraftHaruBackendTest, SupportsBuiltInFontAndTextMeasure) {
    ASSERT_NE(backend().font_backend(), nullptr);
    EXPECT_TRUE(backend().font_backend()->can_use_font("Helvetica", nullptr));
    EXPECT_NO_THROW(backend().font_backend()->set_font("Helvetica", 12.0F, nullptr));

    edit_text_backend().begin_text();
    edit_text_backend().draw_text("Hello backend", 20.0F, 20.0F);
    edit_text_backend().end_text();

    EXPECT_GT(text_backend().measure_text_width("Hello backend"), 0.0F);
}

TEST_F(DocraftHaruBackendTest, ReportsPdfFileExtension) {
    ASSERT_NE(backend().output_backend(), nullptr);
    EXPECT_EQ(backend().output_backend()->file_extension(), ".pdf");
}

TEST_F(DocraftHaruBackendTest, ThrowsWhenSettingUnknownFont) {
    ASSERT_NE(backend().font_backend(), nullptr);
    EXPECT_THROW(backend().font_backend()->set_font("__missing_font__", 12.0F, nullptr),
                 docraft::exception::BackendStateException);
    EXPECT_FALSE(backend().font_backend()->can_use_font("__missing_font__", nullptr));
}

TEST_F(DocraftHaruBackendTest, SavesPdfToFile) {
    const auto output_path = std::filesystem::temp_directory_path() / "docraft_haru_backend_test_output.pdf";

    ASSERT_NE(backend().output_backend(), nullptr);
    backend().output_backend()->save_to_file(output_path.string());

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

    ASSERT_NE(backend().edit_metadata_backend(), nullptr);
    EXPECT_NO_THROW(backend().edit_metadata_backend()->set_document_metadata(metadata));

    const auto output_path = std::filesystem::temp_directory_path() / "docraft_haru_backend_test_metadata_output.pdf";
    ASSERT_NE(backend().output_backend(), nullptr);
    backend().output_backend()->save_to_file(output_path.string());

    ASSERT_TRUE(std::filesystem::exists(output_path));
    EXPECT_GT(std::filesystem::file_size(output_path), 0U);

    std::filesystem::remove(output_path);
}

TEST(DocraftHaruPageBackendLifetimeTest, ClearsProviderRegistrationOnDestruction) {
    auto state = std::make_shared<docraft::backend::pdf::DocraftHaruSharedState>();

    {
        auto page_backend = std::make_unique<docraft::backend::pdf::DocraftHaruPageBackend>(state);
        ASSERT_EQ(state->page_operations_provider, page_backend.get());
    }

    EXPECT_EQ(state->page_operations_provider, nullptr);
    EXPECT_THROW(state->ensure_page_provider(), docraft::exception::BackendStateException);
}
} // namespace docraft::test::backend
