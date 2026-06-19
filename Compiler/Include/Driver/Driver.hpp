#pragma once

#include "Errors/ErrorReporter.hpp"
#include "Utils/SourceManager.hpp"
#include "Tokens/Tokenizer.hpp"
#include "Parser/Parser.hpp"

class Driver 
{
    public:
        Driver()
            : m_srcManager(), 
              m_errReporter( m_srcManager ), 
              m_tokenizer( 
                  m_srcManager, 
                  m_errReporter 
              ),
              m_parser( m_errReporter ) {}

        void compileProgram( std::string& filePath );

        std::expected<void, std::string> parseFile( const std::string& filePath );

    private:
        SourceManager m_srcManager;
        ErrorReporter m_errReporter;
        Tokenizer m_tokenizer;
        Parser m_parser;
};