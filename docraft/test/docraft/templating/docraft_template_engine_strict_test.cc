#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include "docraft/exception/docraft_exceptions.h"
#include "docraft/templating/docraft_template_engine.h"

namespace {
    class DocraftTemplateEngineStrictTest : public ::testing::Test
    {
    protected:
        docraft::templating::DocraftTemplateEngine engine_;
    };

    TEST_F(DocraftTemplateEngineStrictTest, DisabledByDefault)
    {
        EXPECT_FALSE(engine_.is_strict());
    }

    TEST_F(DocraftTemplateEngineStrictTest, NonStrictUnresolvedVariableLeavesPlaceholderUnchanged)
    {
        EXPECT_EQ(engine_.render_template_string("Total: ${missing}"), "Total: ${missing}");
    }

    TEST_F(DocraftTemplateEngineStrictTest, StrictUnresolvedVariableThrows)
    {
        engine_.set_strict();
        EXPECT_TRUE(engine_.is_strict());
        EXPECT_THROW(engine_.render_template_string("Total: ${missing}"),
                     docraft::exception::TemplateVariableNotFoundException);
    }

    TEST_F(DocraftTemplateEngineStrictTest, StrictModeStillResolvesKnownVariables)
    {
        engine_.add_template_variable("title", "Docraft");
        engine_.set_strict();
        EXPECT_EQ(engine_.render_template_string("${title}"), "Docraft");
    }

    TEST_F(DocraftTemplateEngineStrictTest, StrictUnresolvedForeachDataFieldThrows)
    {
        const nlohmann::json item = {{"name", "Alice"}};
        engine_.set_strict();
        EXPECT_THROW(engine_.render_template_string_foreach_item("${data(\"missing_field\")}", item),
                     docraft::exception::TemplateVariableNotFoundException);
    }

    TEST_F(DocraftTemplateEngineStrictTest, StrictModeStillResolvesKnownForeachDataFields)
    {
        const nlohmann::json item = {{"name", "Alice"}};
        engine_.set_strict();
        EXPECT_EQ(engine_.render_template_string_foreach_item("${data(\"name\")}", item), "Alice");
    }

    TEST_F(DocraftTemplateEngineStrictTest, NonStrictUnresolvedForeachDataFieldReturnsEmptyString)
    {
        const nlohmann::json item = {{"name", "Alice"}};
        EXPECT_EQ(engine_.render_template_string_foreach_item("${data(\"missing_field\")}", item), "");
    }
} // namespace
