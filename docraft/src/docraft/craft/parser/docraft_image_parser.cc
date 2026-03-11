#include "docraft/craft/parser/docraft_parser.h"

#include "docraft/craft/parser/docraft_parser_helpers.h"
#include "docraft/model/docraft_image.h"
#include "docraft/utils/docraft_base64.h"

#include <string_view>

namespace docraft::craft::parser {
    namespace {
        bool extract_base64_payload(std::string_view value, std::string_view &payload) {
            const std::string_view prefix = "base64:";
            if (value.starts_with(prefix)) {
                payload = value.substr(prefix.size());
                return true;
            }
            const std::string_view marker = "base64,";
            const auto pos = value.find(marker);
            if (pos != std::string_view::npos) {
                payload = value.substr(pos + marker.size());
                return true;
            }
            return false;
        }
    } // namespace

    std::shared_ptr<model::DocraftNode> DocraftImageParser::parse(const pugi::xml_node &craft_language_source) {
        auto image_node = std::make_shared<model::DocraftImage>();
        //image cannot have path and data at the same time
        if (auto src_attr = (craft_language_source.attribute(elements::image::attribute::kSrc.data()) != nullptr) &&
            (craft_language_source.attribute(elements::image::attribute::kData.data()) != nullptr)) {
            throw std::invalid_argument("Image node cannot have both 'src' and 'data' attributes.");
        }
        if (auto src_attr = craft_language_source.attribute(elements::image::attribute::kSrc.data())) {
            image_node->set_path(src_attr.as_string());
        }
        if (auto raw_data_attr = craft_language_source.attribute(elements::image::attribute::kData.data())) {
            std::string_view payload;
            const std::string_view raw_value = raw_data_attr.as_string();
            if (extract_base64_payload(raw_value, payload)) {
                const auto width_attr = craft_language_source.attribute(elements::image::attribute::kDataWidth.data());
                const auto height_attr = craft_language_source.attribute(elements::image::attribute::kDataHeight.data());
                if (!width_attr || !height_attr) {
                    throw std::invalid_argument("Base64 image data requires data_width and data_height.");
                }
                const int pixel_width = width_attr.as_int();
                const int pixel_height = height_attr.as_int();
                if (pixel_width <= 0 || pixel_height <= 0) {
                    throw std::invalid_argument("Base64 image data has invalid dimensions.");
                }
                auto decoded = utils::decode_base64(payload);
                const auto expected_size = static_cast<size_t>(pixel_width) *
                                           static_cast<size_t>(pixel_height) * 3U;
                if (decoded.size() != expected_size) {
                    throw std::invalid_argument("Base64 image data size does not match dimensions (RGB expected).");
                }
                image_node->set_raw_data(decoded, pixel_width, pixel_height);
            } else {
                image_node->set_has_raw_data(true);
                image_node->set_path(raw_data_attr.as_string());//path is identified by the key in the template
            }
        }
        detail::configure_docraft_node_attributes(image_node, craft_language_source);
        return image_node;
    }
}
