#pragma once

#include "docraft_lib.h"
#include <string>

#include "backend/docraft_image_rendering_backend.h"
#include "backend/docraft_page_rendering_backend.h"
#include "backend/docraft_shape_rendering_backend.h"
#include "backend/docraft_text_rendering_backend.h"

namespace docraft {
    class DocraftDocumentMetadata;
}

namespace docraft::backend {
    /**
     * @brief Aggregated rendering backend interface.
     */
    class DOCRAFT_LIB IDocraftRenderingBackend : public IDocraftTextRenderingBackend,
                                     public IDocraftShapeRenderingBackend,
                                     public IDocraftImageRenderingBackend,
                                     public IDocraftPageRenderingBackend {
    public:
        /**
         * @brief Virtual destructor.
         */
        ~IDocraftRenderingBackend() override = default;
        /**
         * @brief Saves the document to a file path.
         * @param path Output file path.
         */
        virtual void save_to_file(const std::string& path) const = 0;
        /**
         * @brief Returns the preferred file extension for this backend.
         * @return Extension with or without leading dot (e.g. ".pdf" or "pdf").
         */
        [[nodiscard]] virtual std::string file_extension() const = 0;
        /**
         * @brief Registers a TTF font and returns the internal name.
         * @param path Font file path.
         * @param embed Whether to embed the font in the document.
         * @return Backend internal font name.
         */
        virtual const char* register_ttf_font_from_file(const std::string& path, bool embed) const = 0;
        /**
         * @brief Checks whether a font can be used with the given encoder.
         * @param internal_name Backend internal font name.
         * @param encoder Backend encoder name.
         * @return true if the font can be used.
         */
        virtual bool can_use_font(const std::string& internal_name, const char* encoder) const = 0;
        /**
         * @brief Sets the current font and size.
         * @param internal_name Backend internal font name.
         * @param size Font size in points.
         * @param encoder Backend encoder name.
         */
        virtual void set_font(const std::string& internal_name, float size, const char* encoder) const = 0;
        /**
         * @brief Applies document metadata to the backend document.
         * @param metadata Metadata values to apply.
         */
        virtual void set_document_metadata(const DocraftDocumentMetadata &metadata) = 0;
    };
} // docraft::backend
