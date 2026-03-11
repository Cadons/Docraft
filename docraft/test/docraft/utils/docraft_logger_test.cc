#include "docraft/utils/docraft_logger.h"

#include <gtest/gtest.h>

namespace docraft::test::utils {
    class DocraftLoggerTest : public ::testing::Test {
        protected:
            void SetUp() override {
                docraft::utils::DocraftLogger::reset_levels();
            }

            void TearDown() override {
                docraft::utils::DocraftLogger::reset_levels();
            }
    };

    TEST_F(DocraftLoggerTest, DefaultLevelsEnableOnlyWarningAndError) {
        EXPECT_FALSE(docraft::utils::DocraftLogger::is_level_enabled(docraft::utils::DocraftLogger::LogLevel::kDebug));
        EXPECT_FALSE(docraft::utils::DocraftLogger::is_level_enabled(docraft::utils::DocraftLogger::LogLevel::kInfo));
        EXPECT_TRUE(docraft::utils::DocraftLogger::is_level_enabled(docraft::utils::DocraftLogger::LogLevel::kWarning));
        EXPECT_TRUE(docraft::utils::DocraftLogger::is_level_enabled(docraft::utils::DocraftLogger::LogLevel::kError));

        testing::internal::CaptureStdout();
        LOG_DEBUG("debug message");
        LOG_INFO("info message");
        LOG_WARNING("warning message");
        const std::string stdout_log = testing::internal::GetCapturedStdout();

        EXPECT_EQ(stdout_log.find("[DEBUG]"), std::string::npos);
        EXPECT_EQ(stdout_log.find("[INFO]"), std::string::npos);
        EXPECT_NE(stdout_log.find("[WARNING]: warning message"), std::string::npos);

        testing::internal::CaptureStderr();
        LOG_ERROR("error message");
        const std::string stderr_log = testing::internal::GetCapturedStderr();
        EXPECT_NE(stderr_log.find("[ERROR]: error message"), std::string::npos);
    }

    TEST_F(DocraftLoggerTest, CanEnableInfoAndDebug) {
        docraft::utils::DocraftLogger::enable_debug();
        docraft::utils::DocraftLogger::enable_info();

        EXPECT_TRUE(docraft::utils::DocraftLogger::is_level_enabled(docraft::utils::DocraftLogger::LogLevel::kDebug));
        EXPECT_TRUE(docraft::utils::DocraftLogger::is_level_enabled(docraft::utils::DocraftLogger::LogLevel::kInfo));

        testing::internal::CaptureStdout();
        LOG_DEBUG("debug enabled");
        LOG_INFO("info enabled");
        const std::string stdout_log = testing::internal::GetCapturedStdout();

        EXPECT_NE(stdout_log.find("[DEBUG]: debug enabled"), std::string::npos);
        EXPECT_NE(stdout_log.find("[INFO]: info enabled"), std::string::npos);
    }
} // namespace docraft::test::utils
