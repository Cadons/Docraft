#pragma once
#include <string>
using string = std::string_view;

namespace docraft::craft {
    namespace basic::attribute {
        constexpr string kId = "id";
        constexpr string kNodeName = "node_name";
        constexpr string kX = "x";
        constexpr string kY = "y";
        constexpr string kWidth = "width";
        constexpr string kHeight = "height";
        constexpr string kAutoFillHeight = "auto_fill_height";
        constexpr string kAutoFillWidth = "auto_fill_width";
        constexpr string kPadding = "padding";
        constexpr string kWeight = "weight";
        constexpr string kPosition = "position";
        constexpr string kColor = "color";

        namespace position_type {
            constexpr string kBlock = "block";
            constexpr string kAbsolute = "absolute";
        }
    }

    namespace section {
        constexpr string kDocument = "document";
        constexpr string kHeader = "header";
        constexpr string kBody = "body";
        constexpr string kFooter = "footer";

        namespace attribute {
            constexpr string kMarginTop = "margin_top";
            constexpr string kMarginBottom = "margin_bottom";
            constexpr string kMarginLeft = "margin_left";
            constexpr string kMarginRight = "margin_right";
        }
    }

    namespace elements {
        constexpr string kText = "Text";
        constexpr string kImage = "Image";
        constexpr string kTable = "Table";
        constexpr string kTableHeader = "TableHeader";
        constexpr string kTableTitle = "TableTitle";
        constexpr string kTableBody = "TableBody";
        constexpr string kTableRow = "TableRow";
        constexpr string kLayout = "Layout";
        constexpr string kBlankLine = "Blank";
        constexpr string kRectangle = "Rectangle";

        namespace image::attribute {
            constexpr string kSrc = "src";
        }
        namespace table::attribute {
            constexpr string kModel = "model";
        }
        namespace layout::attribute {
            constexpr string kOrientation = "orientation";
        }
        namespace rectangle::attribute {
            constexpr string kBackgroundColor = "background_color";
            constexpr string kBorderColor = "border_color";

        }

        namespace text::attribute {
            constexpr string kFontSize = "font_size";
            constexpr string kFontName = "font_name";
            constexpr string kStyle = "style";
            constexpr string kAlignment = "alignment";
            constexpr string kUnderline = "underline";
        }
    }

    namespace color {
        constexpr string kBlack = "black";
        constexpr string kWhite = "white";
        constexpr string kRed = "red";
        constexpr string kGreen = "green";
        constexpr string kBlue = "blue";
        constexpr string kYellow = "yellow";
        constexpr string kMagenta = "magenta";
        constexpr string kCyan = "cyan";
        constexpr string kPurple = "purple";
    }

    namespace style {
        constexpr string kNormal = "normal";
        constexpr string kBold = "bold";
        constexpr string kItalic = "italic";
        constexpr string kBoldItalic = "bold_italic";
    }

    namespace alignment {
        constexpr string kLeft = "left";
        constexpr string kCenter = "center";
        constexpr string kRight = "right";
        constexpr string kJustified = "justified";
    }
    namespace orientation {
        constexpr string kHorizontal = "horizontal";
        constexpr string kVertical = "vertical";
    }
}
