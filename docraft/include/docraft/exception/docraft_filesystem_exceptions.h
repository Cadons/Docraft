#pragma once

#include "docraft/exception/docraft_exception_base.h"

namespace docraft::exception {
    /**
     * @brief Base exception for file system-related errors.
     */
    class FileSystemException : public DocraftException {
    public:
        using DocraftException::DocraftException;
    };

    /**
     * @brief Exception thrown when a file cannot be found.
     */
    class FileNotFoundException : public FileSystemException {
    public:
        using FileSystemException::FileSystemException;
    };

    /**
     * @brief Exception thrown when a file cannot be opened.
     */
    class CannotOpenFileException : public FileSystemException {
    public:
        using FileSystemException::FileSystemException;
    };
} // namespace docraft::exception

