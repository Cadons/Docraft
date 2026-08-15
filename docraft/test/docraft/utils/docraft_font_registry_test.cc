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
