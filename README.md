# Lemur Parser

This is a simple compiler frontend for a custom language called **Lemur**. It tokenizes and parses a `.lmur` file to build an abstract syntax tree (AST). 
This will be then compiled into assembly which will be used to create an executable.

# Project Structure

```
Lemur2/
├── Compiler/
│   ├── Build/ # CMake build directory (generated)
│   ├── Include/ # Header directory
│   │   ├── AST/
|   |   |   ├── AllASTTypes.hpp
|   |   |   ├── Assignment.hpp
|   |   |   ├── ASTNode.hpp
|   |   |   ├── BinaryExpression.hpp
|   |   |   ├── Block.hpp
|   |   |   ├── Break.hpp
|   |   |   ├── Continue.hpp
|   |   |   ├── ExpressionStatement.hpp
|   |   |   ├── ForLoop.hpp
|   |   |   ├── FunctionCall.hpp
|   |   |   ├── FunctionDeclaration.hpp
|   |   |   ├── FunctionLiteral.hpp
|   |   |   ├── Identifier.hpp
|   |   |   ├── IfConditional.hpp
|   |   |   ├── Literal.hpp
|   |   |   ├── Parameter.hpp
|   |   |   ├── Range.hpp
|   |   |   ├── Return.hpp
|   |   |   ├── Unary.hpp
|   |   |   └── VariableDeclaration.hpp
|   |   ├── DataStructures/
|   |   |   └── Stack.hpp
│   │   ├── Driver/
|   |   |   ├── CompilationUnit.hpp
│   │   |   └── Driver.cpp
|   |   ├── Errors/
|   |   |   ├── ErrorReporter.hpp
|   |   |   └── Errors.hpp
|   |   ├── Parser/
|   |   |   ├── ExpressionParser.hpp
|   |   |   ├── ParameterParser.hpp
|   |   |   ├── Parser.hpp
|   |   |   ├── ParserUtils.hpp
|   |   |   ├── StatementParser.hpp
|   |   |   └── TypeParser.hpp
|   |   ├── Scopes/
|   |   |   ├── NameBinding.hpp
|   |   |   ├── Scope.hpp
|   |   |   └── ScopeTable.hpp
|   |   ├── Symbols/
|   |   |   ├── Symbol.hpp
|   |   |   └── SymbolTable.hpp
|   |   ├── Tokens/
|   |   |   ├── Token.hpp
|   |   |   └── Tokenizer.hpp
|   |   ├── Types/
|   |   |   ├── AllTypes.hpp
|   |   |   ├── CustomType.hpp
|   |   |   ├── FunctionType.hpp
|   |   |   ├── InferredType.hpp
|   |   |   ├── NullType.hpp
|   |   |   ├── OwnershipType.hpp
|   |   |   ├── PrimitiveType.hpp
|   |   |   ├── Type.hpp
|   |   |   ├── TypeTable.hpp
|   |   |   └── UnresolvedType.hpp
|   |   ├── Utils/
|   |   |   ├── Logger.hpp
|   |   |   ├── Output.hpp
|   |   |   ├── OwnershipKind.hpp
|   |   |   ├── SourceLocation.hpp
|   |   |   └── SourceManager.hpp
│   │   └── LanguageConfig.hpp
│   ├── Source/ # Source code directory
│   │   ├── AST/
│   │   |   ├── BinaryExpression.cpp
│   │   |   └── Unary.cpp
│   │   ├── Driver/
│   │   |   └── Driver.cpp
│   │   ├── Errors/
|   |   |   └── ErrorReporter.hpp
│   │   ├── Parser/
│   │   |   ├── ExpressionParser.cpp
│   │   |   ├── ParameterParser.cpp
│   │   |   ├── Parser.cpp
│   │   |   ├── ParserUtils.cpp
│   │   |   ├── StatementParser.cpp
│   │   |   └── TypeParser.cpp
│   │   ├── Symbols/
|   |   |   └── SymbolTable.cpp
│   │   ├── Tokens/
|   |   |   ├── Token.cpp
|   |   |   └── Tokenizer.cpp
|   |   ├── Utils/
|   |   |   ├── Logger.cpp
|   |   |   └── Output.cpp
│   │   └── Run.cpp # Entry point
│   ├── Tests/ # Test source code directory
│   │   └── CMakeLists.txt
│   └── CMakeLists.txt
├── .gitignore
├── Language Semantics Specification.md # extra information about language specification
├── lemurlang.ebnf # Language grammar specification document
├── test.lemur.example # Example file for lemur source code
└── README.md
```

## Getting Started

### 1. Prerequisites

- Ensure you have CMake installed.

- You'll also need a c++ compiler that supports C++23

## 2. Setup

- Clone the repo

- Add a test.lmur file in lemur source code format into the root of the repository

- Then build the project

```bash
cd compiler/build
cmake ..
cmake --build .
```

# 3. Run the compiler

- go to root directory

```bash
./lemur --src ./test.lmur
```
