#pragma once

#include <iosfwd>

namespace dlang {

void parse(std::istream& in);
void dump_tokens(std::istream& in, std::ostream& out);
void dump_ast(std::istream& in, std::ostream& out);

}  // namespace dlang
