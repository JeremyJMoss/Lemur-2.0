#include "Modules/ModuleHeaderScanner.hpp"

#include <string>
#include <format>
#include "SourceControl/SourceManager.hpp"
#include "Errors/ErrorReporter.hpp"
#include "Utils/Logger.hpp"

/* === Utility Functions === */

std::string_view ModuleHeaderScanner::readIdentifier( std::string_view line, std::size_t& pos ) {
    std::size_t start = pos;
    while ( pos < line.size() && isIdentifierPartChar( line[ pos ] ) ) {
        pos++;
    }
    return line.substr( start, pos - start );
}

bool ModuleHeaderScanner::isIdentifierPartChar( char c ) {
    return std::isalnum( static_cast<unsigned char>( c ) ) || c == '_';
}

bool ModuleHeaderScanner::isIdentifierStartChar( char c ) {
    return std::isalpha( static_cast<unsigned char>( c ) ) || c == '_';
}

bool ModuleHeaderScanner::isWhitespaceChar( char c ) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\v' || c == '\f';
}

/* === Main Functions === */

std::expected<ModuleTable, Diagnostic> ModuleHeaderScanner::scan( const fs::path& sourcePath )
{   
    ModuleTable moduleTable;

    Logger::trace( "Started scanning file headers" );

    Logger::info( "Starting Module Resolution" );
    Logger::info( "Scanning file headers" );

    for ( const fs::directory_entry &file : fs::recursive_directory_iterator( sourcePath ) )
    {
        const fs::path path = file.path();
        const std::string pathStr = std::format( "'{}'", path.string() );

        if ( !file.is_regular_file() ) {
            continue;
        }

        if ( path.extension() != ".lmur" )
        {
            Logger::trace(
                "Skipping file",
                std::to_array<Attribute>({
                    { "Path", pathStr },
                    { "Reason", "Wrong extension" }
                })
            );
            continue;
        }

        auto maybeFileId = m_srcManager.addFile( path );

        if( !maybeFileId ) {
            return std::unexpected(
                Diagnostic(
                    "Error saving file to Source Manager",
                    ErrorCategory::Linking,
                    ErrorSeverity::Fatal
                )
            );
        }

        FileId fileId = maybeFileId.value();

        Logger::debug(
            "File added to source manager",
            std::to_array<Attribute>({
                { "Path", pathStr },
                { "FileId", std::to_string( fileId ) }
            })
        );

        m_stream.open( path );

        if ( !m_stream.is_open() ) 
        {
            return std::unexpected( 
                Diagnostic(
                    "Error opening .lmur file",
                    ErrorCategory::FileIO,
                    ErrorSeverity::Fatal
                ) 
            );
        }

        auto maybeModuleIdentifier = parseModuleDirective();

        if ( !maybeModuleIdentifier ) {
            m_errReporter.report( 
                Diagnostic(
                    std::format(
                        "{} for file path {}", 
                        maybeModuleIdentifier.error().message, 
                        pathStr 
                    ), 
                    maybeModuleIdentifier.error().category,
                    maybeModuleIdentifier.error().severity
                ) 
            );
            continue;
        }

        Logger::debug(
            "Module discovered",
            std::to_array<Attribute>({
                { "Module", maybeModuleIdentifier.value() },
                { "Path", pathStr }
            })
        );

        bool inserted = moduleTable.add( fileId, maybeModuleIdentifier.value() );

        if ( !inserted )
        {
            m_errReporter.report(
                Diagnostic(
                    std::format( 
                        "Duplicate module '{}'",
                        maybeModuleIdentifier.value() 
                    ),
                    ErrorCategory::Linking,
                    ErrorSeverity::Fatal
            ));
        }
    }

    return moduleTable;
}

std::expected<std::string, Diagnostic> ModuleHeaderScanner::parseModuleDirective()
{
    enum class State
    {
        Start,
        ExpectName,
        ExpectDotOrEnd
    };

    State state = State::Start;

    std::string moduleName = "";

    while (std::getline(m_stream, m_line))
    {
        m_pos = 0;

        while (m_pos < m_line.length())
        {
            char c = m_line[m_pos];

            // Handle block comments
            if ( m_inComment )
            {
                if (c == '*' && m_pos + 1 < m_line.length() && m_line[m_pos + 1] == '/')
                {
                    m_inComment = false;
                    m_pos += 2;
                }
                else
                {
                    ++m_pos;
                }
                continue;
            }

            // Skip whitespace
            if (isWhitespaceChar(c))
            {
                ++m_pos;
                continue;
            }

            // Skip Line comment
            if (c == '/' && m_pos + 1 < m_line.length() && m_line[m_pos + 1] == '/')
            {
                break;
            }

            // Enter multiline comment
            if (c == '/' && m_pos + 1 < m_line.length() && m_line[m_pos + 1] == '*')
            {
                m_inComment = true;
                m_pos += 2;
                continue;
            }

            // Identifier
            if (isIdentifierStartChar(c))
            {
                std::string_view id = readIdentifier( m_line, m_pos );

                if (state == State::Start)
                {
                    if (id != "module")
                        return std::unexpected(
                            Diagnostic(
                                "Invalid module header directive",
                                ErrorCategory::Linking,
                                ErrorSeverity::Error
                            )
                        );

                    state = State::ExpectName;
                    continue;
                }

                if (state == State::ExpectName)
                {
                    moduleName += std::string( id );
                    state = State::ExpectDotOrEnd;
                    continue;
                }

                return std::unexpected(
                    Diagnostic(
                        "Invalid module header directive",
                        ErrorCategory::Linking,
                        ErrorSeverity::Error
                    )
                );
            }

            // Dot (for module paths)
            if (c == '.')
            {
                if (state != State::ExpectDotOrEnd)
                    return std::unexpected(
                        Diagnostic(
                            "Malformed module header directive",
                            ErrorCategory::Linking, 
                            ErrorSeverity::Error
                        )
                    );

                moduleName += '.';
                state = State::ExpectName;
                ++m_pos;
                continue;
            }

            // End of module header
            if (c == ';')
            {
                if (state != State::ExpectDotOrEnd)
                    return std::unexpected(
                        Diagnostic(
                            "Incomplete module header directive missing ending ';'",
                            ErrorCategory::Linking,
                            ErrorSeverity::Error
                        )
                    );

                ++m_pos;
                return moduleName;
            }

            // Anything else is invalid
            return std::unexpected(
                Diagnostic(
                    "Invalid module header directive", 
                    ErrorCategory::Linking,
                    ErrorSeverity::Error
                )
            );
        }
    }

    return std::unexpected(
        Diagnostic(
            "No module directive found",
            ErrorCategory::Linking,
            ErrorSeverity::Warning
        )
    );
}

std::expected<std::vector<ImportDirective>, Diagnostic> ModuleHeaderScanner::parseImportDirectives()
{
    std::vector<ImportDirective> imports;

    enum class State
    {
        ExpectImport,
        ExpectTarget,
        SkipBraceContents,
        ExpectFrom,
        ExpectName,
        ExpectDotOrEnd,
        ExpectAlias,
        ExpectAliasEnd
    };

    State state = State::ExpectImport;

    std::string importName = "";

    m_braceDepth = 0;
    
    do {
        while (m_pos < m_line.length())
        {
            char c = m_line[m_pos];

            // Handle block comments
            if ( m_inComment )
            {
                if (c == '*' && m_pos + 1 < m_line.length() && m_line[m_pos + 1] == '/')
                {
                    m_inComment = false;
                    m_pos += 2;
                }
                else
                {
                    ++m_pos;
                }
                continue;
            }

            // Skip whitespace
            if ( isWhitespaceChar( c ) )
            {
                ++m_pos;
                continue;
            }

            // Skip line comment
            if (c == '/' && m_pos + 1 < m_line.length() && m_line[m_pos + 1] == '/')
            {
                if (state != State::ExpectImport)
                {
                    return std::unexpected(
                        Diagnostic(
                            "Unexpected end of import directive",
                            ErrorCategory::Linking,
                            ErrorSeverity::Error
                        )
                    );
                }

                // Ignore rest of line
                break;
            }

            // Enter multiline comment
            if (c == '/' && m_pos + 1 < m_line.length() && m_line[m_pos + 1] == '*')
            {
                m_inComment = true;
                m_pos += 2;
                continue;
            }

            if (state == State::ExpectAlias)
            {
                if (isIdentifierStartChar(c))
                {
                    readIdentifier(m_line, m_pos);
                    state = State::ExpectAliasEnd;
                    continue;
                }

                return std::unexpected(
                    Diagnostic(
                        "Expected alias name",
                        ErrorCategory::Linking,
                        ErrorSeverity::Error
                    )
                );
            }

            // End of import directive
            if (c == ';')
            {
                if (state != State::ExpectDotOrEnd && state != State::ExpectAliasEnd)
                    return std::unexpected(
                        Diagnostic(
                            "Incomplete import directive",
                            ErrorCategory::Linking,
                            ErrorSeverity::Error
                        )
                    );

                imports.emplace_back(importName);
                importName.clear();
                state = State::ExpectImport;
                ++m_pos;
                continue;
            }

            if (state == State::SkipBraceContents) 
            {
                if ( c == '{' ) {
                    m_braceDepth++;
                } else if ( c == '}' ) {
                    if (--m_braceDepth == 0 ) {
                        state = State::ExpectFrom;
                    }
                } 
                
                m_pos++;
                continue;
            }

            if (c == '{')
            {
                if (state != State::ExpectTarget) {
                    return std::unexpected(
                        Diagnostic(
                            "Malformed import header directive",
                            ErrorCategory::Linking, 
                            ErrorSeverity::Error
                        )
                    );
                }

                state = State::SkipBraceContents;
                m_braceDepth++;
                m_pos++;
                continue;
            }

            // Identifier
            if (isIdentifierStartChar(c))
            {
                std::string_view id = readIdentifier( m_line, m_pos );

                if (state == State::ExpectImport)
                {
                    if (id != "import")
                        return std::unexpected(
                            Diagnostic(
                                "Invalid import header directive",
                                ErrorCategory::Linking,
                                ErrorSeverity::Error
                            )
                        );

                    state = State::ExpectTarget;
                    continue;
                }

                if (state == State::ExpectTarget || state == State::ExpectName)
                {
                    importName.append( id );
                    state = State::ExpectDotOrEnd;
                    continue;
                }

                if (state == State::ExpectFrom && id == "from") {

                    state = State::ExpectName;
                    continue;
                }

                if (state == State::ExpectDotOrEnd && id == "as" ) {
                    state = State::ExpectAlias;
                    continue;
                }

                return std::unexpected(
                    Diagnostic(
                        "Invalid import header directive",
                        ErrorCategory::Linking,
                        ErrorSeverity::Error
                    )
                );
            }

            // Dot (for module paths)
            if (c == '.')
            {
                if (state != State::ExpectDotOrEnd)
                    return std::unexpected(
                        Diagnostic(
                            "Malformed import header directive",
                            ErrorCategory::Linking, 
                            ErrorSeverity::Error
                        )
                    );

                importName.append( "." );
                state = State::ExpectName;
                ++m_pos;
                continue;
            }

            if (state != State::ExpectImport) {
                // Anything else is invalid
                return std::unexpected(
                    Diagnostic(
                        "Invalid import header directive", 
                        ErrorCategory::Linking,
                        ErrorSeverity::Error
                    )
                );
            }
        }
        m_pos = 0;
    }
    while( std::getline(m_stream, m_line) );

    if (state != State::ExpectImport)
    {
        return std::unexpected(
            Diagnostic(
                "Unexpected end of file in import directive",
                ErrorCategory::Linking,
                ErrorSeverity::Error
            )
        );
    }

    return imports;
}