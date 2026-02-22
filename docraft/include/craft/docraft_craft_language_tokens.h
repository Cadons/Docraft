#pragma once
#include <string>
using string = std::string_view;

namespace docraft::craft {
    /**
     * @brief Attribute names for basic node properties.
     */
    namespace basic::attribute {
        constexpr string kId = "id";
        constexpr string kNodeName = "name";
        constexpr string kX = "x";
        constexpr string kY = "y";
        constexpr string kWidth = "width";
        constexpr string kHeight = "height";
        constexpr string kAutoFillHeight = "auto_fill_height";
        constexpr string kAutoFillWidth = "auto_fill_width";
        constexpr string kPadding = "padding";
        constexpr string kWeight = "weight";
        constexpr string kPosition = "position";
        constexpr string kZIndex = "z_index";
        constexpr string kColor = "color";
        constexpr string kVisible = "visible";

        namespace position_type {
            constexpr string kBlock = "block";
            constexpr string kAbsolute = "absolute";
        }
    }

    /**
     * @brief Section-level tags and attributes.
     */
    namespace section {
        constexpr string kDocument = "document";
        constexpr string kHeader = "header";
        constexpr string kBody = "body";
        constexpr string kFooter = "footer";
        constexpr string kMetadata = "metadata";

        namespace attribute {
            constexpr string kMarginTop = "margin_top";
            constexpr string kMarginBottom = "margin_bottom";
            constexpr string kMarginLeft = "margin_left";
            constexpr string kMarginRight = "margin_right";
        }
    }

    /**
     * @brief Element tag names and element-specific attributes.
     */
    namespace elements {
        constexpr string kText = "Text";
        constexpr string kPageNumber = "PageNumber";
        constexpr string kImage = "Image";
        constexpr string kTable = "Table";
        constexpr string kTHead = "THead";
        constexpr string kTBody = "TBody";
        constexpr string kRow = "Row";
        constexpr string kCell = "Cell";
        constexpr string kTitle = "Title";
        constexpr string kSubtitle = "Subtitle";
        constexpr string kHTitle = "HTitle";
        constexpr string kVTitle = "VTitle";
        constexpr string kLayout = "Layout";
        constexpr string kBlankLine = "Blank";
        constexpr string kRectangle = "Rectangle";
        constexpr string kCircle = "Circle";
        constexpr string kTriangle = "Triangle";
        constexpr string kLine = "Line";
        constexpr string kPolygon = "Polygon";
        constexpr string kNewPage = "NewPage";
        constexpr string kSettings = "Settings";
        constexpr string kList = "List";
        constexpr string kUList = "UList";
        namespace metadata {
            constexpr string kDocumentTitle = "DocumentTitle";
            constexpr string kAuthor = "Author";
            constexpr string kCreator = "Creator";
            constexpr string kProducer = "Producer";
            constexpr string kSubject = "Subject";
            constexpr string kKeywords = "Keywords";
            constexpr string kTrapped = "Trapped";
            constexpr string kGtsPdfx = "GtsPdfx";
            constexpr string kCreationDate = "CreationDate";
            constexpr string kModificationDate = "ModificationDate";
            constexpr string kAutoKeywords = "AutoKeywords";
            namespace date_component {
                constexpr string kYear = "Year";
                constexpr string kMonth = "Month";
                constexpr string kDay = "Day";
                constexpr string kHour = "Hour";
                constexpr string kMinutes = "Minutes";
                constexpr string kSeconds = "Seconds";
                constexpr string kInd = "Ind";
                constexpr string kOffHour = "OffHour";
                constexpr string kOffMinutes = "OffMinutes";
            }
            namespace date::attribute {
                constexpr string kYear = "year";
                constexpr string kMonth = "month";
                constexpr string kDay = "day";
                constexpr string kHour = "hour";
                constexpr string kMinutes = "minutes";
                constexpr string kSeconds = "seconds";
                constexpr string kInd = "ind";
                constexpr string kOffHour = "off_hour";
                constexpr string kOffMinutes = "off_minutes";
            }
            namespace auto_keywords::attribute {
                constexpr string kEnabled = "enabled";
                constexpr string kMaxKeywords = "max_keywords";
                constexpr string kMinLength = "min_length";
                constexpr string kLanguage = "language";
            }
        }
        namespace templating {
            constexpr string kForeach = "foreach";
            namespace foreach::attribute {
                constexpr string kModel = "model";
            }
        }
        namespace list {
            namespace attribute {
                constexpr string kStyle = "style";
            }
            namespace style {
                constexpr string kNumber = "number";
                constexpr string kRoman = "roman";
            }
        }
        namespace ulist {
            namespace attribute {
                constexpr string kDot = "dot";
            }
            namespace dot {
                constexpr string kDash = "-";
                constexpr string kStar = "*";
                constexpr string kCircle = "circle";
                constexpr string kBox = "box";
            }
        }
        namespace settings {
            constexpr string kFonts = "Fonts";
            namespace attribute {
                constexpr string kPageSize = "page_size";
                constexpr string kPageOrientation = "page_orientation";
                constexpr string kHeaderRatio = "header_ratio";
                constexpr string kBodyRatio = "body_ratio";
                constexpr string kFooterRatio = "footer_ratio";
            }
            namespace fonts {
                constexpr string kFont = "Font";
                namespace attribute {
                    constexpr string kName = "name";
                }
                namespace font_type {
                    constexpr string kFontNormal ="FontNormal";
                    constexpr string kFontBold = "FontBold";
                    constexpr string kFontItalic = "FontItalic";
                    constexpr string kFontBoldItalic = "FontBoldItalic";
                    namespace attribute {
                        constexpr string kSrc = "src";
                    }
                }
            }
        }
        namespace image::attribute {
            constexpr string kSrc = "src";
            constexpr string kData = "data";
            constexpr string kDataWidth = "data_width";
            constexpr string kDataHeight = "data_height";
        }
        namespace table::attribute {
            constexpr string kModel = "model";
            constexpr string kBaselineOffset = "baseline_offset";
            constexpr string kTableTile = "TableTile";
        }
        namespace table_header::attribute {
            constexpr string kColor = "color";
        }
        namespace table_title::attribute {
            constexpr string kAlignment = "alignment";
            constexpr string kStyle = "style";
            constexpr string kColor = "color";
            constexpr string kBackgroundColor = "background_color";
        }
        namespace table_htitle::attribute {
            constexpr string kBackgroundColor = "background_color";
        }
        namespace table_vtitle::attribute {
            constexpr string kBackgroundColor = "background_color";
        }
        namespace table_row::attribute {
            constexpr string kBackgroundColor = "background_color";
        }
        namespace table_column::attribute {
            constexpr string kBackgroundColor = "background_color";
            constexpr string kTableTile = "TableTile";
        }
        namespace layout::attribute {
            constexpr string kOrientation = "orientation";
        }
        namespace rectangle::attribute {
            constexpr string kBackgroundColor = "background_color";
            constexpr string kBorderColor = "border_color";
            constexpr string kBorderWidth = "border_width";

        }
        namespace circle::attribute {
            constexpr string kBackgroundColor = "background_color";
            constexpr string kBorderColor = "border_color";
            constexpr string kBorderWidth = "border_width";
        }
        namespace triangle::attribute {
            constexpr string kBackgroundColor = "background_color";
            constexpr string kBorderColor = "border_color";
            constexpr string kBorderWidth = "border_width";
            constexpr string kPoints = "points";
        }
        namespace line::attribute {
            constexpr string kX1 = "x1";
            constexpr string kY1 = "y1";
            constexpr string kX2 = "x2";
            constexpr string kY2 = "y2";
            constexpr string kBorderColor = "border_color";
            constexpr string kBorderWidth = "border_width";
        }
        namespace polygon::attribute {
            constexpr string kBackgroundColor = "background_color";
            constexpr string kBorderColor = "border_color";
            constexpr string kBorderWidth = "border_width";
            constexpr string kPoints = "points";
        }

        namespace text::attribute {
            constexpr string kFontSize = "font_size";
            constexpr string kFontName = "font_name";
            constexpr string kStyle = "style";
            constexpr string kAlignment = "alignment";
            constexpr string kUnderline = "underline";
        }
    }

    /**
     * @brief Supported named color values.
     */
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

    /**
     * @brief Text style string literals.
     */
    namespace style {
        constexpr string kNormal = "normal";
        constexpr string kBold = "bold";
        constexpr string kItalic = "italic";
        constexpr string kBoldItalic = "bold_italic";
    }

    /**
     * @brief Alignment string literals.
     */
    namespace alignment {
        constexpr string kLeft = "left";
        constexpr string kCenter = "center";
        constexpr string kRight = "right";
        constexpr string kJustified = "justified";
    }
    /**
     * @brief Layout orientation string literals.
     */
    namespace orientation {
        constexpr string kHorizontal = "horizontal";
        constexpr string kVertical = "vertical";
    }

}
