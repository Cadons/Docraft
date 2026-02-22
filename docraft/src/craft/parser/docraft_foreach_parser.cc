#include <algorithm>
#include <cctype>
#include <string>
#include <string_view>

#include "craft/parser/docraft_parser.h"
#include "model/docraft_foreach.h"
#include "craft/docraft_craft_language_tokens.h"

namespace docraft::craft::parser {
    namespace {
        /**
         * Helper function to determine if a character is a word character (alphanumeric or underscore).
         * This is used to check if a single quote is part of a word (e.g., in contractions like "don't") and should not be converted to a double quote.
         */
        bool is_word_char(char ch) {
            auto uch = static_cast<unsigned char>(ch);
            // Convert the character to unsigned char to avoid issues with negative values when using std::isalnum, which expects an unsigned char or EOF.
            return std::isalnum(uch) != 0 || ch == '_';
            // This function checks if the character is alphanumeric (letters and digits) or an underscore, which are commonly considered word characters in programming languages and natural language processing.
        }

        /**
         * @brief Normalize a JSON string that may use single quotes for strings instead of double quotes, while preserving
         * valid JSON syntax and handling edge cases such as escaped characters and single quotes within words.
         * @param input
         * @return
         */
        std::string normalize_single_quoted_json(std::string_view input) {
            std::string output;
            output.reserve(input.size());
            bool in_single = false;
            bool in_double = false;
            for (size_t i = 0; i < input.size(); ++i) {
                char ch = input[i]; // Handle escape sequences
                if (ch == '\\') {
                    // If we're in a single-quoted string, we need to handle escaped characters
                    if (in_single && (i + 1) < input.size()) {
                        // Only handle escaping of backslashes and single quotes within single-quoted strings
                        char next = input[i + 1];
                        // If the next character is a backslash or a single quote, we want to keep it as is and skip the escape character
                        if (next == '\\' || next == '\'') {
                            // Add the next character to the output and skip the escape character
                            output.push_back(next);
                            // Skip the next character since we've already added it to the output
                            ++i;
                            continue;
                        }
                    }
                    output.push_back(ch);
                    // For other cases, just add the backslash as is and let the JSON parser handle it
                    continue;
                }
                if (ch == '\'' && !in_double) {
                    // If we're in a single-quoted string, we need to check if this quote is actually a part of a word (e.g., in contractions like "don't") and if so, we should keep it as a single quote instead of converting it to a double quote
                    if (in_single) {
                        // Check if the previous and next characters are word characters, which would indicate that this single quote is part of a word
                        char prev = (i > 0) ? input[i - 1] : '\0';
                        // Use '\0' as a sentinel value for the start and end of the string
                        char next = ((i + 1) < input.size()) ? input[i + 1] : '\0';
                        // If both the previous and next characters are word characters, we should keep this single quote as is
                        if (is_word_char(prev) && is_word_char(next)) {
                            // Add the single quote to the output and continue to the next character
                            output.push_back('\'');
                            continue;
                        }
                    }
                    in_single = !in_single;
                    // Toggle the in_single flag and convert the single quote to a double quote for JSON compatibility
                    output.push_back('"'); // Continue to the next character
                    continue;
                }
                if (ch == '"' && !in_single) {
                    // If we're in a double-quoted string, we need to toggle the in_double flag but keep the double quote as is since it's already valid JSON
                    in_double = !in_double; // Add the double quote to the output and continue to the next character
                    output.push_back(ch); // Continue to the next character
                    continue;
                }
                output.push_back(ch);
            }
            return output;
        }
    }

    DocraftForeachParser::DocraftForeachParser(
        DocraftCraftLanguageParser *craft_language_parser) : craft_language_parser_(craft_language_parser) {
    }

    std::shared_ptr<model::DocraftNode> DocraftForeachParser::parse(const pugi::xml_node &craft_language_source) {
        auto foreach_node = std::make_shared<model::DocraftForeach>();
        if (auto model_attr = craft_language_source.attribute(
            craft::elements::templating::foreach::attribute::kModel.data())) {
            auto model_value = normalize_single_quoted_json(model_attr.as_string());
            foreach_node->set_model(model_value);
            auto children = craft_language_source.children();
            std::vector<std::shared_ptr<model::DocraftNode> > template_nodes;
            for (const auto &child: children) {
                if (child.type() != pugi::node_element) {
                    continue; // Skip non-element nodes
                }
                template_nodes.push_back(craft_language_parser_->parse_node(child));
            }
            foreach_node->set_template_nodes(template_nodes);
            return foreach_node;
        }
        throw std::runtime_error("Missing 'model' attribute in foreach node.");
    }
} // namespace docraft::craft::parser
