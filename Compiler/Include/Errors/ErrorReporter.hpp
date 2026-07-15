#pragma once

/* === Dependencies ===*/

#include <string>
#include <vector>

/* === Imports === */

#include "Errors/Errors.hpp"

/* === Forward Declarations === */

struct SourceRange;
class SourceManager;

/* === Error Reporter === */

class ErrorReporter 
{
    public:
        ErrorReporter( SourceManager& srcManager ) 
            : m_srcManager( srcManager ) {};

        void printErrorDiagnostic(
            Diagnostic& diagnostic
        ) const;

        void printAllDiagnostics();

        void report( const Diagnostic& diagnostic );

        bool hasErrors() const;

        bool hasDiagnostics() const;

        std::size_t getErrCount() const { return m_errCount; }
    private:
        SourceManager& m_srcManager;
        std::vector<Diagnostic> m_diagnostics;
        std::size_t m_errCount = 0;
};