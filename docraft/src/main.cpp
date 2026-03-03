#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "craft/docraft_craft_language_parser.h"
#include "docraft_document.h"
#include "templating/docraft_template_engine.h"

namespace {
    struct CliOptions {
        std::filesystem::path craft_file; //input file
        std::filesystem::path output_file; //output filename
        std::filesystem::path data_file; //data file
        bool has_data_file = false; //flag to indicate if data file is provided
    };

    /**
     * @brief Trims leading and trailing whitespace from a string.
     * @param source Input string to trim.
     * @return A new string with leading and trailing whitespace removed.
     */
    std::string trim_copy(std::string_view source) {
        std::size_t start = 0;
        std::size_t end = source.size();
        while (start < end && std::isspace(static_cast<unsigned char>(source[start]))) {
            ++start;
        }
        while (end > start && std::isspace(static_cast<unsigned char>(source[end - 1]))) {
            --end;
        }
        return std::string(source.substr(start, end - start));
    }

    /**
     * @brief Normalizes a key by converting it to lowercase.
     * @param key Input key string.
     * @return A new string with all characters converted to lowercase.
     */
    std::string normalize_key(std::string key) {
        std::ranges::transform(key, key.begin(), [](const unsigned char ch) {
            return static_cast<char>(std::tolower(ch));
        });
        return key;
    }

    /**
     * @brief Checks if the given file path has a .craft extension (case-insensitive).
     * @param file_path Path to check.
     * @return True if the file has a .craft extension, false otherwise.
     */
    bool has_craft_extension(const std::filesystem::path &file_path) {
        std::string extension = file_path.extension().string();
        std::ranges::transform(extension, extension.begin(), [](const unsigned char ch) {
            return static_cast<char>(std::tolower(ch));
        });
        return extension == ".craft";
    }

    /**
     * @brief Prints usage information for the command-line tool.
     * @param output Output stream to print to (e.g., std::cout or std::cerr).
     * @param program_name Name of the program (typically argv[0]).
     */
    void print_usage(std::ostream &output, const char *program_name) {
        output << "Usage: " << program_name << " <file.craft> <output.pdf> [-d <data.txt>]\n";
        output << "Options:\n";
        output << "  -d, --data <file>  Mapping file with one 'key: value' entry per line.\n";
        output << "  -h, --help         Show this help message.\n";
        output << "  -v, --version      Show version information.\n";
    }

    /**
     * @brief Parses command-line arguments and returns a CliOptions struct.
     * @param argc Argument count.
     * @param argv Argument vector.
     * @return Parsed command-line options.
     * @throws std::runtime_error if arguments are invalid or missing.
     */
    CliOptions parse_args(int argc, char *argv[]) {
        CliOptions options;
        std::vector<std::string> positional_args;

        //handle -v --version before other parsing to allow it to work without required positional arguments
        if (argc==2 && (std::string(argv[1]) == "-v" || std::string(argv[1]) == "--version")) {
            std::cout << "Docraft version " << DOCRAFT_VERSION;
            std::exit(0);
        }
        for (int i = 1; i < argc; ++i) {
            const std::string arg = argv[i];

            if (arg == "-h" || arg == "--help") {
                print_usage(std::cout, argv[0]);
                std::exit(0);
            }

            if (arg == "-d" || arg == "--data") {
                if (i + 1 >= argc) {
                    throw std::runtime_error("Missing file path after " + arg);
                }
                options.data_file = argv[++i]; //get the next argument as data file path
                options.has_data_file = true;
                continue;
            }

            if (!arg.empty() && arg.front() == '-') {
                throw std::runtime_error("Unknown option: " + arg);
            }

            positional_args.push_back(arg); //collect positional arguments
        }

        if (positional_args.size() != 2) {
            throw std::runtime_error("Expected required arguments: <file.craft> <output.pdf>");
        }

        options.craft_file = positional_args[0]; //first positional argument is the input .craft file, always
        options.output_file = positional_args[1]; //second positional argument is the output file, always

        // Validate input and output file paths
        if (!has_craft_extension(options.craft_file)) {
            throw std::runtime_error("Input file must have .craft extension");
        }
        if (options.output_file.filename().empty()) {
            throw std::runtime_error("Output destination must include a filename");
        }
        // If output file has no extension, add .pdf. If it has an extension, ensure it's .pdf (case-insensitive).
        if (!options.output_file.has_extension()) {
            options.output_file += ".pdf";
        } else {
            std::string extension = options.output_file.extension().string();
            // Convert extension to lowercase for case-insensitive comparison
            std::ranges::transform(extension, extension.begin(), [](const unsigned char ch) {
                return static_cast<char>(std::tolower(ch));
            });
            if (extension != ".pdf") {
                throw std::runtime_error("Output file extension must be .pdf");
            }
        }

        return options;
    }

    /**
     * @brief Parses a mapping file with 'key: value' entries into an unordered map.
     * @param mapping_file Path to the mapping file.
     * @return An unordered map where keys are normalized (lowercase) and values are pairs of original key and value.
     * @throws std::runtime_error if the file cannot be opened or if any line is invalid.
     */
    std::unordered_map<std::string, std::pair<std::string, std::string> > parse_data_mapping_file(
        const std::filesystem::path &mapping_file) {
        std::ifstream input(mapping_file);
        if (!input) {
            throw std::runtime_error("Unable to open mapping file: " + mapping_file.string());
        }

        std::unordered_map<std::string, std::pair<std::string, std::string> > mappings;
        std::string line;
        std::size_t line_number = 0;

        //parse key : value
        while (std::getline(input, line)) {
            ++line_number;

            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }

            const std::string trimmed = trim_copy(line);
            if (trimmed.empty() || trimmed.starts_with('#') || trimmed.starts_with(';') || trimmed.starts_with("//")) {
                continue;
            }

            const std::size_t separator = trimmed.find(':');
            if (separator == std::string::npos) {
                throw std::runtime_error(
                    "Invalid mapping at line " + std::to_string(line_number) +
                    " in file '" + mapping_file.string() + "': expected 'key: value'");
            }

            std::string key = trim_copy(trimmed.substr(0, separator));
            std::string value = trim_copy(trimmed.substr(separator + 1));
            if (key.empty()) {
                throw std::runtime_error(
                    "Invalid mapping at line " + std::to_string(line_number) +
                    " in file '" + mapping_file.string() + "': empty key");
            }

            const std::string normalized_key = normalize_key(key);
            if (mappings.contains(normalized_key)) {
                std::cerr << "Warning: duplicate key '" << key << "' at line "
                        << line_number << " (overwriting previous value)\n";
            }

            mappings[normalized_key] = {std::move(key), std::move(value)};
        }

        return mappings;
    }
} // namespace

int main(int argc, char *argv[]) {
    try {
        const CliOptions options = parse_args(argc, argv);
        if (!std::filesystem::exists(options.craft_file)) {
            throw std::runtime_error("Craft file not found: " + options.craft_file.string());
        }

        docraft::craft::DocraftCraftLanguageParser parser;
        parser.load_from_file(options.craft_file.string());

        auto document = parser.get_document();
        if (!document) {
            throw std::runtime_error("Unable to build document from .craft file");
        }

        const std::filesystem::path output_parent = options.output_file.parent_path();
        document->set_document_path(output_parent.empty() ? "." : output_parent.string());
        document->set_document_title(options.output_file.stem().string());

        if (options.has_data_file) {
            if (!std::filesystem::exists(options.data_file)) {
                throw std::runtime_error("Data file not found: " + options.data_file.string());
            }
            const auto mappings = parse_data_mapping_file(options.data_file);
            auto template_engine = std::make_shared<docraft::templating::DocraftTemplateEngine>();
            //load mappings into template engine, using normalized keys for lookup but preserving original keys for variable names
            for (const auto &[_, mapping]: mappings) {
                template_engine->add_template_variable(mapping.first, mapping.second);
            }
            document->set_document_template_engine(template_engine);
        }

        std::filesystem::create_directories(document->document_path());//ensure output directory exists
        document->render();//render the document to PDF

        std::cout << "Generated: " << options.output_file.string() << '\n';
        return 0;
    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        std::cerr << "Run 'docraft_tool --help' for usage.\n";
        return 1;
    }
}
