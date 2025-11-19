# Function Summaries in mccomp.cpp

This document shows examples of the function summary comments added to mccomp.cpp to help developers quickly understand what each function does.

## Debug Functions

```cpp
// Initialize debug level from command line arguments
static void initDebugLevel(int argc, char** argv) { ... }

// Output parser debug message with line/column
static void DEBUG_PARSER(const std::string& msg, int line = -1, int col = -1) { ... }

// Display compilation phase progress
static void ShowCompilationProgress() { ... }
```

## Error Handling

```cpp
// Log compiler error with type, message, location
static void LogCompilerError(ErrorType type, const std::string& msg, 
                            int line, int col, const std::string& context = "") { ... }

// Log type mismatch error
static Value* LogTypeError(const std::string& msg, Type* expected, Type* actual) { ... }

// Log scope/variable not found error
static Value* LogScopeError(const std::string& varName, const std::string& context = "") { ... }
```

## Lexer Functions

```cpp
// Get next token from input (main lexer function)
static TOKEN gettok() { ... }

// Get next token from buffer or lexer
static TOKEN getNextToken() { ... }

// Look ahead at token without consuming
static TOKEN peekToken(int offset = 0) { ... }
```

## Parser Functions - Expressions

```cpp
// Parse integer literal
static std::unique_ptr<ASTnode> ParseIntNumberExpr() { ... }

// Parse primary expression (literals, variables, calls, parens)
static std::unique_ptr<ASTnode> ParsePrimaryExpr() { ... }

// Parse unary expression (!, -, +)
static std::unique_ptr<ASTnode> ParseUnaryExpr() { ... }

// Parse multiplicative expression (*, /, %)
static std::unique_ptr<ASTnode> ParseMulExpr() { ... }

// Parse additive expression (+, -)
static std::unique_ptr<ASTnode> ParseAddExpr() { ... }

// Parse relational expression (<, >, <=, >=)
static std::unique_ptr<ASTnode> ParseRelExpr() { ... }

// Parse equality expression (==, !=)
static std::unique_ptr<ASTnode> ParseEqExpr() { ... }

// Parse logical AND expression (&&)
static std::unique_ptr<ASTnode> ParseAndExpr() { ... }

// Parse logical OR expression (||)
static std::unique_ptr<ASTnode> ParseOrExpr() { ... }

// Parse complete expression (top-level)
static std::unique_ptr<ASTnode> ParseExper() { ... }
```

## Parser Functions - Statements

```cpp
// Parse if statement with optional else
static std::unique_ptr<ASTnode> ParseIfStmt() { ... }

// Parse while loop statement
static std::unique_ptr<ASTnode> ParseWhileStmt() { ... }

// Parse return statement
static std::unique_ptr<ASTnode> ParseReturnStmt() { ... }

// Parse expression statement
static std::unique_ptr<ASTnode> ParseExperStmt() { ... }
```

## Parser Functions - Arrays

```cpp
// Parse second array dimension [m][n]
static bool ParseArrayDimsCont(std::vector<int> &dimensions) { ... }

// Parse third array dimension [n]
static bool ParseArrayDimsCont2(std::vector<int> &dimensions) { ... }

// Parse array subscript expression arr[i][j][k]
static std::unique_ptr<ArrayAccessAST> ParseArrayAccess(const std::string &arrayName) { ... }

// Parse second and third subscripts [j][k]
static bool ParseArrayAccessCont(std::vector<std::unique_ptr<ASTnode>> &indices) { ... }
```

## Helper Functions

```cpp
// Convert type string to LLVM Type
static Type* getTypeFromString(const std::string& typeStr) { ... }

// Create alloca in function entry block
static AllocaInst* CreateEntryBlockAlloca(Function *TheFunction,
                                          const std::string &VarName,
                                          Type *VarType) { ... }

// Check if variable exists in scope
static TypeInfo* checkVariableInScope(const std::string& varName, 
                                     int line = -1, int col = -1) { ... }

// Register variable in symbol table
static void registerVariable(const std::string& varName, 
                            const std::string& typeName, 
                            bool isGlobal) { ... }

// Cast value to destination type with checking
static Value* castToType(Value* V, Type* DestTy, 
                        bool allowNarrowing = true, 
                        const std::string& context = "") { ... }

// Promote values to common type for binary ops
static void promoteTypes(Value*& L, Value*& R) { ... }
```

## Code Generation Functions

```cpp
// Generate IR for integer literal
Value* IntASTnode::codegen() { ... }

// Generate IR for variable reference (load)
Value* VariableASTnode::codegen() { ... }

// Generate IR for binary operation
Value* BinaryExprAST::codegen() { ... }

// Generate IR for assignment
Value* AssignmentExprAST::codegen() { ... }

// Generate IR for function call
Value* CallExprAST::codegen() { ... }

// Generate IR for if-else with basic blocks
Value* IfExprAST::codegen() { ... }

// Generate IR for while loop with basic blocks
Value* WhileExprAST::codegen() { ... }

// Generate IR for return statement
Value* ReturnAST::codegen() { ... }

// Generate IR for block (scope and locals)
Value* BlockAST::codegen() { ... }

// Generate IR for function definition
Function* FunctionDeclAST::codegen() { ... }

// Generate IR for array declaration
Value* ArrayDeclAST::codegen() { ... }

// Generate IR for array access using GEP
Value* ArrayAccessAST::codegen() { ... }

// Generate IR for array element assignment
Value* ArrayAssignmentExprAST::codegen() { ... }
```

## Benefits

These function summaries provide:

1. **Quick Understanding**: Developers can quickly grasp what each function does without reading the implementation
2. **Better Navigation**: IDE tools can show these summaries in tooltips and autocompletion
3. **Documentation**: Acts as inline documentation that stays in sync with code
4. **Maintainability**: Makes it easier to understand code flow during debugging and refactoring

## Summary Statistics

- **Total function summaries added**: 34
- **Categories covered**:
  - Debug functions (7)
  - Error handling (5)
  - Lexer functions (3)
  - Parser functions (14)
  - Helper functions (6)
  - Code generation (18)

All summaries are concise (one line) but descriptive enough to understand the function's purpose.
