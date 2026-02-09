#pragma once
#include <cstddef>
#include <string>

namespace docraft::backend {
    /**
     * @brief Interface for image rendering backends used by Docraft.
     */
    class IDocraftImageRenderingBackend {
    public:
        virtual ~IDocraftImageRenderingBackend() = default;
        /**
         * @brief Draws a PNG image from file.
         */
        virtual void draw_png_image(
            const std::string& path,
            float x,
            float y,
            float width,
            float height) const = 0;
        /**
         * @brief Draws a PNG image from memory.
         */
        virtual void draw_png_image_from_memory(
            const unsigned char* data,
            std::size_t size,
            float x,
            float y,
            float width,
            float height) const = 0;
        /**
         * @brief Draws a JPEG image from file.
         */
        virtual void draw_jpeg_image(
            const std::string& path,
            float x,
            float y,
            float width,
            float height) const = 0;
        /**
         * @brief Draws a JPEG image from memory.
         */
        virtual void draw_jpeg_image_from_memory(
            const unsigned char* data,
            std::size_t size,
            float x,
            float y,
            float width,
            float height) const = 0;
        /**
         * @brief Draws a raw RGB image from file.
         */
        virtual void draw_raw_rgb_image(
            const std::string& path,
            int pixel_width,
            int pixel_height,
            float x,
            float y,
            float width,
            float height) const = 0;
        /**
         * @brief Draws a raw RGB image from memory.
         */
        virtual void draw_raw_rgb_image_from_memory(
            const unsigned char* data,
            int pixel_width,
            int pixel_height,
            float x,
            float y,
            float width,
            float height) const = 0;
    };
} // docraft::backend
