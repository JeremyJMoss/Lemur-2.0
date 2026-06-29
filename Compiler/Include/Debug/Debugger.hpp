#pragma once

/* === Imports === */

#include <iostream>
#include <vector>
#include "AST/AllASTTypes.hpp"

/* === Debugger === */

class Debugger {
    public:
        void printASTTree( const std::vector<Statement*>& statements ) const;
    private:
        void writeIndent( std::ostream& out, std::size_t indent ) const;

        void printAST( const ASTNode* node, std::ostream& out, std::size_t indent = 0, bool hasTrailingComma = false ) const;

        void startBlock( std::ostream& out, std::size_t indent ) const;

        void endBlock( std::ostream& out, std::size_t indent, bool hasTrailingComma = false ) const;

        void printLiteral( const Literal* literal, std::ostream& out, std::size_t indent, bool hasTrailingComma = false ) const;

        void printIdentifier( const Identifier* id, std::ostream& out, std::size_t indent, bool hasTrailingComma = false ) const;

        void printParameter( const Parameter* param, std::ostream& out, std::size_t indent, bool hasTrailingComma = false ) const;

        void printParsedType( const ParsedType* type, std::ostream& out, std::size_t indent, bool hasTrailingComma = false ) const;
};