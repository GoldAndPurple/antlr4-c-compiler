#include <libdlang/options.hpp>

#include <cxxopts.hpp>

#include <fstream>
#include <iostream>
#include <string>

const char* const file_path_opt = "file_path";
const char* const dump_tokens_opt = "dump-tokens";
const char* const dump_ast_opt = "dump-ast";
const char* const dump_asm_opt = "dump-asm";

int main(int argc, char** argv) {
  cxxopts::Options options("dlang-parser", "ANTLR4 dlang parser example");

  options.positional_help("<file_path>");

  try {
    options.add_options()
        (file_path_opt, "", cxxopts::value<std::string>())
        (dump_tokens_opt, "")
        (dump_ast_opt, "")
        (dump_asm_opt, "")
        ("h,help", "Print help");
  } catch (const cxxopts::OptionSpecException& e) {
    std::cerr << e.what() << "\n";
    return -1;
  }

  options.parse_positional({file_path_opt});

  try {
    const auto result = options.parse(argc, argv);

    if (result.count("help") > 0 || result.count(file_path_opt) != 1) {
      std::cout << options.help() << "\n";
      return 0;
    }

    std::ifstream input_stream(result[file_path_opt].as<std::string>());

    if (!input_stream.good()) {
      std::cerr << "Unable to read stream\n";
      return -1;
    }

    dlang::parse(
        input_stream,
        result.count(dump_tokens_opt),
        result.count(dump_ast_opt),
        result.count(dump_asm_opt));

  } catch (const cxxopts::OptionException& e) {
    std::cerr << e.what() << "\n";
    return -1;
  } catch (const std::runtime_error& e) {
    std::cerr << "Parsing error\n";
    std::cerr << e.what() << "\n";
    return -1;
  }
}
