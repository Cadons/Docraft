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
