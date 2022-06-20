parser grammar DlangParser;

options {
	tokenVocab = DlangLexer;
}

global
    :   externalDeclaration* EOF
    ;

externalDeclaration
    :   functionDefinition
    |   declaration
    |   Semi
    ;

typeSpecifier
    :
    (   Void
    |   Char
    |   Int
    |   Float
    ) (Star)?
    ;

blockItemList
    :   blockItem*
    ;

blockItem
    :   statement
    |   declaration
    |   functionCall
    ;

declaration
:   typeSpecifier Identifier (Comma Identifier)* Semi
|   typeSpecifier assignmentExpression (Comma assignmentExpression)* Semi
;


functionParameterList
    :   typeSpecifier Identifier (Comma typeSpecifier Identifier)*
    ;

functionDefinition
    :   typeSpecifier Identifier parameterTypeList compoundStatement
    ;

parameterTypeList
    :   LeftParen functionParameterList? RightParen
    ;

functionCall
    :   Identifier LeftParen identifierList? RightParen Semi
    ;

functionReturn
    :   Identifier LeftParen identifierList? RightParen
    ;

identifierList
    :   expression (Comma expression)*
    ;

expression
    :   expression opr=( Star | Div | Mod ) expression                                      # mulDivExpr
    |   expression opr=( Plus | Minus ) expression                                          # addsubExpr
    |   primaryExpression                                                                   # primeExpr
    ;

primaryExpression
    :   (And?) Identifier
    |   IntegerConstant
    |   FloatConstant
    |   String
    |   LeftParen expression RightParen
    ;

conditionalExpression
    :   expression conditionalOperator expression                               # basicConditionalExpr
    ;

assignmentExpression
    :   Identifier Assign expression
    |   Identifier Assign functionReturn
    ;

statement
    :   compoundStatement
    |   expressionStatement
    |   ifElseStatement
    |   iterationStatement
    |   assignmentStatement
    |   jumpStatement
    ;

compoundStatement
    :   LeftBrace blockItemList? RightBrace
    ;

expressionStatement
    :   expression? Semi
    ;

ifElseStatement
    :   ifStatement elseIfStatement* elseStatement?
    ;

ifStatement
    :   If LeftParen conditionalExpression RightParen statement
    ;

elseIfStatement
    :   Else If LeftParen conditionalExpression RightParen statement
    ;

elseStatement
    :   Else statement
    ;

iterationStatement
    :   While LeftParen conditionalExpression RightParen statement
    ;

assignmentStatement
    :   assignmentExpression Semi
    ;

jumpStatement
    :   Return expression? Semi
    ;

conditionalOperator
    :   Less
    |   LessEqual
    |   Greater
    |   GreaterEqual
    |   Equal
    |   NotEqual
    ;