#pragma once

#include <unordered_map>
#include "Errors/ErrorReporter.hpp"
#include "SourceControl/SourceManager.hpp"
#include "Tokens/Tokenizer.hpp"
#include "Parser/Parser.hpp"
#include "Driver/CompilationUnit.hpp"

using FileId = std::size_t;

class Driver 
{
    public:
        Driver()
            : m_srcManager(), 
              m_errReporter( m_srcManager ) {}

        void compileProgram( std::string& filePath );

        void tokenizeCompilationUnit( CompilationUnit& compUnit );

        void parseCompilationUnit( CompilationUnit& compUnit );

    private:
        SourceManager m_srcManager;
        ErrorReporter m_errReporter;
        std::unordered_map<FileId, CompilationUnit> m_compilationUnits;
};