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
|   |   |   ├── BinaryOperation.hpp
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
|   |   ├── Debug/
|   |   |   └── Debugger.hpp
│   │   ├── Driver/
│   │   |   └── Driver.cpp
|   |   ├── Errors/
|   |   |   ├── ErrorReporter.hpp
|   |   |   └── Errors.hpp
|   |   ├── Parser/
|   |   |   └── Parser.hpp
│   │   ├── Tests/
|   |   ├── Tokens/
|   |   |   ├── Token.hpp
|   |   |   └── Tokenizer.hpp
|   |   ├── Types/
|   |   |   ├── AllTypeInfo.hpp
|   |   |   ├── CustomTypeInfo.hpp
|   |   |   ├── FunctionTypeInfo.hpp
|   |   |   ├── InferredTypeInfo.hpp
|   |   |   ├── NullTypeInfo.hpp
|   |   |   ├── OwnershipTypeInfo.hpp
|   |   |   ├── PrimitiveTypeInfo.hpp
|   |   |   └── TypeInfo.hpp
|   |   ├── Utils/
|   |   |   ├── Logger.hpp
|   |   |   ├── OwnershipKind.hpp
|   |   |   ├── SourceLocation.hpp
|   |   |   └── SourceManager.hpp
│   │   └── LanguageConfig.hpp
│   ├── Source/ # Source code directory
│   │   ├── AST/
│   │   |   ├── BinaryOperation.cpp
│   │   |   └── Unary.cpp
│   │   ├── Debug/
│   │   |   └── Debugger.cpp
│   │   ├── Driver/
│   │   |   └── Driver.cpp
│   │   ├── Errors/
|   |   |   └── ErrorReporter.hpp
│   │   ├── Parser/
│   │   |   └── Parser.cpp
│   │   ├── Tokens/
|   |   |   ├── Token.cpp
|   |   |   └── Tokenizer.cpp
|   |   ├── Utils/
|   |   |   ├── ConfigurationParser.cpp
|   |   |   └── Modifiers.cpp
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
