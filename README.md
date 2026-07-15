# Lemur Parser

This is a simple compiler frontend for a custom language called **Lemur**. It tokenizes and parses `.lmur` files to build an abstract syntax tree (AST). 
This will be then compiled into assembly which will be used to create an executable.

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
./lemur init app
```

```bash
./lemur build --emit-ast
```
