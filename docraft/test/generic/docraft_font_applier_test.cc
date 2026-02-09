#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "docraft_document_context.h"
#include "generic/docraft_font_applier.h"
#include "utils/docraft_font_resolver.h"
#include "model/docraft_text.h"

using namespace docraft;

TEST(DocraftFontApplier, ResolvesBuiltInBold) {
    auto context = std::make_shared<DocraftDocumentContext>();
    auto applier = std::make_shared<generic::DocraftFontApplier>(context);
    context->set_font_applier(applier);

    auto text = std::make_shared<model::DocraftText>("Hello");
    text->set_font_name("Helvetica");
    text->set_style(model::TextStyle::kBold);

    applier->apply_font(text);
    EXPECT_EQ(text->font_name(), "Helvetica-Bold");
}

TEST(DocraftFontApplier, ResolvesBuiltInItalic) {
    auto context = std::make_shared<DocraftDocumentContext>();
    auto applier = std::make_shared<generic::DocraftFontApplier>(context);
    context->set_font_applier(applier);

    auto text = std::make_shared<model::DocraftText>("Hello");
    text->set_font_name("Helvetica");
    text->set_style(model::TextStyle::kItalic);

    applier->apply_font(text);
    EXPECT_EQ(text->font_name(), "Helvetica-Oblique");
}

TEST(DocraftFontApplier, ResolvesBuiltInBoldItalic) {
    auto context = std::make_shared<DocraftDocumentContext>();
    auto applier = std::make_shared<generic::DocraftFontApplier>(context);
    context->set_font_applier(applier);

    auto text = std::make_shared<model::DocraftText>("Hello");
    text->set_font_name("Helvetica");
    text->set_style(model::TextStyle::kBoldItalic);

    applier->apply_font(text);
    EXPECT_EQ(text->font_name(), "Helvetica-BoldOblique");
}

TEST(DocraftFontApplier, FallsBackToHelveticaWhenUnknown) {
    auto context = std::make_shared<DocraftDocumentContext>();
    auto applier = std::make_shared<generic::DocraftFontApplier>(context);
    context->set_font_applier(applier);

    auto text = std::make_shared<model::DocraftText>("Hello");
    text->set_font_name("UnknownFont");
    text->set_style(model::TextStyle::kNormal);

    applier->apply_font(text);
    EXPECT_EQ(text->font_name(), "Helvetica");
}

TEST(DocraftFontResolver, ResolvesBoldItalicFallbacks) {
    utils::DocraftFontResolver resolver;
    resolver.rebuild_index({"MyFont", "MyFont-Bold", "MyFont-Italic"}, {});
    EXPECT_EQ(resolver.resolve("MyFont", model::TextStyle::kBoldItalic), "MyFont-Bold");
    EXPECT_EQ(resolver.resolve("MyFont", model::TextStyle::kItalic), "MyFont-Italic");
}
