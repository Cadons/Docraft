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

#include "docraft/model/docraft_settings.h"

#include <algorithm>

namespace docraft::model {
    void DocraftSettings::add_font(const setting::DocraftFont &font) {
        fonts_.push_back(font);
    }

    void DocraftSettings::remove_font(const std::string &name) {
        auto rm =[&name](const setting::DocraftFont &font) { return font.name == name; };
        auto range = std::ranges::remove_if(fonts_, rm);
        fonts_.erase(range.begin(), range.end());
    }

    const std::vector<setting::DocraftFont> &DocraftSettings::fonts() const {
        return fonts_;
    }

    void DocraftSettings::set_page_format(DocraftPageSize size, DocraftPageOrientation orientation) {
        page_size_ = size;
        page_orientation_ = orientation;
        has_page_format_ = true;
    }

    DocraftPageSize DocraftSettings::page_size() const {
        return page_size_;
    }

    DocraftPageOrientation DocraftSettings::page_orientation() const {
        return page_orientation_;
    }

    bool DocraftSettings::has_page_format() const {
        return has_page_format_;
    }

    void DocraftSettings::set_section_ratios(float header_ratio, float body_ratio, float footer_ratio) {
        header_ratio_ = header_ratio;
        body_ratio_ = body_ratio;
        footer_ratio_ = footer_ratio;
        has_section_ratios_ = true;
    }

    float DocraftSettings::header_ratio() const {
        return header_ratio_;
    }

    float DocraftSettings::body_ratio() const {
        return body_ratio_;
    }

    float DocraftSettings::footer_ratio() const {
        return footer_ratio_;
    }

    bool DocraftSettings::has_section_ratios() const {
        return has_section_ratios_;
    }

    void DocraftSettings::draw(const std::shared_ptr<DocraftDocumentContext> &context) {
        // settings node does not have a visual representation, it only holds global settings for the document
    }
}
