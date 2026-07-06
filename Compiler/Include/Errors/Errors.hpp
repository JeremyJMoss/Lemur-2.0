#pragma once

/* === Imports === */

#include <stdexcept>
#include <string>
#include <variant>
#include "Tokens/Token.hpp"
#include "AST/ASTNode.hpp"
#include "SourceControl/SourceLocation.hpp"

/* === Enum Declarations === */

enum class ErrorCategory {
    Lexical,     // tokenization
    Syntax,      // parsing
    Semantic,    // meaning / type checking
    Internal,    // compiler bug / assertion
    Linking      // Module resolution errors
};

enum class ErrorSeverity {
    Note,
    Warning,
    Error,
    Fatal
};

/* === Utility === */

inline std::string toString( ErrorCategory category )
{
    switch( category )
    {
        case ErrorCategory::Lexical:     return "Lexical";
        case ErrorCategory::Syntax:      return "Syntax";
        case ErrorCategory::Semantic:    return "Semantic";
        case ErrorCategory::Internal:    return "Internal";
        case ErrorCategory::Linking:     return "Linking"; 
        default:                         return "Uncategorised";  
    }
}

inline std::string toString( ErrorSeverity severity )
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

class ModuleHeaderError : public std::exception
{
    public:
        std::string message;
        ErrorSeverity severity;
        ModuleHeaderError( 
            std::string message,
            ErrorSeverity severity
        ) : message( std::move( message ) ), severity( severity ) {}

        const char* what() const noexcept override {
            return message.c_str();
        }
};

class CompilerError : public std::exception 
{
    public:
        std::string message;
        ErrorSeverity severity;
        SourceRange range;
        ErrorCategory category;

        CompilerError( 
            std::string message, 
            ErrorSeverity severity,
            SourceRange range, 
            ErrorCategory category 
        )   : message( std::move(  message  ) ), 
            severity( severity ), 
            range( range ), 
            category( category ) {}
        
        const char* what() const noexcept override {
            return message.c_str();
        }
};

class RuntimeError : public std::exception 
{
    public:
        std::string message;
        ErrorSeverity severity;

        RuntimeError( std::string message, ErrorSeverity severity )
            : message( std::move(  message  ) ), severity( severity ) {};

        const char* what() const noexcept override {
            return message.c_str();
        }
};

struct SemanticError : public std::exception 
{
    std::string message;
    SourceRange location;

    SemanticError( std::string msg, SourceRange location ) : message( std::move(msg) ), location( location ) {}

    const char* what() const noexcept override 
    {
        return message.c_str();
    }
};

struct FatalCompilerError : std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct UnexpectedEndOfInputError : CompilerError {
    UnexpectedEndOfInputError(SourceRange location) 
        : CompilerError(
            "Unexpected end of input",
            ErrorSeverity::Fatal,
            location,
            ErrorCategory::Syntax
        ) {}
};

struct UnexpectedTypeError : CompilerError {
    UnexpectedTypeError(TokenKind expectedType, TokenKind actualType, SourceRange location)
        : CompilerError( 
            "Expected '" + toString( expectedType ) + "' got " + toString( actualType ),
            ErrorSeverity::Error,
            location,
            ErrorCategory::Syntax 
        ) {}
};

struct UnexpectedValueError : CompilerError {
    UnexpectedValueError( TokenSymbol expectedValue, TokenSymbol actualValue, SourceRange location)
        : CompilerError(
            "Expected '" + toString( expectedValue ) + "' got " + toString( actualValue ),
            ErrorSeverity::Error,
            location,
            ErrorCategory::Syntax
        ) {}
    UnexpectedValueError( TokenKeyword expectedValue, TokenKeyword actualValue, SourceRange location)
        : CompilerError(
            "Expected '" + toString( expectedValue ) + "' got " + toString( actualValue ),
            ErrorSeverity::Error,
            location,
            ErrorCategory::Syntax
        ) {}
};

struct ErrorVariant : public std::variant<CompilerError, SemanticError, RuntimeError>
{
    using Base = std::variant<CompilerError, SemanticError, RuntimeError>;
    using Base::Base; // inherit variant's constructors

    // Allow implicit construction from each alternative
    ErrorVariant(const CompilerError& err)    : Base(err) {}
    ErrorVariant(CompilerError&& err)         : Base(std::move(err)) {}
    ErrorVariant(const RuntimeError& err)     : Base(err) {}
    ErrorVariant(RuntimeError&& err)          : Base(std::move(err)) {}
    ErrorVariant(const SemanticError& err)     : Base(err) {}
    ErrorVariant(SemanticError&& err)          : Base(std::move(err)) {}
};