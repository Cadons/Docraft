#pragma once
#include <string>
namespace docraft::utils {
        /**
         * @brief Simple logging utility for console output.
         */
        class DocraftLogger {
                public:
              /**
               * @brief Logs a debug-level message.
               * @param message Message to log.
               */
              static void debug(const std::string& message);
              /**
               * @brief Logs an info-level message.
               * @param message Message to log.
               */
              static void info(const std::string& message);
              /**
               * @brief Logs a warning-level message.
               * @param message Message to log.
               */
              static void warning(const std::string& message);
              /**
               * @brief Logs an error-level message.
               * @param message Message to log.
               */
              static void error(const std::string& message);
        private:
            /**
             * @brief Internal logger for non-error levels.
             * @param message Message to log.
             */
            static void log(const std::string& message);
            /**
             * @brief Internal logger for error levels.
             * @param message Message to log.
             */
            static void log_error(const std::string& message);
        };
#define LOG_INFO(message) docraft::utils::DocraftLogger::info(message)
#define LOG_WARNING(message) docraft::utils::DocraftLogger::warning(message)
#define LOG_ERROR(message) docraft::utils::DocraftLogger::error(message)
#define LOG_DEBUG(message) docraft::utils::DocraftLogger::debug(message)
} // docraft
