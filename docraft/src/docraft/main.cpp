/*
 * Copyright 2026 Matteo Cadoni (https://github.com/cadons)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "docraft/craft/docraft_loom_craft_language_parser.h"
#include "docraft/exception/docraft_exceptions.h"
#include "docraft/loom/docraft_loom_pdf_creator.h"
#include "docraft/templating/docraft_template_engine.h"
#include "docraft/utils/docraft_logger.h"

namespace {
    struct CliOptions
    {
        std::filesystem::path craft_file; //input file
        std::filesystem::path output_file; //output filename
        std::optional<std::filesystem::path> data_file; //optional --data JSON file
    };

    /**
     * @brief Checks if the given file path has a .craft extension (case-insensitive).
     * @param file_path Path to check.
     * @return True if the file has a .craft extension, false otherwise.
     */
    bool has_craft_extension(const std::filesystem::path& file_path)
    {
        std::string extension = file_path.extension().string();
        std::ranges::transform(extension, extension.begin(), [](const unsigned char ch)
        {
            return static_cast<char>(std::tolower(ch));
        });
        return extension == ".craft";
    }

    /**
     * @brief Prints usage information for the command-line tool.
     * @param output Output stream to print to (e.g., std::cout or std::cerr).
     * @param program_name Name of the program (typically argv[0]).
     */
    void print_usage(std::ostream& output, const char* program_name)
    {
        output << "Usage: " << program_name << " <file.craft> <output.pdf> [--data <data.json>]\n";
        output << "Options:\n";
        output << "  --data <data.json> Registers each top-level JSON field as a template\n";
        output << "                     variable ${field}, resolved in the .craft file's\n";
        output << "                     Text/Title/Subtitle content, Image src, and Foreach\n";
        output << "                     model attributes. Arrays/objects are passed through\n";
        output << "                     as their JSON text (for Foreach's model).\n";
        output << "  -h, --help         Show this help message.\n";
        output << "  -v, --version      Show version information.\n";
    }

    /**
     * @brief Parses command-line arguments and returns a CliOptions struct.
     * @param argc Argument count.
     * @param argv Argument vector.
     * @return Parsed command-line options.
     * @throws docraft::exception::ConfigurationException if arguments are invalid.
     */
    CliOptions parse_args(int argc, char* argv[])
    {
        CliOptions options;
        std::vector<std::string> positional_args;

        //handle -v --version before other parsing to allow it to work without required positional arguments
        if (argc == 2 && (std::string(argv[1]) == "-v" || std::string(argv[1]) == "--version"))
        {
            std::cout << "Docraft version " << DOCRAFT_VERSION;
            std::exit(0);
        }
        for (int i = 1; i < argc; ++i)
        {
            const std::string arg = argv[i];

            if (arg == "-h" || arg == "--help")
            {
                print_usage(std::cout, argv[0]);
                std::exit(0);
            }

            if (arg == "--data")
            {
                if (i + 1 >= argc)
                {
                    throw docraft::exception::ConfigurationException("--data requires a <data.json> argument");
                }
                options.data_file = argv[++i];
                continue;
            }

            if (!arg.empty() && arg.front() == '-')
            {
                throw docraft::exception::ConfigurationException("Unknown option: " + arg);
            }

            positional_args.push_back(arg); //collect positional arguments
        }

        if (positional_args.size() != 2)
        {
            throw docraft::exception::ConfigurationException("Expected required arguments: <file.craft> <output.pdf>");
        }

        options.craft_file = positional_args[0]; //first positional argument is the input .craft file, always
        options.output_file = positional_args[1]; //second positional argument is the output file, always

        // Validate input and output file paths
        if (!has_craft_extension(options.craft_file))
        {
            throw docraft::exception::ConfigurationException("Input file must have .craft extension");
        }
        if (options.output_file.filename().empty())
        {
            throw docraft::exception::ConfigurationException("Output destination must include a filename");
        }
        // If output file has no extension, add .pdf. If it has an extension, ensure it's .pdf (case-insensitive).
        if (!options.output_file.has_extension())
        {
            options.output_file += ".pdf";
        }
        else
        {
            std::string extension = options.output_file.extension().string();
            std::ranges::transform(extension, extension.begin(), [](const unsigned char ch)
            {
                return static_cast<char>(std::tolower(ch));
            });
            if (extension != ".pdf")
            {
                throw docraft::exception::ConfigurationException("Output file extension must be .pdf");
            }
        }

        return options;
    }

    /**
     * @brief Loads a JSON file and registers every field as a template variable, via
     * docraft::templating::DocraftTemplateEngine::add_template_variables_from_json()
     * (nested objects flattened to dot-notation keys).
     * @param data_file Path to the JSON file.
     * @return Template engine with every field registered.
     * @throws docraft::exception::ConfigurationException if the file can't be read or
     * parsed, or isn't a JSON object at the top level.
     */
    std::shared_ptr<docraft::templating::DocraftTemplateEngine> build_template_engine_from_json(
        const std::filesystem::path& data_file)
    {
        std::ifstream file(data_file);
        if (!file)
        {
            throw docraft::exception::ConfigurationException("Could not open --data file: " + data_file.string());
        }
        std::stringstream buffer;
        buffer << file.rdbuf();

        nlohmann::json root;
        try
        {
            root = nlohmann::json::parse(buffer.str());
        }
        catch (const nlohmann::json::exception& ex)
        {
            throw docraft::exception::ConfigurationException(
                "Invalid JSON in --data file " + data_file.string() + ": " + ex.what());
        }
        if (!root.is_object())
        {
            throw docraft::exception::ConfigurationException(
                "--data file must contain a JSON object at the top level: " + data_file.string());
        }

        auto engine = std::make_shared<docraft::templating::DocraftTemplateEngine>();
        engine->add_template_variables_from_json(root);
        return engine;
    }
} // namespace

int main(int argc, char* argv[])
{
    docraft::utils::DocraftLogger::enable_error();
    docraft::utils::DocraftLogger::enable_warning();
    docraft::utils::DocraftLogger::enable_info();

    try
    {
        LOG_INFO("Welcome to Docraft Generation Tool!");
        LOG_INFO("Docraft version " + std::string(DOCRAFT_VERSION));

        const CliOptions options = parse_args(argc, argv);
        if (!std::filesystem::exists(options.craft_file))
        {
            throw docraft::exception::FileNotFoundException(
                "docraft/craft file not found: " + options.craft_file.string());
        }

        docraft::craft::DocraftLoomCraftLanguageParser parser;
        if (options.data_file)
        {
            if (!std::filesystem::exists(*options.data_file))
            {
                throw docraft::exception::FileNotFoundException(
                    "--data file not found: " + options.data_file->string());
            }
            parser.set_template_engine(build_template_engine_from_json(*options.data_file));
        }
        parser.load_from_file(options.craft_file);

        auto creator = parser.edit_creator();
        if (!creator)
        {
            throw docraft::exception::RenderingFailedException("Unable to build document from .craft file");
        }

        const std::filesystem::path output_parent = options.output_file.parent_path();
        if (!output_parent.empty())
        {
            std::filesystem::create_directories(output_parent); //ensure output directory exists
        }

        LOG_INFO("Rendering document to PDF...");
        auto timer = std::chrono::high_resolution_clock::now();
        creator->create();
        creator->render(options.output_file);
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - timer).count();
        LOG_INFO("Document rendered in " + std::to_string(duration) + " ms");
        LOG_INFO("Generated: " + options.output_file.string());
        return 0;
    }
    catch (const docraft::exception::DocraftException& ex)
    {
        LOG_ERROR("Error: " + std::string(ex.what()));
        LOG_INFO("Use -h or --help for usage information.");
        return 1;
    }
    catch (const std::exception& ex)
    {
        LOG_ERROR("Error: " + std::string(ex.what()));
        return 1;
    }
}
