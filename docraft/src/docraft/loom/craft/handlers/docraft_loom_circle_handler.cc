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

#include "docraft/loom/craft/handlers/docraft_loom_circle_handler.h"

#include <any>
#include <cmath>
#include <numbers>

#include "docraft/craft/parser/docraft_circle_parser.h"
#include "docraft/exception/docraft_exceptions.h"
#include "docraft/loom/craft/docraft_loom_tree_builder_utils.h"
#include "docraft/loom/nodes/docraft_loom_circle.h"

namespace docraft::loom::craft {
    namespace {
        // Direction of `point` as seen from `center`, in degrees from 12 o'clock and
        // increasing clockwise on the page -- the convention
        // IDocraftShapeRenderingBackend::draw_arc() documents. Both are in the node's own
        // Y-down box coordinates, hence negating dy to get the "up is zero" reference.
        float arc_angle_from_point(const nodes::Position& point, const nodes::Position& center)
        {
            const float dx = point.x - center.x;
            const float dy = point.y - center.y;
            const float degrees = std::atan2(dx, -dy) * 180.0F / std::numbers::pi_v<float>;
            return degrees < 0.0F ? degrees + 360.0F : degrees;
        }
    } // namespace

    std::shared_ptr<nodes::DocraftLoomNode> DocraftLoomCircleHandler::build(
        const docraft::craft::DocraftParsedElement& element, DocraftLoomTableHandlerContext& context)
    {
        const auto& data = std::any_cast<const docraft::craft::parser::ParsedCircleData&>(element.data);

        // `radius` and `width`+`height` are two mutually exclusive ways of sizing the same
        // node: `radius` draws a circle, `width`+`height` inscribe an ellipse in that
        // bounding box (its four extreme points touch the middle of each box side, so an
        // oval is just a Circle whose box isn't square). Honoring both would make the
        // result depend on which one an arbitrary precedence rule happens to prefer, and
        // honoring neither used to silently draw nothing at all -- both are rejected here
        // instead. Note DocraftLoomCircle has no set_width()/set_height(), so
        // apply_common_attributes() below leaves the box attributes alone by design; they
        // are consumed here and only here.
        const bool has_radius = data.radius.has_value();
        const bool has_width = element.common.width.has_value();
        const bool has_height = element.common.height.has_value();
        if (has_radius && (has_width || has_height))
        {
            throw docraft::exception::InvalidInputException(
                "<Circle> accepts either 'radius' or 'width'+'height', not both -- they are two "
                "mutually exclusive ways of sizing the same shape");
        }
        if (!has_radius && !has_width && !has_height)
        {
            throw docraft::exception::InvalidInputException(
                "<Circle> requires a 'radius' attribute, or both 'width' and 'height' to inscribe "
                "an oval in that bounding box");
        }
        if (!has_radius && (!has_width || !has_height))
        {
            throw docraft::exception::InvalidInputException(
                std::string("<Circle> sized by its bounding box requires both 'width' and 'height', but only '")
                + (has_width ? "width" : "height") + "' was given");
        }

        auto node = std::make_shared<nodes::DocraftLoomCircle>();
        apply_shape_style(*node, data, [&context](const std::string& c) { return context.resolve_color(c); });
        if (has_radius)
        {
            if (*data.radius <= 0.0F)
            {
                throw docraft::exception::InvalidInputException("<Circle> 'radius' must be greater than zero");
            }
            node->set_radius(*data.radius);
        }
        else
        {
            if (*element.common.width <= 0.0F || *element.common.height <= 0.0F)
            {
                throw docraft::exception::InvalidInputException(
                    "<Circle> 'width' and 'height' must be greater than zero");
            }
            node->set_radii(*element.common.width / 2.0F, *element.common.height / 2.0F);
        }

        // Arc: the author gives the two endpoints as points in the node's own box, and
        // only their *direction* from the center is used -- the distance comes from the
        // circle's own radius, so a point that doesn't land exactly on the outline is
        // projected onto it rather than being an error. Two points always have two arcs
        // between them; the sweep is fixed clockwise from start to finish, which makes
        // the other one reachable simply by swapping the two.
        const int arc_attribute_count = static_cast<int>(data.start_x.has_value())
            + static_cast<int>(data.start_y.has_value()) + static_cast<int>(data.finish_x.has_value())
            + static_cast<int>(data.finish_y.has_value());
        if (arc_attribute_count != 0)
        {
            if (arc_attribute_count != 4)
            {
                throw docraft::exception::InvalidInputException(
                    "<Circle> arc requires all of 'start_x', 'start_y', 'finish_x' and 'finish_y'");
            }
            if (!node->is_circle())
            {
                throw docraft::exception::InvalidInputException(
                    "<Circle> arcs are supported on circles only, not on an oval sized by "
                    "differing 'width' and 'height'");
            }
            if (data.background_color.has_value())
            {
                throw docraft::exception::InvalidInputException(
                    "<Circle> arc is an open path and cannot be filled -- drop "
                    "'background_color', or use <Polygon> for a filled sector");
            }
            const nodes::Position center = {.x = node->radius_x(), .y = node->radius_y()};
            const float start = arc_angle_from_point({.x = *data.start_x, .y = *data.start_y}, center);
            float finish = arc_angle_from_point({.x = *data.finish_x, .y = *data.finish_y}, center);
            // The backend wants a strictly increasing sweep, so an arc wrapping past 12
            // o'clock is expressed by carrying the end angle a full turn further round.
            if (finish <= start)
            {
                finish += 360.0F;
            }
            node->set_arc(start, finish);
        }

        apply_common_attributes(*node, element.common);
        return node;
    }
} // namespace docraft::loom::craft
