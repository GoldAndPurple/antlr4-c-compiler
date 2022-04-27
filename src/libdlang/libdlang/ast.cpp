#include "ast.hpp"

namespace dlang {

std::any DlangCustomVisitor::visitGlobal(DlangParser::GlobalContext* ctx) {
  ASTNode* n = new ASTNode();
  for (auto decl : ctx->externalDeclaration()) {
    std::any tmp = visit(decl);
    if (tmp.has_value()) {
      if (tmp.type() == typeid(ASTNode*)) {
        n->addChild(std::any_cast<ASTNode*>(tmp));
      } else if (tmp.type() == typeid(ASTNodeFuncDef*)) {
        n->addChild(std::any_cast<ASTNodeFuncDef*>(tmp));
      } 
    }
  }
  return n;
}

std::any DlangCustomVisitor::visitIdentifierList(
    DlangParser::IdentifierListContext* ctx) {
  ASTNodeIdList* n = new ASTNodeIdList(nullptr);
  std::any tmp;
  for (auto expr : ctx->expression()) {
    tmp = visit(expr);
    n->addChild(std::any_cast<ASTNode*>(tmp));
  }

  return n;
}

std::any DlangCustomVisitor::visitPrimeExpr(
    DlangParser::PrimeExprContext* ctx) {
  ASTNode* node = nullptr;
  auto t = ctx->getStart()->getType();
  /* std::cout << ctx->getText() << '\n'; */
  if (t == DlangParser::Identifier) {
    node = new ASTNodeIdentifier(ctx->getStart(), ctx->getStart()->getText());
  } else if (t == DlangParser::IntegerConstant) {
    node =
        new ASTNodeInt(ctx->getStart(), std::stoi(ctx->getStart()->getText()));
  } else if (t == DlangParser::FloatConstant) {
    node = new ASTNodeFloat(
        ctx->getStart(), std::stof(ctx->getStart()->getText()));
  } else if (t == DlangParser::String) {
    node = new ASTNodeString(ctx->getStart(), ctx->getStart()->getText());
  } else if (t == DlangParser::LeftParen) {
    /* node = new ASTNodeExpr();
    node->addChild(std::any_cast<ASTNodeExpr>(ctx->expression()))); */
  } else {
    // errors
  }
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
  ASTNodeParameterList* n = new ASTNodeParameterList(nullptr);
  for (size_t i = 0; i < ctx->Identifier().size(); i++) {
    n->param_names.push_back(ctx->Identifier(i)->getText());
    n->param_types.push_back(
        std::any_cast<int>(visitTypeSpecifier(ctx->typeSpecifier(i))));
  }
  return n;
}

std::any DlangCustomVisitor::visitFunctionDefinition(
    DlangParser::FunctionDefinitionContext* ctx) {
  ASTNodeFuncDef* n = new ASTNodeFuncDef(nullptr);
  n->name = ctx->Identifier()->getText();
  n->returntype = std::any_cast<int>(visitTypeSpecifier(ctx->typeSpecifier()));
  // bad cast
  // n->parameters =
  // std::any_cast<ASTNodeParameterList*>(visit(ctx->parameterTypeList())); also
  // need to add children-block-items
  return n;
}

}  // namespace dlang
