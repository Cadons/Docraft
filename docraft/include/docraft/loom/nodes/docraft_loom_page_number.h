#pragma once
#include <string>

#include "docraft/loom/nodes/docraft_loom_text.h"

namespace docraft::loom::nodes {
    /**
     * @brief Self-updating text node that displays the current page number (and total
     * page count) at render time. Measured using a fixed placeholder string (mirroring
     * legacy's DocraftPageNumber) since the real page count isn't known until pagination
     * completes -- Measure/Layout treat it exactly like ordinary text.
     */
    class DOCRAFT_LIB DocraftLoomPageNumber : public DocraftLoomText
    {
    public:
        DocraftLoomPageNumber();
        ~DocraftLoomPageNumber() override = default;
        void accept(loom::interfaces::DocraftLoomIVisitor& visitor) override;

        /**
         * @brief Display format applied at render time. `{page}` is replaced with the
         * current 1-based page number, `{total}` with the document's total page count.
         * Defaults to `"{page}"`.
         */
        void set_format(const std::string& format) { format_ = format; }
        [[nodiscard]] const std::string& format() const { return format_; }

    private:
        std::string format_ = "{page}";
    };
} // docraft
