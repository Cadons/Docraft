#pragma once

#include "docraft/exception/docraft_exception_base.h"

namespace docraft::exception {
    /**
     * @brief Base exception for template-related errors.
     */
    class TemplateException : public DocraftException {
    public:
        using DocraftException::DocraftException;
    };

    /**
     * @brief Exception thrown when a template variable already exists.
     */
    class TemplateVariableExistsException : public TemplateException {
    public:
        using TemplateException::TemplateException;
    };

    /**
     * @brief Exception thrown when a template variable is not found.
     */
    class TemplateVariableNotFoundException : public TemplateException {
    public:
        using TemplateException::TemplateException;
    };

    /**
     * @brief Exception thrown when template image data is invalid or missing.
     */
    class TemplateImageDataException : public TemplateException {
    public:
        using TemplateException::TemplateException;
    };
} // namespace docraft::exception

