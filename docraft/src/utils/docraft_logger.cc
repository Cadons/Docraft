#include "utils/docraft_logger.h"

#include <atomic>
#include <iostream>
#include <ostream>

namespace docraft::utils {
    namespace {
        std::atomic_bool debug_enabled{false};
        std::atomic_bool info_enabled{false};
        std::atomic_bool warning_enabled{true};
        std::atomic_bool error_enabled{true};

        std::atomic_bool *level_state(const DocraftLogger::LogLevel level) {
            switch (level) {
                case DocraftLogger::LogLevel::kDebug:
                    return &debug_enabled;
                case DocraftLogger::LogLevel::kInfo:
                    return &info_enabled;
                case DocraftLogger::LogLevel::kWarning:
                    return &warning_enabled;
                case DocraftLogger::LogLevel::kError:
                    return &error_enabled;
            }
            return nullptr;
        }
    } // namespace

    void DocraftLogger::debug(const std::string& message) {
        if (!is_level_enabled(LogLevel::kDebug)) {
            return;
        }
        const std::string data = "[DEBUG]: " + message;
        log(data);
    }

    void DocraftLogger::info(const std::string& message) {
        if (!is_level_enabled(LogLevel::kInfo)) {
            return;
        }
        const std::string data = "[INFO]: " + message;
        log(data);
    }

    void DocraftLogger::warning(const std::string &message) {
        if (!is_level_enabled(LogLevel::kWarning)) {
            return;
        }
        const std::string data = "[WARNING]: " + message;
        log(data);
    }

    void DocraftLogger::error(const std::string &message) {
        if (!is_level_enabled(LogLevel::kError)) {
            return;
        }
        const std::string data = "[ERROR]: " + message;
        log_error(data);
    }

    void DocraftLogger::set_level_enabled(const LogLevel level, const bool enabled) {
        std::atomic_bool *state = level_state(level);
        if (state == nullptr) {
            return;
        }
        state->store(enabled, std::memory_order_relaxed);
    }

    bool DocraftLogger::is_level_enabled(const LogLevel level) {
        const std::atomic_bool *state = level_state(level);
        if (state == nullptr) {
            return false;
        }
        return state->load(std::memory_order_relaxed);
    }

    void DocraftLogger::reset_levels() {
        set_level_enabled(LogLevel::kDebug, false);
        set_level_enabled(LogLevel::kInfo, false);
        set_level_enabled(LogLevel::kWarning, true);
        set_level_enabled(LogLevel::kError, true);
    }

    void DocraftLogger::enable_debug(const bool enabled) {
        set_level_enabled(LogLevel::kDebug, enabled);
    }

    void DocraftLogger::enable_info(const bool enabled) {
        set_level_enabled(LogLevel::kInfo, enabled);
    }

    void DocraftLogger::enable_warning(const bool enabled) {
        set_level_enabled(LogLevel::kWarning, enabled);
    }

    void DocraftLogger::enable_error(const bool enabled) {
        set_level_enabled(LogLevel::kError, enabled);
    }

    void DocraftLogger::log(const std::string& message) {
        std::cout << message << std::endl;
    }
    void DocraftLogger::log_error(const std::string& message) {
        std::cerr << message << std::endl;
    }
} // docraft
