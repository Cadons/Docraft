#pragma once

#include "docraft_lib.h"
#include <string>

namespace docraft::backend {
        /**
         * This is the interface for the PDF backend of Docraft.
         */
        class DOCRAFT_LIB IDocraftPDFBackend {
        public:
               /**
                * @brief Measures the width of text using the current font state.
                * @param text Text string.
                * @return Text width in points.
                */
               virtual float measure_text(const std::string& text)=0;
                /**
                 * @brief Draws text using the current font state.
                 * @param text Text string.
                 */
                virtual void draw_text(const std::string& text)=0;

        };
} // docraft
