#pragma once

#include <ostream>
#include <string>
#include <vector>
#include <variant>
#include <filesystem>
#include "AST/ASTNode.hpp"

namespace fs = std::filesystem;

/* === Forward Declarations === */

struct Assignment;
struct BinaryExpression;
struct Block;
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

/* === ASTPrinter === */

class ASTPrinter : public ASTVisitor {
    public:
        /// @brief Prints the AST rooted at the supplied statements.
        ///
        /// @param statements statements to loop over to print
        void print( const std::vector<const Statement*>& statements, const fs::path& outputPath, const std::string_view moduleName );
        
        /* === Visitors for each AST Node === */
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
        void visit( const ModuleDeclaration& modDec ) override;

        /// @brief Converts a binary operator enum to its textual representation.
        /// @param op binary operator enum
        /// @return textual representation of BinaryOperator enum.
        static std::string getBinaryOperator( BinaryOperator op );

        /// @brief Converts a unary operator enum to its textual representation.
        ///
        /// @param unop unary operator enum
        /// @return textual representation of UnaryOperator enum.
        static std::string getUnaryOperator( UnaryOperator unop );

        /// @brief Converts a parsed type enum to its textual representation.
        ///
        /// @param type parsed type kind enum
        /// @return textual representation of ParsedTypeKind enum.
        static std::string getParsedType( const ParsedTypeKind& type );

        /// @brief Converts an ownership kind enum to its textual representation.
        ///
        /// @param kind ownership kind enum
        /// @return textual representation of OwnershipKind enum.
        static std::string getOwnershipKind( const OwnershipKind kind );

        /// @brief Converts a literal value variant to its textual represenstation.
        ///
        /// @param value variant value of multiple literal types
        /// @return textual representation of variant LiteralValue.
        static std::string getLiteralValue( const LiteralValue& value );
    private:
        std::ostream* m_out;
        std::int16_t m_indent = 0;

        void writeIndent() const;
        void startBlock() const;
        void endBlock() const;
        void increaseIndent();
        void decreaseIndent();

        /// @brief Print a file labelled with given label in JSON format
        /// @tparam T Type of the field value 
        /// @param label label JSON field name.
        /// @param value value t be printed alongside label
        /// @param hasComma whether a trailing comma is required after the field
        template <typename T>
        void writeField(std::string_view label, const T& value, bool hasComma = true)
        {
            writeIndent();

            *m_out << '"' << label << "\": ";

            if constexpr (std::is_same_v<T, std::string> ||
                        std::is_same_v<T, std::string_view>)
            {
                *m_out << '"' << value << '"';
            }
            else
            {
                *m_out << value;
            }

            if (hasComma)
                *m_out << ',';

            *m_out << '\n';
        }

        /// @brief Print an array of values for labeled with given label in JSON format
        /// @tparam T Type of the field value inside the vector
        /// @param label label JSON field name.
        /// @param vec Vector of T to print values of inside json array
        /// @param hasComma whether a trailing comma is required after the field
        template <typename T>
        void writeArrayField( std::string_view label, const std::vector<T*>& vec, bool hasComma = true ) {
            writeIndent();
            *m_out << "\"" << label << "\": [";
            if ( !vec.empty() ) {
                *m_out << '\n';
                increaseIndent();
                for ( std::size_t i = 0; i < vec.size(); ++i ) {
                    if ( i != 0 ) {
                        *m_out << ",\n";
                    }
                    vec[i]->accept(*this);
                }
                decreaseIndent();
                *m_out << "\n";
                writeIndent();
            }
            *m_out << "]";

            if ( hasComma ){
                *m_out << ',';
            }

            *m_out << '\n';
        }

        /// @brief Writes a named AST node field in JSON format
        /// @param label label JSON field name.
        /// @param node AST Node to print
        /// @param hasComma whether a trailing comma is required after the field
        void writeNodeField( std::string_view label, const ASTNode& node, bool hasComma = true );

        /// @brief Writes a raw field value with a label in JSON format
        /// @param label label JSON field name.
        /// @param value raw value to print
        /// @param hasComma whether a trailing comma is required after the field
        void writeRawField( std::string_view label, std::string_view value, bool hasComma = true);

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