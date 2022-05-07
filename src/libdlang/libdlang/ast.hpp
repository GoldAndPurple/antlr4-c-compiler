#pragma once

#include <DlangLexer.h>

#include "DlangParserBaseVisitor.h"

namespace dlang {
enum {
  invalid,
  program,
  item,
  declaration,
  assignment,
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

std::string getType(int v);

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
  std::any visitAssignmentStatement(DlangParser::AssignmentStatementContext* ctx);
  std::any visitDeclaration(DlangParser::DeclarationContext* ctx);
  std::any visitAssignmentExpression(
      DlangParser::AssignmentExpressionContext* ctx);

  /*
  JumpStatement
  Declaration
  AssignmentExpression
  AssignmentStatement
    externalDeclataion
    compoundStatemnt
    BlockItemList
    blockItem
    statement
    std::any visitJumpStatement(DlangParser::JumpStatementContext* ctx);*/
};

class ASTVisitor;

class ASTNode {
  size_t token;

 public:
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
class ASTNodeDecl : public ASTNode {
 public:
  // children are declared value-ids
  int type;
  ASTNodeDecl(size_t t = declaration) : ASTNode(t){};

  void accept(ASTVisitor* v) override;
};
class ASTNodeAssign : public ASTNode {
 public:
  // children are ids, whose children are expressions/funcreturns
  ASTNodeAssign(size_t t = assignment) : ASTNode(t){};

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
  virtual void visit(ASTNodeProgram* n) = 0;
  virtual void visit(ASTNodeItem* n) = 0;
  virtual void visit(ASTNodeExpr* n) = 0;
  virtual void visit(ASTNodeInt* n) = 0;
  virtual void visit(ASTNodeFloat* n) = 0;
  virtual void visit(ASTNodeString* n) = 0;
  virtual void visit(ASTNodeIdentifier* n) = 0;
  virtual void visit(ASTNodeDecl* n) = 0;
  virtual void visit(ASTNodeReturn* n) = 0;
  virtual void visit(ASTNodeFuncCall* n) = 0;
  virtual void visit(ASTNodeFuncDef* n) = 0;
  virtual void visit(ASTNodeAssign* n) = 0;
  virtual void visit(ASTNodeIdList* n) = 0;
  virtual void visit(ASTNodeParameterList* n) = 0;
};

class ASTVisitorPrint : public ASTVisitor {
 public:
  std::stringstream treeprint;
  int indent = 0;

  // add visiting of child nodes to placeholders as well
  void visit(ASTNodeProgram* n) {
    treeprint << "(program\n";
    visit_children(n);
    treeprint << ')';
  }
  void visit(ASTNodeItem* n) { visit_children(n); }
  void visit(ASTNodeExpr* n) { visit_children(n); }
  void visit(ASTNodeInt* n) { treeprint << n->value; }
  void visit(ASTNodeFloat* n) { treeprint << n->value; }
  void visit(ASTNodeString* n) { treeprint << n->value; }
  void visit(ASTNodeIdentifier* n) { treeprint << n->value; }
  void visit(ASTNodeDecl* n) {
    print_indent();
    treeprint << "(var " << getType(n->type) << "\n";
    visit_children(n);
    print_indent();
    treeprint << ")\n";
  }
  void visit(ASTNodeAssign* n) {
    for (auto c : n->children) {
      print_indent();
      treeprint << "(" << ((ASTNodeIdentifier*)c)->value << " = ";
      visit_children(c);
      treeprint << ", ";
    }
    treeprint << ")\n";
  }
  void visit(ASTNodeReturn* n) {
    print_indent();
    treeprint << "(return ";
    visit_children(n);
    treeprint << ")\n";
  }
  void visit(ASTNodeFuncCall* n) {
    print_indent();
    treeprint << "(call " << n->name << ' ';
    visit_children(n);
    treeprint << ")\n";
  }
  void visit(ASTNodeFuncDef* n) {
    print_indent();
    treeprint << "(func " << getType(n->returntype) << ' ' << n->name;
    if (n->parameters) {
      visit(n->parameters);
    } else {
      treeprint << "(void)";
    }
    treeprint << '\n';
    visit_children(n);
    print_indent();
    treeprint << ")\n";
  }
  void visit(ASTNodeIdList* n) { visit_children(n); }

  void visit(ASTNodeParameterList* n) {
    for (auto i : n->param_types) {
      print_indent();
      treeprint << getType(n->param_types[i]) << ' ' << n->param_names[i]
                << ' ';
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
      if (c->getToken() == idlist) {
        visit((ASTNodeIdList*)c);

      } else if (c->getToken() == paramlist) {
        visit((ASTNodeParameterList*)c);

      } else if (c->getToken() == funcdef) {
        visit((ASTNodeFuncDef*)c);

      } else if (c->getToken() == funccall) {
        visit((ASTNodeFuncCall*)c);

      } else if (c->getToken() == item) {
        visit((ASTNodeItem*)c);

      } else if (c->getToken() == jump) {
        visit((ASTNodeReturn*)c);

      } else if (c->getToken() == expr) {
        visit((ASTNodeExpr*)c);

      } else if (c->getToken() == value_int) {
        visit((ASTNodeInt*)c);

      } else if (c->getToken() == value_float) {
        visit((ASTNodeFloat*)c);

      } else if (c->getToken() == value_string) {
        visit((ASTNodeString*)c);

      } else if (c->getToken() == value_id) {
        visit((ASTNodeIdentifier*)c);
      } else if (c->getToken() == declaration) {
        visit((ASTNodeDecl*)c);
      } else if (c->getToken() == assignment) {
        visit((ASTNodeAssign*)c);
      } else {
        // invalid token
      }

      // treeprint << '\n';
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
