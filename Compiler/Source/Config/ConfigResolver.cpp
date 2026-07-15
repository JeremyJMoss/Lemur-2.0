#include "Utils/ConfigResolver.hpp"
#include "CLI/CLIConfig.hpp"
#include "Config/CompilerConfig.hpp"
#include "Tokens/Tokenizer.hpp"

void ConfigResolver::mergeConfigurations( const BuildCLIConfig& bConfig, CompilerConfig& cConfig ) {
    if ( bConfig.sourcePath.has_value() ) cConfig.sourcePath = bConfig.sourcePath.value();
    if ( bConfig.outputPath.has_value() ) cConfig.outputPath = bConfig.outputPath.value();
    if ( bConfig.emitAST.has_value() ) cConfig.emitAST = bConfig.emitAST.value();
    if ( bConfig.optimize.has_value() ) cConfig.optimize = bConfig.optimize.value();

    if ( bConfig.entryModule.has_value() ) {
        auto maybeParsedModuleName = Tokenizer::parseModuleName( bConfig.entryModule.value() );
        
        if ( !maybeParsedModuleName ) throw maybeParsedModuleName.error();

        cConfig.entryModule = maybeParsedModuleName.value();
    } 
}