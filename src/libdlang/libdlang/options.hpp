#pragma once

#include <iosfwd>

namespace dlang {

void parse(std::istream& in, size_t tokens_opt, size_t ast_opt, size_t asm_opt);

}  // namespace dlang
