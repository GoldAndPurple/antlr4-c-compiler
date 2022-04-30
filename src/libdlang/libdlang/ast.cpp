#include "ast.hpp"

namespace dlang {

std::string ASTNode::treeprint;

ASTNode* astcast(std::any obj) {
  if (obj.has_value()) {
    if (obj.type() == typeid(ASTNode*)) {
      return std::any_cast<ASTNode*>(obj);
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
    }
  }
  return nullptr;
}

std::any DlangCustomVisitor::visitGlobal(DlangParser::GlobalContext* ctx) {
  ASTNode* n = new ASTNode(program);
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

std::any DlangCustomVisitor::visitPrimeExpr(
    DlangParser::PrimeExprContext* ctx) {
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
    /* node = new ASTNodeExpr();
    node->addChild(std::any_cast<ASTNodeExpr>(ctx->expression()))); */
  } else {
    // errors
  }
  return node;
}

std::any DlangCustomVisitor::visitTypeSpecifier(
    DlangParser::TypeSpecifierContext* ctx) {
  size_t r;
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
  n->returntype =
      std::any_cast<size_t>(visitTypeSpecifier(ctx->typeSpecifier()));
  auto tmp = visit(ctx->parameterTypeList());
  if (tmp.has_value()) {
    n->parameters = (ASTNodeParameterList*)astcast(tmp);
  }
  n->addChild(astcast(visit(ctx->compoundStatement())));
  return n;
}

std::any DlangCustomVisitor::visitFunctionCall(
    DlangParser::FunctionCallContext* ctx) {
  ASTNodeFuncCall* n = new ASTNodeFuncCall(funccall);
  n->name = ctx->Identifier()->getText();
  n->addChild(astcast(visit(ctx->identifierList())));
  return n;
}

}  // namespace dlang
