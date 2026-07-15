#pragma once

/* === Dependencies ===*/

#include <stdexcept>
#include <string>
#include <variant>
#include <optional>
#include <format>

/* === Imports === */

#include "AST/ASTNode.hpp"
#include "SourceControl/SourceLocation.hpp"
#include "Tokens/TokenKeyword.hpp"
#include "Tokens/TokenKind.hpp"
#include "Tokens/TokenSymbol.hpp"
#include "Tokens/TokenUtils.hpp"

/* === Enum Declarations === */

enum class ErrorCategory {
    Lexical,     // Tokenization
    Syntax,      // Parsing
    Semantic,    // mManing / type checking
    Linking,     // Module resolution errors
    Config,      // Project configuration Error
    FileIO       // File opening/input/output
};

enum class ErrorSeverity {
    Note,
    Warning,
    Error,
    Fatal
};

/* === Utility === */

inline std::string_view toString( const ErrorCategory category )
{
    switch( category )
    {
        case ErrorCategory::Lexical:     return "Lexical";
        case ErrorCategory::Syntax:      return "Syntax";
        case ErrorCategory::Semantic:    return "Semantic";
        case ErrorCategory::Linking:     return "Linking";
        case ErrorCategory::Config:      return "Project Configuration";
        case ErrorCategory::FileIO:      return "File I/O";
        default:                         return "Uncategorised";
    }
}

inline std::string_view toString( const ErrorSeverity severity )
{
    switch( severity )
    {
        case ErrorSeverity::Note:    return "Note";
        case ErrorSeverity::Warning: return "Warning";
        case ErrorSeverity::Error:   return "Error";
        case ErrorSeverity::Fatal:   return "Fatal";
        default:                     return "Other";
    }
}

/* === Error Types === */

class ConfigError : public std::exception 
{
    public:
        std::string message;
        ConfigError( 
            std::string message
        ) : message( std::move( message ) ) {}

        const char* what() const noexcept override {
            return message.c_str();
        }
};

class Diagnostic
{
    public:
        const std::string message;
        const ErrorCategory category;
        const ErrorSeverity severity;
        std::optional<const SourceRange> range;

        Diagnostic( 
            std::string message,
            ErrorCategory category, 
            ErrorSeverity severity,
            SourceRange range 
        ) : message( std::move(  message  ) ), 
            category( category ),
            severity( severity ), 
            range( range ) {}

        Diagnostic(
            std::string message,
            ErrorCategory category,
            ErrorSeverity severity
        ) : message( std::move( message ) ),
            category( category ),
            severity( severity ) {}
};

struct CompilationAborted : std::exception {
    const char* what() const noexcept override {
        return "Compilation aborted";
    }
};

struct InternalCompilerError : std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct UnexpectedEndOfInputDiagnostic : Diagnostic {
    UnexpectedEndOfInputDiagnostic(
        SourceRange location
    ) : Diagnostic(
            "Unexpected end of input",
            ErrorCategory::Syntax,
            ErrorSeverity::Fatal,
            location
        ) {}
};

struct UnexpectedTypeDiagnostic : Diagnostic {
    UnexpectedTypeDiagnostic(
        TokenKind expectedType, 
        TokenKind actualType, 
        SourceRange location
    ) : Diagnostic( 
            std::format(
                "Expected '{}' got '{}'", 
                toString( expectedType ), 
                toString( actualType )
            ),
            ErrorCategory::Syntax,
            ErrorSeverity::Error,
            location
        ) {}
};

struct UnexpectedValueDiagnostic : Diagnostic {
    UnexpectedValueDiagnostic( 
        TokenSymbol expectedValue, 
        TokenSymbol actualValue, 
        SourceRange location
    ) : Diagnostic (
            std::format(
                "Expected '{}' got '{}'",
                toString( expectedValue ),
                toString( actualValue )
            ),
            ErrorCategory::Syntax,
            ErrorSeverity::Error,
            location
        ) {}

    UnexpectedValueDiagnostic( 
        TokenKeyword expectedValue, 
        TokenKeyword actualValue, SourceRange location
    ) : Diagnostic(
            std::format( 
                "Expected '{}' got '{}'",
                toString( expectedValue ),
                toString( actualValue )
            ),
            ErrorCategory::Syntax,
            ErrorSeverity::Error,
            location
        ) {}
};