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
#include <memory>

#include "docraft/backend/docraft_font_backend.h"
#include "docraft/backend/docraft_metadata_backend.h"
#include "docraft/backend/docraft_output_backend.h"
#include "docraft/backend/docraft_rendering_backend.h"
#include "docraft/backend/pdf/docraft_haru_shared_state.h"

namespace docraft::backend::pdf {
	class DocraftHaruTextBackend;
	class DocraftHaruLineBackend;
	class DocraftHaruShapeBackend;
	class DocraftHaruImageBackend;
	class DocraftHaruPageBackend;
	class DocraftHaruOutputBackend;
	class DocraftHaruFontBackend;
	class DocraftHaruMetadataBackend;

	/**
	 * @brief This class is responsible for managing the Haru PDF document and providing an interface for rendering operations.
	 */
	class DOCRAFT_LIB DocraftHaruBackend : public docraft::backend::IDocraftBackend {
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

		[[nodiscard]] const docraft::backend::IDocraftOutputBackend *output_backend() const override;

		[[nodiscard]] docraft::backend::IDocraftOutputBackend *edit_output_backend() override;

		[[nodiscard]] const docraft::backend::IDocraftFontBackend *font_backend() const override;

		[[nodiscard]] docraft::backend::IDocraftFontBackend *edit_font_backend() override;

		[[nodiscard]] const docraft::backend::IDocraftMetadataBackend *metadata_backend() const override;

		[[nodiscard]] docraft::backend::IDocraftMetadataBackend *edit_metadata_backend() override;
#pragma endregion

		[[nodiscard]] HPDF_Doc pdf_document() const;

		[[nodiscard]] const DocraftHaruPageBackend *page_backend_impl() const;

		[[nodiscard]] DocraftHaruPageBackend *edit_page_backend_impl();

	private:
		std::shared_ptr<DocraftHaruSharedState> state_;
		std::unique_ptr<DocraftHaruOutputBackend> output_backend_impl_;
		std::unique_ptr<DocraftHaruFontBackend> font_backend_impl_;
		std::unique_ptr<DocraftHaruMetadataBackend> metadata_backend_impl_;
		std::unique_ptr<DocraftHaruTextBackend> text_backend_;
		std::unique_ptr<DocraftHaruLineBackend> line_backend_;
		std::unique_ptr<DocraftHaruShapeBackend> shape_backend_;
		std::unique_ptr<DocraftHaruImageBackend> image_backend_;
		std::unique_ptr<DocraftHaruPageBackend> page_backend_;
	};
} // docraft
