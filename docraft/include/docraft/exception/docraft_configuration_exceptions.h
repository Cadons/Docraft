#pragma once

#include "docraft/exception/docraft_exception_base.h"

namespace docraft::exception {
    /**
     * @brief Exception thrown for configuration-related errors.
     */
    class ConfigurationException : public DocraftException {
    public:
        using DocraftException::DocraftException;
    };

    /**
     * @brief Exception thrown when a required argument is missing.
     */
    class MissingArgumentException : public ConfigurationException {
    public:
        using ConfigurationException::ConfigurationException;
    };
} // namespace docraft::exception

