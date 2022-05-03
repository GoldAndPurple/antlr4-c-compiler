#pragma once

#include <DlangLexer.h>

#include "DlangParserBaseVisitor.h"

namespace dlang {
enum {
  invalid,
  program,
  item,
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

  virtual void accept(ASTVisitor* v) = 0;
};
class ASTNodeProgram : public ASTNode {
 public:
  ASTNodeProgram(size_t t = program) : ASTNode(t){};

  void accept(ASTVisitor* v) override;
};
class ASTNodeItem : public ASTNode {
 public:
  ASTNodeItem(size_t t = item) : ASTNode(t){};

  void accept(ASTVisitor* v) override;
};
class ASTNodeIdList : public ASTNode {
 public:
  // children axe expressions passed to function
  ASTNodeIdList(size_t t = idlist) : ASTNode(t){};

  void accept(ASTVisitor* v) override;
};

class ASTNodeParameterList : public ASTNode {
 public:
  std::vector<std::string> param_names;
  std::vector<int> param_types;
  // no children
  ASTNodeParameterList(size_t t = paramlist) : ASTNode(t){};

  void accept(ASTVisitor* v) override;
};

class ASTNodeFuncDef : public ASTNode {
 public:
  int returntype;
  std::string name;
  ASTNodeParameterList* parameters;

  ASTNodeFuncDef(size_t t = funcdef) : ASTNode(t){};

  void accept(ASTVisitor* v) override;
};

class ASTNodeExpr : public ASTNode {
 public:
  char exprtype;

  ASTNodeExpr(size_t t, char v) : ASTNode(t), exprtype(v){};

  void accept(ASTVisitor* v) override;
};

class ASTNodeInt : public ASTNode {
 public:
  int value;

  ASTNodeInt(size_t t, int v) : ASTNode(t), value(v){};

  void accept(ASTVisitor* v) override;
};
class ASTNodeFloat : public ASTNode {
 public:
  float value;

  ASTNodeFloat(size_t t, float v) : ASTNode(t), value(v){};

  void accept(ASTVisitor* v) override;
};
class ASTNodeString : public ASTNode {
 public:
  std::string value;

  ASTNodeString(size_t t, std::string v) : ASTNode(t), value(v){};

  void accept(ASTVisitor* v) override;
};
class ASTNodeIdentifier : public ASTNode {
 public:
  std::string value;

  ASTNodeIdentifier(size_t t, std::string v) : ASTNode(t), value(v){};

  void accept(ASTVisitor* v) override;
};

class ASTNodeFuncCall : public ASTNode {
 public:
  std::string name;
  // children are expression-parameters
  ASTNodeFuncCall(size_t t = funccall) : ASTNode(t){};

  void accept(ASTVisitor* v) override;
};

class ASTNodeReturn : public ASTNode {
 public:
  // child is return expression
  ASTNodeReturn(size_t t = jump) : ASTNode(t){};

  void accept(ASTVisitor* v) override;
};

class ASTVisitor {
 public:
  std::stringstream treeprint;
  int indent = 0;

  // add visiting of child nodes to placeholders as well
  void visit(ASTNodeProgram* n) {
    print_indent();
    treeprint << "(program\n";
    visit_children(n);
    print_indent();
    treeprint << ')';
  }
  void visit(ASTNodeItem* n) { visit_children(n); }
  void visit(ASTNodeExpr* n) { visit_children(n); }
  void visit(ASTNodeInt* n) { treeprint << n->value; }
  void visit(ASTNodeFloat* n) { treeprint << n->value; }
  void visit(ASTNodeString* n) { treeprint << n->value; }
  void visit(ASTNodeIdentifier* n) { treeprint << n->value; }
  void visit(ASTNodeReturn* n) {
    print_indent();
    treeprint << "(return ";
    visit_children(n);
    print_indent();
    treeprint << ')';
  }
  void visit(ASTNodeFuncCall* n) {
    print_indent();
    treeprint << "(call " << n->name << ' ';
    visit_children(n);
    print_indent();
    treeprint << ')';
  }
  void visit(ASTNodeFuncDef* n) {
    print_indent();
    treeprint << "(func " << n->returntype << ' ' << n->name;
    if (n->parameters) {
      visit(n->parameters);
    } else {
      treeprint << "(void)";
    }
    treeprint << '\n';
    visit_children(n);
    print_indent();
    treeprint << ')';
  }
  void visit(ASTNodeIdList* n) { visit_children(n); }

  void visit(ASTNodeParameterList* n) {
    for (auto i : n->param_types) {
      print_indent();
      treeprint << n->param_types[i] << ' ' << n->param_names[i] << ' ';
    }
  }

  void print_indent() {
    for (int i = 0; i < indent; i++) {
      treeprint << "  ";
    }
  }
  void visit_children(ASTNode* n) {
    indent += 1;
    for (auto c : n->children) {
      if (c->token == idlist) {
        visit((ASTNodeIdList*)c);

      } else if (c->token == paramlist) {
        visit((ASTNodeParameterList*)c);

      } else if (c->token == funcdef) {
        visit((ASTNodeFuncDef*)c);

      } else if (c->token == funccall) {
        visit((ASTNodeFuncCall*)c);

      } else if (c->token == item) {
        visit((ASTNodeItem*)c);

      } else if (c->token == jump) {
        visit((ASTNodeReturn*)c);

      } else if (c->token == expr) {
        visit((ASTNodeExpr*)c);

      } else if (c->token == value_int) {
        visit((ASTNodeInt*)c);

      } else if (c->token == value_float) {
        visit((ASTNodeFloat*)c);

      } else if (c->token == value_string) {
        visit((ASTNodeString*)c);

      } else if (c->token == value_id) {
        visit((ASTNodeIdentifier*)c);
      } else {
        // invalid token
      }

      // visit(c);
      treeprint << '\n';
    }
    indent -= 1;
  }
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
