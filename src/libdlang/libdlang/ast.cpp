#include "ast.hpp"

namespace dlang {

    std::any DlangCustomVisitor::visitPrimaryExpression(DlangParser::PrimaryExpressionContext *ctx){
        ASTNode* node;
        if (ctx->Identifier()){
            node = new ASTNodeIdentifier(ctx->start,ctx->start->getText());
        } else if (ctx->IntegerConstant()){
            node = new ASTNodeInt(ctx->start,std::stoi(ctx->start->getText()));
        } else if (ctx->FloatConstant()){
            node = new ASTNodeFloat(ctx->start,std::stof(ctx->start->getText()));
        } else if (ctx->String()){
            node = new ASTNodeString(ctx->start,ctx->start->getText());
        } else {
            //parenthesis expressions and errors
        }
        return node;
    }/* 
    std::any visitFunctionDefinition(DlangParser::FunctionDefinitionContext* ctx){
        ASTNode node = new ASTNodeFuncDef(ctx->getToken());
        ctx->typeSpecifier()
        return node;
    } */

}  // namespace dlang
