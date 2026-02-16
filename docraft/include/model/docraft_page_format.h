#pragma once

namespace docraft::model {
    /**
     * @brief Supported page sizes.
     */
    enum class DocraftPageSize {
        kA4,
        kA3,
        kA5,
        kLetter,
        kLegal
    };

    /**
     * @brief Page orientation.
     */
    enum class DocraftPageOrientation {
        kPortrait,
        kLandscape
    };
} // namespace docraft::model
