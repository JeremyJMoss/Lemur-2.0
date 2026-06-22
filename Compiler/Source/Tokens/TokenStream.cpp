#include "Tokens/TokenStream.hpp"
#include "Utils/Logger.hpp"

/**
 * Attempts to check the next token in the token list
 * 
 * @param const std::size_t peekIndex How far ahead to look
 * @return std::expected<Token, ErrorVariant> Token in the position from the current position in the token list specified 
 * by peekIndex returns Error otherwise if could not retrieve token
 */
Token TokenStream::peek( const std::size_t offset ) const
{
    size_t index = m_pos + offset;

    // clamp to EOF instead of failing
    if (index >= m_tokens.size())
        return m_tokens.back(); // EOF

    return m_tokens[index];
}

/**
 * Moves the pointer ahead to the next in the token list and returns that token if
 * the token matches the expectedType
 * 
 * @param const std::string& expectedType  Reference to the type that is to be consumed
 * @returns Token if matches expectedType
 * @throws std::runtime_error If reached end of input tokens
 * @throws ParseError If expected type does not match type of current token
 */
Token TokenStream::consume()
{
    m_pos++;

    return m_tokens[m_pos];
}

void TokenStream::reset()
{
    m_pos = 0;
    m_tokens = {};
}