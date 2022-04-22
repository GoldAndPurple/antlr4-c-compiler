#include <gtest/gtest.h>

#include <libdlang/dump_tokens.hpp>

#include <sstream>
#include <string>

namespace dlang::test {

TEST(LexerSuite, Numbers) {
  // Given
  std::stringstream in("0 1234.56789");

  // When
  std::stringstream out;
  dump_tokens(in, out);

  // Then
  EXPECT_EQ(
      out.str(),
      "<1:0>\tIntegerConstant\t0\n"
      "<1:2>\tFloatConstant\t1234.56789\n");
}

}  // namespace dlang::test
