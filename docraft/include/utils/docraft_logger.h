#pragma once
#include <string>
namespace docraft::utils {
        class DocraftLogger {
                public:
              static void debug(const std::string& message);
              static void info(const std::string& message);
              static void warning(const std::string& message);
              static void error(const std::string& message);
        private:
            static void log(const std::string& message);
            static void log_error(const std::string& message);
        };
#define LOG_INFO(message) docraft::utils::DocraftLogger::info(message)
#define LOG_WARNING(message) docraft::utils::DocraftLogger::warning(message)
#define LOG_ERROR(message) docraft::utils::DocraftLogger::error(message)
#define LOG_DEBUG(message) docraft::utils::DocraftLogger::debug(message)
} // docraft
