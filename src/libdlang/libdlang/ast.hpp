#pragma once

#include <DlangLexer.h>

#include "DlangParserBaseVisitor.h"

namespace dlang {

class DlangCustomVisitor : public DlangParserBaseVisitor {
 public:
  std::any visitPrimaryExpression(DlangParser::PrimaryExpressionContext* ctx);

  std::any visitFunctionDefinition(DlangParser::PrimaryExpressionContext* ctx);
  std::any visitFunctionCall(DlangParser::PrimaryExpressionContext* ctx);
  std::any visitTypeSpecifier(DlangParser::PrimaryExpressionContext* ctx);
  std::any visitJumpStatement(DlangParser::PrimaryExpressionContext* ctx);
};

class ASTNode {
 public:
  antlr4::Token* token;
  std::vector<ASTNode*> children;
  
  ASTNode(){}
  ASTNode(antlr4::Token* t) : token(t){};
  size_t getToken() { return token->getType(); }
  void addChild(ASTNode* c) { children.push_back(c); }
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

class ASTNodeFuncDef : public ASTNode {
  public:
  std::string name;
  //also return type and parameters
  ASTNodeFuncDef(antlr4::Token* t, std::string v) : ASTNode(t), name(v){};
};
class ASTNodeFuncCall : public ASTNode {
  public:
  std::string name;
  //also parameters
  ASTNodeFuncCall(antlr4::Token* t, std::string v) : ASTNode(t), name(v){};
};

class ASTNodeReturn : public ASTNode {
  public:
  ASTNodeReturn(antlr4::Token* t) : ASTNode(t){};
};

class ASTVisitor {};

}  // namespace dlang
