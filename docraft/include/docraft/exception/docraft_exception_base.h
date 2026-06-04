#pragma once

#include <string>
#include <utility>

namespace docraft::exception {
    /**
     * @brief Base exception class for the Docraft library.
     *
     * All Docraft-specific exceptions inherit from this class.
     * This class intentionally does not inherit from standard exception types.
     */
    class DocraftException {
    private:
        mutable std::string message_;

    public:
        /**
         * @brief Constructs a DocraftException with a message.
         * @param message The error message.
         */
        explicit DocraftException(const std::string &message) : message_(std::move(message)) {
        }

        /**
         * @brief Returns the exception message as a C-string.
         * @return A C-string containing the error message.
         *
         * This method follows a classic what()-style API.
         */
        [[nodiscard]] const char *what() const noexcept {
            return message_.c_str();
        }

        /**
         * @brief Returns the message as a std::string.
         * @return The error message.
         */
        [[nodiscard]] const std::string &message() const noexcept {
            return message_;
        }

        // Explicit copy and move constructors/assignment for completeness
        DocraftException(const DocraftException &other) noexcept = default;

        DocraftException &operator=(const DocraftException &other) noexcept = default;

        DocraftException(DocraftException &&other) noexcept = default;

        DocraftException &operator=(DocraftException &&other) noexcept = default;

        virtual ~DocraftException() noexcept = default;
    };
} // namespace docraft::exception


