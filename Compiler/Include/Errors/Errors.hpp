#pragma once

#include <exception>
#include <string>
#include <iostream>
#include <algorithm>
#include <tuple>
#include "Tokens/Token.hpp"
#include "AST/ASTNode.hpp"
#include "Utils/SourceLocation.hpp"

struct SemanticError : public std::exception 
{
    std::string message;
    SourceRange location;

    SemanticError( const std::string& msg, SourceRange location ) : message( msg ), location( location ) {}

    const char* what() const noexcept override 
    {
        return message.c_str();
    }
};

struct ModuleError : public std::exception
{
    std::string message;

    ModuleError( const std::string& msg ) : message( msg) {}

    const char* what() const noexcept override
    {
        return message.c_str();
    }
};

struct FatalCompilerError : std::runtime_error {
    using std::runtime_error::runtime_error;
};