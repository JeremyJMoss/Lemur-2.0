#pragma once

/* === Dependencies === */

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <expected>

/* === Imports === */

#include "Tokens/Token.hpp"
#include "Tokens/TokenKeyword.hpp"
#include "Tokens/TokenKind.hpp"
#include "Tokens/TokenSymbol.hpp"
#include "SourceControl/SourceLocation.hpp"
#include "SourceControl/SourceManager.hpp"
#include "Errors/Errors.hpp"
#include "Modules/ModuleHeader.hpp"
#include "Core/Ids.hpp"

/* === Forward Declarations === */

class CompilationUnit;

/* === Tokenizer === */

class Tokenizer
{
    public:
        Tokenizer( CompilationUnit& compUnit ) 
            : m_compUnit( compUnit ) {}
        void tokenizeStream( std::istream& stream );
        static std::expected<std::string, Diagnostic> parseModuleName( std::string_view input );
        void checkIssueWithOutput( FileId fileId );

    private:
        static bool isWhitespaceChar( char c );
        static bool isDigitChar( char c );
        static bool isIdentifierStartChar( char c );
        static bool isIdentifierPartChar( char c );

        static std::string_view readIdentifier( std::string_view line, std::size_t& pos );
        static std::string_view readNumber( std::string_view line, std::size_t& pos );
        static std::string_view readSymbol( std::string_view line, std::size_t& pos, const std::unordered_map<std::string_view, TokenSymbol>& symbols );

        static bool isValidIdentifier( std::string_view s );

        inline const static std::unordered_map<std::string_view, TokenKeyword> m_KEYWORDS = {
            { "lock",      TokenKeyword::Lock },
            { "fn",        TokenKeyword::Fn },
            { "return",    TokenKeyword::Return },
            { "if",        TokenKeyword::If },
            { "else",      TokenKeyword::Else },
            { "for",       TokenKeyword::For },
            { "until",     TokenKeyword::Until },
            { "to",        TokenKeyword::To },
            { "step",      TokenKeyword::Step },
            { "where",     TokenKeyword::Where },
            { "break",     TokenKeyword::Break },
            { "continue",  TokenKeyword::Continue },
            { "infer",     TokenKeyword::Infer },
            { "null",      TokenKeyword::Null },
            { "wref",      TokenKeyword::Wref },
            { "owned",     TokenKeyword::Owned },
            { "shared",    TokenKeyword::Shared },
            { "rref",      TokenKeyword::Rref },
            { "weak",      TokenKeyword::Weak },
            { "in",        TokenKeyword::In },
            { "entry",     TokenKeyword::Entry },
            { "module",    TokenKeyword::Module },
            { "import",    TokenKeyword::Import },
            { "from",      TokenKeyword::From },
            { "as",        TokenKeyword::As },
            { "export",    TokenKeyword::Export }
        };

        inline const static std::unordered_map<std::string_view, TokenSymbol> m_SYMBOLS = {
            { "(", TokenSymbol::LParens },
            { ")", TokenSymbol::RParens },
            { "[", TokenSymbol::LBracket },
            { "]", TokenSymbol::RBracket },
            { "{", TokenSymbol::LBrace },
            { "}", TokenSymbol::RBrace },
            { ".", TokenSymbol::Dot },
            { ",", TokenSymbol::Comma },
            { "==", TokenSymbol::Equals },
            { ">", TokenSymbol::Greater },
            { ">=", TokenSymbol::GreaterEquals },
            { "<=", TokenSymbol::LessEquals },
            { "<", TokenSymbol::Less },
            { "!", TokenSymbol::Not },
            { "!=", TokenSymbol::NotEquals },
            { "=", TokenSymbol::Assign },
            { "%", TokenSymbol::Percent },
            { "+", TokenSymbol::Plus },
            { "-", TokenSymbol::Minus },
            { "*", TokenSymbol::Star },
            { "/", TokenSymbol::Slash },
            { "||", TokenSymbol::Or },
            { "&&", TokenSymbol::And },
            { ";", TokenSymbol::SemiColon },
            { ":", TokenSymbol::Colon },
            { "->", TokenSymbol::Arrow }
        };

        inline const static std::unordered_set<std::string_view> m_BOOLEANS = {
            "true", "false"
        };

        Token m_partialToken = {};
        CompilationUnit& m_compUnit;
        bool m_inToken = false;
        std::size_t m_lineNum = 0;
        std::size_t m_lastLineLength = 0;
        void setPartialToken( TokenKind type, std::string_view value, std::size_t start_line, std::size_t start_pos );
        void appendPartialToken( std::string_view amendment, std::size_t line, std::size_t pos );
        void clearPartialToken();
        static TokenKind getTokenType( std::string_view value );
};