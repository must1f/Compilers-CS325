#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/TargetParser/Host.h"
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <string.h>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

using namespace llvm;
using namespace llvm::sys;

static LLVMContext TheContext;
static IRBuilder<> Builder(TheContext);
static std::unique_ptr<Module> TheModule;
static std::map<std::string, AllocaInst*> NamedValues;
static std::map<std::string, GlobalVariable*> GlobalValues;
static std::map<std::string, std::string> VariableTypes;
static Function *CurrentFunction = nullptr;

// TOKEN class is used to keep track of information about a token
class TOKEN {
public:
  TOKEN() = default;
  int type = -100;
  std::string lexeme;
  int lineNo;
  int columnNo;
  const std::string getIdentifierStr() const;
  const int getIntVal() const;
  const float getFloatVal() const;
  const bool getBoolVal() const;
};

// ANSI color codes for better readability
#define USE_COLORS 1

#if USE_COLORS
  #define COLOR_RESET   std::string("\033[0m")
  #define COLOR_RED     std::string("\033[31m")
  #define COLOR_GREEN   std::string("\033[32m")
  #define COLOR_YELLOW  std::string("\033[33m")
  #define COLOR_BLUE    std::string("\033[34m")
  #define COLOR_MAGENTA std::string("\033[35m")
  #define COLOR_CYAN    std::string("\033[36m")
  #define COLOR_BOLD    std::string("\033[1m")
#else
  #define COLOR_RESET   std::string("")
  #define COLOR_RED     std::string("")
  #define COLOR_GREEN   std::string("")
  #define COLOR_YELLOW  std::string("")
  #define COLOR_BLUE    std::string("")
  #define COLOR_MAGENTA std::string("")
  #define COLOR_CYAN    std::string("")
  #define COLOR_BOLD    std::string("")
#endif

//===----------------------------------------------------------------------===//
// Debug Infrastructure
//===----------------------------------------------------------------------===//

enum class DebugLevel {
    NONE = 0,
    USER = 1,
    PARSER = 2,
    CODEGEN = 3,
    VERBOSE = 4
};

static DebugLevel CurrentDebugLevel = DebugLevel::NONE;

static void initDebugLevel(int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-d" || arg == "--debug") {
            if (i + 1 < argc) {
                std::string level = argv[i + 1];
                if (level == "user") CurrentDebugLevel = DebugLevel::USER;
                else if (level == "parser") CurrentDebugLevel = DebugLevel::PARSER;
                else if (level == "codegen") CurrentDebugLevel = DebugLevel::CODEGEN;
                else if (level == "verbose") CurrentDebugLevel = DebugLevel::VERBOSE;
                i++;
            }
        }
    }
    
    const char* envDebug = getenv("MCCOMP_DEBUG");
    if (envDebug) {
        std::string level = envDebug;
        if (level == "user") CurrentDebugLevel = DebugLevel::USER;
        else if (level == "parser") CurrentDebugLevel = DebugLevel::PARSER;
        else if (level == "codegen") CurrentDebugLevel = DebugLevel::CODEGEN;
        else if (level == "verbose") CurrentDebugLevel = DebugLevel::VERBOSE;
    }
}

static void DEBUG_USER(const std::string& msg) {
    if (CurrentDebugLevel >= DebugLevel::USER) {
        fprintf(stderr, "%s[USER]%s %s\n", COLOR_CYAN.c_str(), COLOR_RESET.c_str(), msg.c_str());
    }
}

static void DEBUG_PARSER(const std::string& msg, int line = -1, int col = -1) {
    if (CurrentDebugLevel >= DebugLevel::PARSER) {
        if (line >= 0) {
            fprintf(stderr, "%s[PARSER:%d:%d]%s %s\n", 
                    COLOR_GREEN.c_str(), line, col, COLOR_RESET.c_str(), msg.c_str());
        } else {
            fprintf(stderr, "%s[PARSER]%s %s\n", 
                    COLOR_GREEN.c_str(), COLOR_RESET.c_str(), msg.c_str());
        }
    }
}

static void DEBUG_CODEGEN(const std::string& msg) {
    if (CurrentDebugLevel >= DebugLevel::CODEGEN) {
        fprintf(stderr, "%s[CODEGEN]%s %s\n", 
                COLOR_YELLOW.c_str(), COLOR_RESET.c_str(), msg.c_str());
    }
}

static void DEBUG_VERBOSE(const std::string& msg) {
    if (CurrentDebugLevel >= DebugLevel::VERBOSE) {
        fprintf(stderr, "%s[VERBOSE]%s %s\n", 
                COLOR_MAGENTA.c_str(), COLOR_RESET.c_str(), msg.c_str());
    }
}

static std::vector<std::string> ParserStack;

static void PARSER_ENTER(const std::string& function, const TOKEN& tok) {
    ParserStack.push_back(function);
    if (CurrentDebugLevel >= DebugLevel::PARSER) {
        std::string indent(ParserStack.size() * 2, ' ');
        fprintf(stderr, "%s[PARSER]%s %s→ Entering %s at line %d, col %d (token: '%s')\n",
                COLOR_GREEN.c_str(), COLOR_RESET.c_str(), indent.c_str(),
                function.c_str(), tok.lineNo, tok.columnNo, tok.lexeme.c_str());
    }
}

static void PARSER_EXIT(const std::string& function, bool success) {
    if (CurrentDebugLevel >= DebugLevel::PARSER) {
        std::string indent(ParserStack.size() * 2, ' ');
        const char* status = success ? "✓" : "✗";
        std::string color = success ? COLOR_GREEN : COLOR_RED;
        fprintf(stderr, "%s[PARSER]%s %s← Exiting %s %s%s%s\n",
                COLOR_GREEN.c_str(), COLOR_RESET.c_str(), indent.c_str(),
                function.c_str(), color.c_str(), status, COLOR_RESET.c_str());
    }
    if (!ParserStack.empty()) {
        ParserStack.pop_back();
    }
}

struct ParserContext {
    std::string currentFunction;
    int blockDepth;
    bool inLoop;
    bool inConditional;
    
    ParserContext() : currentFunction(""), blockDepth(0), inLoop(false), inConditional(false) {}
    
    std::string toString() const {
        std::string result = "Function: " + (currentFunction.empty() ? "(global)" : currentFunction);
        result += ", Block depth: " + std::to_string(blockDepth);
        if (inLoop) result += ", in loop";
        if (inConditional) result += ", in conditional";
        return result;
    }
};

static ParserContext CurrentContext;

static void ShowCompilationProgress() {
    if (CurrentDebugLevel >= DebugLevel::USER) {
        fprintf(stderr, "\n%s%s┌────────────────────────────────┐%s\n",
                COLOR_BOLD.c_str(), COLOR_CYAN.c_str(), COLOR_RESET.c_str());
        fprintf(stderr, "%s%s│  MiniC Compiler - Debug Mode  │%s\n",
                COLOR_BOLD.c_str(), COLOR_CYAN.c_str(), COLOR_RESET.c_str());
        fprintf(stderr, "%s%s└────────────────────────────────┘%s\n\n",
                COLOR_BOLD.c_str(), COLOR_CYAN.c_str(), COLOR_RESET.c_str());
    }
}

static void ShowPhaseComplete(const std::string& phase) {
    if (CurrentDebugLevel >= DebugLevel::USER) {
        fprintf(stderr, "%s✓%s %s complete\n", 
                COLOR_GREEN.c_str(), COLOR_RESET.c_str(), phase.c_str());
    }
}

//===----------------------------------------------------------------------===//
// Error Reporting Infrastructure
//===----------------------------------------------------------------------===//

enum class ErrorType {
    LEXICAL,
    SYNTAX,
    SEMANTIC_TYPE,
    SEMANTIC_SCOPE,
    SEMANTIC_OTHER
};

class CompilerError {
public:
    ErrorType type;
    std::string message;
    int lineNo;
    int columnNo;
    std::string context;
    
    CompilerError(ErrorType t, const std::string& msg, int line = -1, int col = -1, const std::string& ctx = "")
        : type(t), message(msg), lineNo(line), columnNo(col), context(ctx) {}
    
    void print() const {
        std::string typeStr;
        std::string color;
        
        switch(type) {
            case ErrorType::LEXICAL:
                typeStr = "Lexical Error";
                color = COLOR_RED;
                break;
            case ErrorType::SYNTAX:
                typeStr = "Syntax Error";
                color = COLOR_RED;
                break;
            case ErrorType::SEMANTIC_TYPE:
                typeStr = "Type Error";
                color = COLOR_YELLOW;
                break;
            case ErrorType::SEMANTIC_SCOPE:
                typeStr = "Scope Error";
                color = COLOR_YELLOW;
                break;
            case ErrorType::SEMANTIC_OTHER:
                typeStr = "Semantic Error";
                color = COLOR_YELLOW;
                break;
        }
        
        fprintf(stderr, "%s%s%s", COLOR_BOLD.c_str(), color.c_str(), typeStr.c_str());
        
        if (lineNo >= 0) {
            fprintf(stderr, " at line %d", lineNo);
            if (columnNo >= 0) {
                fprintf(stderr, ", column %d", columnNo);
            }
        }
        
        fprintf(stderr, ":%s\n", COLOR_RESET.c_str());
        fprintf(stderr, "  %s\n", message.c_str());
        
        if (!context.empty()) {
            fprintf(stderr, "  %sContext:%s %s\n", 
                    COLOR_CYAN.c_str(), COLOR_RESET.c_str(), context.c_str());
        }
        fprintf(stderr, "\n");
    }
};

static std::vector<CompilerError> ErrorLog;
static bool HasErrors = false;

static void LogCompilerError(ErrorType type, const std::string& msg, 
                             int line = -1, int col = -1, const std::string& context = "") {
    HasErrors = true;
    ErrorLog.emplace_back(type, msg, line, col, context);
}

static void PrintAllErrors() {
    if (ErrorLog.empty()) return;
    
    fprintf(stderr, "\n%s%s╔════════════════════════════════════════════════╗%s\n",
            COLOR_BOLD.c_str(), COLOR_RED.c_str(), COLOR_RESET.c_str());
    fprintf(stderr, "%s%s║  Compilation Failed - %zu Error(s) Found", 
            COLOR_BOLD.c_str(), COLOR_RED.c_str(), ErrorLog.size());
    
    int padding = 16 - std::to_string(ErrorLog.size()).length();
    for (int i = 0; i < padding; i++) fprintf(stderr, " ");
    fprintf(stderr, "║%s\n", COLOR_RESET.c_str());
    
    fprintf(stderr, "%s%s╚════════════════════════════════════════════════╝%s\n\n",
            COLOR_BOLD.c_str(), COLOR_RED.c_str(), COLOR_RESET.c_str());
    
    for (const auto& error : ErrorLog) {
        error.print();
    }
}

//===----------------------------------------------------------------------===//
// Type Helper Functions
//===----------------------------------------------------------------------===//

static std::string getTypeName(Type* T) {
    if (!T) return "unknown";
    
    if (T->isVoidTy()) return "void";
    if (T->isIntegerTy(1)) return "bool";
    if (T->isIntegerTy(32)) return "int";
    if (T->isFloatTy()) return "float";
    if (T->isDoubleTy()) return "double";
    
    if (T->isPointerTy()) {
        return "pointer";
    }
    
    if (T->isFunctionTy()) {
        FunctionType* FT = cast<FunctionType>(T);
        std::string result = getTypeName(FT->getReturnType()) + "(";
        for (unsigned i = 0; i < FT->getNumParams(); i++) {
            if (i > 0) result += ", ";
            result += getTypeName(FT->getParamType(i));
        }
        result += ")";
        return result;
    }
    
    std::string typeStr;
    llvm::raw_string_ostream rso(typeStr);
    T->print(rso);
    rso.flush();
    return typeStr;
}

struct TypeInfo {
    std::string typeName;  // "int", "float", "bool", "void"
    bool isGlobal;
    int line;
    int column;
    
    TypeInfo() : typeName("unknown"), isGlobal(false), line(-1), column(-1) {}
    TypeInfo(const std::string& name, bool global = false, int l = -1, int c = -1)
        : typeName(name), isGlobal(global), line(l), column(c) {}
};



static std::map<std::string, TypeInfo> SymbolTypeTable;

/// TypeInfo - Structure to hold type information

/// getTypeInfo - Get type information for a variable
static TypeInfo* getTypeInfo(const std::string& varName) {
    auto it = SymbolTypeTable.find(varName);
    if (it != SymbolTypeTable.end()) {
        return &it->second;
    }
    return nullptr;
}

static void DUMP_SYMBOL_TABLE() {
    if (CurrentDebugLevel >= DebugLevel::VERBOSE) {
        fprintf(stderr, "\n%s[SYMBOL TABLE DUMP]%s\n", 
                COLOR_CYAN.c_str(), COLOR_RESET.c_str());
        
        fprintf(stderr, "  Symbol Type Table:\n");
        if (SymbolTypeTable.empty()) {
            fprintf(stderr, "    (empty)\n");
        } else {
            for (const auto& pair : SymbolTypeTable) {
                fprintf(stderr, "    %s: %s (%s) [line:%d, col:%d]\n",
                        pair.first.c_str(),
                        pair.second.typeName.c_str(),
                        pair.second.isGlobal ? "global" : "local",
                        pair.second.line,
                        pair.second.column);
            }
        }
        
        fprintf(stderr, "  Local Variables (NamedValues):\n");
        if (NamedValues.empty()) {
            fprintf(stderr, "    (empty)\n");
        } else {
            for (const auto& pair : NamedValues) {
                std::string typeName = getTypeName(pair.second->getAllocatedType());
                fprintf(stderr, "    %s: %s\n", 
                        pair.first.c_str(), 
                        typeName.c_str());
            }
        }
        
        fprintf(stderr, "  Global Variables (GlobalValues):\n");
        if (GlobalValues.empty()) {
            fprintf(stderr, "    (empty)\n");
        } else {
            for (const auto& pair : GlobalValues) {
                std::string typeName = getTypeName(pair.second->getValueType());
                fprintf(stderr, "    %s: %s\n", 
                        pair.first.c_str(), 
                        typeName.c_str());
            }
        }
        
        fprintf(stderr, "  Functions:\n");
        bool hasFunctions = false;
        for (auto& F : TheModule->functions()) {
            if (!F.empty() || F.isDeclaration()) {
                std::string typeName = getTypeName(F.getFunctionType());
                fprintf(stderr, "    %s: %s\n", 
                        F.getName().str().c_str(),
                        typeName.c_str());
                hasFunctions = true;
            }
        }
        if (!hasFunctions) {
            fprintf(stderr, "    (empty)\n");
        }
        
        fprintf(stderr, "  Current Context: %s\n", CurrentContext.toString().c_str());
        fprintf(stderr, "\n");
    }
}

//===----------------------------------------------------------------------===//
// AST Display Infrastructure
//===----------------------------------------------------------------------===//

namespace ASTPrint {
  // Global indentation level for tree printing
  static int indentLevel = 0;
  
  // Generate indentation string
  static std::string indent() {
    return std::string(indentLevel * 2, ' ');
  }
  
  // Tree drawing characters
  static const char* BRANCH = "├─ ";
  static const char* LAST_BRANCH = "└─ ";
  static const char* VERTICAL = "│  ";
  static const char* SPACE = "   ";
  
  // Helper to print with tree structure
  static std::string treePrefix(bool isLast) {
    return isLast ? LAST_BRANCH : BRANCH;
  }
  
  // Helper to continue tree lines
  static std::string treeContinue(bool isLast) {
    return isLast ? SPACE : VERTICAL;
  }
}

FILE *pFile;

//===----------------------------------------------------------------------===//
// Lexer
//===----------------------------------------------------------------------===//

// The lexer returns one of these for known things.
enum TOKEN_TYPE {

  IDENT = -1,        // [a-zA-Z_][a-zA-Z_0-9]*
  ASSIGN = int('='), // '='

  // delimiters
  LBRA = int('{'),  // left brace
  RBRA = int('}'),  // right brace
  LPAR = int('('),  // left parenthesis
  LBOX = int('['),  // left bracket
  RBOX = int(']'),  // right bracket
  RPAR = int(')'),  // right parenthesis
  SC = int(';'),    // semicolon
  COMMA = int(','), // comma

  // types
  INT_TOK = -2,   // "int"
  VOID_TOK = -3,  // "void"
  FLOAT_TOK = -4, // "float"
  BOOL_TOK = -5,  // "bool"

  // keywords
  EXTERN = -6,  // "extern"
  IF = -7,      // "if"
  ELSE = -8,    // "else"
  WHILE = -9,   // "while"
  RETURN = -10, // "return"
  // TRUE   = -12,     // "true"
  // FALSE   = -13,     // "false"

  // literals
  INT_LIT = -14,   // [0-9]+
  FLOAT_LIT = -15, // [0-9]+.[0-9]+
  BOOL_LIT = -16,  // "true" or "false" key words

  // logical operators
  AND = -17, // "&&"
  OR = -18,  // "||"

  // operators
  PLUS = int('+'),    // addition or unary plus
  MINUS = int('-'),   // substraction or unary negative
  ASTERIX = int('*'), // multiplication
  DIV = int('/'),     // division
  MOD = int('%'),     // modular
  NOT = int('!'),     // unary negation

  // comparison operators
  EQ = -19,      // equal
  NE = -20,      // not equal
  LE = -21,      // less than or equal to
  LT = int('<'), // less than
  GE = -23,      // greater than or equal to
  GT = int('>'), // greater than

  // special tokens
  EOF_TOK = 0, // signal end of file

  // invalid
  INVALID = -100 // signal invalid token
};



static std::string globalLexeme;
static int lineNo, columnNo;

const std::string TOKEN::getIdentifierStr() const {
  if (type != IDENT) {
    fprintf(stderr, "%d:%d Error: %s\n", lineNo, columnNo,
            "getIdentifierStr called on non-IDENT token");
    exit(2);
  }
  return lexeme;
}

const int TOKEN::getIntVal() const {
  if (type != INT_LIT) {
    fprintf(stderr, "%d:%d Error: %s\n", lineNo, columnNo,
            "getIntVal called on non-INT_LIT token");
    exit(2);
  }
  return strtod(lexeme.c_str(), nullptr);
}

const float TOKEN::getFloatVal() const {
  if (type != FLOAT_LIT) {
    fprintf(stderr, "%d:%d Error: %s\n", lineNo, columnNo,
            "getFloatVal called on non-FLOAT_LIT token");
    exit(2);
  }
  return strtof(lexeme.c_str(), nullptr);
}

const bool TOKEN::getBoolVal() const {
  if (type != BOOL_LIT) {
    fprintf(stderr, "%d:%d Error: %s\n", lineNo, columnNo,
            "getBoolVal called on non-BOOL_LIT token");
    exit(2);
  }
  return (lexeme == "true");
}

static TOKEN returnTok(std::string lexVal, int tok_type) {
  TOKEN return_tok;
  return_tok.lexeme = lexVal;
  return_tok.type = tok_type;
  return_tok.lineNo = lineNo;
  return_tok.columnNo = columnNo - lexVal.length() - 1;
  return return_tok;
}

// Read file line by line -- or look for \n and if found add 1 to line number
// and reset column number to 0
/// gettok - Return the next token from standard input.
static TOKEN gettok() {

  static int LastChar = ' ';
  static int NextChar = ' ';

  // Skip any whitespace.
  while (isspace(LastChar)) {
    if (LastChar == '\n' || LastChar == '\r') {
      lineNo++;
      columnNo = 1;
    }
    LastChar = getc(pFile);
    columnNo++;
  }

  if (isalpha(LastChar) ||
      (LastChar == '_')) { // identifier: [a-zA-Z_][a-zA-Z_0-9]*
    globalLexeme = LastChar;
    columnNo++;

    while (isalnum((LastChar = getc(pFile))) || (LastChar == '_')) {
      globalLexeme += LastChar;
      columnNo++;
    }

    if (globalLexeme == "int")
      return returnTok("int", INT_TOK);
    if (globalLexeme == "bool")
      return returnTok("bool", BOOL_TOK);
    if (globalLexeme == "float")
      return returnTok("float", FLOAT_TOK);
    if (globalLexeme == "void")
      return returnTok("void", VOID_TOK);
    if (globalLexeme == "bool")
      return returnTok("bool", BOOL_TOK);
    if (globalLexeme == "extern")
      return returnTok("extern", EXTERN);
    if (globalLexeme == "if")
      return returnTok("if", IF);
    if (globalLexeme == "else")
      return returnTok("else", ELSE);
    if (globalLexeme == "while")
      return returnTok("while", WHILE);
    if (globalLexeme == "return")
      return returnTok("return", RETURN);
    if (globalLexeme == "true") {
      //   BoolVal = true;
      return returnTok("true", BOOL_LIT);
    }
    if (globalLexeme == "false") {
      //   BoolVal = false;
      return returnTok("false", BOOL_LIT);
    }
    return returnTok(globalLexeme.c_str(), IDENT);
  }

  if (LastChar == '=') {
    NextChar = getc(pFile);
    if (NextChar == '=') { // EQ: ==
      LastChar = getc(pFile);
      columnNo += 2;
      return returnTok("==", EQ);
    } else {
      LastChar = NextChar;
      columnNo++;
      return returnTok("=", ASSIGN);
    }
  }

  if (LastChar == '{') {
    LastChar = getc(pFile);
    columnNo++;
    return returnTok("{", LBRA);
  }
  if (LastChar == '}') {
    LastChar = getc(pFile);
    columnNo++;
    return returnTok("}", RBRA);
  }
  if (LastChar == '(') {
    LastChar = getc(pFile);
    columnNo++;
    return returnTok("(", LPAR);
  }
  if (LastChar == ')') {
    LastChar = getc(pFile);
    columnNo++;
    return returnTok(")", RPAR);
  }
  if (LastChar == ';') {
    LastChar = getc(pFile);
    columnNo++;
    return returnTok(";", SC);
  }
  if (LastChar == ',') {
    LastChar = getc(pFile);
    columnNo++;
    return returnTok(",", COMMA);
  }

  if (isdigit(LastChar) || LastChar == '.') { // Number: [0-9]+.
    std::string NumStr;

    if (LastChar == '.') { // Floatingpoint Number: .[0-9]+
      do {
        NumStr += LastChar;
        LastChar = getc(pFile);
        columnNo++;
      } while (isdigit(LastChar));

      //   FloatVal = strtof(NumStr.c_str(), nullptr);
      return returnTok(NumStr, FLOAT_LIT);
    } else {
      do { // Start of Number: [0-9]+
        NumStr += LastChar;
        LastChar = getc(pFile);
        columnNo++;
      } while (isdigit(LastChar));

      if (LastChar == '.') { // Floatingpoint Number: [0-9]+.[0-9]+)
        do {
          NumStr += LastChar;
          LastChar = getc(pFile);
          columnNo++;
        } while (isdigit(LastChar));

        // FloatVal = strtof(NumStr.c_str(), nullptr);
        return returnTok(NumStr, FLOAT_LIT);
      } else { // Integer : [0-9]+
        // IntVal = strtod(NumStr.c_str(), nullptr);
        return returnTok(NumStr, INT_LIT);
      }
    }
  }

  if (LastChar == '&') {
    NextChar = getc(pFile);
    if (NextChar == '&') { // AND: &&
      LastChar = getc(pFile);
      columnNo += 2;
      return returnTok("&&", AND);
    } else {
      LastChar = NextChar;
      columnNo++;
      return returnTok("&", int('&'));
    }
  }

  if (LastChar == '|') {
    NextChar = getc(pFile);
    if (NextChar == '|') { // OR: ||
      LastChar = getc(pFile);
      columnNo += 2;
      return returnTok("||", OR);
    } else {
      LastChar = NextChar;
      columnNo++;
      return returnTok("|", int('|'));
    }
  }

  if (LastChar == '!') {
    NextChar = getc(pFile);
    if (NextChar == '=') { // NE: !=
      LastChar = getc(pFile);
      columnNo += 2;
      return returnTok("!=", NE);
    } else {
      LastChar = NextChar;
      columnNo++;
      return returnTok("!", NOT);
      ;
    }
  }

  if (LastChar == '<') {
    NextChar = getc(pFile);
    if (NextChar == '=') { // LE: <=
      LastChar = getc(pFile);
      columnNo += 2;
      return returnTok("<=", LE);
    } else {
      LastChar = NextChar;
      columnNo++;
      return returnTok("<", LT);
    }
  }

  if (LastChar == '>') {
    NextChar = getc(pFile);
    if (NextChar == '=') { // GE: >=
      LastChar = getc(pFile);
      columnNo += 2;
      return returnTok(">=", GE);
    } else {
      LastChar = NextChar;
      columnNo++;
      return returnTok(">", GT);
    }
  }

  if (LastChar == '/') { // could be division or could be the start of a comment
    LastChar = getc(pFile);
    columnNo++;
    if (LastChar == '/') { // definitely a comment
      do {
        LastChar = getc(pFile);
        columnNo++;
      } while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');

      if (LastChar != EOF)
        return gettok();
    } else
      return returnTok("/", DIV);
  }

  // Check for end of file.  Don't eat the EOF.
  if (LastChar == EOF) {
    columnNo++;
    return returnTok("0", EOF_TOK);
  }

  // Otherwise, just return the character as its ascii value.
  int ThisChar = LastChar;
  std::string s(1, ThisChar);
  LastChar = getc(pFile);
  columnNo++;
  return returnTok(s, int(ThisChar));
}

//===----------------------------------------------------------------------===//
// Parser
//===----------------------------------------------------------------------===//

/// CurTok/getNextToken - Provide a simple token buffer.  CurTok is the current
/// token the parser is looking at.  getNextToken reads another token from the
/// lexer and updates CurTok with its results.
static TOKEN CurTok;
static std::deque<TOKEN> tok_buffer;

static TOKEN getNextToken() {

  if (tok_buffer.size() == 0)
    tok_buffer.push_back(gettok());

  TOKEN temp = tok_buffer.front();
  tok_buffer.pop_front();

  return CurTok = temp;
}

static void putBackToken(TOKEN tok) { tok_buffer.push_front(tok); }

static TOKEN peekToken(int offset = 0) {
  // Ensure we have enough tokens in the buffer
  while (tok_buffer.size() <= static_cast<size_t>(offset)) {
    tok_buffer.push_back(gettok());
  }
  return tok_buffer[offset];
}

static TOKEN peekNextToken() {
  return peekToken(0);
}

/// ASTnode - Base class for all AST nodes.
class ASTnode {

public:
  virtual ~ASTnode() {}
  virtual Value *codegen() { return nullptr; };
  virtual std::string to_string() const { return ""; };
};

/// IntASTnode - Class for integer literals like 1, 2, 10,
class IntASTnode : public ASTnode {
  int Val;
  TOKEN Tok;
  std::string Name;

public:
  IntASTnode(TOKEN tok, int val) : Val(val), Tok(tok) {}
  const std::string &getType() const { return Tok.lexeme; }
  int getValue() const { return Val; }
  
  virtual Value *codegen() override;

  virtual std::string to_string() const override {
    return std::string(COLOR_CYAN) + "IntLiteral" + std::string(COLOR_RESET) + "(" + 
           std::string(COLOR_BOLD) + std::to_string(Val) + std::string(COLOR_RESET) + 
           " : " + std::string(COLOR_YELLOW) + Tok.lexeme + std::string(COLOR_RESET) + ")";
  }
};

/// BoolASTnode - Class for boolean literals true and false,
class BoolASTnode : public ASTnode {
  bool Bool;
  TOKEN Tok;

public:
  BoolASTnode(TOKEN tok, bool B) : Bool(B), Tok(tok) {}
  const std::string &getType() const { return Tok.lexeme; }
  bool getValue() const { return Bool; }

  virtual Value *codegen() override;
  
  virtual std::string to_string() const override {
    return std::string(COLOR_CYAN) + "BoolLiteral" + std::string(COLOR_RESET) + "(" + 
           std::string(COLOR_BOLD) + std::string(Bool ? "true" : "false") + std::string(COLOR_RESET) + 
           " : " + std::string(COLOR_YELLOW) + Tok.lexeme + std::string(COLOR_RESET) + ")";
  }
};

/// FloatASTnode - Node class for floating point literals like "1.0".
class FloatASTnode : public ASTnode {
  double Val;
  TOKEN Tok;

public:
  FloatASTnode(TOKEN tok, double Val) : Val(Val), Tok(tok) {}
  const std::string &getType() const { return Tok.lexeme; }
  double getValue() const { return Val; }
  
  virtual Value *codegen() override;

  virtual std::string to_string() const override {
    return std::string(COLOR_CYAN) + "FloatLiteral" + std::string(COLOR_RESET) + "(" + 
           std::string(COLOR_BOLD) + std::to_string(Val) + std::string(COLOR_RESET) + 
           " : " + std::string(COLOR_YELLOW) + Tok.lexeme + std::string(COLOR_RESET) + ")";
  }
};

/// VariableASTnode - Class for referencing a variable (i.e. identifier), like
/// "a".
enum IDENT_TYPE { IDENTIFIER = 0 };
class VariableASTnode : public ASTnode {
protected:
  TOKEN Tok;
  std::string Name;
  IDENT_TYPE VarType;

public:
  VariableASTnode(TOKEN tok, const std::string &Name)
      : Tok(tok), Name(Name), VarType(IDENT_TYPE::IDENTIFIER) {}
  const std::string &getName() const { return Name; }
  const std::string &getType() const { return Tok.lexeme; }
  const IDENT_TYPE getVarType() const { return VarType; }

  virtual Value *codegen() override;

  virtual std::string to_string() const override {
    return std::string(COLOR_GREEN) + "VarRef" + std::string(COLOR_RESET) + "(" + 
           std::string(COLOR_BOLD) + Name + std::string(COLOR_RESET) + ")";
  }
};

/// ParamAST - Class for a parameter declaration
class ParamAST {
  std::string Name;
  std::string Type;

public:
  ParamAST(const std::string &name, const std::string &type)
      : Name(name), Type(type) {}
  const std::string &getName() const { return Name; }
  const std::string &getType() const { return Type; }
};

/// DeclAST - Base class for declarations, variables and functions
class DeclAST : public ASTnode {

public:
  virtual ~DeclAST() {}
};

/// VarDeclAST - Class for a variable declaration
class VarDeclAST : public DeclAST {
  std::unique_ptr<VariableASTnode> Var;
  std::string Type;

public:
  VarDeclAST(std::unique_ptr<VariableASTnode> var, const std::string &type)
      : Var(std::move(var)), Type(type) {}
  const std::string &getType() const { return Type; }
  const std::string &getName() const { return Var->getName(); }

  virtual std::string to_string() const override {
    return std::string(COLOR_CYAN) + "VarDecl" + std::string(COLOR_RESET) + " [" + 
           std::string(COLOR_YELLOW) + Type + std::string(COLOR_RESET) + " " + 
           std::string(COLOR_BOLD) + Var->getName() + std::string(COLOR_RESET) + "]";
  }
};

/// GlobVarDeclAST - Class for a Global variable declaration
class GlobVarDeclAST : public DeclAST {
  std::unique_ptr<VariableASTnode> Var;
  std::string Type;

public:
  GlobVarDeclAST(std::unique_ptr<VariableASTnode> var, const std::string &type)
      : Var(std::move(var)), Type(type) {}
  const std::string &getType() const { return Type; }
  const std::string &getName() const { return Var->getName(); }

  virtual Value *codegen() override;

  virtual std::string to_string() const override {
    return std::string(COLOR_CYAN) + "GlobalVarDecl" + std::string(COLOR_RESET) + " [" + 
           std::string(COLOR_YELLOW) + Type + std::string(COLOR_RESET) + " " + 
           std::string(COLOR_BOLD) + Var->getName() + std::string(COLOR_RESET) + "]";
  }
};

/// FunctionPrototypeAST - Class for a function declaration's signature
class FunctionPrototypeAST {
  std::string Name;
  std::string Type;
  std::vector<std::unique_ptr<ParamAST>> Params; // vector of parameters

public:
  FunctionPrototypeAST(const std::string &name, const std::string &type,
                       std::vector<std::unique_ptr<ParamAST>> params)
      : Name(name), Type(type), Params(std::move(params)) {}

  const std::string &getName() const { return Name; }
  const std::string &getType() const { return Type; }
  int getSize() const { return Params.size(); }
  std::vector<std::unique_ptr<ParamAST>> &getParams() { return Params; }

  Function* codegen();

  std::string to_string() const {
    std::string result = std::string(COLOR_CYAN) + "FunctionProto" + std::string(COLOR_RESET) + " '" + 
                        std::string(COLOR_BOLD) + Name + std::string(COLOR_RESET) + "'\n";
    
    result += ASTPrint::indent() + ASTPrint::BRANCH;
    result += std::string(COLOR_BLUE) + "ReturnType: " + std::string(COLOR_RESET) + 
             std::string(COLOR_YELLOW) + Type + std::string(COLOR_RESET) + "\n";
    
    result += ASTPrint::indent() + ASTPrint::LAST_BRANCH;
    result += std::string(COLOR_BLUE) + "Parameters (" + std::to_string(Params.size()) + 
             "):" + std::string(COLOR_RESET);
    
    if (Params.empty()) {
      result += " " + std::string(COLOR_YELLOW) + "(none)" + std::string(COLOR_RESET);
    } else {
      result += "\n";
      ASTPrint::indentLevel++;
      for (size_t i = 0; i < Params.size(); i++) {
        bool isLast = (i == Params.size() - 1);
        result += ASTPrint::indent() + ASTPrint::treePrefix(isLast);
        result += std::string(COLOR_YELLOW) + Params[i]->getType() + std::string(COLOR_RESET) + " " + 
                 std::string(COLOR_BOLD) + Params[i]->getName() + std::string(COLOR_RESET);
        if (!isLast) result += "\n";
      }
      ASTPrint::indentLevel--;
    }
    
    return result;
  }
};

class ExprAST : public ASTnode {
  std::unique_ptr<ASTnode> Node1;
  char Op;
  std::unique_ptr<ASTnode> Node2;

public:
  ExprAST(std::unique_ptr<ASTnode> node1, char op,
          std::unique_ptr<ASTnode> node2)
      : Node1(std::move(node1)), Op(op), Node2(std::move(node2)) {}
  const std::string &getType();
};

/// BlockAST - Class for a block with declarations followed by statements
class BlockAST : public ASTnode {
  std::vector<std::unique_ptr<VarDeclAST>> LocalDecls; // vector of local decls
  std::vector<std::unique_ptr<ASTnode>> Stmts;         // vector of statements

public:
  BlockAST(std::vector<std::unique_ptr<VarDeclAST>> localDecls,
           std::vector<std::unique_ptr<ASTnode>> stmts)
      : LocalDecls(std::move(localDecls)), Stmts(std::move(stmts)) {}
  
  virtual Value *codegen() override;

  virtual std::string to_string() const override {
    std::string result = std::string(COLOR_CYAN) + "Block" + std::string(COLOR_RESET) + "\n";
    
    // Print local declarations
    if (!LocalDecls.empty()) {
      result += ASTPrint::indent() + ASTPrint::BRANCH;
      result += std::string(COLOR_BLUE) + "LocalDecls (" + std::to_string(LocalDecls.size()) + 
               "):" + std::string(COLOR_RESET) + "\n";
      
      ASTPrint::indentLevel++;
      for (size_t i = 0; i < LocalDecls.size(); i++) {
        bool isLast = (i == LocalDecls.size() - 1) && Stmts.empty();
        result += ASTPrint::indent() + ASTPrint::treePrefix(isLast);
        result += LocalDecls[i]->to_string();
        if (i < LocalDecls.size() - 1 || !Stmts.empty()) result += "\n";
      }
      ASTPrint::indentLevel--;
    }
    
    // Print statements
    if (!Stmts.empty()) {
      if (!LocalDecls.empty()) result += "\n";
      
      result += ASTPrint::indent() + ASTPrint::LAST_BRANCH;
      result += std::string(COLOR_BLUE) + "Statements (" + std::to_string(Stmts.size()) + 
               "):" + std::string(COLOR_RESET) + "\n";
      
      ASTPrint::indentLevel++;
      for (size_t i = 0; i < Stmts.size(); i++) {
        bool isLast = (i == Stmts.size() - 1);
        result += ASTPrint::indent() + ASTPrint::treePrefix(isLast);
        
        if (Stmts[i]) {
          std::string stmtStr = Stmts[i]->to_string();
          // Don't add extra newline - just append the statement directly
          result += stmtStr;  // ← CHANGED: removed the conditional newline before
        } else {
          result += std::string(COLOR_RED) + "nullptr" + std::string(COLOR_RESET);
        }
        
        if (!isLast) result += "\n";
      }
      ASTPrint::indentLevel--;
    }
    
    if (LocalDecls.empty() && Stmts.empty()) {
      result += ASTPrint::indent() + ASTPrint::LAST_BRANCH;
      result += std::string(COLOR_YELLOW) + "(empty)" + std::string(COLOR_RESET);
    }
    
    return result;
  }
};

/// FunctionDeclAST - This class represents a function definition itself.
class FunctionDeclAST : public DeclAST {
  std::unique_ptr<FunctionPrototypeAST> Proto;
  std::unique_ptr<ASTnode> Block;

public:
  FunctionDeclAST(std::unique_ptr<FunctionPrototypeAST> Proto,
                  std::unique_ptr<ASTnode> Block)
      : Proto(std::move(Proto)), Block(std::move(Block)) {}

  virtual Value *codegen() override;

  virtual std::string to_string() const override {
    std::string result = std::string(COLOR_GREEN) + std::string(COLOR_BOLD) + "╔═══ FunctionDecl ═══╗" 
                        + std::string(COLOR_RESET) + "\n";
    
    ASTPrint::indentLevel++;
    result += ASTPrint::indent() + Proto->to_string() + "\n";
    result += ASTPrint::indent() + std::string(COLOR_BLUE) + "Body:" + std::string(COLOR_RESET) + "\n";
    
    ASTPrint::indentLevel++;
    if (Block) {
      result += ASTPrint::indent() + Block->to_string();
    } else {
      result += ASTPrint::indent() + std::string(COLOR_RED) + "nullptr" + std::string(COLOR_RESET);
    }
    ASTPrint::indentLevel--;
    ASTPrint::indentLevel--;
    
    result += "\n" + ASTPrint::indent() + 
             std::string(COLOR_GREEN) + std::string(COLOR_BOLD) + "╚════════════════════╝" + std::string(COLOR_RESET);
    
    return result;
  }
};

/// IfExprAST - Expression class for if/then/else.
class IfExprAST : public ASTnode {
  std::unique_ptr<ASTnode> Cond, Then, Else;

public:
  IfExprAST(std::unique_ptr<ASTnode> Cond, std::unique_ptr<ASTnode> Then,
            std::unique_ptr<ASTnode> Else)
      : Cond(std::move(Cond)), Then(std::move(Then)), Else(std::move(Else)) {}

  virtual Value *codegen() override;

  virtual std::string to_string() const override {
    std::string result = std::string(COLOR_MAGENTA) + "IfStmt" + std::string(COLOR_RESET) + "\n";
    
    ASTPrint::indentLevel++;
    
    // Condition
    result += ASTPrint::indent() + ASTPrint::BRANCH;
    result += std::string(COLOR_BLUE) + "Condition: " + std::string(COLOR_RESET);
    if (Cond) {
      std::string condStr = Cond->to_string();
      if (condStr.find('\n') != std::string::npos) {
        result += "\n";
        ASTPrint::indentLevel++;
        result += ASTPrint::indent() + condStr;
        ASTPrint::indentLevel--;
      } else {
        result += condStr;
      }
    } else {
      result += std::string(COLOR_RED) + "nullptr" + std::string(COLOR_RESET);
    }
    result += "\n";
    
    // Then block
    bool hasElse = (Else != nullptr);
    result += ASTPrint::indent() + (hasElse ? ASTPrint::BRANCH : ASTPrint::LAST_BRANCH);
    result += std::string(COLOR_BLUE) + "Then: " + std::string(COLOR_RESET);
    if (Then) {
      std::string thenStr = Then->to_string();
      if (thenStr.find('\n') != std::string::npos) {
        result += "\n";
        ASTPrint::indentLevel++;
        result += ASTPrint::indent() + thenStr;
        ASTPrint::indentLevel--;
      } else {
        result += thenStr;
      }
    } else {
      result += std::string(COLOR_RED) + "nullptr" + std::string(COLOR_RESET);
    }
    
    // Else block (if present)
    if (hasElse) {
      result += "\n" + ASTPrint::indent() + ASTPrint::LAST_BRANCH;
      result += std::string(COLOR_BLUE) + "Else: " + std::string(COLOR_RESET);
      std::string elseStr = Else->to_string();
      if (elseStr.find('\n') != std::string::npos) {
        result += "\n";
        ASTPrint::indentLevel++;
        result += ASTPrint::indent() + elseStr;
        ASTPrint::indentLevel--;
      } else {
        result += elseStr;
      }
    }
    
    ASTPrint::indentLevel--;
    
    return result;
  }

};

/// WhileExprAST - Expression class for while.
class WhileExprAST : public ASTnode {
  std::unique_ptr<ASTnode> Cond, Body;

public:
  WhileExprAST(std::unique_ptr<ASTnode> cond, std::unique_ptr<ASTnode> body)
      : Cond(std::move(cond)), Body(std::move(body)) {}

  virtual Value *codegen() override;

  virtual std::string to_string() const override {
    std::string result = std::string(COLOR_MAGENTA) + "WhileStmt" + std::string(COLOR_RESET) + "\n";
    
    // Condition
    result += ASTPrint::indent() + ASTPrint::BRANCH;
    result += std::string(COLOR_BLUE) + "Condition:" + std::string(COLOR_RESET) + "\n";
    ASTPrint::indentLevel++;
    result += ASTPrint::indent() + (Cond ? Cond->to_string() : 
             std::string(COLOR_RED) + "nullptr" + std::string(COLOR_RESET));
    ASTPrint::indentLevel--;
    result += "\n";
    
    // Body
    result += ASTPrint::indent() + ASTPrint::LAST_BRANCH;
    result += std::string(COLOR_BLUE) + "Body:" + std::string(COLOR_RESET) + "\n";
    ASTPrint::indentLevel++;
    result += ASTPrint::indent() + (Body ? Body->to_string() : 
             std::string(COLOR_RED) + "nullptr" + std::string(COLOR_RESET));
    ASTPrint::indentLevel--;
    
    return result;
  }

};

/// ReturnAST - Class for a return value
class ReturnAST : public ASTnode {
  std::unique_ptr<ASTnode> Val;

public:
  ReturnAST(std::unique_ptr<ASTnode> value) : Val(std::move(value)) {}

  virtual Value *codegen() override;

  virtual std::string to_string() const override {
    if (Val) {
      std::string result = std::string(COLOR_MAGENTA) + "ReturnStmt" + std::string(COLOR_RESET) + "\n";
      
      ASTPrint::indentLevel++;
      result += ASTPrint::indent() + ASTPrint::LAST_BRANCH;
      result += std::string(COLOR_BLUE) + "Value: " + std::string(COLOR_RESET);
      
      std::string valStr = Val->to_string();
      if (valStr.find('\n') != std::string::npos) {
        result += "\n";
        ASTPrint::indentLevel++;
        result += ASTPrint::indent() + valStr;
        ASTPrint::indentLevel--;
      } else {
        result += valStr;
      }
      
      ASTPrint::indentLevel--;
      
      return result;
    } else {
      return std::string(COLOR_MAGENTA) + "ReturnStmt " + std::string(COLOR_RESET) + 
            std::string(COLOR_YELLOW) + "(void)" + std::string(COLOR_RESET);
    }
  }

};

/// ArgsAST - Class for a function argumetn in a function call
class ArgsAST : public ASTnode {
  std::string Callee;
  std::vector<std::unique_ptr<ASTnode>> ArgsList;

public:
  ArgsAST(const std::string &Callee, std::vector<std::unique_ptr<ASTnode>> list)
      : Callee(Callee), ArgsList(std::move(list)) {}

};

//===----------------------------------------------------------------------===//
// Enhanced Error Logging Functions
//===----------------------------------------------------------------------===//

static std::unique_ptr<ASTnode> LogError(TOKEN tok, const char *Str) {
    LogCompilerError(ErrorType::SYNTAX, Str, tok.lineNo, tok.columnNo, 
                     "Token: '" + tok.lexeme + "'");
    return nullptr;
}

static std::unique_ptr<ASTnode> LogError(const char *Str) {
    LogCompilerError(ErrorType::SYNTAX, Str, lineNo, columnNo);
    return nullptr;
}

static std::unique_ptr<FunctionPrototypeAST> LogErrorP(TOKEN tok, const char *Str) {
    LogCompilerError(ErrorType::SYNTAX, Str, tok.lineNo, tok.columnNo,
                     "Token: '" + tok.lexeme + "'");
    return nullptr;
}

static Value* LogErrorV(const char *Str) {
    LogCompilerError(ErrorType::SEMANTIC_OTHER, Str);
    return nullptr;
}

static Value* LogErrorV(const std::string& Str) {
    LogCompilerError(ErrorType::SEMANTIC_OTHER, Str);
    return nullptr;
}

static Value* LogTypeError(const std::string& msg, Type* expected, Type* actual) {
    std::string fullMsg = msg + "\n  Expected: " + 
                         getTypeName(expected) + 
                         "\n  Actual: " + 
                         getTypeName(actual);
    LogCompilerError(ErrorType::SEMANTIC_TYPE, fullMsg);
    return nullptr;
}

static Value* LogScopeError(const std::string& varName, const std::string& context = "") {
    std::string msg = "Undefined variable '" + varName + "'";
    LogCompilerError(ErrorType::SEMANTIC_SCOPE, msg, -1, -1, context);
    return nullptr;
}

static Function* LogErrorF(const char *Str) {
    LogCompilerError(ErrorType::SEMANTIC_OTHER, Str);
    return nullptr;
}


//===----------------------------------------------------------------------===//
// AST Printing Helper
//===----------------------------------------------------------------------===//

static void printAST(const std::unique_ptr<ASTnode>& node, const std::string& label) {
  if (!node) {
    fprintf(stderr, "\n%s%s=== %s ===%s\n", 
            COLOR_BOLD.c_str(), COLOR_RED.c_str(), label.c_str(), COLOR_RESET.c_str());
    fprintf(stderr, "%sNode is nullptr!%s\n\n", COLOR_RED.c_str(), COLOR_RESET.c_str());
    return;
  }
  
  fprintf(stderr, "\n%s%s╔═══════════════════════════════════════╗%s\n", 
          COLOR_BOLD.c_str(), COLOR_GREEN.c_str(), COLOR_RESET.c_str());
  fprintf(stderr, "%s%s║  %s%-35s%s%s║%s\n", 
          COLOR_BOLD.c_str(), COLOR_GREEN.c_str(), COLOR_RESET.c_str(), label.c_str(), 
          COLOR_BOLD.c_str(), COLOR_GREEN.c_str(), COLOR_RESET.c_str());
  fprintf(stderr, "%s%s╚═══════════════════════════════════════╝%s\n\n", 
          COLOR_BOLD.c_str(), COLOR_GREEN.c_str(), COLOR_RESET.c_str());
  
  ASTPrint::indentLevel = 0;
  fprintf(stderr, "%s\n\n", node->to_string().c_str());
}

/// BinaryExprAST - Expression class for binary operators
class BinaryExprAST : public ASTnode {
  std::string Op;
  std::unique_ptr<ASTnode> LHS, RHS;

public:
  BinaryExprAST(std::string op, std::unique_ptr<ASTnode> lhs, std::unique_ptr<ASTnode> rhs) : 
                Op(op), LHS(std::move(lhs)), RHS(std::move(rhs)) {}

  const std::string &getOp() const {return Op;}
  std::unique_ptr<ASTnode> &getLHS() {return LHS; }
  std::unique_ptr<ASTnode> &getRHS() {return RHS; }

  virtual Value* codegen() override;

  virtual std::string to_string() const override {
    std::string result = std::string(COLOR_MAGENTA) + "BinaryExpr [" + 
                        std::string(COLOR_BOLD) + Op + std::string(COLOR_RESET) + "]\n";
    
    ASTPrint::indentLevel++;
    
    // Print left operand
    result += ASTPrint::indent() + ASTPrint::BRANCH;
    result += std::string(COLOR_BLUE) + "LHS: " + std::string(COLOR_RESET);
    if (LHS) {
      std::string lhsStr = LHS->to_string();
      if (lhsStr.find('\n') != std::string::npos) {
        result += "\n";
        ASTPrint::indentLevel++;
        result += ASTPrint::indent() + lhsStr;
        ASTPrint::indentLevel--;
      } else {
        result += lhsStr;
      }
    } else {
      result += std::string(COLOR_RED) + "nullptr" + std::string(COLOR_RESET);
    }
    result += "\n";
    
    // Print right operand
    result += ASTPrint::indent() + ASTPrint::LAST_BRANCH;
    result += std::string(COLOR_BLUE) + "RHS: " + std::string(COLOR_RESET);
    if (RHS) {
      std::string rhsStr = RHS->to_string();
      if (rhsStr.find('\n') != std::string::npos) {
        result += "\n";
        ASTPrint::indentLevel++;
        result += ASTPrint::indent() + rhsStr;
        ASTPrint::indentLevel--;
      } else {
        result += rhsStr;
      }
    } else {
      result += std::string(COLOR_RED) + "nullptr" + std::string(COLOR_RESET);
    }
    
    ASTPrint::indentLevel--;
    
    return result;
  }
};

/// UnaryExprAST - Expression class for unary operators
class UnaryExprAST : public ASTnode {
  std::string Op; 
  std::unique_ptr<ASTnode> Operand;

public:
  UnaryExprAST(std::string op, std::unique_ptr<ASTnode> operand)
      : Op(op), Operand(std::move(operand)) {}
  
  const std::string &getOp() const { return Op; }
  std::unique_ptr<ASTnode> &getOperand() { return Operand; }

  virtual Value* codegen() override;

  virtual std::string to_string() const override {
    std::string result = std::string(COLOR_MAGENTA) + "UnaryExpr" + std::string(COLOR_RESET) + " [" + 
                        std::string(COLOR_BOLD) + Op + std::string(COLOR_RESET) + "]\n";
    
    result += ASTPrint::indent() + ASTPrint::LAST_BRANCH;
    result += std::string(COLOR_BLUE) + "Operand: " + std::string(COLOR_RESET);
    if (Operand) {
      ASTPrint::indentLevel++;
      std::string opStr = Operand->to_string();
      if (opStr.find('\n') != std::string::npos) {
        result += "\n" + ASTPrint::indent() + opStr;
      } else {
        result += opStr;
      }
      ASTPrint::indentLevel--;
    } else {
      result += std::string(COLOR_RED) + "nullptr" + std::string(COLOR_RESET);
    }
    
    return result;
  }
};

/// CallExprAST - Expression class for function calls
class CallExprAST : public ASTnode {
  std::string Callee;  
  std::vector<std::unique_ptr<ASTnode>> Args;  

public:
  CallExprAST(const std::string &callee,
              std::vector<std::unique_ptr<ASTnode>> args)
      : Callee(callee), Args(std::move(args)) {}
  
  const std::string &getCallee() const { return Callee; }
  std::vector<std::unique_ptr<ASTnode>> &getArgs() { return Args; }

  virtual Value* codegen() override;

  virtual std::string to_string() const override {
  std::string result = std::string(COLOR_MAGENTA) + "FunctionCall '" + 
                      std::string(COLOR_BOLD) + Callee + std::string(COLOR_RESET) + "'\n";
  
  ASTPrint::indentLevel++;
  
  result += ASTPrint::indent() + ASTPrint::LAST_BRANCH;
  result += std::string(COLOR_BLUE) + "Arguments (" + std::to_string(Args.size()) + 
           "):" + std::string(COLOR_RESET);
  
  if (Args.empty()) {
    result += " " + std::string(COLOR_YELLOW) + "(none)" + std::string(COLOR_RESET);
  } else {
    result += "\n";
    ASTPrint::indentLevel++;
    for (size_t i = 0; i < Args.size(); i++) {
      bool isLast = (i == Args.size() - 1);
      result += ASTPrint::indent() + ASTPrint::treePrefix(isLast);
      result += std::string(COLOR_BLUE) + "Arg[" + std::to_string(i) + "]: " + std::string(COLOR_RESET);
      
      if (Args[i]) {
        std::string argStr = Args[i]->to_string();
        if (argStr.find('\n') != std::string::npos) {
          result += "\n";
          ASTPrint::indentLevel++;
          result += ASTPrint::indent() + argStr;
          ASTPrint::indentLevel--;
        } else {
          result += argStr;
        }
      } else {
        result += std::string(COLOR_RED) + "nullptr" + std::string(COLOR_RESET);
      }
      
      if (!isLast) result += "\n";
    }
    ASTPrint::indentLevel--;
  }
  
  ASTPrint::indentLevel--;
  
  return result;
}
};


/// AssignmentExprAST - Expression class for assignments
class AssignmentExprAST : public ASTnode {
  std::string VarName;  
  std::unique_ptr<ASTnode> RHS;  

public:
  AssignmentExprAST(const std::string &varname,
                    std::unique_ptr<ASTnode> rhs)
      : VarName(varname), RHS(std::move(rhs)) {}
  
  const std::string &getVarName() const { return VarName; }
  std::unique_ptr<ASTnode> &getRHS() { return RHS; }

  virtual Value* codegen() override;

  virtual std::string to_string() const override {
  std::string result = std::string(COLOR_MAGENTA) + "AssignmentExpr" + std::string(COLOR_RESET) + "\n";
  
  ASTPrint::indentLevel++;
  
  result += ASTPrint::indent() + ASTPrint::BRANCH;
  result += std::string(COLOR_BLUE) + "Target: " + std::string(COLOR_RESET) + 
           std::string(COLOR_BOLD) + VarName + std::string(COLOR_RESET) + "\n";
  
  result += ASTPrint::indent() + ASTPrint::LAST_BRANCH;
  result += std::string(COLOR_BLUE) + "Value: " + std::string(COLOR_RESET);
  
  if (RHS) {
    std::string rhsStr = RHS->to_string();
    if (rhsStr.find('\n') != std::string::npos) {
      result += "\n";
      ASTPrint::indentLevel++;
      result += ASTPrint::indent() + rhsStr;
      ASTPrint::indentLevel--;
    } else {
      result += rhsStr;
    }
  } else {
    result += std::string(COLOR_RED) + "nullptr" + std::string(COLOR_RESET);
  }
  
  ASTPrint::indentLevel--;
  
  return result;
}
};




//===----------------------------------------------------------------------===//
// Recursive Descent - Function call for each production
//===----------------------------------------------------------------------===//

static std::unique_ptr<ASTnode> ParseDecl();
static std::unique_ptr<ASTnode> ParseStmt();
static std::unique_ptr<ASTnode> ParseBlock();
static std::unique_ptr<ASTnode> ParseExper();
static std::unique_ptr<ParamAST> ParseParam();
static std::unique_ptr<VarDeclAST> ParseLocalDecl();
static std::vector<std::unique_ptr<ASTnode>> ParseStmtListPrime();

// element ::= FLOAT_LIT
static std::unique_ptr<ASTnode> ParseFloatNumberExpr() {
  auto Result = std::make_unique<FloatASTnode>(CurTok, CurTok.getFloatVal());
  getNextToken(); // consume the number
  return std::move(Result);
}

// element ::= INT_LIT
static std::unique_ptr<ASTnode> ParseIntNumberExpr() {
  auto Result = std::make_unique<IntASTnode>(CurTok, CurTok.getIntVal());
  getNextToken(); // consume the number
  return std::move(Result);
}

// element ::= BOOL_LIT
static std::unique_ptr<ASTnode> ParseBoolExpr() {
  auto Result = std::make_unique<BoolASTnode>(CurTok, CurTok.getBoolVal());
  getNextToken(); // consume the number
  return std::move(Result);
}

// param_list_prime ::= "," param param_list_prime
//                   |  ε
static std::vector<std::unique_ptr<ParamAST>> ParseParamListPrime() {
  std::vector<std::unique_ptr<ParamAST>> param_list;

  if (CurTok.type == COMMA) { // more parameters in list
    getNextToken();           // eat ","

    auto param = ParseParam();
    if (param) {
      printf("found param in param_list_prime: %s\n", param->getName().c_str());
      param_list.push_back(std::move(param));
      auto param_list_prime = ParseParamListPrime();
      for (unsigned i = 0; i < param_list_prime.size(); i++) {
        param_list.push_back(std::move(param_list_prime.at(i)));
      }
    }
  } else if (CurTok.type == RPAR) { // FOLLOW(param_list_prime)
    // expand by param_list_prime ::= ε
    // do nothing
  } else {
    LogError(CurTok, "expected ',' or ')' in list of parameter declarations");
  }

  return param_list;
}


// param ::= var_type IDENT
// param ::= var_type IDENT
static std::unique_ptr<ParamAST> ParseParam() {
  std::string Type = CurTok.lexeme; // keep track of the type of the param
  getNextToken();                   // eat the type token

  if (CurTok.type == IDENT) { // parameter declaration
    std::string Name = CurTok.getIdentifierStr();
    getNextToken(); // eat "IDENT"
    
    // Actually create and return the parameter!
    return std::make_unique<ParamAST>(Name, Type);
  } else {
    return LogError(CurTok, "expected identifier in parameter declaration"), nullptr;
  }

  return nullptr;
}

// param_list ::= param param_list_prime
static std::vector<std::unique_ptr<ParamAST>> ParseParamList() {
  std::vector<std::unique_ptr<ParamAST>> param_list;

  auto param = ParseParam();
  if (param) {
    param_list.push_back(std::move(param));
    auto param_list_prime = ParseParamListPrime();
    for (unsigned i = 0; i < param_list_prime.size(); i++) {
      param_list.push_back(std::move(param_list_prime.at(i)));
    }
  }

  return param_list;
}

// params ::= param_list
//         |  ε
static std::vector<std::unique_ptr<ParamAST>> ParseParams() {
  std::vector<std::unique_ptr<ParamAST>> param_list;

  std::string Type;
  std::string Name = "";

  if (CurTok.type == INT_TOK || CurTok.type == FLOAT_TOK ||
      CurTok.type == BOOL_TOK) { // FIRST(param_list)

    auto list = ParseParamList();
    for (unsigned i = 0; i < list.size(); i++) {
      param_list.push_back(std::move(list.at(i)));
    }

  } else if (CurTok.type == VOID_TOK) { // FIRST("void")
    // void
    // check that the next token is a )
    getNextToken(); // eat 'void'
    if (CurTok.type != RPAR) {
      LogError(CurTok, "expected ')', after 'void' in \
       end of function declaration");
    }
  } else if (CurTok.type == RPAR) { // FOLLOW(params)
    // expand by params ::= ε
    // do nothing
  } else {
    LogError(
        CurTok,
        "expected 'int', 'bool' or 'float' in function declaration or ') in \
       end of function declaration");
  }

  return param_list;
}

// TODO : Task 2 - Parser

// args ::= arg_list
//      |  ε

// arg_list ::= arg_list "," expr
//      | expr

// rval ::= rval "||" rval
//      | rval "&&" rval
//      | rval "==" rval | rval "!=" rval
//      | rval "<=" rval | rval "<" rval | rval ">=" rval | rval ">" rval
//      | rval "+" rval | rval "-" rval
//      | rval "*" rval | rval "/" rval | rval "%" rval
//      | "-" rval | "!" rval
//      | "(" expr ")"
//      | IDENT | IDENT "(" args ")"
//      | INT_LIT | FLOAT_LIT | BOOL_LIT


// args ::= arg_list
//      |  ε


// Helper function to parse function calls
// Called when we've seen "IDENT ("
static std::unique_ptr<ASTnode> ParseFunctionCall(const std::string &callee, TOKEN tok) {
  // At this point, we've already consumed IDENT and '('
  // Current token should be start of args or ')'
  
  std::vector<std::unique_ptr<ASTnode>> args;
  
  // Check if there are no arguments
  if (CurTok.type == RPAR) {
    // Empty argument list
    return std::make_unique<CallExprAST>(callee, std::move(args));
  }
  
  // Parse first argument
  auto arg = ParseExper();
  if (!arg)
    return nullptr;
  args.push_back(std::move(arg));
  
  // Parse remaining arguments (if any)
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
//              | INT_LIT
//              | FLOAT_LIT
//              | BOOL_LIT
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
      
      auto call = ParseFunctionCall(idName, idTok);
      if (!call)
        return nullptr;
      
      if (CurTok.type != RPAR)
        return LogError(CurTok, "expected ')' after arguments");
      
      getNextToken(); // eat ')'
      return call;
    }
    
    // Just a variable reference
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
  
  // Case 1: Unary minus
  if (CurTok.type == MINUS) {
    getNextToken(); // eat '-'
    auto operand = ParseUnaryExpr();
    if (!operand)
      return nullptr;
    
    return std::make_unique<UnaryExprAST>("-", std::move(operand));
  }
  
  // Case 2: Unary not
  if (CurTok.type == NOT) {
    getNextToken(); // eat '!'
    auto operand = ParseUnaryExpr(); 
    if (!operand)
      return nullptr;
    
    return std::make_unique<UnaryExprAST>("!", std::move(operand));
  }
  
  // Case 3: Primary expression (no unary operator)
  return ParsePrimaryExpr();
}


// mul_expr ::= unary_expr mul_expr_prime
// mul_expr_prime ::= "*" unary_expr mul_expr_prime
//                | "/" unary_expr mul_expr_prime
//                | "%" unary_expr mul_expr_prime
//                | ε
static std::unique_ptr<ASTnode> ParseMulExpr() {
  // Parse the left-hand side (a unary expression)
  auto LHS = ParseUnaryExpr();
  if (!LHS)
    return nullptr;
  
  // Parse mul_expr_prime (handle *, /, % operators)
  while (CurTok.type == ASTERIX || CurTok.type == DIV || CurTok.type == MOD) {
    std::string op;
    
    if (CurTok.type == ASTERIX)
      op = "*";
    else if (CurTok.type == DIV)
      op = "/";
    else if (CurTok.type == MOD)
      op = "%";
    
    getNextToken(); // eat the operator
    
    // Parse the right-hand side
    auto RHS = ParseUnaryExpr();
    if (!RHS)
      return nullptr;
    
    // Create binary expression and make it the new LHS
    LHS = std::make_unique<BinaryExprAST>(op, std::move(LHS), std::move(RHS));
  }
  
  return LHS;
}

// add_expr ::= mul_expr add_expr_prime
// add_expr_prime ::= "+" mul_expr add_expr_prime
//                | "-" mul_expr add_expr_prime
//                | ε
static std::unique_ptr<ASTnode> ParseAddExpr() {
  auto LHS = ParseMulExpr(); // Parse higher precedence first
  if (!LHS)
    return nullptr;
  
  // Handle + and - operators
  while (CurTok.type == PLUS || CurTok.type == MINUS) {
    std::string op = (CurTok.type == PLUS) ? "+" : "-";
    getNextToken(); // eat operator
    
    auto RHS = ParseMulExpr(); // Parse higher precedence on right
    if (!RHS)
      return nullptr;
    
    LHS = std::make_unique<BinaryExprAST>(op, std::move(LHS), std::move(RHS));
  }
  
  return LHS;
}

// rel_expr ::= add_expr rel_expr_prime
// rel_expr_prime ::= "<=" add_expr rel_expr_prime
//                | "<" add_expr rel_expr_prime
//                | ">=" add_expr rel_expr_prime
//                | ">" add_expr rel_expr_prime
//                | ε
static std::unique_ptr<ASTnode> ParseRelExpr() {
  auto LHS = ParseAddExpr();
  if (!LHS)
    return nullptr;
  
  // Handle <, <=, >, >= operators
  while (CurTok.type == LT || CurTok.type == LE || 
         CurTok.type == GT || CurTok.type == GE) {
    std::string op;
    
    if (CurTok.type == LT)
      op = "<";
    else if (CurTok.type == LE)
      op = "<=";
    else if (CurTok.type == GT)
      op = ">";
    else if (CurTok.type == GE)
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
// eq_expr_prime ::= "==" rel_expr eq_expr_prime
//               | "!=" rel_expr eq_expr_prime
//               | ε
static std::unique_ptr<ASTnode> ParseEqExpr() {
  auto LHS = ParseRelExpr();
  if (!LHS)
    return nullptr;
  
  // Handle == and != operators
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
// and_expr_prime ::= "&&" eq_expr and_expr_prime
//                | ε
static std::unique_ptr<ASTnode> ParseAndExpr() {
  auto LHS = ParseEqExpr();
  if (!LHS)
    return nullptr;
  
  // Handle && operator
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
// or_expr_prime ::= "||" and_expr or_expr_prime
//               | ε
static std::unique_ptr<ASTnode> ParseOrExpr() {
  auto LHS = ParseAndExpr();
  if (!LHS)
    return nullptr;
  
  // Handle || operator
  while (CurTok.type == OR) {
    getNextToken(); // eat '||'
    
    auto RHS = ParseAndExpr();
    if (!RHS)
      return nullptr;
    
    LHS = std::make_unique<BinaryExprAST>("||", std::move(LHS), std::move(RHS));
  }
  
  return LHS;
}


/// ParseExper - Parse an expression with LL(2) lookahead.
///
/// Grammar:
///   expr ::= IDENT "=" expr
///         |  or_expr
///
/// This production requires LL(2) lookahead because both alternatives
/// begin with IDENT. We must peek at the second token to decide:
///   - FIRST₂(IDENT "=" expr) = {(IDENT, =)}
///   - FIRST₂(or_expr)        = {(IDENT, +), (IDENT, *), ...}
///
/// Implementation uses explicit lookahead rather than backtracking
/// for proper predictive parsing.
static std::unique_ptr<ASTnode> ParseExper() {
    PARSER_ENTER("ParseExper", CurTok);
    
    if (CurTok.type == IDENT) {
        TOKEN nextTok = peekNextToken();
        
        DEBUG_PARSER("Checking for assignment: IDENT='" + CurTok.lexeme + 
                    "', next token='" + nextTok.lexeme + "'");
        
        if (nextTok.type == ASSIGN) {
            std::string varName = CurTok.getIdentifierStr();
            DEBUG_PARSER("Found assignment to variable '" + varName + "'");
            
            getNextToken();
            getNextToken();
            
            auto RHS = ParseExper();
            if (!RHS) {
                PARSER_EXIT("ParseExper", false);
                return nullptr;
            }
            
            auto result = std::make_unique<AssignmentExprAST>(varName, std::move(RHS));
            PARSER_EXIT("ParseExper", true);
            return result;
        }
    }
    
    auto result = ParseOrExpr();
    PARSER_EXIT("ParseExper", result != nullptr);
    return result;
}


// expr_stmt ::= expr ";"
//            |  ";"
static std::unique_ptr<ASTnode> ParseExperStmt() {

  if (CurTok.type == SC) { // empty statement
    getNextToken();        // eat ;
    return nullptr;
  } else {
    auto expr = ParseExper();
    if (expr) {
      if (CurTok.type == SC) {
        getNextToken(); // eat ;
        return expr;
      } else {
        LogError(CurTok, "expected ';' to end expression statement");
      }
    } else
      return nullptr;
  }
  return nullptr;
}

// else_stmt  ::= "else" block
//             |  ε
static std::unique_ptr<ASTnode> ParseElseStmt() {

  if (CurTok.type == ELSE) { // FIRST(else_stmt)
    // expand by else_stmt  ::= "else" "{" stmt "}"
    getNextToken(); // eat "else"

    if (!(CurTok.type == LBRA)) {
      return LogError(
          CurTok, "expected { to start else block of if-then-else statment");
    }
    auto Else = ParseBlock();
    if (!Else)
      return nullptr;
    return Else;
  } else if (CurTok.type == NOT || CurTok.type == MINUS ||
             CurTok.type == PLUS || CurTok.type == LPAR ||
             CurTok.type == IDENT || CurTok.type == INT_LIT ||
             CurTok.type == BOOL_LIT || CurTok.type == FLOAT_LIT ||
             CurTok.type == SC || CurTok.type == LBRA || CurTok.type == WHILE ||
             CurTok.type == IF || CurTok.type == ELSE ||
             CurTok.type == RETURN ||
             CurTok.type == RBRA) { // FOLLOW(else_stmt)
    // expand by else_stmt  ::= ε
    // return an empty statement
    return nullptr;
  } else
    LogError(CurTok, "expected 'else' or one of \
    '!', '-', '+', '(' , IDENT , INT_LIT, BOOL_LIT, FLOAT_LIT, ';', \
    '{', 'while', 'if', 'else', ε, 'return', '}' ");

  return nullptr;
}

// if_stmt ::= "if" "(" expr ")" block else_stmt
static std::unique_ptr<ASTnode> ParseIfStmt() {
  getNextToken(); // eat the if.
  if (CurTok.type == LPAR) {
    getNextToken(); // eat (
    // condition.
    auto Cond = ParseExper();
    if (!Cond)
      return nullptr;
    if (CurTok.type != RPAR)
      return LogError(CurTok, "expected )");
    getNextToken(); // eat )

    if (!(CurTok.type == LBRA)) {
      return LogError(CurTok, "expected { to start then block of if statment");
    }

    auto Then = ParseBlock();
    if (!Then)
      return nullptr;
    auto Else = ParseElseStmt();

    return std::make_unique<IfExprAST>(std::move(Cond), std::move(Then),
                                       std::move(Else));

  } else
    return LogError(CurTok, "expected (");

  return nullptr;
}

// return_stmt ::= "return" ";"
//             |  "return" expr ";"
static std::unique_ptr<ASTnode> ParseReturnStmt() {
  getNextToken(); // eat the return
  if (CurTok.type == SC) {
    getNextToken(); // eat the ;
    // return a null value
    return std::make_unique<ReturnAST>(std::move(nullptr));
  } else if (CurTok.type == NOT || CurTok.type == MINUS ||
             CurTok.type == PLUS || CurTok.type == LPAR ||
             CurTok.type == IDENT || CurTok.type == BOOL_LIT ||
             CurTok.type == INT_LIT ||
             CurTok.type == FLOAT_LIT) { // FIRST(expr)
    auto val = ParseExper();
    if (!val)
      return nullptr;

    if (CurTok.type == SC) {
      getNextToken(); // eat the ;
      return std::make_unique<ReturnAST>(std::move(val));
    } else
      return LogError(CurTok, "expected ';'");
  } else
    return LogError(CurTok, "expected ';' or expression");

  return nullptr;
}

// while_stmt ::= "while" "(" expr ")" stmt
static std::unique_ptr<ASTnode> ParseWhileStmt() {

  getNextToken(); // eat the while.
  if (CurTok.type == LPAR) {
    getNextToken(); // eat (
    // condition.
    auto Cond = ParseExper();
    if (!Cond)
      return nullptr;
    if (CurTok.type != RPAR)
      return LogError(CurTok, "expected )");
    getNextToken(); // eat )

    auto Body = ParseStmt();
    if (!Body)
      return nullptr;

    return std::make_unique<WhileExprAST>(std::move(Cond), std::move(Body));
  } else
    return LogError(CurTok, "expected (");
}

// stmt ::= expr_stmt
//      |  block
//      |  if_stmt
//      |  while_stmt
//      |  return_stmt
static std::unique_ptr<ASTnode> ParseStmt() {

  if (CurTok.type == NOT || CurTok.type == MINUS || CurTok.type == PLUS ||
      CurTok.type == LPAR || CurTok.type == IDENT || CurTok.type == BOOL_LIT ||
      CurTok.type == INT_LIT || CurTok.type == FLOAT_LIT ||
      CurTok.type == SC) { // FIRST(expr_stmt)
    // expand by stmt ::= expr_stmt
    auto expr_stmt = ParseExperStmt();
    fprintf(stderr, "Parsed an expression statement\n");
    return expr_stmt;
  } else if (CurTok.type == LBRA) { // FIRST(block)
    auto block_stmt = ParseBlock();
    if (block_stmt) {
      fprintf(stderr, "Parsed a block\n");
      return block_stmt;
    }
  } else if (CurTok.type == IF) { // FIRST(if_stmt)
    auto if_stmt = ParseIfStmt();
    if (if_stmt) {
      fprintf(stderr, "Parsed an if statment\n");
      return if_stmt;
    }
  } else if (CurTok.type == WHILE) { // FIRST(while_stmt)
    auto while_stmt = ParseWhileStmt();
    if (while_stmt) {
      fprintf(stderr, "Parsed a while statment\n");
      return while_stmt;
    }
  } else if (CurTok.type == RETURN) { // FIRST(return_stmt)
    auto return_stmt = ParseReturnStmt();
    if (return_stmt) {
      fprintf(stderr, "Parsed a return statment\n");
      return return_stmt;
    }
  }
  // else if(CurTok.type == RBRA) { // FOLLOW(stmt_list_prime)
  //  expand by stmt_list_prime ::= ε
  //  do nothing
  //}
  else { // syntax error
    return LogError(CurTok, "expected BLA BLA\n");
  }
  return nullptr;
}

// stmt_list ::= stmt stmt_list_prime
static std::vector<std::unique_ptr<ASTnode>> ParseStmtList() {
  std::vector<std::unique_ptr<ASTnode>> stmt_list; // vector of statements
  auto stmt = ParseStmt();
  if (stmt) {
    stmt_list.push_back(std::move(stmt));
  }
  auto stmt_list_prime = ParseStmtListPrime();
  for (unsigned i = 0; i < stmt_list_prime.size(); i++) {
    stmt_list.push_back(std::move(stmt_list_prime.at(i)));
  }
  return stmt_list;
}

// stmt_list_prime ::= stmt stmt_list_prime
//                  |  ε
static std::vector<std::unique_ptr<ASTnode>> ParseStmtListPrime() {
  std::vector<std::unique_ptr<ASTnode>> stmt_list; // vector of statements
  if (CurTok.type == NOT || CurTok.type == MINUS || CurTok.type == PLUS ||
      CurTok.type == LPAR || CurTok.type == IDENT || CurTok.type == BOOL_LIT ||
      CurTok.type == INT_LIT || CurTok.type == FLOAT_LIT || CurTok.type == SC ||
      CurTok.type == LBRA || CurTok.type == WHILE || CurTok.type == IF ||
      CurTok.type == ELSE || CurTok.type == RETURN) { // FIRST(stmt)
    // expand by stmt_list ::= stmt stmt_list_prime
    auto stmt = ParseStmt();
    if (stmt) {
      stmt_list.push_back(std::move(stmt));
    }
    auto stmt_prime = ParseStmtListPrime();
    for (unsigned i = 0; i < stmt_prime.size(); i++) {
      stmt_list.push_back(std::move(stmt_prime.at(i)));
    }

  } else if (CurTok.type == RBRA) { // FOLLOW(stmt_list_prime)
    // expand by stmt_list_prime ::= ε
    // do nothing
  }
  return stmt_list; // note stmt_list can be empty as we can have empty blocks,
                    // etc.
}

// local_decls_prime ::= local_decl local_decls_prime
//                    |  ε
static std::vector<std::unique_ptr<VarDeclAST>> ParseLocalDeclsPrime() {
  std::vector<std::unique_ptr<VarDeclAST>>
      local_decls_prime; // vector of local decls

  if (CurTok.type == INT_TOK || CurTok.type == FLOAT_TOK ||
      CurTok.type == BOOL_TOK) { // FIRST(local_decl)
    auto local_decl = ParseLocalDecl();
    if (local_decl) {
      local_decls_prime.push_back(std::move(local_decl));
    }
    auto prime = ParseLocalDeclsPrime();
    for (unsigned i = 0; i < prime.size(); i++) {
      local_decls_prime.push_back(std::move(prime.at(i)));
    }
  } else if (CurTok.type == MINUS || CurTok.type == NOT ||
             CurTok.type == LPAR || CurTok.type == IDENT ||
             CurTok.type == INT_LIT || CurTok.type == FLOAT_LIT ||
             CurTok.type == BOOL_LIT || CurTok.type == SC ||
             CurTok.type == LBRA || CurTok.type == IF || CurTok.type == WHILE ||
             CurTok.type == RETURN) { // FOLLOW(local_decls_prime)
    // expand by local_decls_prime ::=  ε
    // do nothing;
  } else {
    LogError(
        CurTok,
        "expected '-', '!', ('' , IDENT , STRING_LIT , INT_LIT , FLOAT_LIT, \
      BOOL_LIT, ';', '{', 'if', 'while', 'return' after local variable declaration\n");
  }

  return local_decls_prime;
}

// local_decl ::= var_type IDENT ";"
// var_type ::= "int"
//           |  "float"
//           |  "bool"
static std::unique_ptr<VarDeclAST> ParseLocalDecl() {
  TOKEN PrevTok;
  std::string Type;
  std::string Name = "";
  std::unique_ptr<VarDeclAST> local_decl;

  if (CurTok.type == INT_TOK || CurTok.type == FLOAT_TOK ||
      CurTok.type == BOOL_TOK) { // FIRST(var_type)
    PrevTok = CurTok;
    getNextToken(); // eat 'int' or 'float or 'bool'
    if (CurTok.type == IDENT) {
      Type = PrevTok.lexeme;
      Name = CurTok.getIdentifierStr(); // save the identifier name
      auto ident = std::make_unique<VariableASTnode>(CurTok, Name);
      local_decl = std::make_unique<VarDeclAST>(std::move(ident), Type);

      getNextToken(); // eat 'IDENT'
      if (CurTok.type != SC) {
        LogError(CurTok, "Expected ';' to end local variable declaration");
        return nullptr;
      }
      getNextToken(); // eat ';'
      fprintf(stderr, "Parsed a local variable declaration\n");
    } else {
      LogError(CurTok, "expected identifier' in local variable declaration");
      return nullptr;
    }
  }
  return local_decl;
}

// local_decls ::= local_decl local_decls_prime
static std::vector<std::unique_ptr<VarDeclAST>> ParseLocalDecls() {
  std::vector<std::unique_ptr<VarDeclAST>> local_decls; // vector of local decls

  if (CurTok.type == INT_TOK || CurTok.type == FLOAT_TOK ||
      CurTok.type == BOOL_TOK) { // FIRST(local_decl)

    auto local_decl = ParseLocalDecl();
    if (local_decl) {
      local_decls.push_back(std::move(local_decl));
    }
    auto local_decls_prime = ParseLocalDeclsPrime();
    for (unsigned i = 0; i < local_decls_prime.size(); i++) {
      local_decls.push_back(std::move(local_decls_prime.at(i)));
    }

  } else if (CurTok.type == MINUS || CurTok.type == NOT ||
             CurTok.type == LPAR || CurTok.type == IDENT ||
             CurTok.type == INT_LIT || CurTok.type == RETURN ||
             CurTok.type == FLOAT_LIT || CurTok.type == BOOL_LIT ||
             CurTok.type == COMMA || CurTok.type == LBRA || CurTok.type == IF ||
             CurTok.type == WHILE) { // FOLLOW(local_decls)
                                     // do nothing
  } else {
    LogError(
        CurTok,
        "expected '-', '!', '(' , IDENT , STRING_LIT , INT_LIT , FLOAT_LIT, \
        BOOL_LIT, ';', '{', 'if', 'while', 'return'");
  }

  return local_decls;
}

// parse block
// block ::= "{" local_decls stmt_list "}"
static std::unique_ptr<ASTnode> ParseBlock() {
  std::vector<std::unique_ptr<VarDeclAST>> local_decls; // vector of local decls
  std::vector<std::unique_ptr<ASTnode>> stmt_list;      // vector of statements

  getNextToken(); // eat '{'

  local_decls = ParseLocalDecls();
  fprintf(stderr, "Parsed a set of local variable declaration\n");
  stmt_list = ParseStmtList();
  fprintf(stderr, "Parsed a list of statements\n");
  if (CurTok.type == RBRA)
    getNextToken(); // eat '}'
  else {            // syntax error
    LogError(CurTok, "expected '}' , close body of block");
    return nullptr;
  }

  return std::make_unique<BlockAST>(std::move(local_decls),
                                    std::move(stmt_list));
}

// decl ::= var_decl
//       |  fun_decl
static std::unique_ptr<ASTnode> ParseDecl() {
  std::string IdName;
  std::vector<std::unique_ptr<ParamAST>> param_list;

  TOKEN PrevTok = CurTok; // to keep track of the type token

  if (CurTok.type == VOID_TOK || CurTok.type == INT_TOK ||
      CurTok.type == FLOAT_TOK || CurTok.type == BOOL_TOK) {
    getNextToken(); // eat the VOID_TOK, INT_TOK, BOOL_TOK or FLOAT_TOK

    IdName = CurTok.getIdentifierStr(); // save the identifier name

    if (CurTok.type == IDENT) {
      auto ident = std::make_unique<VariableASTnode>(CurTok, IdName);
      getNextToken(); // eat the IDENT
      if (CurTok.type ==
          SC) {         // found ';' then this is a global variable declaration.
        getNextToken(); // eat ;
        
        fprintf(stderr, "Parsed a variable declaration\n");

        if (PrevTok.type != VOID_TOK) {
          // Declare as ASTnode pointer
          std::unique_ptr<ASTnode> globVar = std::make_unique<GlobVarDeclAST>(
              std::move(ident), PrevTok.lexeme);

          globVar->codegen();
          
          printAST(globVar, "Global Variable: " + IdName);
          return globVar;
        } else {
          return LogError(PrevTok,
                          "Cannot have variable declaration with type 'void'");
        }
      } else if (CurTok.type ==
                 LPAR) { // found '(' then this is a function declaration.
        getNextToken();  // eat (

        auto P =
            ParseParams(); // parse the parameters, returns a vector of params
        // if (P.size() == 0) return nullptr;
        fprintf(stderr, "Parsed parameter list for function\n");

        if (CurTok.type != RPAR) // syntax error
          return LogError(CurTok, "expected ')' in function declaration");

        getNextToken();          // eat )
        if (CurTok.type != LBRA) // syntax error
          return LogError(
              CurTok, "expected '{' in function declaration, function body");

        auto B = ParseBlock(); // parse the function body
        if (!B)
          return nullptr;
        else
          fprintf(stderr, "Parsed block of statements in function\n");

        // now create a Function prototype
        // create a Function body
        // put these to together
        // and return a std::unique_ptr<FunctionDeclAST>
        fprintf(stderr, "Parsed a function declaration\n");

        auto Proto = std::make_unique<FunctionPrototypeAST>(
            IdName, PrevTok.lexeme, std::move(P));
        std::unique_ptr<ASTnode> funcDecl = std::make_unique<FunctionDeclAST>(std::move(Proto),
                                                                       std::move(B));
        
        funcDecl->codegen();
        printAST(funcDecl, "Function: " + IdName);
        return funcDecl;
      } else
        return LogError(CurTok, "expected ';' or ('");
    } else
      return LogError(CurTok, "expected an identifier");

  } else
    LogError(CurTok,
             "expected 'void', 'int' or 'float' or EOF token"); // syntax error

  return nullptr;
}

// decl_list_prime ::= decl decl_list_prime
//                  |  ε
static void ParseDeclListPrime() {
  if (CurTok.type == VOID_TOK || CurTok.type == INT_TOK ||
      CurTok.type == FLOAT_TOK || CurTok.type == BOOL_TOK) { // FIRST(decl)

    if (auto decl = ParseDecl()) {
      fprintf(stderr, "Parsed a top-level variable or function declaration\n");
    }
    ParseDeclListPrime();
  } else if (CurTok.type == EOF_TOK) { // FOLLOW(decl_list_prime)
    // expand by decl_list_prime ::= ε
    // do nothing
  } else { // syntax error
    LogError(CurTok, "expected 'void', 'int', 'bool' or 'float' or EOF token");
  }
}

// decl_list ::= decl decl_list_prime
static void ParseDeclList() {
  auto decl = ParseDecl();
  if (decl) {
    fprintf(stderr, "Parsed a top-level variable or function declaration\n");
    ParseDeclListPrime();
  }
}

// extern ::= "extern" type_spec IDENT "(" params ")" ";"
static std::unique_ptr<FunctionPrototypeAST> ParseExtern() {
  std::string IdName;
  TOKEN PrevTok;

  if (CurTok.type == EXTERN) {
    getNextToken(); // eat the EXTERN

    if (CurTok.type == VOID_TOK || CurTok.type == INT_TOK ||
        CurTok.type == FLOAT_TOK || CurTok.type == BOOL_TOK) {

      PrevTok = CurTok; // to keep track of the type token
      getNextToken();   // eat the VOID_TOK, INT_TOK, BOOL_TOK or FLOAT_TOK

      if (CurTok.type == IDENT) {
        IdName = CurTok.getIdentifierStr(); // save the identifier name
        auto ident = std::make_unique<VariableASTnode>(CurTok, IdName);
        getNextToken(); // eat the IDENT

        if (CurTok.type ==
            LPAR) {       // found '(' - this is an extern function declaration.
          getNextToken(); // eat (

          auto P =
              ParseParams(); // parse the parameters, returns a vector of params
          if (P.size() == 0)
            return nullptr;
          else
            fprintf(stderr, "Parsed parameter list for external function\n");

          if (CurTok.type != RPAR) // syntax error
            return LogErrorP(
                CurTok, "expected ')' in closing extern function declaration");

          getNextToken(); // eat )

          if (CurTok.type == SC) {
            getNextToken(); // eat ";"
            auto Proto = std::make_unique<FunctionPrototypeAST>(
                IdName, PrevTok.lexeme, std::move(P));
            return Proto;
          } else
            return LogErrorP(
                CurTok,
                "expected ;' in ending extern function declaration statement");
        } else
          return LogErrorP(CurTok,
                           "expected (' in extern function declaration");
      }

    } else
      LogErrorP(CurTok, "expected 'void', 'int' or 'float' in extern function "
                        "declaration\n"); // syntax error
  }

  return nullptr;
}

// extern_list_prime ::= extern extern_list_prime
//                   |  ε

static void ParseExternListPrime() {
  if (CurTok.type == EXTERN) { // FIRST(extern)
    if (auto Extern = ParseExtern()) {
      fprintf(stderr, "Parsed a top-level external function declaration -- 2\n");
      
      // Generate code for external function declaration
      if (Function* ExternF = Extern->codegen()) {
          fprintf(stderr, "Generated code for external function: %s\n", 
                  Extern->getName().c_str());
      } else {
          fprintf(stderr, "Error generating code for external function: %s\n",
                  Extern->getName().c_str());
      }
    }
    ParseExternListPrime();
  } else if (CurTok.type == VOID_TOK || CurTok.type == INT_TOK ||
             CurTok.type == FLOAT_TOK ||
             CurTok.type == BOOL_TOK) { // FOLLOW(extern_list_prime)
    // expand by decl_list_prime ::= ε
    // do nothing
  } else { // syntax error
    LogError(CurTok, "expected 'extern' or 'void', 'int', 'float', 'bool'");
  }
}

// extern_list ::= extern extern_list_prime
static void ParseExternList() {
  auto Extern = ParseExtern();
  if (Extern) {
    fprintf(stderr, "Parsed a top-level external function declaration -- 1\n");
    
    // Generate code for external function declaration
    if (Function* ExternF = Extern->codegen()) {
        fprintf(stderr, "Generated code for external function: %s\n", 
                Extern->getName().c_str());
    } else {
        fprintf(stderr, "Error generating code for external function: %s\n",
                Extern->getName().c_str());
    }
    
    if (CurTok.type == EXTERN)
      ParseExternListPrime();
  }
}



// program ::= extern_list decl_list
static void parser() {
  if (CurTok.type == EOF_TOK)
    return;
  ParseExternList();
  if (CurTok.type == EOF_TOK)
    return;
  ParseDeclList();
  if (CurTok.type == EOF_TOK)
    return;
}

//===----------------------------------------------------------------------===//
// Code Generation
//===----------------------------------------------------------------------===//


//===----------------------------------------------------------------------===//
// Symbol Tables and Helper Functions
//===----------------------------------------------------------------------===//

/// getTypeFromString - Convert MiniC type string to LLVM Type*
static Type* getTypeFromString(const std::string& typeStr) {
    if (typeStr == "int")
        return Type::getInt32Ty(TheContext);
    if (typeStr == "float")
        return Type::getFloatTy(TheContext);
    if (typeStr == "bool")
        return Type::getInt1Ty(TheContext);
    if (typeStr == "void")
        return Type::getVoidTy(TheContext);
    
    fprintf(stderr, "Error: Unknown type '%s'\n", typeStr.c_str());
    return nullptr;
}

/// CreateEntryBlockAlloca - Create alloca in entry block of function
/// From LLVM Tutorial Chapter 7
static AllocaInst* CreateEntryBlockAlloca(Function *TheFunction,
                                          const std::string &VarName,
                                          Type* VarType) {
    IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
                     TheFunction->getEntryBlock().begin());
    return TmpB.CreateAlloca(VarType, nullptr, VarName);
}


//===----------------------------------------------------------------------===//
// Code Generation - AST Node Implementations
//===----------------------------------------------------------------------===//

/// IntASTnode::codegen - Generate LLVM IR for integer literals
Value* IntASTnode::codegen() {
    DEBUG_CODEGEN("Generating integer literal: " + std::to_string(Val));
    return ConstantInt::get(Type::getInt32Ty(TheContext), APInt(32, Val, true));
}

/// FloatASTnode::codegen - Generate LLVM IR for float literals
Value* FloatASTnode::codegen() {
    DEBUG_CODEGEN("Generating float literal: " + std::to_string(Val));
    return ConstantFP::get(Type::getFloatTy(TheContext), APFloat((float)Val));
}

/// BoolASTnode::codegen - Generate LLVM IR for boolean literals
Value* BoolASTnode::codegen() {
    DEBUG_CODEGEN("Generating boolean literal: " + std::string(Bool ? "true" : "false"));
    return ConstantInt::get(Type::getInt1Ty(TheContext), APInt(1, Bool ? 1 : 0, false));
}

/// checkVariableInScope - Check if variable is in scope and return its type
static TypeInfo* checkVariableInScope(const std::string& varName, int line = -1, int col = -1) {
    DEBUG_CODEGEN("Checking scope for variable: " + varName);
    
    // Check local scope first
    if (NamedValues.find(varName) != NamedValues.end()) {
        TypeInfo* info = getTypeInfo(varName);
        if (info) {
            DEBUG_CODEGEN("  Found in local scope: " + info->typeName);
            return info;
        }
    }
    
    // Check global scope
    if (GlobalValues.find(varName) != GlobalValues.end()) {
        TypeInfo* info = getTypeInfo(varName);
        if (info) {
            DEBUG_CODEGEN("  Found in global scope: " + info->typeName);
            return info;
        }
    }
    
    // Variable not found
    DEBUG_CODEGEN("  ERROR: Variable not found in any scope");
    std::string msg = "Undefined variable '" + varName + "'";
    if (CurrentContext.currentFunction.empty()) {
        msg += " in global scope";
    } else {
        msg += " in function '" + CurrentContext.currentFunction + "'";
    }
    LogCompilerError(ErrorType::SEMANTIC_SCOPE, msg, line, col);
    return nullptr;
}

/// VariableASTnode::codegen - Generate LLVM IR for variable references
Value* VariableASTnode::codegen() {
    DEBUG_CODEGEN("Loading variable: " + Name);
    
    // Check scope and get type information
    TypeInfo* typeInfo = checkVariableInScope(Name, Tok.lineNo, Tok.columnNo);
    if (!typeInfo) {
        // Error already logged by checkVariableInScope
        DUMP_SYMBOL_TABLE();
        return nullptr;
    }
    
    // Try local scope first
    AllocaInst* V = NamedValues[Name];
    if (V) {
        DEBUG_CODEGEN("  Found in local scope: " + getTypeName(V->getAllocatedType()));
        DEBUG_CODEGEN("  Type from symbol table: " + typeInfo->typeName);
        
        // Verify type consistency
        Type* expectedType = getTypeFromString(typeInfo->typeName);
        if (V->getAllocatedType() != expectedType) {
            LogCompilerError(ErrorType::SEMANTIC_TYPE,
                           "Type mismatch for variable '" + Name + "'",
                           Tok.lineNo, Tok.columnNo);
            return nullptr;
        }
        
        return Builder.CreateLoad(V->getAllocatedType(), V, Name.c_str());
    }
    
    // Try global scope
    GlobalVariable* GV = GlobalValues[Name];
    if (GV) {
        DEBUG_CODEGEN("  Found in global scope: " + getTypeName(GV->getValueType()));
        DEBUG_CODEGEN("  Type from symbol table: " + typeInfo->typeName);
        
        // Verify type consistency
        Type* expectedType = getTypeFromString(typeInfo->typeName);
        if (GV->getValueType() != expectedType) {
            LogCompilerError(ErrorType::SEMANTIC_TYPE,
                           "Type mismatch for global variable '" + Name + "'",
                           Tok.lineNo, Tok.columnNo);
            return nullptr;
        }
        
        return Builder.CreateLoad(GV->getValueType(), GV, Name.c_str());
    }
    
    // Should never reach here if checkVariableInScope worked correctly
    DEBUG_CODEGEN("  INTERNAL ERROR: Variable found in symbol table but not in scope maps");
    DUMP_SYMBOL_TABLE();
    return nullptr;
}

//===----------------------------------------------------------------------===//
// Type Conversion Helpers
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Enhanced Type System
//===----------------------------------------------------------------------===//






/// registerVariable - Register a variable in the symbol table with type info
static void registerVariable(const std::string& varName, const std::string& typeName, 
                             bool isGlobal = false, int line = -1, int col = -1) {
    SymbolTypeTable[varName] = TypeInfo(typeName, isGlobal, line, col);
    DEBUG_VERBOSE("Registered variable '" + varName + "' with type '" + typeName + 
                 "' (global: " + (isGlobal ? "yes" : "no") + ")");
}



/// checkFunctionExists - Check if function is declared
static Function* checkFunctionExists(const std::string& funcName, int line = -1, int col = -1) {
    DEBUG_CODEGEN("Checking function: " + funcName);
    
    Function* F = TheModule->getFunction(funcName);
    if (!F) {
        DEBUG_CODEGEN("  ERROR: Function not found");
        std::string msg = "Call to undefined function '" + funcName + "'";
        
        // Provide suggestions for similar function names
        std::string suggestion;
        for (auto& Fn : TheModule->functions()) {
            std::string fnName = Fn.getName().str();
            if (fnName.find(funcName.substr(0, std::min((size_t)3, funcName.length()))) != std::string::npos) {
                suggestion = "\n  Did you mean '" + fnName + "'?";
                break;
            }
        }
        
        LogCompilerError(ErrorType::SEMANTIC_SCOPE, msg + suggestion, line, col);
        return nullptr;
    }
    
    DEBUG_CODEGEN("  Function found: " + getTypeName(F->getFunctionType()));
    return F;
}

/// getValueType - Get the LLVM type of a Value
static Type* getValueType(Value* V) {
    if (!V) return nullptr;
    return V->getType();
}

/// isNarrowingConversion - Check if conversion from From to To is narrowing
static bool isNarrowingConversion(Type* From, Type* To) {
    if (!From || !To) return false;
    if (From == To) return false;
    
    if (From->isFloatTy() && To->isIntegerTy(32))
        return true;
    if (From->isIntegerTy(32) && To->isIntegerTy(1))
        return true;
    if (From->isDoubleTy() && To->isFloatTy())
        return true;
    if (From->isFloatTy() && To->isIntegerTy(1))
        return true;
    
    return false;
}

/// isWideningConversion - Check if conversion is widening (safe)
static bool isWideningConversion(Type* From, Type* To) {
    if (!From || !To) return false;
    
    // int to float is widening
    if (From->isIntegerTy(32) && To->isFloatTy())
        return true;
    
    // bool to int is widening
    if (From->isIntegerTy(1) && To->isIntegerTy(32))
        return true;
    
    // bool to float is widening (through int)
    if (From->isIntegerTy(1) && To->isFloatTy())
        return true;
    
    // float to double is widening
    if (From->isFloatTy() && To->isDoubleTy())
        return true;
    
    return false;
}

/// castToType - Perform type conversions with proper checking
static Value* castToType(Value* V, Type* DestTy, bool allowNarrowing = true, 
                         const std::string& context = "") {
    if (!V || !DestTy) {
        DEBUG_CODEGEN("  ERROR: Null value or type in castToType");
        return nullptr;
    }
    
    Type* SrcTy = V->getType();
    
    if (SrcTy == DestTy)
        return V;
    
    DEBUG_VERBOSE("  Type conversion needed: " + getTypeName(SrcTy) + " -> " + getTypeName(DestTy));
    
    // Check for narrowing conversion
    if (!allowNarrowing && isNarrowingConversion(SrcTy, DestTy)) {
        std::string msg = "Narrowing conversion not allowed";
        if (!context.empty()) {
            msg += " in " + context;
        }
        msg += "\n  From: " + getTypeName(SrcTy) + "\n  To: " + getTypeName(DestTy);
        LogCompilerError(ErrorType::SEMANTIC_TYPE, msg);
        return nullptr;
    }
    
    // Perform widening conversions
    
    // int to float (widening)
    if (SrcTy->isIntegerTy(32) && DestTy->isFloatTy()) {
        DEBUG_VERBOSE("  Converting int to float");
        return Builder.CreateSIToFP(V, DestTy, "itof");
    }
    
    // bool to int (widening)
    if (SrcTy->isIntegerTy(1) && DestTy->isIntegerTy(32)) {
        DEBUG_VERBOSE("  Converting bool to int");
        return Builder.CreateZExt(V, DestTy, "btoi");
    }
    
    // bool to float (widening through int)
    if (SrcTy->isIntegerTy(1) && DestTy->isFloatTy()) {
        DEBUG_VERBOSE("  Converting bool to float (via int)");
        Value* AsInt = Builder.CreateZExt(V, Type::getInt32Ty(TheContext), "btoi");
        return Builder.CreateSIToFP(AsInt, DestTy, "itof");
    }
    
    // double to float conversion (handle APFloat operations)
    if (SrcTy->isDoubleTy() && DestTy->isFloatTy()) {
        DEBUG_VERBOSE("  Converting double to float");
        return Builder.CreateFPTrunc(V, DestTy, "fptrunc");
    }
    
    // float to double conversion
    if (SrcTy->isFloatTy() && DestTy->isDoubleTy()) {
        DEBUG_VERBOSE("  Converting float to double");
        return Builder.CreateFPExt(V, DestTy, "fpext");
    }
    
    // Narrowing conversions (only if allowed)
    if (allowNarrowing) {
        // float to int (narrowing)
        if (SrcTy->isFloatTy() && DestTy->isIntegerTy(32)) {
            DEBUG_VERBOSE("  Converting float to int (narrowing - allowed)");
            return Builder.CreateFPToSI(V, DestTy, "ftoi");
        }
        
        // int to bool (narrowing - used in conditionals)
        if (SrcTy->isIntegerTy(32) && DestTy->isIntegerTy(1)) {
            DEBUG_VERBOSE("  Converting int to bool (narrowing - allowed)");
            return Builder.CreateICmpNE(V, ConstantInt::get(SrcTy, 0), "tobool");
        }
        
        // float/double to bool for conditionals
        if ((SrcTy->isFloatTy() || SrcTy->isDoubleTy()) && DestTy->isIntegerTy(1)) {
            DEBUG_VERBOSE("  Converting float/double to bool");
            return Builder.CreateFCmpONE(V, ConstantFP::get(SrcTy, 0.0), "tobool");
        }
    }
    
    // Unsupported conversion
    std::string msg = "Cannot convert between types";
    if (!context.empty()) {
        msg += " in " + context;
    }
    msg += "\n  From: " + getTypeName(SrcTy) + "\n  To: " + getTypeName(DestTy);
    LogCompilerError(ErrorType::SEMANTIC_TYPE, msg);
    return nullptr;
}

/// checkTypeCompatibility - Check if two types are compatible for operations
static bool checkTypeCompatibility(Type* T1, Type* T2, const std::string& operation) {
    if (!T1 || !T2) return false;
    
    // Same types are always compatible
    if (T1 == T2) return true;
    
    // Numeric types are compatible with each other (with conversion)
    bool t1Numeric = T1->isIntegerTy(32) || T1->isFloatTy() || T1->isIntegerTy(1);
    bool t2Numeric = T2->isIntegerTy(32) || T2->isFloatTy() || T2->isIntegerTy(1);
    
    if (t1Numeric && t2Numeric) return true;
    
    DEBUG_CODEGEN("  Type incompatibility in " + operation);
    DEBUG_CODEGEN("    Type 1: " + getTypeName(T1));
    DEBUG_CODEGEN("    Type 2: " + getTypeName(T2));
    
    return false;
}


// Duplicate checkNarrowingConversion removed; use the earlier definition above.


/// promoteTypes - Promote two values to common type for binary operations
static void promoteTypes(Value*& L, Value*& R) {
    Type* LTy = L->getType();
    Type* RTy = R->getType();
    
    if (LTy == RTy)
        return;
    
    // **FIX: Normalize f64 to f32 first**
    if (LTy->isDoubleTy()) {
        L = Builder.CreateFPTrunc(L, Type::getFloatTy(TheContext), "fptrunc");
        LTy = L->getType();
    }
    if (RTy->isDoubleTy()) {
        R = Builder.CreateFPTrunc(R, Type::getFloatTy(TheContext), "fptrunc");
        RTy = R->getType();
    }
    
    // Now promote to float if either is float
    if (LTy->isFloatTy() && RTy->isIntegerTy(32)) {
        R = Builder.CreateSIToFP(R, LTy, "itof");
    } else if (RTy->isFloatTy() && LTy->isIntegerTy(32)) {
        L = Builder.CreateSIToFP(L, RTy, "itof");
    }
    // Promote bool to int if needed
    else if (LTy->isIntegerTy(32) && RTy->isIntegerTy(1)) {
        R = Builder.CreateZExt(R, LTy, "btoi");
    } else if (RTy->isIntegerTy(32) && LTy->isIntegerTy(1)) {
        L = Builder.CreateZExt(L, RTy, "btoi");
    }
    // Promote bool to float through int
    else if (LTy->isFloatTy() && RTy->isIntegerTy(1)) {
        R = Builder.CreateZExt(R, Type::getInt32Ty(TheContext), "btoi");
        R = Builder.CreateSIToFP(R, LTy, "itof");
    } else if (RTy->isFloatTy() && LTy->isIntegerTy(1)) {
        L = Builder.CreateZExt(L, Type::getInt32Ty(TheContext), "btoi");
        L = Builder.CreateSIToFP(L, RTy, "itof");
    }
}

/// BinaryExprAST::codegen - Generate code for binary operators
Value* BinaryExprAST::codegen() {
    DEBUG_CODEGEN("Generating binary expression: " + Op);
    
    Value* L = LHS->codegen();
    Value* R = RHS->codegen();
    
    if (!L || !R) {
        DEBUG_CODEGEN("  ERROR: Failed to generate operands");
        return nullptr;
    }
    
    DEBUG_VERBOSE("  LHS type: " + getTypeName(L->getType()));
    DEBUG_VERBOSE("  RHS type: " + getTypeName(R->getType()));
    
    Type* OrigLTy = L->getType();
    Type* OrigRTy = R->getType();
    
    promoteTypes(L, R);
    
    DEBUG_VERBOSE("  After promotion: " + getTypeName(L->getType()));
    
    Type* OpType = L->getType();
    
    if (Op == "+") {
        DEBUG_CODEGEN("  Creating addition");
        if (OpType->isFloatingPointTy())
            return Builder.CreateFAdd(L, R, "fadd");
        else if (OpType->isIntegerTy())
            return Builder.CreateAdd(L, R, "add");
        else {
            std::string msg = "Invalid operand types for operator '+'";
            LogCompilerError(ErrorType::SEMANTIC_TYPE, msg, -1, -1,
                           "LHS: " + getTypeName(OrigLTy) + ", RHS: " + getTypeName(OrigRTy));
            return nullptr;
        }
    }
    
    if (Op == "-") {
        DEBUG_CODEGEN("  Creating subtraction");
        if (OpType->isFloatingPointTy())
            return Builder.CreateFSub(L, R, "fsub");
        else if (OpType->isIntegerTy())
            return Builder.CreateSub(L, R, "sub");
        else {
            std::string msg = "Invalid operand types for operator '-'";
            LogCompilerError(ErrorType::SEMANTIC_TYPE, msg, -1, -1,
                           "LHS: " + getTypeName(OrigLTy) + ", RHS: " + getTypeName(OrigRTy));
            return nullptr;
        }
    }
    
    if (Op == "*") {
        DEBUG_CODEGEN("  Creating multiplication");
        if (OpType->isFloatingPointTy())
            return Builder.CreateFMul(L, R, "fmul");
        else if (OpType->isIntegerTy())
            return Builder.CreateMul(L, R, "mul");
        else {
            std::string msg = "Invalid operand types for operator '*'";
            LogCompilerError(ErrorType::SEMANTIC_TYPE, msg, -1, -1,
                           "LHS: " + getTypeName(OrigLTy) + ", RHS: " + getTypeName(OrigRTy));
            return nullptr;
        }
    }
    
    if (Op == "/") {
        DEBUG_CODEGEN("  Creating division");
        if (OpType->isFloatingPointTy())
            return Builder.CreateFDiv(L, R, "fdiv");
        else if (OpType->isIntegerTy())
            return Builder.CreateSDiv(L, R, "sdiv");
        else {
            std::string msg = "Invalid operand types for operator '/'";
            LogCompilerError(ErrorType::SEMANTIC_TYPE, msg, -1, -1,
                           "LHS: " + getTypeName(OrigLTy) + ", RHS: " + getTypeName(OrigRTy));
            return nullptr;
        }
    }
    
    if (Op == "%") {
        DEBUG_CODEGEN("  Creating modulo");
        if (OpType->isIntegerTy())
            return Builder.CreateSRem(L, R, "mod");
        else {
            std::string msg = "Modulo operator '%' requires integer operands";
            LogCompilerError(ErrorType::SEMANTIC_TYPE, msg, -1, -1,
                           "Got: " + getTypeName(OpType));
            return nullptr;
        }
      }
    
    if (Op == "<") {
    DEBUG_CODEGEN("  Creating less than comparison");
    if (OpType->isFloatingPointTy())
        return Builder.CreateFCmpOLT(L, R, "flt");
    else if (OpType->isIntegerTy())
        return Builder.CreateICmpSLT(L, R, "lt");
    else {
        std::string msg = "Invalid operand types for operator '<'";
        LogCompilerError(ErrorType::SEMANTIC_TYPE, msg, -1, -1,
                       "Got: " + getTypeName(OpType));
        return nullptr;
      }
    }

    if (Op == "<=") {
        DEBUG_CODEGEN("  Creating less than or equal comparison");
        if (OpType->isFloatingPointTy())
            return Builder.CreateFCmpOLE(L, R, "fle");
        else if (OpType->isIntegerTy())
            return Builder.CreateICmpSLE(L, R, "le");
        else {
            std::string msg = "Invalid operand types for operator '<='";
            LogCompilerError(ErrorType::SEMANTIC_TYPE, msg, -1, -1,
                          "Got: " + getTypeName(OpType));
            return nullptr;
        }
    }

    if (Op == ">") {
        DEBUG_CODEGEN("  Creating greater than comparison");
        if (OpType->isFloatingPointTy())
            return Builder.CreateFCmpOGT(L, R, "fgt");
        else if (OpType->isIntegerTy())
            return Builder.CreateICmpSGT(L, R, "gt");
        else {
            std::string msg = "Invalid operand types for operator '>'";
            LogCompilerError(ErrorType::SEMANTIC_TYPE, msg, -1, -1,
                          "Got: " + getTypeName(OpType));
            return nullptr;
        }
    }

    if (Op == ">=") {
        DEBUG_CODEGEN("  Creating greater than or equal comparison");
        if (OpType->isFloatingPointTy())
            return Builder.CreateFCmpOGE(L, R, "fge");
        else if (OpType->isIntegerTy())
            return Builder.CreateICmpSGE(L, R, "ge");
        else {
            std::string msg = "Invalid operand types for operator '>='";
            LogCompilerError(ErrorType::SEMANTIC_TYPE, msg, -1, -1,
                          "Got: " + getTypeName(OpType));
            return nullptr;
        }
    }

    if (Op == "==") {
        DEBUG_CODEGEN("  Creating equality comparison");
        if (OpType->isFloatingPointTy())
            return Builder.CreateFCmpOEQ(L, R, "feq");
        else if (OpType->isIntegerTy())
            return Builder.CreateICmpEQ(L, R, "eq");
        else {
            std::string msg = "Invalid operand types for operator '=='";
            LogCompilerError(ErrorType::SEMANTIC_TYPE, msg, -1, -1,
                          "Got: " + getTypeName(OpType));
            return nullptr;
        }
    }

    if (Op == "!=") {
        DEBUG_CODEGEN("  Creating inequality comparison");
        if (OpType->isFloatingPointTy())
            return Builder.CreateFCmpONE(L, R, "fne");
        else if (OpType->isIntegerTy())
            return Builder.CreateICmpNE(L, R, "ne");
        else {
            std::string msg = "Invalid operand types for operator '!='";
            LogCompilerError(ErrorType::SEMANTIC_TYPE, msg, -1, -1,
                          "Got: " + getTypeName(OpType));
            return nullptr;
        }
    }
    
    if (Op == "&&") {
        DEBUG_CODEGEN("  Creating logical AND");
        L = castToType(L, Type::getInt1Ty(TheContext));
        R = castToType(R, Type::getInt1Ty(TheContext));
        return Builder.CreateAnd(L, R, "and");
    }
    
    if (Op == "||") {
        DEBUG_CODEGEN("  Creating logical OR");
        L = castToType(L, Type::getInt1Ty(TheContext));
        R = castToType(R, Type::getInt1Ty(TheContext));
        return Builder.CreateOr(L, R, "or");
    }
    
    LogCompilerError(ErrorType::SEMANTIC_OTHER, 
                    "Unknown binary operator: '" + Op + "'");
    return nullptr;
}

/// UnaryExprAST::codegen - Generate code for unary operators
Value* UnaryExprAST::codegen() {
    DEBUG_CODEGEN("Generating unary expression: " + Op);
    
    Value* OperandV = Operand->codegen();
    if (!OperandV) {
        DEBUG_CODEGEN("  ERROR: Failed to generate operand");
        return nullptr;
    }
    
    Type* OpType = OperandV->getType();
    DEBUG_VERBOSE("  Operand type: " + getTypeName(OpType));
    
    if (Op == "-") {
        if (OpType->isFloatingPointTy()) {
            DEBUG_CODEGEN("  Creating floating-point negation");
            return Builder.CreateFNeg(OperandV, "fneg");
        } else if (OpType->isIntegerTy(32)) {
            DEBUG_CODEGEN("  Creating integer negation");
            return Builder.CreateNeg(OperandV, "neg");
        } else {
            std::string msg = "Unary operator '-' requires numeric operand";
            LogCompilerError(ErrorType::SEMANTIC_TYPE, msg, -1, -1,
                           "Got: " + getTypeName(OpType));
            return nullptr;
        }
    }
    
    if (Op == "!") {
        DEBUG_CODEGEN("  Creating logical NOT");
        OperandV = castToType(OperandV, Type::getInt1Ty(TheContext));
        if (!OperandV) {
            return LogErrorV("Cannot convert operand to boolean for '!' operator");
        }
        return Builder.CreateNot(OperandV, "not");
    }
    
    LogCompilerError(ErrorType::SEMANTIC_OTHER, 
                    "Unknown unary operator: '" + Op + "'");
    return nullptr;
}

/// AssignmentExprAST::codegen - Generate code for assignments
Value* AssignmentExprAST::codegen() {
    DEBUG_CODEGEN("Generating assignment to: " + VarName);
    
    Value* Val = RHS->codegen();
    if (!Val) {
        DEBUG_CODEGEN("  ERROR: Failed to generate RHS");
        return nullptr;
    }
    
    DEBUG_VERBOSE("  RHS type: " + getTypeName(Val->getType()));
    
    AllocaInst* Variable = NamedValues[VarName];
    
    if (!Variable) {
        GlobalVariable* GV = GlobalValues[VarName];
        if (!GV) {
            DEBUG_CODEGEN("  ERROR: Variable not found");
            DUMP_SYMBOL_TABLE();
            return LogScopeError(VarName, CurrentContext.toString());
        }
        
        DEBUG_CODEGEN("  Assigning to global variable");
        Type* VarType = GV->getValueType();
        DEBUG_VERBOSE("  Expected type: " + getTypeName(VarType));
        
        if (Val->getType() != VarType) {
            Type* OrigType = Val->getType();
            Val = castToType(Val, VarType);
            if (!Val) {
                DEBUG_CODEGEN("  ERROR: Type conversion failed");
                return LogTypeError("Type mismatch in assignment to global '" + VarName + "'",
                                   VarType, OrigType);
            }
            DEBUG_VERBOSE("  Type converted from " + getTypeName(OrigType) + 
                         " to " + getTypeName(VarType));
        }
        
        Builder.CreateStore(Val, GV);
        DEBUG_CODEGEN("  Assignment successful");
        return Val;
    }
    
    DEBUG_CODEGEN("  Assigning to local variable");
    Type* VarType = Variable->getAllocatedType();
    DEBUG_VERBOSE("  Expected type: " + getTypeName(VarType));
    
    if (Val->getType() != VarType) {
        Type* OrigType = Val->getType();
        Val = castToType(Val, VarType);
        if (!Val) {
            DEBUG_CODEGEN("  ERROR: Type conversion failed");
            return LogTypeError("Type mismatch in assignment to local '" + VarName + "'",
                               VarType, OrigType);
        }
        DEBUG_VERBOSE("  Type converted from " + getTypeName(OrigType) + 
                     " to " + getTypeName(VarType));
    }
    
    Builder.CreateStore(Val, Variable);
    DEBUG_CODEGEN("  Assignment successful");
    return Val;
}

/// CallExprAST::codegen - Generate code for function calls
Value* CallExprAST::codegen() {
    DEBUG_CODEGEN("Generating function call: " + Callee);
    
    // Check if function exists
    Function* CalleeF = checkFunctionExists(Callee);
    if (!CalleeF) {
        // Error already logged
        return nullptr;
    }
    
    DEBUG_VERBOSE("  Function signature: " + getTypeName(CalleeF->getFunctionType()));
    DEBUG_VERBOSE("  Expected args: " + std::to_string(CalleeF->arg_size()));
    DEBUG_VERBOSE("  Provided args: " + std::to_string(Args.size()));
    
    // Check argument count
    if (CalleeF->arg_size() != Args.size()) {
        DEBUG_CODEGEN("  ERROR: Argument count mismatch");
        std::string msg = "Function '" + Callee + "' expects " + 
                         std::to_string(CalleeF->arg_size()) + " argument(s), but " +
                         std::to_string(Args.size()) + " provided";
        LogCompilerError(ErrorType::SEMANTIC_TYPE, msg);
        return nullptr;
    }
    
    std::vector<Value*> ArgsV;
    unsigned Idx = 0;
    
    for (auto &Arg : Args) {
        DEBUG_VERBOSE("  Generating argument " + std::to_string(Idx));
        
        Value* ArgVal = Arg->codegen();
        if (!ArgVal)
            return nullptr;
        
        Type* ExpectedType = CalleeF->getFunctionType()->getParamType(Idx);
        Type* ActualType = ArgVal->getType();
        
        DEBUG_VERBOSE("    Expected: " + getTypeName(ExpectedType));
        DEBUG_VERBOSE("    Actual: " + getTypeName(ActualType));
        
        // Check type compatibility and convert if needed
        if (ActualType != ExpectedType) {
            // Per spec: allow widening, disallow narrowing
            if (isNarrowingConversion(ActualType, ExpectedType)) {
                std::string msg = "Narrowing conversion in argument " + std::to_string(Idx + 1) + 
                                " of function '" + Callee + "'";
                msg += "\n  Expected: " + getTypeName(ExpectedType);
                msg += "\n  Provided: " + getTypeName(ActualType);
                LogCompilerError(ErrorType::SEMANTIC_TYPE, msg);
                return nullptr;
            }
            
            ArgVal = castToType(ArgVal, ExpectedType, false, 
                               "function call argument " + std::to_string(Idx + 1));
            if (!ArgVal) {
                // Error already logged
                return nullptr;
            }
            DEBUG_VERBOSE("    Converted successfully");
        }
        
        ArgsV.push_back(ArgVal);
        Idx++;
    }
    
    DEBUG_CODEGEN("  Call successful");
    
    if (CalleeF->getReturnType()->isVoidTy())
        return Builder.CreateCall(CalleeF, ArgsV);
    else
        return Builder.CreateCall(CalleeF, ArgsV, "calltmp");
}

/// IfExprAST::codegen - Generate code for if/then/else
Value* IfExprAST::codegen() {
    Value* CondV = Cond->codegen();
    if (!CondV)
        return nullptr;
    
    // Convert condition to bool
    CondV = castToType(CondV, Type::getInt1Ty(TheContext));
    
    Function* TheFunction = Builder.GetInsertBlock()->getParent();
    
    // Create blocks for then, else, and merge
    BasicBlock* ThenBB = BasicBlock::Create(TheContext, "then", TheFunction);
    BasicBlock* ElseBB = BasicBlock::Create(TheContext, "else");
    BasicBlock* MergeBB = BasicBlock::Create(TheContext, "ifcont");
    
    if (Else) {
        Builder.CreateCondBr(CondV, ThenBB, ElseBB);
    } else {
        Builder.CreateCondBr(CondV, ThenBB, MergeBB);
    }
    
    // Emit then block
    Builder.SetInsertPoint(ThenBB);
    Value* ThenV = Then->codegen();
    if (!ThenV)
        return nullptr;
    Builder.CreateBr(MergeBB);
    ThenBB = Builder.GetInsertBlock();
    
    // Emit else block
    if (Else) {
        TheFunction->insert(TheFunction->end(), ElseBB);
        Builder.SetInsertPoint(ElseBB);
        Value* ElseV = Else->codegen();
        if (!ElseV)
            return nullptr;
        Builder.CreateBr(MergeBB);
        ElseBB = Builder.GetInsertBlock();
    }
    
    // Emit merge block
    TheFunction->insert(TheFunction->end(), MergeBB);
    Builder.SetInsertPoint(MergeBB);
    
    return Constant::getNullValue(Type::getInt32Ty(TheContext));
}

/// WhileExprAST::codegen - Generate code for while loops
Value* WhileExprAST::codegen() {
    Function* TheFunction = Builder.GetInsertBlock()->getParent();
    
    // Create blocks for loop
    BasicBlock* LoopBB = BasicBlock::Create(TheContext, "loop", TheFunction);
    BasicBlock* BodyBB = BasicBlock::Create(TheContext, "body");
    BasicBlock* AfterBB = BasicBlock::Create(TheContext, "afterloop");
    
    // Branch to loop header
    Builder.CreateBr(LoopBB);
    
    // Emit loop header (condition check)
    Builder.SetInsertPoint(LoopBB);
    Value* CondV = Cond->codegen();
    if (!CondV)
        return nullptr;
    
    // Convert condition to bool
    CondV = castToType(CondV, Type::getInt1Ty(TheContext));
    
    Builder.CreateCondBr(CondV, BodyBB, AfterBB);
    
    // Emit loop body
    TheFunction->insert(TheFunction->end(), BodyBB);
    Builder.SetInsertPoint(BodyBB);
    Value* BodyV = Body->codegen();
    if (!BodyV)
        return nullptr;
    
    // Branch back to loop header
    Builder.CreateBr(LoopBB);
    
    // Emit after block
    TheFunction->insert(TheFunction->end(), AfterBB);
    Builder.SetInsertPoint(AfterBB);
    
    return Constant::getNullValue(Type::getInt32Ty(TheContext));
}

/// ReturnAST::codegen - Generate code for return statements
Value* ReturnAST::codegen() {
    DEBUG_CODEGEN("Generating return statement");
    
    Function* TheFunction = Builder.GetInsertBlock()->getParent();
    Type* FuncRetType = TheFunction->getReturnType();
    
    // Case 1: Void return
    if (!Val) {
        if (!FuncRetType->isVoidTy()) {
            DEBUG_CODEGEN("  ERROR: Non-void function must return a value");
            return LogErrorV("Non-void function '" + 
                           TheFunction->getName().str() + 
                           "' must return a value");
        }
        DEBUG_CODEGEN("  Creating void return");
        return Builder.CreateRetVoid();
    }
    
    // Case 2: Value return
    if (FuncRetType->isVoidTy()) {
        DEBUG_CODEGEN("  ERROR: Void function cannot return a value");
        return LogErrorV("Void function '" + 
                       TheFunction->getName().str() + 
                       "' cannot return a value");
    }
    
    Value* RetVal = Val->codegen();
    if (!RetVal)
        return nullptr;
    
    Type* RetType = RetVal->getType();
    DEBUG_VERBOSE("  Return value type: " + getTypeName(RetType));
    DEBUG_VERBOSE("  Function return type: " + getTypeName(FuncRetType));
    
    if (RetType != FuncRetType) {
        // Check if this is a narrowing conversion (NOT ALLOWED per spec)
        if (isNarrowingConversion(RetType, FuncRetType)) {
            DEBUG_CODEGEN("  ERROR: Narrowing conversion in return");
            std::string msg = "Return type mismatch in function '" + 
                            TheFunction->getName().str() + 
                            "' - narrowing conversion not allowed";
            return LogTypeError(msg, FuncRetType, RetType);
        }
        
        // Widening conversion (ALLOWED per spec)
        DEBUG_VERBOSE("  Applying widening conversion");
        RetVal = castToType(RetVal, FuncRetType, /*allowNarrowing=*/false);
        if (!RetVal) {
            DEBUG_CODEGEN("  ERROR: Type conversion failed");
            return LogTypeError("Cannot convert return value type", 
                              FuncRetType, RetType);
        }
    }
    
    DEBUG_CODEGEN("  Return successful");
    return Builder.CreateRet(RetVal);
}

/// BlockAST::codegen - Generate code for blocks
Value* BlockAST::codegen() {
    std::map<std::string, AllocaInst*> OldBindings;
    Function* TheFunction = Builder.GetInsertBlock()->getParent();
    
    // Generate code for local declarations
    // Generate code for local declarations
    for (auto& decl : LocalDecls) {
        const std::string& VarName = decl->getName();
        const std::string& TypeStr = decl->getType();
        
        DEBUG_CODEGEN("  Declaring local variable: " + VarName + " : " + TypeStr);
        
        // Check if shadowing a global variable (allowed)
        if (GlobalValues.find(VarName) != GlobalValues.end()) {
            DEBUG_CODEGEN("    Shadowing global variable '" + VarName + "'");
        }
        
        // Save old binding if variable already exists in current scope
        if (NamedValues[VarName]) {
            OldBindings[VarName] = NamedValues[VarName];
            DEBUG_CODEGEN("    Shadowing local variable from outer scope");
        }
        
        Type* VarType = getTypeFromString(TypeStr);
        if (!VarType) {
            LogCompilerError(ErrorType::SEMANTIC_TYPE, 
                            "Invalid type '" + TypeStr + "' for variable '" + VarName + "'");
            return nullptr;
        }
        
        AllocaInst* Alloca = CreateEntryBlockAlloca(TheFunction, VarName, VarType);
        
        // Initialize to zero
        if (VarType->isIntegerTy(32)) {
            Builder.CreateStore(ConstantInt::get(VarType, 0), Alloca);
        } else if (VarType->isFloatTy()) {
            Builder.CreateStore(ConstantFP::get(VarType, 0.0), Alloca);
        } else if (VarType->isIntegerTy(1)) {
            Builder.CreateStore(ConstantInt::get(VarType, 0), Alloca);
        }
        
        NamedValues[VarName] = Alloca;
        registerVariable(VarName, TypeStr, false);  // Register in symbol table
    }
    
    // Generate code for statements
    Value* LastVal = nullptr;
    for (auto& stmt : Stmts) {
        if (stmt) {  // Check if statement is not null (empty statement)
            Value* StmtVal = stmt->codegen();
            if (!StmtVal)
                return nullptr;
            LastVal = StmtVal;
        }
    }
    
    // Restore old bindings
    for (auto& binding : OldBindings) {
        NamedValues[binding.first] = binding.second;
    }
    
    // Remove variables that went out of scope
    for (auto& decl : LocalDecls) {
        if (OldBindings.find(decl->getName()) == OldBindings.end()) {
            NamedValues.erase(decl->getName());
            VariableTypes.erase(decl->getName());
        }
    }
    
    return LastVal ? LastVal : Constant::getNullValue(Type::getInt32Ty(TheContext));
}

/// FunctionDeclAST::codegen - Generate code for function definitions
Value* FunctionDeclAST::codegen() {
    // Check if function already exists
    Function* TheFunction = TheModule->getFunction(Proto->getName());
    
    if (!TheFunction) {
        // Create function type
        Type* RetType = getTypeFromString(Proto->getType());
        std::vector<Type*> ParamTypes;
        
        for (auto& param : Proto->getParams()) {
            ParamTypes.push_back(getTypeFromString(param->getType()));
        }
        
        FunctionType* FT = FunctionType::get(RetType, ParamTypes, false);
        TheFunction = Function::Create(FT, Function::ExternalLinkage, 
                                      Proto->getName(), TheModule.get());
        
        // Set parameter names
        unsigned Idx = 0;
        for (auto& Arg : TheFunction->args()) {
            Arg.setName(Proto->getParams()[Idx++]->getName());
        }
    }
    
    // Create entry block
    BasicBlock* BB = BasicBlock::Create(TheContext, "entry", TheFunction);
    Builder.SetInsertPoint(BB);
    
    // Save old function
    Function* OldFunction = CurrentFunction;
    CurrentFunction = TheFunction;
    
    // Clear variable scope
    NamedValues.clear();
    
    // Create allocas for parameters
    unsigned Idx = 0;
    for (auto& Arg : TheFunction->args()) {
        std::string ArgName(Arg.getName());
        AllocaInst* Alloca = CreateEntryBlockAlloca(TheFunction, ArgName, Arg.getType());
        Builder.CreateStore(&Arg, Alloca);
        NamedValues[ArgName] = Alloca;
        std::string TypeStr = Proto->getParams()[Idx++]->getType();
        VariableTypes[ArgName] = TypeStr;
        registerVariable(ArgName, TypeStr, false);  // Register parameter in symbol table
    }
    
    // Generate function body
    if (Value* RetVal = Block->codegen()) {
        // Check if the last block has a terminator
        if (!Builder.GetInsertBlock()->getTerminator()) {
            if (TheFunction->getReturnType()->isVoidTy()) {
                Builder.CreateRetVoid();
            } else {
                // For non-void functions, return a default value
                if (TheFunction->getReturnType()->isIntegerTy(32)) {
                    Builder.CreateRet(ConstantInt::get(TheFunction->getReturnType(), 0));
                } else if (TheFunction->getReturnType()->isFloatTy()) {
                    Builder.CreateRet(ConstantFP::get(TheFunction->getReturnType(), 0.0));
                } else if (TheFunction->getReturnType()->isIntegerTy(1)) {
                    Builder.CreateRet(ConstantInt::get(TheFunction->getReturnType(), 0));
                }
            }
        }
        
        // Verify function
        verifyFunction(*TheFunction);

        for (auto& param : Proto->getParams()) {
            SymbolTypeTable.erase(param->getName());
        }
        
        CurrentFunction = OldFunction;
        return TheFunction;
    }
    
    // Error - remove function
    TheFunction->eraseFromParent();
    CurrentFunction = OldFunction;
    return nullptr;
}

/// FunctionPrototypeAST::codegen - Generate code for function prototypes (extern declarations)
Function* FunctionPrototypeAST::codegen() {
    // Check if function already exists
    Function* TheFunction = TheModule->getFunction(getName());
    if (TheFunction) {
        return TheFunction;
    }
    
    // Use llvm::Type to avoid conflict with class member 'Type'
    llvm::Type* RetType = getTypeFromString(getType());
    if (!RetType) {
        return LogErrorF("Invalid return type in function prototype");
    }
    
    std::vector<llvm::Type*> ParamTypes;
    for (auto& param : getParams()) {
        llvm::Type* ParamType = getTypeFromString(param->getType());
        if (!ParamType) {
            return LogErrorF("Invalid parameter type in function prototype");
        }
        ParamTypes.push_back(ParamType);
    }
    
    FunctionType* FT = FunctionType::get(RetType, ParamTypes, false);
    
    // Create function with external linkage
    TheFunction = Function::Create(FT, Function::ExternalLinkage, 
                                   getName(), TheModule.get());
    
    // Set parameter names
    unsigned Idx = 0;
    for (auto& Arg : TheFunction->args()) {
        Arg.setName(getParams()[Idx++]->getName());
    }
    
    return TheFunction;
}


/// GlobVarDeclAST::codegen - Generate code for global variable declarations
Value* GlobVarDeclAST::codegen() {
    DEBUG_CODEGEN("Generating global variable: " + getName());
    
    // Check if variable already exists
    if (GlobalValues.find(getName()) != GlobalValues.end()) {
        std::string msg = "Redeclaration of global variable '" + getName() + "'";
        LogCompilerError(ErrorType::SEMANTIC_SCOPE, msg);
        return nullptr;
    }
    
    llvm::Type* VarType = getTypeFromString(getType());
    if (!VarType) {
        return LogErrorV("Invalid type for global variable '" + getName() + "'");
    }
    
    // Create global variable with zero initializer
    Constant* InitVal = nullptr;
    
    if (VarType->isIntegerTy(32)) {
        InitVal = ConstantInt::get(VarType, 0);
    } else if (VarType->isFloatTy()) {
        InitVal = ConstantFP::get(VarType, 0.0);
    } else if (VarType->isIntegerTy(1)) {
        InitVal = ConstantInt::get(VarType, 0);
    } else {
        return LogErrorV("Unsupported type for global variable '" + getName() + "'");
    }
    
    GlobalVariable* GV = new GlobalVariable(
        *TheModule,
        VarType,
        false,
        GlobalValue::CommonLinkage,
        InitVal,
        getName()
    );
    
    GlobalValues[getName()] = GV;
    registerVariable(getName(), getType(), true);  // Register in symbol table
    
    DEBUG_CODEGEN("  Global variable created successfully");
    return GV;
}



//===----------------------------------------------------------------------===//
// AST Printer
//===----------------------------------------------------------------------===//

// void IntASTnode::display(int tabs) {
//   printf("%s\n",getType().c_str());
// }


//===----------------------------------------------------------------------===//
// Main driver code.
//===----------------------------------------------------------------------===//
int main(int argc, char **argv) {
    initDebugLevel(argc, argv);
    ShowCompilationProgress();
    
    std::string inputFile;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-d" || arg == "--debug") {
            i++;
            continue;
        }
        if (arg[0] != '-') {
            inputFile = arg;
            break;
        }
    }
    
    if (inputFile.empty()) {
        std::cout << "Usage: ./mccomp [options] InputFile\n";
        std::cout << "Options:\n";
        std::cout << "  -d, --debug <level>   Set debug level (user, parser, codegen, verbose)\n";
        std::cout << "\nOr set MCCOMP_DEBUG environment variable\n";
        return 1;
    }
    
    DEBUG_USER("Opening file: " + inputFile);
    pFile = fopen(inputFile.c_str(), "r");
    if (pFile == NULL) {
        perror("Error opening file");
        return 1;
    }

    lineNo = 1;
    columnNo = 1;
    
    DEBUG_USER("Starting lexical analysis...");
    getNextToken();
    
    if (CurrentDebugLevel < DebugLevel::PARSER) {
        fprintf(stderr, "Lexer Finished\n");
    }
    ShowPhaseComplete("Lexical analysis");

    TheModule = std::make_unique<Module>("mini-c", TheContext);

    DEBUG_USER("Starting parsing...");
    parser();
    
    if (HasErrors) {
        PrintAllErrors();
        fclose(pFile);
        return 1;
    }
    
    if (CurrentDebugLevel < DebugLevel::PARSER) {
        fprintf(stderr, "Parsing Finished\n");
    }
    ShowPhaseComplete("Parsing");
    DEBUG_USER("Starting code generation...");
    
    printf("********************* FINAL IR (begin) ****************************\n");
    
    auto Filename = "output.ll";
    std::error_code EC;
    raw_fd_ostream dest(Filename, EC, sys::fs::OF_None);

    if (EC) {
        errs() << "Could not open file: " << EC.message();
        return 1;
    }
    
    TheModule->print(dest, nullptr);
    printf("********************* FINAL IR (end) ******************************\n");

    fclose(pFile);
    
    ShowPhaseComplete("Code generation");
    
    fprintf(stderr, "\n%s%s✓ Compilation Successful!%s\n", 
            COLOR_BOLD.c_str(), COLOR_GREEN.c_str(), COLOR_RESET.c_str());
    fprintf(stderr, "Output: output.ll\n\n");
    
    return 0;
}
