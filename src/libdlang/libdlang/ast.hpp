#pragma once

#include <DlangLexer.h>

#include "DlangParserBaseVisitor.h"

namespace dlang {
enum {
  invalid,
  program,
  funcdef,
  funccall,
  paramlist,
  idlist,
  jump,
  expr,
  value_id,
  value_string,
  value_int,
  value_float,
  type_void,
  type_int,
  type_float,
  type_char,
};

class DlangCustomVisitor : public DlangParserBaseVisitor {
 public:
  std::any visitGlobal(DlangParser::GlobalContext* ctx);
  std::any visitPrimeExpr(DlangParser::PrimeExprContext* ctx);
  std::any visitIdentifierList(DlangParser::IdentifierListContext* ctx);
  std::any visitFunctionParameterList(
      DlangParser::FunctionParameterListContext* ctx);
  std::any visitFunctionDefinition(DlangParser::FunctionDefinitionContext* ctx);
  std::any visitFunctionCall(DlangParser::FunctionCallContext* ctx);
  std::any visitTypeSpecifier(DlangParser::TypeSpecifierContext* ctx);
  std::any visitCompoundStatement(DlangParser::CompoundStatementContext* ctx);
  std::any visitBlockItemList(DlangParser::BlockItemListContext* ctx);
  std::any visitJumpStatement(DlangParser::JumpStatementContext* ctx);

  /*
  JumpStatement
    externalDeclataion
    compoundStatemnt
    BlockItemList
    blockItem
    statement
    std::any visitJumpStatement(DlangParser::JumpStatementContext* ctx);*/
};

class ASTVisitor;

class ASTNode {
 public:
  size_t token;
  std::vector<ASTNode*> children;

  ASTNode(size_t t = 0) : token(t) {}
  size_t getToken() { return token; }
  void addChild(ASTNode* c) { children.push_back(c); }

  void accept(ASTVisitor* v);
};
class ASTNodeIdList : public ASTNode {
 public:
  // children axe expressions passed to function
  ASTNodeIdList(size_t t = idlist) : ASTNode(t){};
};

class ASTNodeParameterList : public ASTNode {
 public:
  std::vector<std::string> param_names;
  std::vector<int> param_types;
  // no children
  ASTNodeParameterList(size_t t = paramlist) : ASTNode(t){};
};

class ASTNodeFuncDef : public ASTNode {
 public:
  int returntype;
  std::string name;
  ASTNodeParameterList* parameters;

  ASTNodeFuncDef(size_t t = funcdef) : ASTNode(t){};
};

class ASTNodeExpr : public ASTNode {
 public:
  char exprtype;

  ASTNodeExpr(size_t t, char v) : ASTNode(t), exprtype(v){};
};

class ASTNodeInt : public ASTNode {
 public:
  int value;

  ASTNodeInt(size_t t, int v) : ASTNode(t), value(v){};
};
class ASTNodeFloat : public ASTNode {
 public:
  float value;

  ASTNodeFloat(size_t t, float v) : ASTNode(t), value(v){};
};
class ASTNodeString : public ASTNode {
 public:
  std::string value;

  ASTNodeString(size_t t, std::string v) : ASTNode(t), value(v){};
};
class ASTNodeIdentifier : public ASTNode {
 public:
  std::string value;

  ASTNodeIdentifier(size_t t, std::string v) : ASTNode(t), value(v){};
};

class ASTNodeFuncCall : public ASTNode {
 public:
  std::string name;
  // children are expression-parameters
  ASTNodeFuncCall(size_t t = funccall) : ASTNode(t){};
};

class ASTNodeReturn : public ASTNode {
 public:
  // child is return expression
  ASTNodeReturn(size_t t = jump) : ASTNode(t){};
};

class ASTVisitor {
 public:
  std::stringstream treeprint;

  void visit(ASTNode* n) { n->accept(this); }
};


/*
ASTNode
ASTNodeExpr
ASTNodeInt
ASTNodeFloat
ASTNodeString
ASTNodeIdentifier
ASTNodeReturn
ASTNodeFuncCall
ASTNodeFuncDef
ASTNodeIdList
ASTNodeParameterList
*/

}  // namespace dlang
