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
#include <memory>
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
		/**
		 * @brief Creates a Haru PDF backend with a new document and page.
		 */
		DocraftHaruBackend();
		/**
		 * @brief Releases Haru resources.
		 */
		~DocraftHaruBackend() override;
#pragma region capabilities
		[[nodiscard]] const docraft::backend::IDocraftLineRenderingBackend *line_rendering() const override;
		[[nodiscard]] docraft::backend::IDocraftLineRenderingBackend *edit_line_rendering() override;
		[[nodiscard]] const docraft::backend::IDocraftTextRenderingBackend *text_rendering() const override;
		[[nodiscard]] docraft::backend::IDocraftTextRenderingBackend *edit_text_rendering() override;
		[[nodiscard]] const docraft::backend::IDocraftShapeRenderingBackend *shape_rendering() const override;
		[[nodiscard]] docraft::backend::IDocraftShapeRenderingBackend *edit_shape_rendering() override;
		[[nodiscard]] const docraft::backend::IDocraftImageRenderingBackend *image_rendering() const override;
		[[nodiscard]] docraft::backend::IDocraftImageRenderingBackend *edit_image_rendering() override;
		[[nodiscard]] const docraft::backend::IDocraftPageRenderingBackend *page_rendering() const override;
		[[nodiscard]] docraft::backend::IDocraftPageRenderingBackend *edit_page_rendering() override;
#pragma endregion
#pragma region backend lifecycle

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
	private:
		class TextHaruBackend;
		class LineHaruBackend;
		class ShapeHaruBackend;
		class ImageHaruBackend;
		class PageHaruBackend;
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
		std::unique_ptr<TextHaruBackend> text_backend_;
		std::unique_ptr<LineHaruBackend> line_backend_;
		std::unique_ptr<ShapeHaruBackend> shape_backend_;
		std::unique_ptr<ImageHaruBackend> image_backend_;
		std::unique_ptr<PageHaruBackend> page_backend_;
		HPDF_Doc pdf_;
		std::vector<HPDF_Page> pages_;
		size_t current_page_number_ = 0;
		mutable float fill_alpha_ = 1.0F;
		mutable float stroke_alpha_ = 1.0F;
	};
} // docraft
