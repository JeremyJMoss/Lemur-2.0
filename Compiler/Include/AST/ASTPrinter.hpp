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
        void visit( const BinaryExpression& binExp ) override;
        void visit( const Block& block ) override;
        void visit( const Break& breakStmt ) override;
        void visit( const Continue& continueStmt ) override;
        void visit( const Return& returnStmt ) override;
        void visit( const VariableDeclaration& varDec ) override;
        void visit( const FunctionDeclaration& funDec ) override;
        void visit( const FunctionLiteral& funLit ) override;
        void visit( const IfConditional& ifCond ) override;
        void visit( const ForLoop& forl ) override;
        void visit( const Range& range ) override;
        void visit( const Unary& unary ) override;
        void visit( const FunctionCall& funCall ) override;
        void visit( const Parameter& parameter ) override;
        void visit( const ParsedType& parsedType ) override;

        static std::string getBinaryOperator( BinaryOperators op );

        static std::string getUnaryOperator( const UnaryOperator& unop );

        static std::string getParsedType( const ParsedTypeKind& type );

        static std::string getOwnershipKind( const OwnershipKind kind );

        static std::string getLiteralValue( const LiteralValue& value );

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

        template <typename T>
        void writeArrayField( const std::string& label, const std::vector<T*>& array, bool hasComma = true ) {
            writeIndent();
            *m_out << "\"" << label << "\": [";
            if ( !array.empty() ) {
                *m_out << '\n';
                increaseIndent();
                for ( std::size_t i = 0; i < array.size(); ++i ) {
                    if ( i != 0 ) {
                        *m_out << ",\n";
                    }
                    array[i]->accept(*this);
                }
                decreaseIndent();
                *m_out << "\n";
            }
            writeIndent();
            *m_out << "]";

            if ( hasComma ){
                *m_out << ',';
            }

            *m_out << '\n';
        }

        void writeNodeField( const std::string& label, const ASTNode& node, bool hasComma = true );

        void writeRawField( const std::string& label, const std::string& value, bool hasComma = true);

        void writeField( const std::string& label, const std::string& value, bool hasComma = true );

        static std::string getCharToString( char c );

        static std::string trimTrailingZeros( const std::string& str );
};