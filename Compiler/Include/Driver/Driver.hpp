#pragma once

/* === Imports === */

#include <unordered_map>
#include "Errors/ErrorReporter.hpp"
#include "SourceControl/SourceManager.hpp"
#include "Driver/CompilationUnit.hpp"
#include "Modules/ModuleTable.hpp"

using FileId = std::size_t;

/* === Forward Declarations === */

struct CompilerConfig;

/* === Compiler Driver === */

class Driver 
{
    public:
        Driver( const CompilerConfig& config )
            : m_config( config ),
              m_srcManager(), 
              m_errReporter( m_srcManager ),
              m_modules() {}

        void compileProgram();

    private:
        void compile();
        void tokenizeCompilationUnit( CompilationUnit& compUnit );
        void parseCompilationUnit( CompilationUnit& compUnit );
        
        const CompilerConfig& m_config;
        SourceManager m_srcManager;
        ErrorReporter m_errReporter;
        ModuleTable m_modules;
        std::unordered_map<FileId, CompilationUnit> m_compilationUnits;
};