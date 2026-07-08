#pragma once

/* === Imports === */

#include <string>
#include "Modules/DependencyGraph.hpp"

/* === Forward Declarations === */

struct ModuleInfo;
class SourceManager;
class ErrorReporter;

/* === Import Resolver === */

class ImportResolver
{
    public:
        ImportResolver( 
            SourceManager& srcManager, 
            ErrorReporter& errReporter
        ) : m_srcManager( srcManager ), 
            m_errReporter( errReporter ) {}

        DependencyGraph resolve( const ModuleInfo& entryModule );
    private:
        SourceManager& m_srcManager;
        ErrorReporter& m_errReporter;
};