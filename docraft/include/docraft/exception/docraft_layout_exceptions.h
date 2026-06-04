#pragma once

#include "docraft/exception/docraft_exception_base.h"

namespace docraft::exception {
    /**
     * @brief Base exception for layout-related errors.
     */
    class LayoutException : public DocraftException {
    public:
        using DocraftException::DocraftException;
    };

    /**
     * @brief Exception thrown for layout configuration errors.
     */
    class LayoutConfigurationException : public LayoutException {
    public:
        using LayoutException::LayoutException;
    };
} // namespace docraft::exception

