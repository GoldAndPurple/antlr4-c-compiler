#include <libdlang/ast.hpp>
#include <libdlang/options.hpp>

#include <DlangLexer.h>
#include <DlangParser.h>

#include <antlr4-runtime.h>
#include <fmt/format.h>

namespace dlang {

static std::string_view get_token_type_name(
    const DlangLexer& lexer,
    const antlr4::Token& token) {
  return lexer.getVocabulary().getSymbolicName(token.getType());
}

void parse(std::istream& in) {
  antlr4::ANTLRInputStream stream(in);
  DlangLexer lexer(&stream);
  antlr4::CommonTokenStream tokens(&lexer);
  DlangParser parser(&tokens);
  CustomErrorListener error_listener;
  parser.removeErrorListeners();
  parser.addErrorListener(&error_listener);

  // token dump
  /* for (auto token = lexer.nextToken(); token->getType() != antlr4::Token::EOF;
       token = lexer.nextToken()) {
    std::cout << fmt::format(
        "<{row}:{col}>\t{token_class}\t{lexeme}\n",
        fmt::arg("row", token->getLine()),
        fmt::arg("col", token->getCharPositionInLine()),
        fmt::arg("token_class", get_token_type_name(lexer, *token)),
        fmt::arg("lexeme", token->getText()));
  } */
  lexer.reset();

  // ast creation
  DlangCustomVisitor visitor;
  ASTNodeProgram* result =
      std::any_cast<ASTNodeProgram*>(visitor.visitGlobal(parser.global()));

  if (!error_listener.errors.empty()) {
    for (auto& e : error_listener.errors) {
      std::cerr << e << '\n';
    }
    return;
  }

  //symtab creation
  ASTVisitorScope scope_maker;
  try {
    scope_maker.visit(result);
  } catch (const std::runtime_error& e) {
    std::cerr << e.what() << "\n";
    return;
  }

  //ast printout
  ASTVisitorPrint ast_printer;
  ast_printer.visit(result);
  std::cout << ast_printer.treeprint.str();


}

void dump_tokens(std::istream& in, std::ostream& out) {
  antlr4::ANTLRInputStream stream(in);
  DlangLexer lexer(&stream);

  for (auto token = lexer.nextToken(); token->getType() != antlr4::Token::EOF;
       token = lexer.nextToken()) {
    out << fmt::format(
        "<{row}:{col}>\t{token_class}\t{lexeme}\n",
        fmt::arg("row", token->getLine()),
        fmt::arg("col", token->getCharPositionInLine()),
        fmt::arg("token_class", get_token_type_name(lexer, *token)),
        fmt::arg("lexeme", token->getText()));
  }
}

void dump_ast(std::istream& in, std::ostream& out) {
  antlr4::ANTLRInputStream stream(in);
  DlangLexer lexer(&stream);
  antlr4::CommonTokenStream tokens(&lexer);
  DlangParser parser(&tokens);

  CustomErrorListener error_listener;
  parser.removeErrorListeners();
  parser.addErrorListener(&error_listener);

  /*   out << parser.global()->toStringTree(&parser, true);
    parser.reset(); */

  DlangCustomVisitor visitor;
  ASTNodeProgram* result =
      std::any_cast<ASTNodeProgram*>(visitor.visitGlobal(parser.global()));

  if (!error_listener.errors.empty()) {
    for (auto& e : error_listener.errors) {
      std::cerr << e << '\n';
    }
    return;
  }
  ASTVisitorScope scope_maker;
  try {
    scope_maker.visit(result);
  } catch (const std::runtime_error& e) {
    std::cerr << e.what() << "\n";
    return;
  }

  ASTVisitorPrint ast_printer;
  ast_printer.visit(result);

  out << fmt::format("{tree}\n", fmt::arg("tree", ast_printer.treeprint.str()));
}

}  // namespace dlang
