#pragma once

#include <string>
#include <vector>

#include <pugixml.hpp>

#include "model/docraft_position.h"

namespace docraft::craft::parser::detail {
    std::vector<model::DocraftPoint> parse_points_attribute(const pugi::xml_node &node, const char *attr_name);
}
