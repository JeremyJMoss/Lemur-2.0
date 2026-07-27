#pragma once

/* === Dependencies === */

#include <ostream>
#include <string>
#include <vector>
#include <variant>
#include <filesystem>

namespace fs = std::filesystem;

/* === Imports === */

#include "AST/ASTNode.hpp"
#include "Debug/JsonWriter.hpp"

/* === Forward Declarations === */

struct Assignment;
struct BinaryExpression;
struct Block;
struct BlockStatement;
struct Break;
struct Continue;
struct ExpressionStatement;
struct ForLoop;
struct FunctionCall;
struct FunctionDeclaration;
struct FunctionLiteral;
struct Identifier;
struct IfConditional;
struct Literal;
struct Parameter;
struct ParsedType;
struct Range;
struct Return;
struct Unary;
struct VariableDeclaration;
struct ModuleDeclaration;
struct Import;

enum class BinaryOperator : std::uint8_t;
enum class OwnershipKind : std::uint8_t;
enum class ParsedTypeKind : std::uint8_t;
enum class UnaryOperator : std::uint8_t;

using LiteralValue = std::variant<
    std::string, 
    char, 
    int, 
    float, 
    bool, 
    std::monostate
>;

/* === AST Json Writer === */

class ASTJsonWriter : public ASTVisitor {
    public:
        ASTJsonWriter( JsonWriter& jsonWriter )
            : m_writer( jsonWriter ) {}

        void write( const std::vector<const Statement*>& statements, const fs::path& outputPath, const std::string_view moduleName );

        static std::string getBinaryOperator( BinaryOperator op );

        static std::string getUnaryOperator( UnaryOperator unop );

        static std::string getParsedType( const ParsedTypeKind& type );

        static std::string getOwnershipKind( const OwnershipKind kind );

        static std::string getLiteralValue( const LiteralValue& value );
    private:
        JsonWriter& m_writer;

        /* === Visitors for each AST Node === */
        void visit( const Literal& ) override;
        void visit( const Assignment& assignment ) override;
        void visit( const Identifier& ) override;
        void visit( const ExpressionStatement& expressionStmt ) override;
        void visit( const BinaryExpression& binExp ) override;
        void visit( const Block& block ) override;
        void visit( const BlockStatement& blockStmt ) override;
        void visit( const Break& ) override;
        void visit( const Continue& ) override;
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
        void visit( const ParsedType& ) override;
        void visit( const ModuleDeclaration& ) override;
        void visit( const ImportedSymbol& ) override;
        void visit( const Import& ) override;
        void visit( const QualifiedName& ) override;

        /// @brief Writes a named AST node field in JSON format
        /// @param label label JSON field name.
        /// @param node AST Node to print
        /// @param hasComma whether a trailing comma is required after the field
        void writeNodeField( std::string_view label, const ASTNode& node, bool hasComma = true );

        /// @brief Converts a char value to it's JSON formatted textual representation for printing.
        ///
        /// @param c char to convert to string
        /// @return the char as a string representation for JSON
        static std::string getCharToString( char c );

    
        /// @brief Removes insignificant trailing zeros from a floating-point literal string for JSON formatting.
        ///
        /// @param str string of float to remove trailing zeros from
        /// @return newly constructed string with trailing zeros removed
        static std::string trimTrailingZeros( std::string_view str );
};