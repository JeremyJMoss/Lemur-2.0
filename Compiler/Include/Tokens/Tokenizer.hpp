#pragma once
#include <vector>
#include <string>
#include <regex>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include "Tokens/Token.hpp"
#include "Utils/SourceLocation.hpp"
#include "Utils/SourceManager.hpp"
#include "Errors/ErrorReporter.hpp"

class Tokenizer 
{
    public:
        Tokenizer( SourceManager& sm, ErrorReporter& errReporter ) : m_srcManager( sm ), m_errReporter( errReporter ) {}
        std::vector<Token> tokenizeFile( const std::string& filePath );
        void checkIssueWithOutput( std::size_t fileId );

    private:
        static const std::regex s_WHITESPACE;
        static const std::regex s_TOKEN_PATTERN;
        static const std::regex s_RE_INTEGER;
        static const std::regex s_RE_FLOAT;
        static const std::regex s_RE_CHAR;
        static const std::regex s_RE_SYMBOL;

        const std::unordered_map<std::string_view, TokenKeyword> m_KEYWORDS = {
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
            { "mut",       TokenKeyword::Mut },
            { "own",       TokenKeyword::Own },
            { "share",     TokenKeyword::Share },
            { "view",      TokenKeyword::View },
            { "weak",      TokenKeyword::Weak },
            { "atomic",    TokenKeyword::Atomic },
            { "in",        TokenKeyword::In },
        };

        const std::unordered_map<std::string_view, TokenSymbol> m_SYMBOLS = {
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

        const std::unordered_set<std::string_view> m_BOOLEANS = {
            "true", "false"
        };

        Token m_partialToken = {};
        SourceManager& m_srcManager;
        ErrorReporter& m_errReporter;
        std::vector<Token> m_tokens;
        bool m_inToken = false;
        std::size_t m_lineNum = 0;
        void setPartialToken( TokenKind type, const std::string& value, std::size_t start_line, std::size_t start_pos );
        void appendPartialToken( const std::string& amendment, std::size_t line, std::size_t pos );
        void clearPartialToken();
        void resetState();
        bool matchRegex( const std::string& input, std::size_t pos, std::smatch& match, const std::regex& tokenPattern );
        TokenKind getTokenType( const std::string& value );
        std::vector<Token> tokenizeStream( std::istream& stream, std::size_t fileId, bool onlyHeader = false );
};