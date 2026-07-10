#pragma once

/* === Imports === */

#include <string>
#include "Modules/DependencyGraph.hpp"

/* === Forward Declarations === */

struct ModuleHeader;
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

        DependencyGraph resolve( const ModuleHeader& entryModule );
    private:
        SourceManager& m_srcManager;
        ErrorReporter& m_errReporter;
};