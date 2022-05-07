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

  /*   out << parser.global()->toStringTree(&parser, true);
    parser.reset(); */

  DlangCustomVisitor visitor;
  ASTNodeProgram* result =
      std::any_cast<ASTNodeProgram*>(visitor.visitGlobal(parser.global()));
  ASTVisitorPrint v;
  v.visit(result);

  out << fmt::format("{tree}\n", fmt::arg("tree", v.treeprint.str()));
}

}  // namespace dlang
