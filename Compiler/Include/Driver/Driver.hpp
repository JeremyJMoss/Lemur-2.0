#pragma once

/* === Dependencies ===*/

#include <unordered_map>

/* === Imports === */

#include "Errors/ErrorReporter.hpp"
#include "SourceControl/SourceManager.hpp"
#include "Driver/CompilationUnit.hpp"
#include "Modules/ModuleTable.hpp"
#include "Core/CompilerContext.hpp"
#include "Core/Ids.hpp"

/* === Forward Declarations === */

struct CompilerConfig;

/* === Compiler Driver === */

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
        CompilerContext m_ctx;
        SourceManager m_srcManager;
        ErrorReporter m_errReporter;

        std::unordered_map<FileId, CompilationUnit> m_compilationUnits;
};