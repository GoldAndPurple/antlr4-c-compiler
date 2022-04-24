parser grammar DlangParser;

options {
	tokenVocab = DlangLexer;
}

typeSpecifier
    :
    (   Void
    |   Bool
    |   Char
    |   Int
    |   Float
    |   Double
    )
    ;

blockItemList
    :   blockItem
    |   blockItemList blockItem
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
    :   expression opr=( Star | Div | Mod ) expression                       # mulDivExpr
    |   expression opr=( Plus | Minus ) expression                             # addminExpr
    |   expression opr=( LeftShift | RightShift | And | Or | Negate | Caret ) expression   # bitExpr
    |   primaryExpression                                                   # primExpr
    ;

primaryExpression
    :   Identifier
    |   IntegerConstant
    |   FloatConstant
    |   LeftParen expression RightParen
    ;

conditionalExpression
    :   expression ConditionalOperator expression                               # basicConditionalExpr
    |   conditionalExpression ConditionalConnectOperator conditionalExpression  # connectedConditionalExpr
    |   LeftParen conditionalExpression RightParen                              # parenthesizedConditionalExpr
    |   Not LeftParen conditionalExpression RightParen                          # negatedConditionalExpr
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
    |   unaryStatement Semi
    ;

compoundStatement
    :   LeftBrace blockItemList? RightBrace
    ;

expressionStatement
    :   expression? Semi
    ;

ifElseStatement
    :   ifStatement elseIfStatement* elseStatement*
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

unaryStatement
    :   PlusPlus   Identifier  # unaryIncrementStatement
    |   MinusMinus Identifier  # unaryDecrementStatement
    |   Identifier PlusPlus    # unaryIncrementStatement
    |   Identifier MinusMinus  # unaryDecrementStatement
    ;