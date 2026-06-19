#include "Errors/ErrorReporter.hpp"
#include "Errors/Errors.hpp"

void ErrorReporter::printErrorDiagnostic(
    ErrorSeverity severity,
    const std::string& message, 
    const SourceRange& range
) const
{
    size_t startLine = range.start.line;
    size_t endLine   = range.end.line;
    size_t startCol  = range.start.column;
    size_t endCol    = range.end.column;
    const std::string& fileName = m_srcManager.getFileName( range.fileId );
    
    std::vector<std::tuple<std::string, size_t>> lines;

    for ( size_t i = startLine; i <= endLine; i++ ) 
    {
        lines.emplace_back( m_srcManager.getLine( range.fileId, i ), i );
    }

    std::string lineNumberWidth = std::to_string( endLine );
    size_t width = lineNumberWidth.length();

    std::cerr << toString( severity ) << ": " << fileName << " " << "at line " << startLine << ", column " << ( startCol + 1 ) << ": " << message << std::endl;

    for ( const std::tuple<std::string, size_t>& line : lines ) 
    {
        size_t lineNo = std::get<1>(line);
        std::string lineNoStr = std::to_string(lineNo);
        std::string ind(width - lineNoStr.length(), ' ');
        std::string lineStr = std::get<0>(line);

        std::cerr << ind << lineNoStr << " | " << lineStr << std::endl;
        std::cerr << std::string(width, ' ') << " | ";

        if ( lineNo == startLine && lineNo == endLine ) 
        {
            // Single-line range
            std::cerr << std::string(startCol, ' ') << std::string(std::max(1, (int) endCol - (int) startCol), '^');
        } 
        else if ( lineNo == startLine ) 
        {
            // First line of a multi-line range
            std::cerr << std::string(startCol, ' ') << std::string(lineStr.size() - (startCol - 1), '^');
        } 
        else if ( lineNo == endLine ) 
        {
            // Last line of a multi-line range
            std::cerr << std::string(0, ' ') << std::string(std::min((int) endCol - 1, (int)lineStr.size()), '^');
        } 
        else 
        {
            // Middle line
            std::cerr << std::string(lineStr.size(), '^');
        }

        std::cerr << std::endl;
    }
}

void ErrorReporter::printErrorDiagnostic(
    CompilerError& compErr
) const
{
    size_t startLine = compErr.range.start.line;
    size_t endLine   = compErr.range.end.line;
    size_t startCol  = compErr.range.start.column;
    size_t endCol    = compErr.range.end.column;
    const std::string& fileName = m_srcManager.getFileName( compErr.range.fileId );
    
    std::vector<std::tuple<std::string, size_t>> lines;

    for ( size_t i = startLine; i <= endLine; i++ ) 
    {
        lines.emplace_back( m_srcManager.getLine( compErr.range.fileId, i ), i );
    }

    std::string lineNumberWidth = std::to_string( endLine );
    size_t width = lineNumberWidth.length();

    std::cerr << toString( compErr.category ) << " " << 
        toString( compErr.severity ) << ": " << 
        fileName << " at line " << startLine << 
        ", column " << (startCol + 1) << ": " << 
        compErr.message << std::endl;

    for ( const std::tuple<std::string, size_t>& line : lines ) 
    {
        size_t lineNo = std::get<1>( line );
        std::string lineNoStr = std::to_string( lineNo );
        std::string ind( width - lineNoStr.length(), ' ' );
        std::string lineStr = std::get<0>( line );

        std::cerr << ind << lineNoStr << " | " << lineStr << std::endl;
        std::cerr << std::string( width, ' ' ) << " | ";

        if ( lineNo == startLine && lineNo == endLine ) 
        {
            // Single-line range
            std::cerr << std::string( startCol, ' ' ) << std::string( std::max( 1, ( int ) endCol - ( int ) startCol ), '^' );
        } 
        else if ( lineNo == startLine ) 
        {
            // First line of a multi-line range
            std::cerr << std::string( startCol, ' ' ) << std::string( lineStr.size() - ( startCol - 1 ), '^' );
        } 
        else if ( lineNo == endLine ) 
        {
            // Last line of a multi-line range
            std::cerr << std::string( 0, ' ' ) << std::string( std::min( ( int ) endCol - 1, ( int ) lineStr.size() ), '^' );
        } 
        else 
        {
            // Middle line
            std::cerr << std::string( lineStr.size(), '^' );
        }

        std::cerr << std::endl;
    }
}

void ErrorReporter::printAllDiagnostics()
{
    for ( auto error: m_runtimeErrors )
    {
        std::cerr << "[" << toString( error.severity ) << "] Internal compiler issue: " << error.message << std::endl;
    }

    m_runtimeErrors.clear();

    for ( auto error : m_compilerErrors )
    {
        printErrorDiagnostic( error );
    }

    m_compilerErrors.clear();
}

void ErrorReporter::report( CompilerError&& compErr ) 
{
    if ( m_errCount > 50 ) throw FatalCompilerError( "Too many errors to continue on." );

    m_compilerErrors.emplace_back( std::move( compErr ) );

    if ( compErr.severity == ErrorSeverity::Fatal ) 
    {
        m_errCount++;
        throw FatalCompilerError( "Fatal error caught during compilation" );
    }

    if ( compErr.severity == ErrorSeverity::Error ) m_errCount++;
    }

void ErrorReporter::report( RuntimeError&& runErr )
{
    if ( m_errCount > 50 ) throw FatalCompilerError( "Too many errors to continue on." );

    m_runtimeErrors.emplace_back( std::move( runErr ) );
    
    if ( runErr.severity == ErrorSeverity::Fatal ) 
    {
        m_errCount++;
        throw FatalCompilerError( "Fatal error caught during compilation" );
    }

    if ( runErr.severity == ErrorSeverity::Error ) m_errCount++;
}

bool ErrorReporter::hasFatalErrors() const
{
    for ( const auto& err : m_compilerErrors )
        if ( err.severity == ErrorSeverity::Fatal ) return true;
    for ( const auto& err : m_runtimeErrors )
        if ( err.severity == ErrorSeverity::Fatal ) return true;
    return false;
}

bool ErrorReporter::hasErrors() const
{
    return m_errCount > 0;
}