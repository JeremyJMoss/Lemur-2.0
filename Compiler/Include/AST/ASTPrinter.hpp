#pragma once

#include <ostream>
#include "AST/AllASTTypes.hpp"

class ASTPrinter : public ASTVisitor {
    public:
        void visit( const Literal& lit ) override;
        void visit( const Assignment& assignment ) override;
    private:
        std::ostream* m_out;
        int64_t m_indent = 0;

        void writeIndent() const;

        void startBlock() const;

        void endBlock() const;

        std::string getBinaryOperator( BinaryOperators op ) const;

        static std::string getCharToString( char c );

        static std::string trimTrailingZeros( const std::string& str );

        std::string getLiteralValue( const LiteralValue& value ) const;
};