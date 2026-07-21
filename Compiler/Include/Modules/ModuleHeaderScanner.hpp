#pragma once

/* === Dependencies ===*/

#include <expected>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

/* === Imports === */

#include "Errors/Errors.hpp"
#include "Modules/ModuleTable.hpp"
#include "Modules/ImportDirective.hpp"

/* === Forward Declarations === */

class CompilerContext;

/* === Module Header Scanner === */

class ModuleHeaderScanner {
    public:
        ModuleHeaderScanner( CompilerContext& ctx ) 
            : m_ctx( ctx ) {}
        std::expected<int, Diagnostic> scan( const fs::path& sourcePath, CompilerContext& ctx );
        std::expected<std::string, Diagnostic> parseModuleDirective();
        std::expected<std::vector<ImportDirective>, Diagnostic> parseImportDirectives();
    private:
        CompilerContext& m_ctx;

        std::ifstream m_stream;

        std::string m_line;
        std::size_t m_pos = 0;
        bool m_inComment = false;
        std::size_t m_braceDepth = 0;

        std::string_view readIdentifier( std::string_view line, std::size_t& pos );
        bool isIdentifierPartChar( char c );
        bool isIdentifierStartChar( char c );
        bool isWhitespaceChar( char c );

};