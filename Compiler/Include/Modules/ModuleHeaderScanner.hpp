#pragma once

/* === Imports === */

#include <expected>
#include <fstream>
#include <filesystem>
#include "Errors/Errors.hpp"
#include "Modules/ModuleInfo.hpp"
#include "Modules/ModuleTable.hpp"

namespace fs = std::filesystem;

/* === Forward Declarations === */

class ErrorReporter;
class SourceManager;

/* === Module Header Scanner === */

class ModuleHeaderScanner {
    public:
        ModuleHeaderScanner( ErrorReporter& errReporter, SourceManager& srcManager ) 
            : m_errReporter( errReporter ), m_srcManager( srcManager ) {}
        std::expected<ModuleTable, Diagnostic> scan( const fs::path& sourcePath );
        std::expected<std::string, Diagnostic> parseModuleDirective();
        std::expected<std::vector<std::string>, Diagnostic> parseImportStatements();
    private:
        ErrorReporter& m_errReporter;
        SourceManager& m_srcManager;

        std::ifstream m_stream;

        std::string m_line;
        std::size_t m_pos = 0;
        bool m_inComment = false;

        std::string_view readIdentifier( std::string_view line, std::size_t& pos );
        bool isIdentifierPartChar( char c );
        bool isIdentifierStartChar( char c );
        bool isWhitespaceChar( char c );

};