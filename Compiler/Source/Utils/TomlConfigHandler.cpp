#include "Utils/TomlConfigHandler.hpp"

#include "Config/Config.hpp"
#include "Errors/Errors.hpp"
#include "Tokens/Tokenizer.hpp"
#include <toml++/toml.hpp>
#include <filesystem>
#include <expected>

namespace fs = std::filesystem;

std::expected<fs::path, Diagnostic> TomlConfigHandler::findProjectRoot( fs::path start )
{
    while (true)
    {
        if ( fs::exists( start / "lemur.toml" ) )
            return start;

        if ( start.has_parent_path() )
            start = start.parent_path();
        else
            return std::unexpected( 
                Diagnostic( 
                    "No lemur.toml found in directory tree.",
                    ErrorCategory::FileIO,
                    ErrorSeverity::Fatal
                ) 
            );
    }
}

std::expected<CompilerConfig, Diagnostic> TomlConfigHandler::parseOrFail() 
{ 
    CompilerConfig config;

    try
    {
        auto maybeProjectRoot = findProjectRoot( fs::current_path() );

        if ( !maybeProjectRoot ) {
            return std::unexpected( maybeProjectRoot.error() );
        }

        const fs::path projectRoot = std::move(maybeProjectRoot.value());

        fs::path tomlFilePath = fs::absolute( projectRoot / "lemur.toml" );

        toml::table tbl = toml::parse_file( tomlFilePath.string() );

        if (auto build = tbl["build"].as_table())
        {
            if (auto src = build->get_as<std::string>("source")) {
                config.sourcePath = fs::absolute(projectRoot / src->get());
            } else {
                return std::unexpected(
                    Diagnostic(
                        "Missing required \"source\" configuration value in lemur.toml file",
                        ErrorCategory::Config,
                        ErrorSeverity::Fatal
                    )
                );
            }

            if (auto entry = build->get_as<std::string>("entry")) {
                std::string entryModuleString = entry->get();
                
                auto maybeParsedModuleName = Tokenizer::parseModuleName( entryModuleString );

                if (!maybeParsedModuleName) {
                    return std::unexpected(maybeParsedModuleName.error());
                }

                config.entryModule = maybeParsedModuleName.value();
            } else {
                return std::unexpected(
                    Diagnostic(
                        "Missing required \"entry\" configuration value in lemur.toml file",
                        ErrorCategory::Linking,
                        ErrorSeverity::Fatal
                    )
                );
            }

            if (auto output = build->get_as<std::string>("output")) {
                config.outputPath = fs::absolute(projectRoot / output->get());
            } else {
                return std::unexpected(
                    Diagnostic(
                        "Missing required \"output\" configuration value in lemur.toml file",
                        ErrorCategory::Config,
                        ErrorSeverity::Fatal
                    )
                );
            }

            if (auto emitAst = build->get_as<bool>("emitAst")) {
                config.emitAST = emitAst->get();
            }
            
            if (auto optimize = build->get_as<bool>("optimize")) {
                config.optimize = optimize->get();
            }

        }
    }
    catch ( const toml::parse_error& err )
    {
        throw InternalCompilerError( "Toml Parsing Failed: " + std::string( err.what() ) );
    }

    return config;
}

bool TomlConfigHandler::createTomlFile(std::string_view projectName) 
{
    fs::path tomlFilePath = fs::absolute( fs::current_path() / fs::path("lemur.toml" ));

    try
    {
        toml::table root;

        // Build section
        toml::table build;

        build.insert("source", "src");
        build.insert("output", "build");
        build.insert("entry", "app");

        root.insert("build", build);

        toml::table project;

        project.insert("name", projectName);
        project.insert("version", "1.0.0");
        project.insert("language", "lemur");

        root.insert("project", project);


        // Ensure directory exists
        if (tomlFilePath.has_parent_path())
        {
            fs::create_directories(tomlFilePath.parent_path());
        }

        // Write to file
        std::ofstream file(tomlFilePath);

        if (!file.is_open())
            return false;

        file << toml::toml_formatter{ root };

        return true;
    }
    catch (...)
    {
        return false;
    } 
}