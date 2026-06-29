#pragma once

#include <ostream>
#include "AST/AllASTTypes.hpp"

class ASTPrinter : public ASTVisitor {
    public:
        void print( const std::vector<const Statement*>& statements );
        
        // Visitors for each AST Node
        void visit( const Literal& lit ) override;
        void visit( const Assignment& assignment ) override;
        void visit( const Identifier& identifier ) override;
        void visit( const ExpressionStatement& expressionStmt ) override; 

    private:
        std::ostream* m_out;
        int64_t m_indent = 0;

        void writeIndent() const;

        void startBlock() const;

        void endBlock() const;

        void increaseIndent();

        void decreaseIndent();

        template <typename T>
        void writeField( const std::string& label, const T& value, bool hasComma = true ) {
            writeIndent();

            *m_out << '"' << label << "\": " << value;

            if ( hasComma )
                *m_out << ',';

            *m_out << '\n';
        }

        void ASTPrinter::writeField( const std::string& label, const std::string& value, bool hasComma );

        std::string getBinaryOperator( BinaryOperators op ) const;

        static std::string getCharToString( char c );

        static std::string trimTrailingZeros( const std::string& str );

        std::string getLiteralValue( const LiteralValue& value ) const;
};