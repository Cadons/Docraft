#pragma once
#include <string>

namespace docraft::backend {
        /**
         * This is the interface for the PDF backend of Docraft.
         */
        class IDocraftPDFBackend {
        public:
               virtual float measure_text(const std::string& text)=0;
                virtual void draw_text(const std::string& text)=0;

        };
} // docraft
