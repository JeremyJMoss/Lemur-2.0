#include <string>
#include "Errors/ErrorReporter.hpp"
#include "Errors/Errors.hpp"
#include "SourceControl/SourceManager.hpp"
#include "Utils/Output.hpp"

void ErrorReporter::printErrorDiagnostic(
    Diagnostic& diagnostic
) const
{
    std::string buffer;

    if ( diagnostic.range ) 
    {
        const SourceRange& range = diagnostic.range.value();
        std::size_t startLine = range.start.line;
        std::size_t endLine   = range.end.line;
        std::size_t startCol  = range.start.column;
        std::size_t endCol    = range.end.column;
        FileId fileId = range.fileId;
        const fs::path& filePath = m_srcManager.getFilePath( fileId );

        std::string lineNumberWidth = std::to_string( endLine );
        std::size_t width = lineNumberWidth.length();

        std::format_to( 
            std::back_inserter( buffer ),
            "{} {}: {} at line {}, column {}: {}\n",
            toString( diagnostic.category ),
            toString( diagnostic.severity ),
            filePath.string(),
            startLine,
            startCol + 1,
            diagnostic.message
        );

        for ( std::size_t lineNo = startLine; lineNo <= endLine; ++lineNo )
        {
            auto maybeLineStr = m_srcManager.getLine( fileId, lineNo );

            if ( !maybeLineStr ) throw InternalCompilerError( maybeLineStr.error().message );

            std::string lineStr = maybeLineStr.value();

            const std::string lineNoStr = std::to_string( lineNo );

            buffer.append(width - lineNoStr.length(), ' ');

            std::format_to( 
                std::back_inserter( buffer ), 
                "{} | {}\n",
                lineNoStr,
                lineStr
            );

            buffer.append(width, ' ');
            buffer += " | ";

            if ( lineNo == startLine && lineNo == endLine ) 
            {
                // Single-line range
                buffer.append( startCol, ' ' ); 
                buffer.append( std::max( 1, ( int ) endCol - ( int ) startCol ), '^' );
            } 
            else if ( lineNo == startLine ) 
            {
                // First line of a multi-line range
                buffer.append( startCol, ' ' );
                buffer.append( lineStr.size() - ( startCol - 1 ), '^' );
            } 
            else if ( lineNo == endLine ) 
            {
                // Last line of a multi-line range
                buffer.append( 0, ' ' );
                buffer.append( std::min( ( int ) endCol - 1, ( int ) lineStr.size() ), '^' );
            } 
            else 
            {
                // Middle line
                buffer.append(lineStr.size(), '^' );
            }
        }

    } else {
        std::format_to( 
            std::back_inserter( buffer ),
            "{} {}: {}",
            toString( diagnostic.category ),
            toString( diagnostic.severity ),
            diagnostic.message
        );
    }
    
    Output::info( buffer );
}

void ErrorReporter::printAllDiagnostics()
{
    for ( Diagnostic& error: m_diagnostics )
    {
        printErrorDiagnostic( error );
    }

    m_diagnostics.clear();
}

void ErrorReporter::report( const Diagnostic& diagnostic ) 
{
    m_diagnostics.emplace_back( std::move( diagnostic ) );

    if ( diagnostic.severity >= ErrorSeverity::Error ) m_errCount++;

    if ( m_errCount > 50 ) throw CompilationAborted();

    if ( diagnostic.severity == ErrorSeverity::Fatal ) throw CompilationAborted(); 
}

bool ErrorReporter::hasErrors() const
{
    return m_errCount > 0;
}

bool ErrorReporter::hasDiagnostics() const
{    
    return m_diagnostics.size() > 0;
}