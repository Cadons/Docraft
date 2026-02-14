#pragma once
#include <string>

#include "backend/docraft_image_rendering_backend.h"
#include "backend/docraft_shape_rendering_backend.h"
#include "backend/docraft_text_rendering_backend.h"

namespace docraft::backend {
    /**
     * @brief Aggregated rendering backend interface.
     */
    class IDocraftRenderingBackend : public IDocraftTextRenderingBackend,
                                     public IDocraftShapeRenderingBackend,
                                     public IDocraftImageRenderingBackend {
    public:
        ~IDocraftRenderingBackend() override = default;
        /**
         * @brief Returns the current page width in points.
         */
        virtual float page_width() const = 0;
        /**
         * @brief Returns the current page height in points.
         */
        virtual float page_height() const = 0;
        /**
         * @brief Saves the document to a file path.
         */
        virtual void save_to_file(const std::string& path) const = 0;
        /**
         * @brief Registers a TTF font and returns the internal name.
         */
        virtual const char* register_ttf_font_from_file(const std::string& path, bool embed) const = 0;
        /**
         * @brief Checks whether a font can be used with the given encoder.
         */
        virtual bool can_use_font(const std::string& internal_name, const char* encoder) const = 0;
        /**
         * @brief Sets the current font and size.
         */
        virtual void set_font(const std::string& internal_name, float size, const char* encoder) const = 0;
    };
} // docraft::backend
