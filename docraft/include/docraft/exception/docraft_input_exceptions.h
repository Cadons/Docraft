#pragma once

#include "docraft/exception/docraft_exception_base.h"

namespace docraft::exception {
    /**
     * @brief Exception thrown for invalid input validation errors.
     */
    class InvalidInputException : public DocraftException {
    public:
        using DocraftException::DocraftException;
    };
} // namespace docraft::exception

