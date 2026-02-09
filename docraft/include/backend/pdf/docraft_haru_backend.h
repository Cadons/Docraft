#pragma once
#include <hpdf.h>
#include <cstddef>
#include <string>

#include "backend/docraft_rendering_backend.h"

namespace docraft::backend::pdf {
	/**
	 * @brief This class is responsible for managing the Haru PDF document and providing an interface for rendering operations.
	 */
class DocraftHaruBackend : public docraft::backend::IDocraftRenderingBackend {
	public:
		DocraftHaruBackend();
		~DocraftHaruBackend() override = default;
#pragma region text rendering
		void begin_text() const override;
		void end_text() const override;
		void draw_text(const std::string& text, float x, float y) const override;
		void set_text_color(float r, float g, float b) const override;
		void draw_text_matrix(
			const std::string& text,
			float scale_x,
			float skew_x,
			float skew_y,
			float scale_y,
			float translate_x,
			float translate_y) const override;
		float measure_text_width(const std::string& text) const override;
#pragma endregion
#pragma region line rendering
		void set_stroke_color(float r, float g, float b) const override;
		void set_line_width(float thickness) const override;
		void draw_line(float x1, float y1, float x2, float y2) const override;
#pragma endregion
#pragma region shape rendering
		void save_state() const override;
		void restore_state() const override;
		void set_fill_color(float r, float g, float b) const override;
		void set_fill_alpha(float alpha) const override;
		void set_stroke_alpha(float alpha) const override;
		void draw_rectangle(float x, float y, float width, float height) const override;
		void fill() const override;
		void stroke() const override;
		void fill_stroke() const override;
#pragma endregion
#pragma region image rendering
		void draw_png_image(
			const std::string& path,
			float x,
			float y,
			float width,
			float height) const override;
		void draw_png_image_from_memory(
			const unsigned char* data,
			std::size_t size,
			float x,
			float y,
			float width,
			float height) const override;
		void draw_jpeg_image(
			const std::string& path,
			float x,
			float y,
			float width,
			float height) const override;
		void draw_jpeg_image_from_memory(
			const unsigned char* data,
			std::size_t size,
			float x,
			float y,
			float width,
			float height) const override;
		void draw_raw_rgb_image(
			const std::string& path,
			int pixel_width,
			int pixel_height,
			float x,
			float y,
			float width,
			float height) const override;
		void draw_raw_rgb_image_from_memory(
			const unsigned char* data,
			int pixel_width,
			int pixel_height,
			float x,
			float y,
			float width,
			float height) const override;
#pragma endregion
#pragma region backend lifecycle
		float page_width() const override;
		float page_height() const override;
		void save_to_file(const std::string& path) const override;
		const char* register_ttf_font_from_file(const std::string& path, bool embed) const override;
		bool can_use_font(const std::string& internal_name, const char* encoder) const override;
		void set_font(const std::string& internal_name, float size, const char* encoder) const override;
#pragma endregion
	private:
		void apply_alpha_state() const;
		HPDF_Doc pdf_;
		HPDF_Page page_;
		mutable float fill_alpha_ = 1.0F;
		mutable float stroke_alpha_ = 1.0F;
	};
} // docraft
