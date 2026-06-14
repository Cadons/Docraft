#include <type_traits>

#include <gtest/gtest.h>

#include "docraft/exception/docraft_exceptions.h"
#include "docraft/utils/docraft_mock_rendering_backend.h"

// DocraftException is a custom polymorphic base type.
static_assert(std::is_polymorphic_v<docraft::exception::DocraftException>);
static_assert(std::is_base_of_v<docraft::exception::DocraftException,
    docraft::exception::ConfigurationException>);
static_assert(std::is_base_of_v<docraft::exception::DocraftException,
    docraft::exception::BackendStateException>);
static_assert(std::is_base_of_v<docraft::exception::DocraftException,
    docraft::exception::DataFormatException>);
static_assert(std::is_base_of_v<docraft::exception::DocraftException,
    docraft::exception::FileSystemException>);

TEST(DocraftExceptionTest, MockBackendThrowsCustomBackendStateException) {
    auto config = docraft::test::utils::MockBackendSharedState::Config{};
    config.supports_page_backend = false;
    auto state = std::make_shared<docraft::test::utils::MockBackendSharedState>(config);

    EXPECT_THROW(state->ensure_page_available(), docraft::exception::BackendStateException);
}

TEST(DocraftExceptionTest, MockPageBackendThrowsCustomBackendStateException) {
    auto config = docraft::test::utils::MockBackendSharedState::Config{};
    config.initial_pages = 1;
    config.supports_page_backend = true;
    auto state = std::make_shared<docraft::test::utils::MockBackendSharedState>(config);
    docraft::test::utils::MockPageBackend page_backend{state};

    EXPECT_THROW(page_backend.move_to_next_page(), docraft::exception::BackendStateException);
}




