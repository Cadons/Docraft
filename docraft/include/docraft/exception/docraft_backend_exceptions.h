#pragma once

#include "docraft/exception/docraft_exception_base.h"

namespace docraft::exception {
    /**
     * @brief Base exception for backend-related errors.
     */
    class BackendStateException : public DocraftException {
    public:
        using DocraftException::DocraftException;
    };

    /**
     * @brief Exception thrown when a required capability is unavailable.
     */
    class CapabilityUnavailableException : public BackendStateException {
    public:
        using BackendStateException::BackendStateException;
    };

    /**
     * @brief Exception thrown for page state errors.
     */
    class PageStateException : public BackendStateException {
    public:
        using BackendStateException::BackendStateException;
    };
} // namespace docraft::exception

