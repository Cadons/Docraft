#pragma once

#include "docraft/exception/docraft_exception_base.h"

namespace docraft::exception {
    /**
     * @brief Base exception for rendering-related errors.
     */
    class RenderingException : public DocraftException {
    public:
        using DocraftException::DocraftException;
    };

    /**
     * @brief Exception thrown when rendering fails.
     */
    class RenderingFailedException : public RenderingException {
    public:
        using RenderingException::RenderingException;
    };
} // namespace docraft::exception

