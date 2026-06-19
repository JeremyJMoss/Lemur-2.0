#pragma once

/* === Imports === */
#include <string>
#include <vector>
#include <iostream>
#include <tuple>
#include <variant>
#include "Utils/SourceLocation.hpp"
#include "Utils/SourceManager.hpp"

enum class ErrorCategory {
    Lexical,   // tokenization
    Syntax,    // parsing
    Semantic,  // meaning / type checking
    Internal,  // compiler bug / assertion
    Linking    // Module resolution errors
};

inline std::string toString( const ErrorCategory& category )
{
    switch( category )
    {
        case ErrorCategory::Lexical:  return "Lexical";
        case ErrorCategory::Syntax:   return "Syntax";
        case ErrorCategory::Semantic: return "Semantic";
        case ErrorCategory::Internal: return "Internal";
        case ErrorCategory::Linking:  return "Linking"; 
        default:                      return "Uncategorised";  
    }
}

enum class ErrorSeverity {
    Note,
    Warning,
    Error,
    Fatal
};

inline std::string toString( const ErrorSeverity& severity )
{
    switch( severity )
    {
        case ErrorSeverity::Note:    return "Note";
        case ErrorSeverity::Warning: return "Warning";
        case ErrorSeverity::Error:   return "Error";
        case ErrorSeverity::Fatal:   return "Fatal Error";
        default:                     return "Other"; 
    }
}

class CompilerError {
    public:
        ErrorSeverity severity;
        std::string message;
        SourceRange range;
        ErrorCategory category;

        CompilerError( ErrorSeverity severity, const std::string& message, 
            SourceRange range, ErrorCategory category )
            : severity( severity ), message( std::move(  message  ) ), 
            range( range ), category( category )  {}
};

class RuntimeError {
    public:
        ErrorSeverity severity;
        std::string message;

        RuntimeError( ErrorSeverity severity, const std::string& message )
            : severity( severity ), message( std::move(  message  ) ) {};
};

struct ErrorVariant : public std::variant<CompilerError, RuntimeError>
{
    using Base = std::variant<CompilerError, RuntimeError>;
    using Base::Base; // inherit variant's constructors

    // Allow implicit construction from each alternative
    ErrorVariant(const CompilerError& err)    : Base(err) {}
    ErrorVariant(CompilerError&& err)         : Base(std::move(err)) {}
    ErrorVariant(const RuntimeError& err)     : Base(err) {}
    ErrorVariant(RuntimeError&& err)          : Base(std::move(err)) {}
};

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

        void report( CompilerError&& compErr );

        void report( RuntimeError&& runErr );

        bool hasFatalErrors() const;

        bool hasErrors() const;

        size_t getErrCount() const { return m_errCount; }
    private:
        SourceManager& m_srcManager;
        std::vector<CompilerError> m_compilerErrors;
        std::vector<RuntimeError> m_runtimeErrors;
        size_t m_errCount = 0;
};