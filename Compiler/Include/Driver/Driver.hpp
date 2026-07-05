#pragma once

#include <unordered_map>
#include "Errors/ErrorReporter.hpp"
#include "SourceControl/SourceManager.hpp"
#include "Tokens/Tokenizer.hpp"
#include "Parser/Parser.hpp"
#include "Driver/CompilationUnit.hpp"

using FileId = std::size_t;

struct CompilerConfig;

class Driver 
{
    public:
        Driver( const CompilerConfig& config )
            : m_config( config ),
              m_srcManager(), 
              m_errReporter( m_srcManager ) {}

        void compileProgram();

    private:
        void compile();
        void tokenizeCompilationUnit( CompilationUnit& compUnit );
        void parseCompilationUnit( CompilationUnit& compUnit );
        
        const CompilerConfig& m_config;
        SourceManager m_srcManager;
        ErrorReporter m_errReporter;
        std::unordered_map<FileId, CompilationUnit> m_compilationUnits;
};