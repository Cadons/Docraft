#pragma once

#include "docraft/exception/docraft_exception_base.h"

namespace docraft::exception {
    /**
     * @brief Base exception for data format-related errors.
     */
    class DataFormatException : public DocraftException {
    public:
        using DocraftException::DocraftException;
    };

    /**
     * @brief Exception thrown when JSON parsing fails.
     */
    class InvalidJSONException : public DataFormatException {
    public:
        using DataFormatException::DataFormatException;
    };
} // namespace docraft::exception

