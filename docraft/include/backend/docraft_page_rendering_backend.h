#pragma once

#include "docraft_lib.h"
#include <cstddef>

#include "model/docraft_page_format.h"

namespace docraft::backend {
    /**
     * @brief Interface for page-level operations and page metadata.
     */
    class DOCRAFT_LIB IDocraftPageRenderingBackend {
    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~IDocraftPageRenderingBackend() = default;
        /**
         * @brief Returns the current page width in points.
         * @return Page width in points.
         */
        virtual float page_width() const = 0;
        /**
         * @brief Returns the current page height in points.
         * @return Page height in points.
         */
        virtual float page_height() const = 0;
        /**
         * @brief Adds a new page to the document.
         */
        virtual void add_new_page() = 0;
        /**
         * @brief Moves the cursor to the next page.
         */
        virtual void move_to_next_page() = 0;
        /**
         * @brief Navigates to a specific page (0-based index).
         * @param page_number Destination page index.
         */
        virtual void go_to_page(std::size_t page_number) = 0;
        /**
         * @brief Navigates to the first page.
         */
        virtual void go_to_first_page() = 0;
        /**
         * @brief Navigates to the previous page.
         */
        virtual void go_to_previous_page() = 0;
        /**
         * @brief Navigates to the last page.
         */
        virtual void go_to_last_page() = 0;
        /**
         * @brief Sets the page size and orientation.
         * @param size Page size.
         * @param orientation Page orientation.
         */
        virtual void set_page_format(model::DocraftPageSize size,
                                     model::DocraftPageOrientation orientation) = 0;
        /**
         * @brief Returns the current page number.
         * @return Current page number.
         */
        virtual std::size_t current_page_number() const = 0;
        /**
         * @brief Returns the total number of pages in the document.
         * @return Total page count.
         */
        virtual std::size_t total_page_count() const = 0;
    };
} // docraft::backend
