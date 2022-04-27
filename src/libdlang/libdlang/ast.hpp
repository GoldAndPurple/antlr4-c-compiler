#pragma once

#include <DlangLexer.h>

#include "DlangParserBaseVisitor.h"

namespace dlang {

  enum {
    type_void,
    type_int,
    type_float,
    type_char,
  };

class DlangCustomVisitor : public DlangParserBaseVisitor {
 public:
  //static DlangLexer lexer;

  std::any visitGlobal(DlangParser::GlobalContext* ctx);
  std::any visitPrimeExpr(DlangParser::PrimeExprContext* ctx);
  std::any visitIdentifierList(DlangParser::IdentifierListContext *ctx);
  std::any visitFunctionParameterList(DlangParser::FunctionParameterListContext *ctx);
  std::any visitFunctionDefinition(DlangParser::FunctionDefinitionContext*ctx); 
  std::any visitTypeSpecifier(DlangParser::TypeSpecifierContext* ctx);

  /*   
    externalDeclataion
    std::any visitFunctionCall(DlangParser::FunctionCallContext* ctx);
    s
    parameterTypeList
    compoundStatemnt
    blockItemList
    blockItem
    statement
    std::any visitJumpStatement(DlangParser::JumpStatementContext* ctx);*/
};

class ASTNode {
 public:
  antlr4::Token* token;
  std::vector<ASTNode*> children;

  ASTNode(antlr4::Token* t = nullptr) : token(t) {}
  size_t getToken() { return (token) ? token->getType() : 0; }
  void addChild(ASTNode* c) { children.push_back(c); }
};
class ASTNodeIdList : public ASTNode {
 public:
  //children axe expressions passed to function
  ASTNodeIdList(antlr4::Token* t) : ASTNode(t){};
};

class ASTNodeParameterList : public ASTNode {
 public:
  std::vector<std::string> param_names;
  std::vector<int> param_types;
  //no children
  ASTNodeParameterList(antlr4::Token* t) : ASTNode(t){};
};

class ASTNodeFuncDef : public ASTNode {
 public:
  int returntype;
  std::string name;
  ASTNodeParameterList *parameters;

  ASTNodeFuncDef(antlr4::Token* t) : ASTNode(t){};
};

class ASTNodeExpr : public ASTNode {
 public:
  char exprtype;

  ASTNodeExpr(antlr4::Token* t, char v) : ASTNode(t), exprtype(v){};
};

class ASTNodeInt : public ASTNode {
 public:
  int value;

  ASTNodeInt(antlr4::Token* t, int v) : ASTNode(t), value(v){};
};
class ASTNodeFloat : public ASTNode {
 public:
  float value;

  ASTNodeFloat(antlr4::Token* t, float v) : ASTNode(t), value(v){};
};
class ASTNodeString : public ASTNode {
 public:
  std::string value;

  ASTNodeString(antlr4::Token* t, std::string v) : ASTNode(t), value(v){};
};
class ASTNodeIdentifier : public ASTNode {
 public:
  std::string value;

  ASTNodeIdentifier(antlr4::Token* t, std::string v) : ASTNode(t), value(v){};
};

class ASTNodeFuncCall : public ASTNode {
 public:
  std::string name;
  // children are expression-parameters
  ASTNodeFuncCall(antlr4::Token* t, std::string v) : ASTNode(t), name(v){};
};

class ASTNodeReturn : public ASTNode {
 public:
 // child is return expression
  ASTNodeReturn(antlr4::Token* t) : ASTNode(t){};
};

class ASTVisitor {};

}  // namespace dlang
