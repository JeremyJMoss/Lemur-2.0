#include <fstream>
#include "Debug/Debugger.hpp"

void Debugger::printASTTree( const std::vector<Statement*>& statements ) const
{
    std::ofstream outFile("ast_output.json", std::ios::out | std::ios::binary);
    outFile.rdbuf()->pubsetbuf(nullptr, 1 << 20); // 1MB buffer

    outFile << "[" << '\n';

    for ( std::size_t i = 0; i < statements.size(); ++i ) 
    {
        printAST( statements[i], outFile, 
            1, i != statements.size() - 1 );
    }

    outFile << "]" << '\n';

    outFile.close();
}



void Debugger::printLiteral( const Literal* literal, std::ostream& out, std::size_t indent, bool hasTrailingComma ) const
{
    startBlock( out, indent );
    writeIndent( out, indent + 1 );
    out << "\"id\": \"" << literal->id << "\"," << '\n';
    writeIndent( out, indent + 1 );
    out << "\"type\": \"Literal\"," << '\n';
    writeIndent( out, indent + 1 );
    out << "\"value\": " << toString( literal->value ) << '\n';
    endBlock( out, indent, hasTrailingComma );
}

void Debugger::printIdentifier( const Identifier* identifier, std::ostream& out, std::size_t indent, bool hasTrailingComma ) const
{
    startBlock( out, indent );
    writeIndent( out, indent + 1 );
    out << "\"id\": \"" << identifier->id << "\"," << '\n';
    writeIndent( out, indent + 1 );
    out << "\"type\": \"Identifier\"," << '\n';
    writeIndent( out, indent + 1 ); 
    out << "\"name\": \"" << identifier->name << "\"" << '\n';
    endBlock( out, indent, hasTrailingComma );
}

void Debugger::printParameter( const Parameter* param, std::ostream& out, std::size_t indent, bool hasTrailingComma ) const
{
    startBlock( out, indent );
    writeIndent( out, indent + 1 );
    out << "\"id\": \"" << param->id << "\"," << '\n';
    writeIndent( out, indent + 1 );
    out << "\"type\": \"Parameter\"," << '\n';
    writeIndent( out, indent + 1 );
    out << "\"identifier\": " << '\n';
    printAST( param->identifier, out, indent + 2, true );
    writeIndent( out, indent + 1 );
    out << "\"paramType\": " << '\n';
    printAST( param->paramType, out, indent + 2, true );
    writeIndent( out, indent + 1 );
    out << "\"defaultValue\": " << '\n';
    printAST( param->defaultValue, out, indent + 2, false );
    endBlock( out, indent, hasTrailingComma );
}

void Debugger::printParsedType( const ParsedType* type, std::ostream& out, std::size_t indent, bool hasTrailingComma ) const
{
    switch ( type->kind ) 
    {
        case ParsedTypeKind::Function: 
        {
            auto functionType = static_cast<const ParsedFunctionType*>( type );
            startBlock( out, indent );
            writeIndent( out, indent + 1 );
            out << "\"id\": \"" << functionType->id << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"type\": \"" + toString( functionType->kind ) + "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"returnType\": " << '\n';
            printAST( functionType->returnType, out, indent + 2, true );
            writeIndent( out, indent + 1 );
            out << "\"parameters\": [";
            if ( !functionType->parameters.empty() ) 
            {
                out << '\n';

                for ( std::size_t i = 0; i < functionType->parameters.size(); ++i ) 
                {
                    printAST( functionType->parameters[i], out, 
                        indent + 2, i != functionType->parameters.size() - 1 );
                }

                writeIndent( out, indent + 1 );
            }
            out << "]" << '\n';
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case ParsedTypeKind::Inferred: 
        {
            auto inferredType = static_cast<const ParsedInferredType*>( type );
            startBlock( out, indent );
            writeIndent( out, indent + 1 );
            out << "\"id\": \"" << inferredType->id << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"type\": \"Inferred\"" << '\n';
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case ParsedTypeKind::Ownership: 
        {
            auto ownershipType = static_cast<const ParsedOwnershipType*>( type );
            startBlock(out, indent );
            writeIndent( out, indent + 1 );
            out << "\"id\": \"" << ownershipType->id << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"type\": \"" + toString( ownershipType->kind ) + "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"ownership\": \"" << toString( ownershipType->ownership ) + "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"inner\": " << '\n';
            printAST( ownershipType->inner, out, indent + 2 );
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case ParsedTypeKind::Array: 
        {
            auto arrayType = static_cast<const ParsedArrayType*>( type );
            startBlock( out, indent );
            writeIndent( out, indent + 1 );
            out << "\"id\": \"" << arrayType->id << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"type\": \"Array\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"elementType\":" << '\n';
            printAST( arrayType->elementType, out, indent + 2, true );
            writeIndent( out, indent + 1 );
            out << "\"arraySize\":" << '\n';
            printAST( arrayType->size, out, indent + 2 );
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case ParsedTypeKind::Named:
        {
            auto namedType = static_cast<const ParsedNamedType*>( type );
            startBlock( out, indent );
            writeIndent( out, indent + 1 );
            out << "\"id\": \"" << namedType->id << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"type\": \"Named\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"identifier\":" << '\n';
            printAST( namedType->identifier, out, indent + 2 );
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        default: 
        {
            writeIndent( out, indent );
            out << "\"Unknown Type Reference\"\n";
            break;
        }
    }
}

void Debugger::printAST( const ASTNode* node, std::ostream& out, std::size_t indent, bool hasTrailingComma ) const 
{
    if ( !node ) 
    {
        writeIndent( out, indent );
        out << "null\n";
        return;
    }

    switch ( node->type() ) 
    {
        case ASTNodeType::ExpressionStatement: 
        {
            auto expstmt = static_cast<const ExpressionStatement*>( node );
            startBlock( out, indent );
            writeIndent( out, indent + 1 );
            out << "\"id\": \"" << node->id << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"type\": \"" << toString( node->type() ) << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"expression\": " << '\n';
            printAST( expstmt->expression, out, indent + 2 );
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case ASTNodeType::Literal: 
        {
            printLiteral( static_cast<const Literal*>( node ), out, indent, hasTrailingComma );
            break;
        }

        case ASTNodeType::Identifier: 
        {
            printIdentifier( static_cast<const Identifier*>( node ), out, indent, hasTrailingComma );
            break;
        }

        case ASTNodeType::Block: 
        {
            auto block = static_cast<const Block*>( node );
            startBlock( out, indent );
            writeIndent( out, indent + 1 );
            out << "\"id\": \"" << node->id << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"type\": \"" << toString( node->type() ) << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"statements\": [";
            if ( !block->statements.empty() ) 
            {
                out << '\n';
                for ( std::size_t i = 0; i < block->statements.size(); ++i ) 
                {
                    printAST( block->statements[i], out, indent + 2, i != block->statements.size() - 1 );
                }
                writeIndent( out, indent + 1 );
            }
            out << "]" << '\n';
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case ASTNodeType::Break: 
        {
            startBlock( out, indent );
            writeIndent( out, indent + 1 );
            out << "\"id\": \"" << node->id << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"type\": \"" << toString( node->type() ) << "\"" << '\n';
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case ASTNodeType::Continue: 
        {
            startBlock( out, indent );
            writeIndent( out, indent + 1 );
            out << "\"id\": \"" << node->id << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"type\": \"" << toString( node->type() ) << "\"" << '\n';
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case ASTNodeType::Return: 
        {
            auto ret = static_cast<const Return*>( node );
            startBlock( out, indent );
            writeIndent( out, indent + 1 );
            out << "\"id\": \"" << node->id << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"type\": \"" << toString( node->type() ) << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"value\": " << '\n';
            printAST( ret->value, out, indent + 2 );
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case ASTNodeType::Unary: 
        {
            auto un = static_cast<const Unary*>( node );
            startBlock( out, indent );
            writeIndent( out, indent + 1 );
            out << "\"id\": \"" << node->id << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"type\": \"" << toString( node->type() ) << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"op\": \"" << toString( un->op ) << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"argument\": " << '\n';
            printAST( un->argument, out, indent + 2 );
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case ASTNodeType::VariableDeclaration:
        {
            auto decl = static_cast<const VariableDeclaration*>( node );
            startBlock( out, indent );
            writeIndent( out, indent + 1 );
            out << "\"id\": \"" << node->id << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"type\": \"" << toString( node->type() ) << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"locked\": " << ( decl->locked ? "true," : "false," ) << '\n';
            writeIndent( out, indent + 1 );
            out << "\"varType\": " << '\n';
            printAST( decl->varType, out, indent + 2, true );
            writeIndent( out, indent + 1 );
            out << "\"identifier\": " << '\n';
            printAST( decl->identifier, out, indent + 2, true );
            writeIndent( out, indent + 1 );
            out << "\"initialiser\": " << '\n';
            printAST( decl->initialiser, out, indent + 2 );
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case ASTNodeType::Assignment: 
        {
            auto assign = static_cast<const Assignment*>( node );
            startBlock( out, indent );
            writeIndent( out, indent + 1 );
            out << "\"id\": \"" << node->id << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"type\": \"" << toString( node->type() ) << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"identifier\": " << '\n';
            printAST( assign->identifier, out, indent + 2, true );
            writeIndent( out, indent + 1 );
            out << "\"value\": " << '\n';
            printAST( assign->value, out, indent + 2 );
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case ASTNodeType::FunctionDeclaration: 
        {
            auto func = static_cast<const FunctionDeclaration*>( node );
            startBlock( out, indent );
            writeIndent( out, indent + 1 );
            out << "\"id\": \"" << node->id << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"type\": \"" << toString( node->type() ) << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"returnType\": " << '\n'; 
            printAST( func->returnType, out, indent + 2, true );
            writeIndent( out, indent + 1 );
            out << "\"parameters\": [";
            if ( !func->parameters.empty() ) 
            {
                out << '\n';

                for ( std::size_t i = 0; i < func->parameters.size(); ++i ) 
                {
                    printParameter( func->parameters[i], out, 
                        indent + 2, i != func->parameters.size() - 1 );
                }

                writeIndent( out, indent + 1 );
            }
            out << "]," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"body\": " << '\n';
            printAST( func->body, out, indent + 2 );
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case ASTNodeType::FunctionLiteral: 
        {
            auto func = static_cast<const FunctionLiteral*>( node );
            startBlock( out, indent );
            writeIndent( out, indent + 1 );
            out << "\"id\": \"" << node->id << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"type\": \"" << toString( node->type() ) << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"returnType\": " << '\n';
            printAST( func->returnType, out, indent + 2, true );
            writeIndent( out, indent + 1 );
            out << "\"parameters\": [";
            if ( !func->parameters.empty() ) 
            {
                out << '\n';

                for ( std::size_t i = 0; i < func->parameters.size(); ++i ) 
                {
                    printParameter( func->parameters[i], out, 
                        indent + 2, i != func->parameters.size() - 1 );
                }

                writeIndent( out, indent + 1 );
            }
            out << "]," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"body\": " << '\n';
            printAST( func->body, out, indent + 2 );
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case ASTNodeType::IfConditional: 
        {
            auto ifs = static_cast<const IfConditional*>( node );
            startBlock( out, indent );
            writeIndent( out, indent + 1 );
            out << "\"id\": \"" << node->id << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"type\": \"" << toString( node->type() ) << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"condition\": " << '\n';
            printAST( ifs->condition, out, indent + 2, true );
            writeIndent( out, indent + 1 );
            out << "\"then\": " << '\n';
            printAST( ifs->then, out, indent + 2, true );
            writeIndent( out, indent + 1 );
            out << "\"else\": " << '\n';
            printAST( ifs->elseStatement, out, indent + 2 );
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case ASTNodeType::Range: 
        {
            auto rng = static_cast<const Range*>( node );
            startBlock( out, indent );
            writeIndent( out, indent + 1 );
            out << "\"id\": \"" << node->id << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"type\": \"" << toString( node->type() ) << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"start\": " << '\n';
            printAST( rng->start, out, indent + 2, true );
            writeIndent( out, indent + 1 );
            out << "\"end\": " << '\n';
            printAST( rng->end, out, indent + 2, true );
            writeIndent( out, indent + 1 );
            out << "\"inclusive\": " << ( rng->inclusive ? "true" : "false" ) << '\n';
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case ASTNodeType::ForLoop: 
        {
            auto forl = static_cast<const ForLoop*>( node );
            startBlock( out, indent );
            writeIndent( out, indent + 1 );
            out << "\"id\": \"" << node->id << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"type\": \"" << toString( node->type() ) << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"loopVar\": " << '\n';
            printAST( forl->loopVar, out, indent + 2, true );
            writeIndent( out, indent + 1 );
            out << "\"iterable\": " << '\n';
            printAST( forl->iterable, out, indent + 2, true );
            writeIndent( out, indent + 1 );
            out << "\"step\": " << '\n';
            printAST( forl->step, out, indent + 2, true );
            writeIndent( out, indent + 1 );
            out << "\"condition\": " << '\n';
            printAST( forl->condition, out, indent + 2, true );
            writeIndent( out, indent + 1 );
            out << "\"body\": " << '\n';
            printAST( forl->body, out, indent + 2 );
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case ASTNodeType::BinaryExpression: 
        {
            auto* bin = static_cast<const BinaryExpression*>( node );
            startBlock( out, indent );
            writeIndent( out, indent + 1 );
            out << "\"id\": \"" << node->id << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"type\": \"" << toString( node->type() ) << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"op\": \"" << toString( bin->op ) << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"left\": " << '\n';
            printAST( bin->left, out, indent + 2, true );
            writeIndent( out, indent + 1 );
            out << "\"right\": " << '\n';
            printAST( bin->right, out, indent + 2 );
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case ASTNodeType::ParsedType: 
        {
            auto* tref = static_cast<const ParsedType*>( node );
            printParsedType( tref, out, indent, hasTrailingComma );
            break;
        }

        case ASTNodeType::FunctionCall: 
        {
            auto callExpr = static_cast<const FunctionCall*>( node );
            startBlock( out, indent );
            writeIndent( out, indent + 1 );
            out << "\"id\": \"" << node->id << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"type\": \"" << toString( node->type() ) << "\"," << '\n';
            writeIndent( out, indent + 1 );
            out << "\"callee\": " << '\n';
            printAST( callExpr->callee, out, indent + 2, true );
            writeIndent( out, indent + 1 );
            out << "\"arguments\": [";
            if ( !callExpr->arguments.empty() ) 
            {
                out << '\n';

                for ( std::size_t i = 0; i < callExpr->arguments.size(); ++i ) 
                {
                    printAST( callExpr->arguments[i], out, 
                        indent + 2, i != callExpr->arguments.size() - 1 );
                }

                writeIndent( out, indent + 1 );
            }
            out << "]" << '\n';
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        default: 
        {
            writeIndent( out, indent );
            out << "\"Unknown AST Node\"\n";
            break;
        }
    }
}
