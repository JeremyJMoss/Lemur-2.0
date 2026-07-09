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
        ExpectDotOrEnd,
        ExpectImportKeyword
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