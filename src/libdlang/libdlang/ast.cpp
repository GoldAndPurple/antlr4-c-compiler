#include "ast.hpp"

namespace dlang {

std::string getType(int v) {
  if (v == type_void) {
    return "void";
  } else if (v == type_int) {
    return "int";
  } else if (v == type_float) {
    return "float";
  } else if (v == type_char) {
    return "char";
  } else {
    // error
    return std::to_string(v);
  }
}

ASTNode* astcast(std::any obj) {
  if (obj.has_value()) {
    if (obj.type() == typeid(ASTNode*)) {
      return std::any_cast<ASTNode*>(obj);
    } else if (obj.type() == typeid(ASTNodeBlock*)) {
      return std::any_cast<ASTNodeBlock*>(obj);
    } else if (obj.type() == typeid(ASTNodeProgram*)) {
      return std::any_cast<ASTNodeProgram*>(obj);
    } else if (obj.type() == typeid(ASTNodeIdList*)) {
      return std::any_cast<ASTNodeIdList*>(obj);
    } else if (obj.type() == typeid(ASTNodeParameterList*)) {
      return std::any_cast<ASTNodeParameterList*>(obj);
    } else if (obj.type() == typeid(ASTNodeFuncDef*)) {
      return std::any_cast<ASTNodeFuncDef*>(obj);
    } else if (obj.type() == typeid(ASTNodeExpr*)) {
      return std::any_cast<ASTNodeExpr*>(obj);
    } else if (obj.type() == typeid(ASTNodeFuncCall*)) {
      return std::any_cast<ASTNodeFuncCall*>(obj);
    } else if (obj.type() == typeid(ASTNodeReturn*)) {
      return std::any_cast<ASTNodeReturn*>(obj);
    } else if (obj.type() == typeid(ASTNodeInt*)) {
      return std::any_cast<ASTNodeInt*>(obj);
    } else if (obj.type() == typeid(ASTNodeFloat*)) {
      return std::any_cast<ASTNodeFloat*>(obj);
    } else if (obj.type() == typeid(ASTNodeString*)) {
      return std::any_cast<ASTNodeString*>(obj);
    } else if (obj.type() == typeid(ASTNodeIdentifier*)) {
      return std::any_cast<ASTNodeIdentifier*>(obj);
    } else if (obj.type() == typeid(ASTNodeDecl*)) {
      return std::any_cast<ASTNodeDecl*>(obj);
    } else if (obj.type() == typeid(ASTNodeAssign*)) {
      return std::any_cast<ASTNodeAssign*>(obj);
    }
  }
  return nullptr;
}

std::any DlangCustomVisitor::visitGlobal(DlangParser::GlobalContext* ctx) {
  ASTNodeProgram* n = new ASTNodeProgram(program);
  for (auto decl : ctx->externalDeclaration()) {
    std::any tmp = visit(decl);
    if (tmp.has_value()) {
      n->addChild(astcast(tmp));
    }
  }
  return n;
}

std::any DlangCustomVisitor::visitIdentifierList(
    DlangParser::IdentifierListContext* ctx) {
  ASTNodeIdList* n = new ASTNodeIdList(idlist);
  std::any tmp;
  for (auto expr : ctx->expression()) {
    tmp = visit(expr);
    n->addChild(astcast(tmp));
  }

  return n;
}

std::any DlangCustomVisitor::visitPrimaryExpression(DlangParser::PrimaryExpressionContext* ctx){
  ASTNode* node = nullptr;
  auto t = ctx->getStart()->getType();
  /* std::cout << ctx->getText() << '\n'; */
  if (t == DlangParser::Identifier) {
    node = new ASTNodeIdentifier(value_id, ctx->getStart()->getText());
  } else if (t == DlangParser::IntegerConstant) {
    node = new ASTNodeInt(value_int, std::stoi(ctx->getStart()->getText()));
  } else if (t == DlangParser::FloatConstant) {
    node = new ASTNodeFloat(value_float, std::stof(ctx->getStart()->getText()));
  } else if (t == DlangParser::String) {
    node = new ASTNodeString(value_string, ctx->getStart()->getText());
  } else if (t == DlangParser::LeftParen) {
    // visitation return nothng for some bloody reason
    node = astcast(visit(ctx->expression()));
  } else {
    // errors
  }
  return node;
}

std::any DlangCustomVisitor::visitMulDivExpr(
    DlangParser::MulDivExprContext* ctx) {
  char e;
  if (ctx->Star()) {
    e = '*';
  } else if (ctx->Div()) {
    e = '/';
  } else {
    e = '%';
  }
  ASTNodeExpr* node = new ASTNodeExpr(expr, e);
  node->addChild(astcast(visit(ctx->expression()[0])));
  node->addChild(astcast(visit(ctx->expression()[1])));
  return node;
}
std::any DlangCustomVisitor::visitAddsubExpr(
    DlangParser::AddsubExprContext* ctx) {
  char e;
  if (ctx->Plus()) {
    e = '+';
  } else {
    e = '-';
  }
  ASTNodeExpr* node = new ASTNodeExpr(expr, e);
  node->addChild(astcast(visit(ctx->expression()[0])));
  node->addChild(astcast(visit(ctx->expression()[1])));
  return node;
}

std::any DlangCustomVisitor::visitTypeSpecifier(
    DlangParser::TypeSpecifierContext* ctx) {
  int r;
  if (ctx->Void()) {
    r = type_void;
  } else if (ctx->Int()) {
    r = type_int;
  } else if (ctx->Float()) {
    r = type_float;
  } else if (ctx->Char()) {
    r = type_char;
  } else {
    // error
  }
  return r;
}

std::any DlangCustomVisitor::visitFunctionParameterList(
    DlangParser::FunctionParameterListContext* ctx) {
  ASTNodeParameterList* n = new ASTNodeParameterList(paramlist);
  for (size_t i = 0; i < ctx->Identifier().size(); i++) {
    n->param_names.push_back(ctx->Identifier(i)->getText());
    n->param_types.push_back(
        std::any_cast<int>(visitTypeSpecifier(ctx->typeSpecifier(i))));
  }
  return n;
}

std::any DlangCustomVisitor::visitFunctionDefinition(
    DlangParser::FunctionDefinitionContext* ctx) {
  ASTNodeFuncDef* n = new ASTNodeFuncDef(funcdef);
  n->name = ctx->Identifier()->getText();
  n->returntype = std::any_cast<int>(visitTypeSpecifier(ctx->typeSpecifier()));
  if (ctx->parameterTypeList()) {
    n->parameters =
        (ASTNodeParameterList*)astcast(visit(ctx->parameterTypeList()));
  }
  // add statements directly as children, and not compounds
  ASTNode* statements = astcast(visit(ctx->compoundStatement()));
  if (statements) {
    n->children = statements->children;
  }
  return n;
}

std::any DlangCustomVisitor::visitFunctionCall(
    DlangParser::FunctionCallContext* ctx) {
  ASTNodeFuncCall* n = new ASTNodeFuncCall(funccall);
  n->name = ctx->Identifier()->getText();
  n->addChild(astcast(visit(ctx->identifierList())));
  return n;
}

std::any DlangCustomVisitor::visitCompoundStatement(
    DlangParser::CompoundStatementContext* ctx) {
  ASTNodeBlock* n = new ASTNodeBlock();
  for (auto item : ctx->blockItemList()->blockItem()) {
    n->addChild(astcast(visit(item)));
  }
  return n;
}
/*
std::any DlangCustomVisitor::visitBlockItemList(
    DlangParser::BlockItemListContext* ctx) {
  ASTNodeItem* n = new ASTNodeItem();
  for (auto item : ctx->blockItem()) {
    n->addChild(astcast(visit(item)));
  }
  return n;
} */

std::any DlangCustomVisitor::visitAssignmentStatement(
    DlangParser::AssignmentStatementContext* ctx) {
  return visit(ctx->assignmentExpression());
}

std::any DlangCustomVisitor::visitJumpStatement(
    DlangParser::JumpStatementContext* ctx) {
  ASTNodeReturn* n = new ASTNodeReturn(jump);
  n->addChild(astcast(visit(ctx->expression())));
  return n;
}

std::any DlangCustomVisitor::visitDeclaration(
    DlangParser::DeclarationContext* ctx) {
  ASTNodeDecl* n = new ASTNodeDecl();

  // uninitialised identitifiers
  n->type = std::any_cast<int>(visitTypeSpecifier(ctx->typeSpecifier()));
  for (auto id : ctx->Identifier()) {
    ASTNodeIdentifier* i = new ASTNodeIdentifier(value_id, id->getText());
    n->addChild(i);
  }
  // assign-expressions
  for (auto id : ctx->assignmentExpression()) {
    ASTNodeAssign* i = (ASTNodeAssign*)astcast(visit(id));
    n->addChild(i);
  }

  return n;
}

std::any DlangCustomVisitor::visitAssignmentExpression(
    DlangParser::AssignmentExpressionContext* ctx) {
  ASTNodeAssign* n = new ASTNodeAssign();
  ASTNodeIdentifier* i =
      new ASTNodeIdentifier(value_id, ctx->Identifier()->getText());
  n->addChild(i);
  ASTNode* e = (ASTNode*)astcast(visit(ctx->expression()));
  i->addChild(e);
  return n;
}

// ASTvisitor accepts
void ASTNodeBlock::accept(ASTVisitor* v) {
  v->visit(this);
}
void ASTNodeProgram::accept(ASTVisitor* v) {
  v->visit(this);
}
void ASTNodeDecl::accept(ASTVisitor* v) {
  v->visit(this);
}
void ASTNodeExpr::accept(ASTVisitor* v) {
  v->visit(this);
}
void ASTNodeInt::accept(ASTVisitor* v) {
  v->visit(this);
}
void ASTNodeFloat::accept(ASTVisitor* v) {
  v->visit(this);
}
void ASTNodeString::accept(ASTVisitor* v) {
  v->visit(this);
}
void ASTNodeIdentifier::accept(ASTVisitor* v) {
  v->visit(this);
}
void ASTNodeFuncCall::accept(ASTVisitor* v) {
  v->visit(this);
}
void ASTNodeReturn::accept(ASTVisitor* v) {
  v->visit(this);
}
void ASTNodeFuncDef::accept(ASTVisitor* v) {
  v->visit(this);
}
void ASTNodeIdList::accept(ASTVisitor* v) {
  v->visit(this);
}
void ASTNodeParameterList::accept(ASTVisitor* v) {
  v->visit(this);
}
void ASTNodeAssign::accept(ASTVisitor* v) {
  v->visit(this);
}

}  // namespace dlang
