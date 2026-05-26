/*
 * Copyright 2026 Matteo Cadoni (https://github.com/cadons)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "docraft/docraft_lib.h"
#include <hpdf.h>
#include <cstddef>
#include <string>
#include <vector>

#include "docraft/backend/docraft_rendering_backend.h"
#include "docraft/model/docraft_position.h"

namespace docraft::backend::pdf {
	/**
	 * @brief This class is responsible for managing the Haru PDF document and providing an interface for rendering operations.
	 */
	class DOCRAFT_LIB DocraftHaruBackend : public docraft::backend::IDocraftRenderingBackend {
	public:
		class LineHaruBackend;
		class ShapeHaruBackend;
		class TextHaruBackend;
		class ImageHaruBackend;
		class PageHaruBackend;

		/**
		 * @brief Creates a Haru PDF backend with a new document and page.
		 */
		DocraftHaruBackend();
		/**
		 * @brief Releases Haru resources.
		 */
		~DocraftHaruBackend() override;
#pragma region text rendering
		/**
		 * @brief Begins a text object.
		 */
		void begin_text() const;
		/**
		 * @brief Ends a text object.
		 */
		void end_text() const;
		/**
		 * @brief Draws text at the given coordinates.
		 */
		void draw_text(const std::string& text, float x, float y) const;
		/**
		 * @brief Sets text fill color.
		 */
		void set_text_color(float r, float g, float b) const;
		/**
		 * @brief Draws text with a custom transformation matrix.
		 */
		void draw_text_matrix(
			const std::string& text,
			float scale_x,
			float skew_x,
			float skew_y,
			float scale_y,
			float translate_x,
			float translate_y) const;
		/**
		 * @brief Measures text width using current font settings.
		 */
		float measure_text_width(const std::string& text) const;
#pragma endregion
#pragma region line rendering
		/**
		 * @brief Sets stroke color for lines and shapes.
		 */
		void set_stroke_color(float r, float g, float b) const;
		/**
		 * @brief Sets line width in points.
		 */
		void set_line_width(float thickness) const;
		/**
		 * @brief Draws a line between two points.
		 */
		void draw_line(float x1, float y1, float x2, float y2) const;
#pragma endregion
#pragma region shape rendering
		/**
		 * @brief Saves the current graphics state.
		 */
		void save_state() const;
		/**
		 * @brief Restores the previous graphics state.
		 */
		void restore_state() const;
		/**
		 * @brief Sets fill color for shapes.
		 */
		void set_fill_color(float r, float g, float b) const;
		/**
		 * @brief Sets fill alpha for shapes.
		 */
		void set_fill_alpha(float alpha) const;
		/**
		 * @brief Sets stroke alpha for shapes.
		 */
		void set_stroke_alpha(float alpha) const;
		/**
		 * @brief Adds a rectangle path.
		 */
		void draw_rectangle(float x, float y, float width, float height) const;
		/**
		 * @brief Adds a circle path.
		 */
		void draw_circle(float center_x, float center_y, float radius) const;
		/**
		 * @brief Adds a polygon path.
		 */
		void draw_polygon(const std::vector<model::DocraftPoint> &points) const;
		/**
		 * @brief Fills the current path.
		 */
		void fill() const;
		/**
		 * @brief Strokes the current path.
		 */
		void stroke() const;
		/**
		 * @brief Fills and strokes the current path.
		 */
		void fill_stroke() const;
#pragma endregion
#pragma region image rendering
		/**
		 * @brief Draws a PNG image from file.
		 */
		void draw_png_image(
			const std::string& path,
			float x,
			float y,
			float width,
			float height) const;
		/**
		 * @brief Draws a PNG image from memory.
		 */
		void draw_png_image_from_memory(
			const unsigned char* data,
			std::size_t size,
			float x,
			float y,
			float width,
			float height) const;
		/**
		 * @brief Draws a JPEG image from file.
		 */
		void draw_jpeg_image(
			const std::string& path,
			float x,
			float y,
			float width,
			float height) const;
		/**
		 * @brief Draws a JPEG image from memory.
		 */
		void draw_jpeg_image_from_memory(
			const unsigned char* data,
			std::size_t size,
			float x,
			float y,
			float width,
			float height) const;
		/**
		 * @brief Draws a raw RGB image from file.
		 */
		void draw_raw_rgb_image(
			const std::string& path,
			int pixel_width,
			int pixel_height,
			float x,
			float y,
			float width,
			float height) const;
		/**
		 * @brief Draws a raw RGB image from memory.
		 */
		void draw_raw_rgb_image_from_memory(
			const unsigned char* data,
			int pixel_width,
			int pixel_height,
			float x,
			float y,
			float width,
			float height) const;
#pragma endregion
#pragma region backend lifecycle

		[[nodiscard]] const std::shared_ptr<docraft::backend::IDocraftLineRenderingBackend>& edit_line() const override;
		[[nodiscard]] const std::shared_ptr<docraft::backend::IDocraftShapeRenderingBackend>& edit_shape() const override;
		[[nodiscard]] const std::shared_ptr<docraft::backend::IDocraftTextRenderingBackend>& edit_text() const override;
		[[nodiscard]] const std::shared_ptr<docraft::backend::IDocraftImageRenderingBackend>& edit_image() const override;
		[[nodiscard]] const std::shared_ptr<docraft::backend::IDocraftPageRenderingBackend>& edit_page() const override;

		void save_to_file(const std::string& path) const override;
		[[nodiscard]] std::string file_extension() const override;
		/**
		 * @brief Registers a TTF font and returns the internal name.
		 */
		const char* register_ttf_font_from_file(const std::string& path, bool embed) const override;
		/**
		 * @brief Returns whether the backend can use a font with the given encoder.
		 */
		bool can_use_font(const std::string& internal_name, const char* encoder) const override;
		/**
		 * @brief Sets the current font and size.
		 */
		void set_font(const std::string& internal_name, float size, const char* encoder) const override;
		/**
		 * @brief Applies document metadata to the PDF info dictionary.
		 */
		void set_document_metadata(const DocraftDocumentMetadata &metadata) override;
#pragma endregion
#pragma region page management
/**
		 * @brief Returns the current page width in points.
		 */
		float page_width() const;
		/**
		 * @brief Returns the current page height in points.
		 */
		float page_height() const;
		/**
		 * @brief Adds a new page to the document and makes it the current page.
		 */
		void add_new_page();
		/**
		 * @brief Moves the cursor to the next page if it exists.
		 * @throws std::runtime_error if already at the last page.
		 */
		void move_to_next_page();
		/**
		 * @brief Navigates to a specific page (0-based index).
		 * @param page_number Destination page index.
		 * @throws std::runtime_error if the page number is out of range.
		 */
		void go_to_page(std::size_t page_number);
		/**
		 * @brief Navigates to the first page.
		 */
		void go_to_first_page();
		/**
		 * @brief Navigates to the previous page.
		 * @throws std::runtime_error if already at the first page.
		 */
		void go_to_previous_page();
		/**
		 * @brief Navigates to the last page.
		 */
		void go_to_last_page();
		/**
		 * @brief Sets the page size and orientation.
		 */
		void set_page_format(model::DocraftPageSize size,
		                     model::DocraftPageOrientation orientation);
		/**
		 * @brief Returns the current page number (1-based index).
		 */
		std::size_t current_page_number() const;
		/**
		 * @brief Returns the total number of pages in the document.
		 */
		std::size_t total_page_count() const;
#pragma endregion
	private:
		/**
		 * @brief Creates a new page and adds it to the document.
		 */
		void create_new_page();
		/**
		 * @brief Returns the current page index (0-based) for internal use.
		 */
		size_t internal_current_page_index() const;
		/**
		 * @brief Applies the current page format to a page handle.
		 */
		void apply_page_format(HPDF_Page page) const;
		HPDF_PageSizes page_size_ = HPDF_PAGE_SIZE_A4;
		HPDF_PageDirection page_direction_ = HPDF_PAGE_PORTRAIT;
		/**
		 * @brief Applies the current alpha state to the Haru graphics state.
		 */
		void apply_alpha_state() const;
		HPDF_Doc pdf_;
		std::vector<HPDF_Page> pages_;
		size_t current_page_number_ = 0;
		mutable float fill_alpha_ = 1.0F;
		mutable float stroke_alpha_ = 1.0F;
		std::shared_ptr<docraft::backend::IDocraftLineRenderingBackend> edit_line_;
		std::shared_ptr<docraft::backend::IDocraftShapeRenderingBackend> edit_shape_;
		std::shared_ptr<docraft::backend::IDocraftTextRenderingBackend> edit_text_;
		std::shared_ptr<docraft::backend::IDocraftImageRenderingBackend> edit_image_;
		std::shared_ptr<docraft::backend::IDocraftPageRenderingBackend> edit_page_;
	};
} // docraft
