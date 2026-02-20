#pragma once
#include <string_view>
#include <vector>

namespace docraft::utils {
    /**
     * @brief Decodes a base64 string into raw bytes.
     * @param input Base64 string (no data URI prefix).
     * @return Decoded bytes.
     * @throws std::invalid_argument if the input is not valid base64.
     */
    std::vector<unsigned char> decode_base64(std::string_view input);
}
