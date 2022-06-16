#pragma once

#include <DlangLexer.h>

#include "DlangParserBaseVisitor.h"

namespace dlang {

enum {
  invalid,
  program,
  block,
  declaration,
  assignment,
  funcdef,
  funccall,
  paramlist,
  idlist,
  jump,
  expr,
  cond,
  value_id,
  value_string,
  value_int,
  value_float,
  type_void,
  type_int,
  type_float,
  type_char,
  type_void_pointer,
  type_int_pointer,
  type_float_pointer,
  type_char_pointer,
};

std::string getType(int v);

class DlangCustomVisitor : public DlangParserBaseVisitor {
 public:
  std::any visitGlobal(DlangParser::GlobalContext* ctx);
  std::any visitPrimaryExpression(DlangParser::PrimaryExpressionContext* ctx);
  std::any visitIdentifierList(DlangParser::IdentifierListContext* ctx);
  std::any visitFunctionParameterList(
      DlangParser::FunctionParameterListContext* ctx);
  std::any visitParameterTypeList(DlangParser::ParameterTypeListContext* ctx);
  std::any visitFunctionDefinition(DlangParser::FunctionDefinitionContext* ctx);
  std::any visitFunctionCall(DlangParser::FunctionCallContext* ctx);
  std::any visitTypeSpecifier(DlangParser::TypeSpecifierContext* ctx);
  std::any visitCompoundStatement(DlangParser::CompoundStatementContext* ctx);
  std::any visitJumpStatement(DlangParser::JumpStatementContext* ctx);
  std::any visitAssignmentStatement(
      DlangParser::AssignmentStatementContext* ctx);
  std::any visitDeclaration(DlangParser::DeclarationContext* ctx);
  std::any visitAssignmentExpression(
      DlangParser::AssignmentExpressionContext* ctx);
  std::any visitMulDivExpr(DlangParser::MulDivExprContext* ctx);
  std::any visitAddsubExpr(DlangParser::AddsubExprContext* ctx);
  std::any visitBasicConditionalExpr(
      DlangParser::BasicConditionalExprContext* ctx);
  std::any visitIfStatement(DlangParser::IfStatementContext* ctx);

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
class ASTNodeBlock : public ASTNode {
 public:
  ASTNodeBlock(size_t t = block) : ASTNode(t){};

  void accept(ASTVisitor* v) override;
};
class ASTNodeDecl : public ASTNode {
 public:
  // children are declared ids or assignment nodes
  int type;
  ASTNodeDecl(size_t t = declaration) : ASTNode(t){};

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
  ASTNodeExpr(size_t t = expr) : ASTNode(t){};
  virtual void accept(ASTVisitor* v) override;
};

class ASTNodeBinary : public ASTNodeExpr {
 public:
  std::string exprtype;

  ASTNodeBinary(size_t t, const std::string v) : ASTNodeExpr(t), exprtype(v){};

  void accept(ASTVisitor* v) override;
};

class ASTNodeConditional : public ASTNode {
 public:
  ASTNodeExpr* condition;
  ASTNodeConditional* elseif;
  // linked list of cond expressions
  ASTNodeConditional(size_t t = cond) : ASTNode(t){};

  void accept(ASTVisitor* v) override;
};

class ASTNodeInt : public ASTNodeExpr {
 public:
  int value;

  ASTNodeInt(size_t t, int v) : ASTNodeExpr(t), value(v){};

  void accept(ASTVisitor* v) override;
};
class ASTNodeFloat : public ASTNodeExpr {
 public:
  float value;

  ASTNodeFloat(size_t t, float v) : ASTNodeExpr(t), value(v){};

  void accept(ASTVisitor* v) override;
};
class ASTNodeString : public ASTNodeExpr {
 public:
  std::string value;

  ASTNodeString(size_t t, std::string v) : ASTNodeExpr(t), value(v){};

  void accept(ASTVisitor* v) override;
};
class ASTNodeIdentifier : public ASTNodeExpr {
 public:
  std::string value;

  ASTNodeIdentifier(size_t t, std::string v) : ASTNodeExpr(t), value(v){};

  void accept(ASTVisitor* v) override;
};
class ASTNodeAssign : public ASTNode {
 public:
  ASTNodeIdentifier* id;
  // children are expressions to be assigned
  ASTNodeAssign(size_t t = assignment) : ASTNode(t){};

  void accept(ASTVisitor* v) override;
};

class ASTNodeFuncCall : public ASTNodeExpr {
 public:
  std::string name;
  // children are expression-parameters
  ASTNodeFuncCall(size_t t = funccall) : ASTNodeExpr(t){};

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
  virtual void visit(ASTNodeProgram* n) { visit_children(n); }
  virtual void visit(ASTNodeBlock* n) { visit_children(n); }
  // virtual void visit(ASTNodeExpr* n) { visit_children(n); }
  virtual void visit(ASTNodeBinary* n) { visit_children(n); }
  virtual void visit(ASTNodeInt* n) { visit_children(n); }
  virtual void visit(ASTNodeFloat* n) { visit_children(n); }
  virtual void visit(ASTNodeString* n) { visit_children(n); }
  virtual void visit(ASTNodeIdentifier* n) { visit_children(n); }
  virtual void visit(ASTNodeDecl* n) { visit_children(n); }
  virtual void visit(ASTNodeReturn* n) { visit_children(n); }
  virtual void visit(ASTNodeFuncCall* n) { visit_children(n); }
  virtual void visit(ASTNodeFuncDef* n) { visit_children(n); }
  virtual void visit(ASTNodeAssign* n) { visit_children(n); }
  virtual void visit(ASTNodeIdList* n) { visit_children(n); }
  virtual void visit(ASTNodeParameterList* n) { visit_children(n); }
  virtual void visit(ASTNodeConditional* n) { visit_children(n); }

  virtual void visit(ASTNode* c) {
    if (c->getToken() == idlist) {
      visit((ASTNodeIdList*)c);

    } else if (c->getToken() == cond) {
      visit((ASTNodeConditional*)c);

    } else if (c->getToken() == paramlist) {
      visit((ASTNodeParameterList*)c);

    } else if (c->getToken() == funcdef) {
      visit((ASTNodeFuncDef*)c);

    } else if (c->getToken() == funccall) {
      visit((ASTNodeFuncCall*)c);

    } else if (c->getToken() == block) {
      visit((ASTNodeBlock*)c);

    } else if (c->getToken() == jump) {
      visit((ASTNodeReturn*)c);

    } else if (c->getToken() == expr) {
      visit((ASTNodeBinary*)c);

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
  }

  virtual void visit_children(ASTNode* n) {
    for (auto c : n->children) {
      visit(c);
    }
  }
};

class ASTVisitorPrint : public ASTVisitor {
 public:
  std::stringstream treeprint;
  int indent = 0;

  void visit_children(ASTNode* n) {
    indent += 1;
    ASTVisitor::visit_children(n);
    indent -= 1;
  }

  void visit(ASTNodeProgram* n) {
    treeprint << "(program\n";
    visit_children(n);
    treeprint << ')';
  }
  void visit(ASTNodeInt* n) { treeprint << n->value; }
  void visit(ASTNodeFloat* n) { treeprint << n->value; }
  void visit(ASTNodeString* n) { treeprint << n->value; }
  void visit(ASTNodeIdentifier* n) { treeprint << n->value; }
  void visit(ASTNodeConditional* n) {
    // no if else management
    print_indent();
    treeprint << "if ";
    ASTVisitor::visit(n->condition);
    treeprint << " then\n";
    visit_children(n);
  }
  void visit(ASTNodeBinary* n) {
    treeprint << '(';
    ASTVisitor::visit(n->children[0]);
    treeprint << ' ' << n->exprtype << ' ';
    ASTVisitor::visit(n->children[1]);
    treeprint << ')';
  }
  void visit(ASTNodeDecl* n) {
    print_indent();
    treeprint << "(var " << getType(n->type) << "\n";
    visit_children(n);
    print_indent();
    treeprint << ")\n";
  }
  void visit(ASTNodeAssign* n) {
    print_indent();
    treeprint << "(" << n->id->value << " = ";
    visit_children(n);
    treeprint << " )\n";
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
    treeprint << '(';
    for (size_t i = 0; i < n->param_types.size(); i++) {
      print_indent();
      treeprint << getType(n->param_types[i]) << ' ' << n->param_names[i]
                << ' ';
    }
    treeprint << ')';
  }

  void print_indent() {
    for (int i = 0; i < indent; i++) {
      treeprint << "  ";
    }
  }
};

class CustomErrorListener : public antlr4::BaseErrorListener {
 public:
  std::vector<std::string> errors;

  void syntaxError(
      antlr4::Recognizer* recognizer,
      antlr4::Token* offendingSymbol,
      size_t line,
      size_t charPositionInLine,
      const std::string& msg,
      std::exception_ptr e) override {
    if (recognizer != nullptr && offendingSymbol != nullptr && e != nullptr) {
      errors.push_back(
          '<' + std::to_string(line) + ':' +
          std::to_string(charPositionInLine) + "> " + msg);
      /* std::cerr << '<' << line << ':' << charPositionInLine << "> " << msg
                << '\n'; */
    }
  }
};

// using Value = std::pair(int, std::variant<int,float,std::string>);

class Scope {
 public:
  Scope* parent;
  std::string name;
  std::vector<Scope*> children;
  std::map<std::string, int> symtab;

  Scope(Scope* p, const std::string n = "") : parent(p), name(n) {}
  void nest_scope(Scope* c) { children.push_back(c); }

  bool declared(std::string id) { return (symtab.find(id) != symtab.end()); }
  bool exists(std::string id) {
    Scope* tmp = this;
    while (tmp != nullptr) {
      if (tmp->declared(id)) {
        return true;
      }
      tmp = tmp->parent;
    }
    return false;
  }
  int add(std::string id, int type) {
    if (declared(id)) {
      /* redeclaration error */
      return -1;
    }
    symtab[id] = type;
    return 0;
  }
  int get(const std::string id) {
    Scope* tmp = this;
    while (tmp != nullptr) {
      if (tmp->declared(id)) {
        return symtab[id];
      }
      tmp = tmp->parent;
    }
    return -1;
  }
  Scope* get_scope(const std::string n) {
    for (auto* c : this->children) {
      if (c->name == n) {
        return c;
      }
    }
    return nullptr;
  }
};

class ASTVisitorScope : public ASTVisitor {
 public:
  std::vector<std::string> errors;
  Scope global = Scope(nullptr);
  Scope* current = &global;

  void visit(ASTNodeProgram* n) {
    visit_children(n);
    if ((global.declared("main") == false) ||
        (global.symtab["main"] != funcdef)) {
      errors.push_back("main function not declared");
    }
  }

  void visit(ASTNodeFuncDef* n) {
    Scope* s = new Scope(current, n->name);
    current->nest_scope(s);
    if (global.add(n->name, funcdef) == -1) {
      errors.push_back("id \"" + n->name + "\" redeclared");
    }
    current = s;

    if (n->parameters != nullptr) {
      auto names = n->parameters->param_names;
      auto types = n->parameters->param_types;
      for (size_t i = 0; i < names.size(); i++) {
        s->add(names[i], types[i]);
      }
    }

    visit_children(n);
    current = s->parent;
  }

  void visit(ASTNodeBlock* n) {
    Scope* s = new Scope(current);
    current->nest_scope(s);
    current = s;
    visit_children(n);
    current = s->parent;
  }

  void visit(ASTNodeDecl* n) {
    for (auto c : n->children) {
      ASTNodeIdentifier* tmp;
      if (c->getToken() == value_id) {
        tmp = (ASTNodeIdentifier*)c;
      } else /* if (c->getToken() == assignment) */ {
        tmp = ((ASTNodeAssign*)c)->id;
      }
      if (current->add(tmp->value, n->type) == -1) {
        errors.push_back("id \"" + tmp->value + "\" redeclared");
      }
    }
  }
  /*
  type-checking for expressions
    void visit(ASTNodeBinary* n){

    } */

  void visit(ASTNodeIdentifier* n) {
    if (!current->exists(n->value)) {
      errors.push_back("id \"" + n->value + "\" not declared");
    }
  }
};

}  // namespace dlang
