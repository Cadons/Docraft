#pragma once

#include "docraft/exception/docraft_exception_base.h"

namespace docraft::exception {
    /**
     * @brief Exception thrown when a feature is not yet implemented.
     */
    class NotImplementedException : public DocraftException {
    public:
        using DocraftException::DocraftException;
    };
} // namespace docraft::exception

