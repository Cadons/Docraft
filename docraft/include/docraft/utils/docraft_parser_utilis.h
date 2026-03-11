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
#include <string>
#include <nlohmann/adl_serializer.hpp>

namespace docraft::utils {
        class DocraftParserUtilis {
        public:
                /**
                 * @brief Extracts a value from a JSON object based on a data request string.
                 * The data request can be a simple key or a dot-separated path for nested values.
                 * @param data_request The data request string (e.g., "${data(name)" or "${data("age")").
                 * @param item The JSON object to extract data from.
                 * @return The extracted value as a string, or an empty string if not found.
                 */
                static std::string extract_data_attribute(const std::string &data_request, const nlohmann::json& item);
                static std::string extract_data_attribute(const std::vector<unsigned char>& data_request, const nlohmann::json& item);

                static bool is_data_request(const std::string &data_request);

                static bool is_template_variable(const std::string &variable);

                static bool is_data_request(const std::vector<unsigned char> &data);
        };
} // docraft