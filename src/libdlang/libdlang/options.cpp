#include <libdlang/ast.hpp>
#include <libdlang/codegen.hpp>
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

void parse(
    std::istream& in,
    size_t tokens_opt,
    size_t ast_opt,
    size_t asm_opt) {
  antlr4::ANTLRInputStream stream(in);
  DlangLexer lexer(&stream);
  antlr4::CommonTokenStream tokens(&lexer);
  DlangParser parser(&tokens);
  CustomErrorListener error_listener;
  parser.removeErrorListeners();
  parser.addErrorListener(&error_listener);

  // token dump
  if (tokens_opt > 0) {
    for (auto token = lexer.nextToken(); token->getType() != antlr4::Token::EOF;
         token = lexer.nextToken()) {
      std::cout << fmt::format(
          "<{row}:{col}>\t{token_class}\t{lexeme}\n",
          fmt::arg("row", token->getLine()),
          fmt::arg("col", token->getCharPositionInLine()),
          fmt::arg("token_class", get_token_type_name(lexer, *token)),
          fmt::arg("lexeme", token->getText()));
    }
    std::cout << '\n' << '\n' << '\n';
  }
  lexer.reset();

  // ast creation
  DlangCustomVisitor visitor;
  ASTNodeProgram* result =
      std::any_cast<ASTNodeProgram*>(visitor.visitGlobal(parser.global()));

  if (!error_listener.errors.empty()) {
    std::cerr << "Parsing error:\n";
    for (auto& e : error_listener.errors) {
      std::cerr << e << '\n';
    }
    return;
  }

  // symtab creation
  ASTVisitorScope scope_maker;
  scope_maker.visit(result);
  if (!scope_maker.errors.empty()) {
    std::cerr << "Semantic error:\n";
    for (auto& e : scope_maker.errors) {
      std::cerr << e << '\n';
    }
    return;
  }

  // ast printout
  if (ast_opt > 0) {
    ASTVisitorPrint ast_printer;
    ast_printer.visit(result);
    std::cout << ast_printer.treeprint.str();

    std::cout << '\n' << '\n' << '\n';
  }

  // codegen
  ASTVisitorCodegen generator(&(scope_maker.global));
  generator.visit(result);
  if (asm_opt > 0) {
    generator.modul->print(llvm::outs(), nullptr);
  }

  std::error_code err;
  llvm::raw_fd_ostream* out_file =
      new llvm::raw_fd_ostream("intermediate.ll", err);
  generator.modul->print(*out_file, nullptr);
}

}  // namespace dlang
