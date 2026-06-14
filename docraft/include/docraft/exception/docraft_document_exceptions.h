#pragma once

#include "docraft/exception/docraft_exception_base.h"

namespace docraft::exception {
    /**
     * @brief Base exception for document-related errors.
     */
    class DocumentException : public DocraftException {
    public:
        using DocraftException::DocraftException;
    };

    /**
     * @brief Exception thrown when document state is invalid.
     */
    class DocumentStateException : public DocumentException {
    public:
        using DocumentException::DocumentException;
    };

    /**
     * @brief Exception thrown for model-related document errors.
     */
    class ModelException : public DocumentException {
    public:
        using DocumentException::DocumentException;
    };
} // namespace docraft::exception

