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

#include "docraft/model/docraft_position.h"

#include <format>
#include <sstream>

#include "docraft/utils/docraft_logger.h"

namespace docraft::model {
#pragma region DocraftPoint
    std::string DocraftPoint::to_string() const {
        return std::format("{{{};{}}}", x, y);
    }
#pragma endregion
#pragma region DocraftTransform
    DocraftTransform::DocraftTransform() = default;

    DocraftTransform::DocraftTransform(const DocraftPoint &point,const float &width, const float &height): width_(width),height_(height) {
        compute_transform(point,width,height);
    }

    float DocraftTransform::rotation() const {
        return rotation_;
    }

    const DocraftPoint & DocraftTransform::position() const {
        return position_;
    }

    const DocraftPoint & DocraftTransform::center() const {
        return center_;
    }

    float DocraftTransform::width() const {
        return width_;
    }

    float DocraftTransform::height() const {
        return height_;
    }

    float DocraftTransform::padding() const {
        return padding_;
    }

    const DocraftAnchor &DocraftTransform::anchors() const {
        return anchor_;
    }

    const DocraftAnchor &DocraftTransform::content_anchors() const {
        return content_anchor_;
    }


    void DocraftTransform::compute_transform(const DocraftPoint &point, const float &width, const float &height) {
        //compute the normal rectangle
        position_ = point;
        anchor_.top_left = point;
        anchor_.top_center ={.x=(point.x+(width/2)),.y=point.y};
        anchor_.top_right ={.x=(point.x+width),.y=point.y};
        anchor_.bottom_left = {.x=point.x,.y=point.y - height};
        anchor_.bottom_center ={.x=(point.x+(width/2)),.y=point.y - height};
        anchor_.bottom_right ={.x=(point.x+width),.y=point.y - height};
        anchor_.left_center ={.x=point.x,.y=point.y - (height/2)};
        anchor_.right_center ={.x=point.x + width,.y=point.y - (height/2)};
        center_ = {.x=anchor_.top_center.x,.y=anchor_.left_center.y};
        //compute the content rectangle
        content_anchor_.top_left = {.x=anchor_.top_left.x+padding(),.y=anchor_.top_left.y-padding()};
        content_anchor_.top_center = { .x= anchor_.top_center.x,.y=anchor_.top_center.y-padding()};//x is constant
        content_anchor_.top_right = {.x =anchor_.top_right.x - padding(),.y=anchor_.top_right.y - padding()};
        content_anchor_.bottom_left = {.x=anchor_.bottom_left.x + padding(),.y=anchor_.bottom_left.y + padding()};
        content_anchor_.bottom_center = {.x=anchor_.bottom_center.x,.y=anchor_.bottom_left.y+padding()};//x is constant
        content_anchor_.bottom_right = {.x=anchor_.bottom_right.x - padding(),.y=anchor_.bottom_right.y + padding()};
        content_anchor_.left_center={.x=anchor_.left_center.x+padding(),.y=anchor_.left_center.y};//y is constant
        content_anchor_.right_center={.x=anchor_.right_center.x-padding(),.y=anchor_.right_center.y};//y is constant
    }

    void DocraftTransform::set_position(const DocraftPoint &point) {
        position_ = point;
        compute_transform(point,width_,height_);
    }

    void DocraftTransform::set_width(const float &width) {
        width_ = width;
        compute_transform(position_,width_,height_);
    }

    void DocraftTransform::set_height(const float &height) {
        height_ = height;
        compute_transform(position_,width_,height_);
    }

    void DocraftTransform::set_padding(const float &padding) {
        padding_ = padding;
        compute_transform(position_,width_,height_);
    }

    std::string DocraftTransform::to_string() const {
        std::ostringstream oss;
        oss << "Position: " << position().to_string() << ", Width: " << width() << ", Height: " << height();
        return oss.str();
    }
#pragma endregion
}
