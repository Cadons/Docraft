/*
 * Copyright 2026 Matteo Cadoni (https://github.com/cadons)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "docraft/docraft_lib.h"
#include <string>
namespace docraft::utils {
    /**
     * @brief Simple logging utility for console output.
     */
    class DOCRAFT_LIB DocraftLogger {
        public:
            enum class LogLevel {
                kDebug,
                kInfo,
                kWarning,
                kError
            };

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

            /**
             * @brief Enables/disables a specific log level.
             * @param level Log level to update.
             * @param enabled Whether the level should be active.
             */
            static void set_level_enabled(LogLevel level, bool enabled = true);
            /**
             * @brief Returns current activation state of a log level.
             * @param level Level to query.
             * @return true if active.
             */
            static bool is_level_enabled(LogLevel level);
            /**
             * @brief Resets levels to defaults: warning/error enabled, info/debug disabled.
             */
            static void reset_levels();

            static void enable_debug(bool enabled = true);
            static void enable_info(bool enabled = true);
            static void enable_warning(bool enabled = true);
            static void enable_error(bool enabled = true);

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
