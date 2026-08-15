#include <algorithm>

#include <gtest/gtest.h>

#include "docraft/utils/docraft_font_registry.h"

TEST(DocraftFontRegistryTest, ResolveFontAliasReturnsTargetWhenRegistered)
{
    auto& registry = docraft::utils::DocraftFontRegistry::instance();
    registry.register_font_alias("MyFont-Bold-Alias-Test", "InternalHaruName-Test");
    EXPECT_EQ(registry.resolve_font_alias("MyFont-Bold-Alias-Test"), "InternalHaruName-Test");
}

TEST(DocraftFontRegistryTest, ResolveFontAliasPassesThroughUnknownName)
{
    auto& registry = docraft::utils::DocraftFontRegistry::instance();
    EXPECT_EQ(registry.resolve_font_alias("NeverRegistered-Alias-Test"), "NeverRegistered-Alias-Test");
}

TEST(DocraftFontRegistryTest, RegisteredFontNamesIncludesAliases)
{
    auto& registry = docraft::utils::DocraftFontRegistry::instance();
    registry.register_font_alias("AliasListed-Test", "SomeInternalName-Test");
    const auto names = registry.registered_font_names();
    EXPECT_NE(std::find(names.begin(), names.end(), "AliasListed-Test"), names.end());
}

TEST(DocraftFontRegistryTest, RawFontNamesExcludesAliasOnlyNames)
{
    auto& registry = docraft::utils::DocraftFontRegistry::instance();
    registry.register_font_alias("AliasOnly-Test", "SomeInternalName-Test");
    const auto raw_names = registry.raw_font_names();
    EXPECT_EQ(std::find(raw_names.begin(), raw_names.end(), "AliasOnly-Test"), raw_names.end());
}

// Regression test: fonts.h's static registrars for the bundled fonts (docraft/fonts.json)
// only run if fonts.h's translation unit actually gets linked into the final binary --
// a static library linker drops it otherwise, since nothing else in it is referenced. This
// links against the real static archive (unlike a mock-backed unit test), so it is the
// only test that would have caught that regression.
TEST(DocraftFontRegistryTest, BundledFontsAreRegisteredAtStartup)
{
    const auto names = docraft::utils::DocraftFontRegistry::instance().raw_font_names();
    EXPECT_NE(std::find(names.begin(), names.end(), "Roboto"), names.end());
    EXPECT_NE(std::find(names.begin(), names.end(), "OpenSans"), names.end());
}
