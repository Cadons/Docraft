#include "utils/docraft_logger.h"

#include <iostream>
#include <ostream>

namespace docraft::utils {
    void DocraftLogger::debug(const std::string& message) {
        const std::string data = "[DEBUG]: " + message;
        log(data);
    }

    void DocraftLogger::info(const std::string& message) {
        const std::string data = "[INFO]: " + message;
        log(data);
    }

    void DocraftLogger::warning(const std::string &message) {
        const std::string data = "[WARNING]: " + message;
        log(data);
    }

    void DocraftLogger::error(const std::string &message) {
        const std::string data = "[ERROR]: " + message;
        log_error(data);
    }

    void DocraftLogger::log(const std::string& message) {
        std::cout << message << std::endl;
    }
    void DocraftLogger::log_error(const std::string& message) {
        std::cerr << message << std::endl;
    }
} // docraft