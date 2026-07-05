#pragma once

/* === Imports === */
#include <string>
#include <vector>
#include <iostream>
#include <tuple>
#include <variant>
#include "SourceControl/SourceLocation.hpp"
#include "SourceControl/SourceManager.hpp"
#include "Errors/Errors.hpp"

class ErrorReporter 
{
    public:
        ErrorReporter( SourceManager& srcManager ) : m_srcManager( srcManager ) {};

        void printErrorDiagnostic(
            ErrorSeverity severity,
            const std::string& message,
            const SourceRange& range
        ) const;

        void printErrorDiagnostic(
            CompilerError& compErr
        ) const;

        void printAllDiagnostics();

        void report( const ModuleHeaderError& modHeadErr );

        void report( const CompilerError& compErr );

        void report( const RuntimeError& runErr );

        void report( const SemanticError& semErr );

        bool hasFatalErrors() const;

        bool hasErrors() const;

        std::size_t getErrCount() const { return m_errCount; }
    private:
        SourceManager& m_srcManager;
        std::vector<CompilerError> m_compilerErrors;
        std::vector<RuntimeError> m_runtimeErrors;
        std::vector<SemanticError> m_semanticErrors;
        std::vector<ModuleHeaderError> m_moduleHeaderErrors;
        std::size_t m_errCount = 0;
};