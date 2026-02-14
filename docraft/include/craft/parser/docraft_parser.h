#pragma once
#include "craft/docraft_craft_language_parser.h"
#include "craft/i_docraft_parser.h"
#include "model/docraft_rectangle.h"
#include "model/docraft_settings.h"

namespace docraft::craft::parser {
    class DocraftRectangleParser : public IDocraftParser {
    public:
        std::shared_ptr<model::DocraftNode> parse(const pugi::xml_node &craft_language_source) override;
        static void apply_attributes_to(const std::shared_ptr<model::DocraftRectangle>& from, const std::shared_ptr<model::DocraftRectangle>& to);
    };
    class DocraftHeaderParser : public DocraftRectangleParser {
    public:
        std::shared_ptr<model::DocraftNode> parse(const pugi::xml_node &craft_language_source) override;
    };

    class DocraftBodyParser : public DocraftRectangleParser {
    public:
        std::shared_ptr<model::DocraftNode> parse(const pugi::xml_node &craft_language_source) override;
    };

    class DocraftFooterParser : public DocraftRectangleParser {
    public:
        std::shared_ptr<model::DocraftNode> parse(const pugi::xml_node &craft_language_source) override;
    };
    class DocraftLayoutParser : public IDocraftParser {
    public:
        std::shared_ptr<model::DocraftNode> parse(const pugi::xml_node &craft_language_source) override;
    };
    class DocraftTextParser : public IDocraftParser {
    public:
        std::shared_ptr<model::DocraftNode> parse(const pugi::xml_node &craft_language_source) override;
    };
    class DocraftImageParser : public IDocraftParser {
    public:
        std::shared_ptr<model::DocraftNode> parse(const pugi::xml_node &craft_language_source) override;
    };
    class DocraftTableParser : public IDocraftParser {
    public:
        std::shared_ptr<model::DocraftNode> parse(const pugi::xml_node &craft_language_source) override;
    };

    class DocraftBlackLineParser : public IDocraftParser {
    public:
        std::shared_ptr<model::DocraftNode> parse(const pugi::xml_node &craft_language_source) override;
    };
    class DocraftSettingsParser : public IDocraftParser {
    public:
        std::shared_ptr<model::DocraftNode> parse(const pugi::xml_node &craft_language_source) override;
    };

} // docraft
