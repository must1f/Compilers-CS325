//===----------------------------------------------------------------------===//
// Expression Parsing - Precedence Climbing Implementation
//===----------------------------------------------------------------------===//



// Forward declarations
static std::unique_ptr<ASTnode> ParseOrExpr();
static std::unique_ptr<ASTnode> ParsePrimaryExpr();

// Helper: Parse function call arguments and create CallExprAST
static std::unique_ptr<ASTnode> ParseFunctionCall(const std::string &callee) {
  // At this point, we've consumed IDENT and '('
  std::vector<std::unique_ptr<ASTnode>> args;
  
  // Check for empty argument list
  if (CurTok.type == RPAR) {
    return std::make_unique<CallExprAST>(callee, std::move(args));
  }
  
  // Parse first argument
  auto arg = ParseExper();
  if (!arg)
    return nullptr;
  args.push_back(std::move(arg));
  
  // Parse additional arguments
  while (CurTok.type == COMMA) {
    getNextToken(); // eat ','
    
    arg = ParseExper();
    if (!arg)
      return nullptr;
    
    args.push_back(std::move(arg));
  }
  
  return std::make_unique<CallExprAST>(callee, std::move(args));
}

// primary_expr ::= "(" expr ")"
//              | IDENT "(" args ")"
//              | IDENT
//              | INT_LIT | FLOAT_LIT | BOOL_LIT
static std::unique_ptr<ASTnode> ParsePrimaryExpr() {
  // Case 1: Parenthesized expression
  if (CurTok.type == LPAR) {
    getNextToken(); // eat '('
    auto expr = ParseExper();
    if (!expr)
      return nullptr;
    
    if (CurTok.type != RPAR)
      return LogError(CurTok, "expected ')'");
    
    getNextToken(); // eat ')'
    return expr;
  }
  
  // Case 2: Identifier (variable or function call)
  if (CurTok.type == IDENT) {
    std::string idName = CurTok.getIdentifierStr();
    TOKEN idTok = CurTok;
    getNextToken(); // eat identifier
    
    // Check for function call
    if (CurTok.type == LPAR) {
      getNextToken(); // eat '('
      
      auto call = ParseFunctionCall(idName);
      if (!call)
        return nullptr;
      
      if (CurTok.type != RPAR)
        return LogError(CurTok, "expected ')' after arguments");
      
      getNextToken(); // eat ')'
      return call;
    }
    
    // Variable reference
    return std::make_unique<VariableASTnode>(idTok, idName);
  }
  
  // Case 3-5: Literals
  if (CurTok.type == INT_LIT)
    return ParseIntNumberExpr();
  
  if (CurTok.type == FLOAT_LIT)
    return ParseFloatNumberExpr();
  
  if (CurTok.type == BOOL_LIT)
    return ParseBoolExpr();
  
  return LogError(CurTok, "expected expression");
}

// unary_expr ::= "-" unary_expr
//            | "!" unary_expr
//            | primary_expr
static std::unique_ptr<ASTnode> ParseUnaryExpr() {
  // Unary minus
  if (CurTok.type == MINUS) {
    getNextToken(); // eat '-'
    auto operand = ParseUnaryExpr();
    if (!operand)
      return nullptr;
    return std::make_unique<UnaryExprAST>("-", std::move(operand));
  }
  
  // Unary not
  if (CurTok.type == NOT) {
    getNextToken(); // eat '!'
    auto operand = ParseUnaryExpr();
    if (!operand)
      return nullptr;
    return std::make_unique<UnaryExprAST>("!", std::move(operand));
  }
  
  // No unary operator, parse primary
  return ParsePrimaryExpr();
}

// mul_expr ::= unary_expr mul_expr_prime
// mul_expr_prime ::= ("*" | "/" | "%") unary_expr mul_expr_prime | ε
static std::unique_ptr<ASTnode> ParseMulExpr() {
  auto LHS = ParseUnaryExpr();
  if (!LHS)
    return nullptr;
  
  // Handle *, /, % operators (left-associative)
  while (CurTok.type == ASTERIX || CurTok.type == DIV || CurTok.type == MOD) {
    std::string op;
    if (CurTok.type == ASTERIX)
      op = "*";
    else if (CurTok.type == DIV)
      op = "/";
    else
      op = "%";
    
    getNextToken(); // eat operator
    
    auto RHS = ParseUnaryExpr();
    if (!RHS)
      return nullptr;
    
    LHS = std::make_unique<BinaryExprAST>(op, std::move(LHS), std::move(RHS));
  }
  
  return LHS;
}

// add_expr ::= mul_expr add_expr_prime
// add_expr_prime ::= ("+" | "-") mul_expr add_expr_prime | ε
static std::unique_ptr<ASTnode> ParseAddExpr() {
  auto LHS = ParseMulExpr();
  if (!LHS)
    return nullptr;
  
  // Handle + and - operators (left-associative)
  while (CurTok.type == PLUS || CurTok.type == MINUS) {
    std::string op = (CurTok.type == PLUS) ? "+" : "-";
    getNextToken(); // eat operator
    
    auto RHS = ParseMulExpr();
    if (!RHS)
      return nullptr;
    
    LHS = std::make_unique<BinaryExprAST>(op, std::move(LHS), std::move(RHS));
  }
  
  return LHS;
}

// rel_expr ::= add_expr rel_expr_prime
// rel_expr_prime ::= ("<=" | "<" | ">=" | ">") add_expr rel_expr_prime | ε
static std::unique_ptr<ASTnode> ParseRelExpr() {
  auto LHS = ParseAddExpr();
  if (!LHS)
    return nullptr;
  
  // Handle <, <=, >, >= operators (left-associative)
  while (CurTok.type == LT || CurTok.type == LE || 
         CurTok.type == GT || CurTok.type == GE) {
    std::string op;
    if (CurTok.type == LT)
      op = "<";
    else if (CurTok.type == LE)
      op = "<=";
    else if (CurTok.type == GT)
      op = ">";
    else
      op = ">=";
    
    getNextToken(); // eat operator
    
    auto RHS = ParseAddExpr();
    if (!RHS)
      return nullptr;
    
    LHS = std::make_unique<BinaryExprAST>(op, std::move(LHS), std::move(RHS));
  }
  
  return LHS;
}

// eq_expr ::= rel_expr eq_expr_prime
// eq_expr_prime ::= ("==" | "!=") rel_expr eq_expr_prime | ε
static std::unique_ptr<ASTnode> ParseEqExpr() {
  auto LHS = ParseRelExpr();
  if (!LHS)
    return nullptr;
  
  // Handle == and != operators (left-associative)
  while (CurTok.type == EQ || CurTok.type == NE) {
    std::string op = (CurTok.type == EQ) ? "==" : "!=";
    getNextToken(); // eat operator
    
    auto RHS = ParseRelExpr();
    if (!RHS)
      return nullptr;
    
    LHS = std::make_unique<BinaryExprAST>(op, std::move(LHS), std::move(RHS));
  }
  
  return LHS;
}

// and_expr ::= eq_expr and_expr_prime
// and_expr_prime ::= "&&" eq_expr and_expr_prime | ε
static std::unique_ptr<ASTnode> ParseAndExpr() {
  auto LHS = ParseEqExpr();
  if (!LHS)
    return nullptr;
  
  // Handle && operator (left-associative)
  while (CurTok.type == AND) {
    getNextToken(); // eat '&&'
    
    auto RHS = ParseEqExpr();
    if (!RHS)
      return nullptr;
    
    LHS = std::make_unique<BinaryExprAST>("&&", std::move(LHS), std::move(RHS));
  }
  
  return LHS;
}

// or_expr ::= and_expr or_expr_prime
// or_expr_prime ::= "||" and_expr or_expr_prime | ε
static std::unique_ptr<ASTnode> ParseOrExpr() {
  auto LHS = ParseAndExpr();
  if (!LHS)
    return nullptr;
  
  // Handle || operator (left-associative)
  while (CurTok.type == OR) {
    getNextToken(); // eat '||'
    
    auto RHS = ParseAndExpr();
    if (!RHS)
      return nullptr;
    
    LHS = std::make_unique<BinaryExprAST>("||", std::move(LHS), std::move(RHS));
  }
  
  return LHS;
}

// expr ::= IDENT "=" expr | rval
// rval ::= or_expr
static std::unique_ptr<ASTnode> ParseExper() {
  // Check for assignment
  if (CurTok.type == IDENT) {
    std::string varName = CurTok.getIdentifierStr();
    TOKEN varTok = CurTok;
    
    getNextToken(); // eat identifier
    
    // Check for assignment operator
    if (CurTok.type == ASSIGN) {
      getNextToken(); // eat '='
      
      // Parse RHS (recursive for right-associativity: x = y = 5)
      auto RHS = ParseExper();
      if (!RHS)
        return nullptr;
      
      return std::make_unique<AssignmentExprAST>(varName, std::move(RHS));
    } else {
      // Not an assignment, put tokens back and parse as rval
      putBackToken(CurTok);
      CurTok = varTok;
    }
  }
  
  // Parse as rval (or_expr is the top of expression hierarchy)
  return ParseOrExpr();
}

// args and arg_list are handled inline in ParseFunctionCall
// These standalone versions are not strictly needed:

static std::vector<std::unique_ptr<ASTnode>> ParseArgs() {
  std::vector<std::unique_ptr<ASTnode>> args;
  
  // Empty args
  if (CurTok.type == RPAR) {
    return args;
  }
  
  // Parse arguments
  auto arg = ParseExper();
  if (!arg)
    return args;
  args.push_back(std::move(arg));
  
  while (CurTok.type == COMMA) {
    getNextToken(); // eat ','
    arg = ParseExper();
    if (!arg)
      return args;
    args.push_back(std::move(arg));
  }
  
  return args;
}

static std::unique_ptr<ASTnode> ParseArgList() {
  // Not needed as separate function - handled in ParseArgs
  return nullptr;
}