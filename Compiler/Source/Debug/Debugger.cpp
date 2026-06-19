#include <fstream>
#include "Debug/Debugger.hpp"
#include "Types/AllTypeInfo.hpp"


void Debugger::printASTTree( const std::vector<std::unique_ptr<Statement>>& statements ) const
{
    std::ofstream outFile("ast_output.json");

    outFile << "[" << std::endl;

    for ( size_t i = 0; i < statements.size(); ++i ) 
    {
        printAST( statements[i].get(), outFile, 
            1, i != statements.size() - 1 );
    }

    outFile << "]" << std::endl;

    outFile.close();
}

std::string Debugger::indentStr( size_t indent ) const
{
    return std::string( indent * 2, ' ' );
}

void Debugger::startBlock( std::ostream& out, size_t indent ) const
{
    out << indentStr( indent ) << "{" << std::endl;
}

void Debugger::endBlock( std::ostream& out, size_t indent, bool hasTrailingComma ) const
{
    out << indentStr( indent ) << "}"; 
    if ( hasTrailingComma ) out << ",";
    out << std::endl;
}

void Debugger::printLiteral( const Literal* literal, std::ostream& out, size_t indent, bool hasTrailingComma ) const
{
    startBlock( out, indent );
    out << indentStr( indent + 1 ) << "\"type\": \"Literal\"," << std::endl;
    out << indentStr( indent + 1 ) << "\"value\": " << toString( literal->value ) << ( literal->resolvedType ? "," : "" ) << std::endl;
    if (literal->resolvedType){
        out << indentStr( indent + 1 ) << "\"resolvedType\": ";
        out << "\"" << toString( *literal->resolvedType ) << "\"" << std::endl;
    }
    endBlock( out, indent, hasTrailingComma );
}

void Debugger::printIdentifier( const Identifier* id, std::ostream& out, size_t indent, bool hasTrailingComma ) const
{
    startBlock( out, indent );
    out << indentStr( indent + 1 ) << "\"type\": \"Identifier\"," << std::endl;
    out << indentStr( indent + 1 ) << "\"name\": \"" << id->name << "\"" << ( id->resolvedType ? "," : "" ) << std::endl;
    if (id->resolvedType) {
        out << indentStr( indent + 1 ) << "\"resolvedType\": ";
        out << "\"" << toString( *id->resolvedType ) << "\"" << std::endl;
    }
    endBlock( out, indent, hasTrailingComma );
}

void Debugger::printParameter( const Parameter* param, std::ostream& out, size_t indent, bool hasTrailingComma ) const
{
    startBlock( out, indent );
    out << indentStr( indent + 1 ) << "\"name\": \"" << param->name << "\"," << std::endl;
    out << indentStr( indent + 1 ) << "\"type\": " << std::endl;
    printAST( param->paramType.get(), out, indent + 2, param->resolvedType != nullptr );
    if (param->resolvedType) {
        out << indentStr( indent + 1 ) << "\"resolvedType\": ";
        out <<  "\"" << toString( *param->resolvedType ) << "\"" << std::endl;
    }
    endBlock( out, indent, hasTrailingComma );
}

void Debugger::printParsedType( const ParsedType& type, std::ostream& out, size_t indent, bool hasTrailingComma ) const
{
    switch ( type.kind ) 
    {
        case Type::Function: 
        {
            startBlock( out, indent );
            out << indentStr( indent + 1 ) << "\"type\": \"" + toString( type.kind ) + "\"," << std::endl;
            out << indentStr( indent + 1 ) << "\"returnType\": " << std::endl;
            printAST( type.inner.get(), out, indent + 2, true );
            out << indentStr( indent + 1 ) << "\"parameters\": [";
            if ( !type.typeArgs.empty() ) 
            {
                out << std::endl;

                for ( size_t i = 0; i < type.typeArgs.size(); ++i ) 
                {
                    printAST( type.typeArgs[i].get(), out, 
                        indent + 2, i != type.typeArgs.size() - 1 );
                }

                out << indentStr( indent + 1 );
            }
            out << "]" << std::endl;
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case Type::Inferred: 
        {
            startBlock( out, indent );
            out << indentStr( indent + 1 ) << "\"type\": \"Inferred\"," << std::endl;
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case Type::Ownership: 
        {
            startBlock(out, indent );
            out << indentStr( indent + 1 ) << "\"type\": \"" + toString( type.kind ) + "\"," << std::endl;
            out << indentStr( indent + 1 ) << "\"ownership\": \"" << toString( type.ownership ) + "\"," << std::endl;
            out << indentStr( indent + 1 ) << "\"inner\": " << std::endl;
            printAST( type.inner.get(), out, indent + 2 );
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case Type::Generic: 
        {
            startBlock( out, indent );
            out << indentStr( indent + 1 ) << "\"type\": \"Reference\"," << std::endl;
            out << indentStr( indent + 1 ) << "\"name\": \"" << type.identifier->name << "\"," << std::endl;
            out << indentStr( indent + 1 ) << "\"arguments\": [";
            if ( !type.typeArgs.empty() ) 
            {
                out << std::endl;

                for ( size_t i = 0; i < type.typeArgs.size(); ++i ) 
                {
                    printAST( type.typeArgs[i].get(), out, 
                        indent + 2, i != type.typeArgs.size() - 1 );
                }

                out << indentStr( indent + 1 );
            }
            out << "]" << std::endl; 
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        default: 
        {
            out << indentStr( indent ) << "\"Unknown Type Reference\"\n";
            break;
        }
    }
}

void Debugger::printAST( const ASTNode* node, std::ostream& out, size_t indent, bool hasTrailingComma ) const 
{
    if ( !node ) 
    {
        out << indentStr( indent ) << "null\n";
        return;
    }

    switch ( node->type() ) 
    {
        case ASTNodeType::ExpressionStatement: 
        {
            auto expstmt = static_cast<const ExpressionStatement*>( node );
            startBlock( out, indent );
            out << indentStr( indent + 1 ) << "\"type\": \"" << toString( node->type() ) << "\"," << std::endl;
            out << indentStr( indent + 1 ) << "\"expression\": " << std::endl;
            printAST( expstmt->expression.get(), out, indent + 2 );
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
            out << indentStr( indent + 1 ) << "\"type\": \"" << toString( node->type() ) << "\"," << std::endl;
            out << indentStr( indent + 1 ) << "\"statements\": [";
            if ( !block->statements.empty() ) 
            {
                out << std::endl;
                for ( size_t i = 0; i < block->statements.size(); ++i ) 
                {
                    printAST( block->statements[i].get(), out, indent + 2, i != block->statements.size() - 1 );
                }
                out << indentStr( indent + 1 );
            }
            out << "]" << std::endl;
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case ASTNodeType::Break: 
        {
            startBlock( out, indent );
            out << indentStr( indent + 1 ) << "\"type\": \"" << toString( node->type() ) << "\"" << std::endl;
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case ASTNodeType::Continue: 
        {
            startBlock( out, indent );
            out << indentStr( indent + 1 ) << "\"type\": \"" << toString( node->type() ) << "\"" << std::endl;
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case ASTNodeType::Return: 
        {
            auto ret = static_cast<const Return*>( node );
            startBlock( out, indent );
            out << indentStr( indent + 1 ) << "\"type\": \"" << toString( node->type() ) << "\"," << std::endl;
            out << indentStr( indent + 1 ) << "\"value\": " << std::endl;
            printAST( ret->value.get(), out, indent + 2, ret->resolvedType != nullptr );
            if (ret->resolvedType) {
                out << indentStr( indent + 1 ) << "\"resolvedType\": ";
                out <<  "\"" << toString( *ret->resolvedType ) << "\"" << std::endl;
            }
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case ASTNodeType::Unary: 
        {
            auto un = static_cast<const Unary*>( node );
            startBlock( out, indent );
            out << indentStr( indent + 1 ) << "\"type\": \"" << toString( node->type() ) << "\"," << std::endl;
            out << indentStr( indent + 1 ) << "\"op\": \"" << toString( un->op ) << "\"," << std::endl;
            out << indentStr( indent + 1 ) << "\"argument\": " << std::endl;
            printAST( un->argument.get(), out, indent + 2 );
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case ASTNodeType::VariableDeclaration:
        {
            auto decl = static_cast<const VariableDeclaration*>( node );
            startBlock( out, indent );
            out << indentStr( indent + 1 ) << "\"type\": \"" << toString( node->type() ) << "\"," << std::endl;
            out << indentStr( indent + 1 ) << "\"locked\": " << ( decl->locked ? "true," : "false," ) << std::endl;
            out << indentStr( indent + 1 ) << "\"varType\": " << std::endl;
            printAST( decl->varType.get(), out, indent + 2, true );
            out << indentStr( indent + 1 ) << "\"identifier\": " << std::endl;
            printAST( decl->identifier.get(), out, indent + 2, true );
            out << indentStr( indent + 1 ) << "\"initialiser\": " << std::endl;
            printAST( decl->initialiser.get(), out, indent + 2 );
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case ASTNodeType::Assignment: 
        {
            auto assign = static_cast<const Assignment*>( node );
            startBlock( out, indent );
            out << indentStr( indent + 1 ) << "\"type\": \"" << toString( node->type() ) << "\"," << std::endl;
            out << indentStr( indent + 1 ) << "\"identifier\": " << std::endl;
            printAST( assign->identifier.get(), out, indent + 2, true );
            out << indentStr( indent + 1 ) << "\"value\": " << std::endl;
            printAST( assign->value.get(), out, indent + 2 );
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case ASTNodeType::FunctionDeclaration: 
        {
            auto func = static_cast<const FunctionDeclaration*>( node );
            startBlock( out, indent );
            out << indentStr( indent + 1 ) << "\"type\": \"" << toString( node->type() ) << "\"," << std::endl;
            out << indentStr( indent + 1 ) << "\"returnType\": " << std::endl; 
            printAST( func->returnType.get(), out, indent + 2, true );
            out << indentStr( indent + 1 ) << "\"parameters\": [";
            if ( !func->parameters.empty() ) 
            {
                out << std::endl;

                for ( size_t i = 0; i < func->parameters.size(); ++i ) 
                {
                    printParameter( func->parameters[i].get(), out, 
                        indent + 2, i != func->parameters.size() - 1 );
                }

                out << indentStr( indent + 1 );
            }
            out << "]," << std::endl;
            out << indentStr( indent + 1 ) << "\"body\": " << std::endl;
            printAST( func->body.get(), out, indent + 2 );
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case ASTNodeType::FunctionLiteral: 
        {
            auto func = static_cast<const FunctionLiteral*>( node );
            startBlock( out, indent );
            out << indentStr( indent + 1 ) << "\"type\": \"" << toString( node->type() ) << "\"," << std::endl;
            out << indentStr( indent + 1 ) << "\"returnType\": " << std::endl;
            printAST( func->returnType.get(), out, indent + 2, true );
            out << indentStr( indent + 1 ) << "\"parameters\": [";
            if ( !func->parameters.empty() ) 
            {
                out << std::endl;

                for ( size_t i = 0; i < func->parameters.size(); ++i ) 
                {
                    printParameter( func->parameters[i].get(), out, 
                        indent + 2, i != func->parameters.size() - 1 );
                }

                out << indentStr( indent + 1 );
            }
            out << "]," << std::endl;
            out << indentStr( indent + 1 ) << "\"body\": " << std::endl;
            printAST( func->body.get(), out, indent + 2 );
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case ASTNodeType::IfConditional: 
        {
            auto ifs = static_cast<const IfConditional*>( node );
            startBlock( out, indent );
            out << indentStr( indent + 1 ) << "\"type\": \"" << toString( node->type() ) << "\"," << std::endl;
            out << indentStr( indent + 1 ) << "\"condition\": " << std::endl;
            printAST( ifs->condition.get(), out, indent + 2, true );
            out << indentStr( indent + 1 ) << "\"then\": " << std::endl;
            printAST( ifs->then.get(), out, indent + 2, true );
            out << indentStr( indent + 1 ) << "\"else\": " << std::endl;
            printAST( ifs->elseStatement.get(), out, indent + 2 );
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case ASTNodeType::Range: 
        {
            auto rng = static_cast<const Range*>( node );
            startBlock( out, indent );
            out << indentStr( indent + 1 ) << "\"type\": \"" << toString( node->type() ) << "\"," << std::endl;
            out << indentStr( indent + 1 ) << "\"start\": " << std::endl;
            printAST( rng->start.get(), out, indent + 2, true );
            out << indentStr( indent + 1 ) << "\"end\": " << std::endl;
            printAST( rng->end.get(), out, indent + 2, true );
            out << indentStr( indent + 1 ) << "\"inclusive\": " << ( rng->inclusive ? "true" : "false" ) << std::endl;
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case ASTNodeType::ForLoop: 
        {
            auto forl = static_cast<const ForLoop*>( node );
            startBlock( out, indent );
            out << indentStr( indent + 1 ) << "\"type\": \"" << toString( node->type() ) << "\"," << std::endl;
            out << indentStr( indent + 1 ) << "\"loopVar\": " << std::endl;
            printAST( forl->loopVar.get(), out, indent + 2, true );
            out << indentStr( indent + 1 ) << "\"iterable\": " << std::endl;
            printAST( forl->iterable.get(), out, indent + 2, true );
            out << indentStr( indent + 1 ) << "\"step\": " << std::endl;
            printAST( forl->step.get(), out, indent + 2, true );
            out << indentStr( indent + 1 ) << "\"condition\": " << std::endl;
            printAST( forl->condition.get(), out, indent + 2, true );
            out << indentStr( indent + 1 ) << "\"body\": " << std::endl;
            printAST( forl->body.get(), out, indent + 2 );
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case ASTNodeType::BinaryOperation: 
        {
            auto* bin = static_cast<const BinaryOperation*>( node );
            startBlock( out, indent );
            out << indentStr( indent + 1 ) << "\"type\": \"" << toString( node->type() ) << "\"," << std::endl;
            out << indentStr( indent + 1 ) << "\"op\": \"" << toString( bin->op ) << "\"," << std::endl;
            out << indentStr( indent + 1 ) << "\"left\": " << std::endl;
            printAST( bin->left.get(), out, indent + 2, true );
            out << indentStr( indent + 1 ) << "\"right\": " << std::endl;
            printAST( bin->right.get(), out, indent + 2 );
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        case ASTNodeType::ParsedType: 
        {
            auto* tref = static_cast<const ParsedType*>( node );
            printParsedType( *( tref ), out, indent + 2, hasTrailingComma );
            break;
        }

        case ASTNodeType::FunctionCall: 
        {
            auto callExpr = static_cast<const FunctionCall*>( node );
            startBlock( out, indent );
            out << indentStr( indent + 1 ) << "\"type\": \"" << toString( node->type() ) << "\"," << std::endl;
            out << indentStr( indent + 1 ) << "\"callee\": " << std::endl;
            printAST( callExpr->callee.get(), out, indent + 2, true );
            out << indentStr( indent + 1 ) << "\"params\": [";
            if ( !callExpr->params.empty() ) 
            {
                out << std::endl;

                for ( size_t i = 0; i < callExpr->params.size(); ++i ) 
                {
                    printAST( callExpr->params[i].get(), out, 
                        indent + 2, i != callExpr->params.size() - 1 );
                }

                out << indentStr( indent + 1 );
            }
            out << "]" << std::endl;
            endBlock( out, indent, hasTrailingComma );
            break;
        }

        default: 
        {
            out << indentStr( indent ) << "\"Unknown AST Node\"\n";
            break;
        }
    }
}
