#include "model/docraft_position.h"

#include <format>
#include <sstream>

#include "utils/docraft_logger.h"

namespace docraft::model {
#pragma region DocraftPoint
    std::string DocraftPoint::to_string() const {
        return std::format("{{{};{}}}", x, y);
    }
#pragma endregion
#pragma region DocraftTrasform
    DocraftTransform::DocraftTransform() {
    }

    const DocraftPoint &DocraftTransform::top_left() const {
        return top_left_;
    }

    const DocraftPoint &DocraftTransform::top_center() const {
        return top_center_;
    }

    const DocraftPoint &DocraftTransform::top_right() const {
        return top_right_;
    }

    const DocraftPoint &DocraftTransform::bottom_left() const {
        return bottom_left_;
    }

    const DocraftPoint &DocraftTransform::bottom_center() const {
        return bottom_center_;
    }

    const DocraftPoint &DocraftTransform::bottom_right() const {
        return bottom_right_;
    }

    const DocraftPoint &DocraftTransform::left_center() const {
        return left_center_;
    }

    const DocraftPoint &DocraftTransform::right_center() const {
        return right_center_;
    }

    float DocraftTransform::rotation() const {
        return rotation_;
    }

    std::string DocraftTransform::to_string() const {
        std::ostringstream oss;

        constexpr int kTotalInnerWidth = 43;
        constexpr std::string kDashes = "-----------";
        const std::string k_empty_padding = std::string(kTotalInnerWidth, ' ');
        const std::string k_mid_spacer = std::string(kTotalInnerWidth - 10, ' ');

        oss << "\n " << top_left().to_string() << "           "
                << top_center().to_string() << "           "
                << top_right().to_string() << "\n";

        oss << "|" << std::string(kTotalInnerWidth, '-') << "|\n";

        for (int i = 0; i < 3; i++) oss << "|" << k_empty_padding << "|\n";

        oss << "|" << left_center().to_string() << k_mid_spacer
                << right_center().to_string() << "|\n";

        for (int i = 0; i < 3; i++) oss << "|" << k_empty_padding << "|\n";

        oss << "|" << std::string(kTotalInnerWidth, '-') << "|\n";

        oss << " " << bottom_left().to_string() << "           "
                << bottom_center().to_string() << "           "
                << bottom_right().to_string() << "\n";

        return oss.str();
    }
#pragma endregion
}
