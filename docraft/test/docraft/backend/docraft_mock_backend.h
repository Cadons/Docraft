#pragma once
#include <gmock/gmock.h>

#include "docraft/backend/docraft_text_rendering_backend.h"

namespace docraft::test::backend {
    class MockDocraftTextRenderingBackend : public docraft::backend::IDocraftTextRenderingBackend
    {
    public:
        MOCK_CONST_METHOD0(begin_text, void());
        MOCK_CONST_METHOD0(end_text, void());
        MOCK_CONST_METHOD3(draw_text, void(const std::string &text, float x, float y));
        MOCK_CONST_METHOD3(set_text_color, void(float r, float g, float b));
        MOCK_CONST_METHOD7(draw_text_matrix,
                           void(const std::string &text,
                               float scale_x,
                               float skew_x,
                               float skew_y,
                               float scale_y,
                               float translate_x,
                               float translate_y));
        MOCK_CONST_METHOD3(measure_text_width,
                           float(const std::string& text, const std::string& font_name, float font_size));
        MOCK_CONST_METHOD2(measure_text_height, float(const std::string& font_name, float font_size));
        MOCK_CONST_METHOD2(set_font, void (const std::string& font_name, float font_size));
    };
} // namespace backend
