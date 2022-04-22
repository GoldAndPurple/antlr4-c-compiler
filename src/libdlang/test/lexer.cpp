#include <gtest/gtest.h>

#include <libdlang/dump_tokens.hpp>

#include <sstream>
#include <string>

namespace dlang::test {

TEST(LexerSuite, Numbers) {
  // Given
  std::stringstream in("0 1234.56789 -77");

  // When
  std::stringstream out;
  dump_tokens(in, out);

  // Then
  EXPECT_EQ(
      out.str(),
      "<1:0>\tIntegerConstant\t0\n"
      "<1:2>\tFloatConstant\t1234.56789\n"
      "<1:13>\tIntegerConstant\t-77\n");
}

TEST(LexerSuite, Ids) {
  // Given
  std::stringstream in("terrible b8");

  // When
  std::stringstream out;
  dump_tokens(in, out);

  // Then
  EXPECT_EQ(
      out.str(),
      "<1:0>\tIdentifier\tterrible\n"
      "<1:9>\tIdentifier\tb8\n");
}

TEST(LexerSuite, HelloWorld) {
  // Given
  std::stringstream in("void main()\n{\n    printf(\"Hello, world!\");\n}");

  // When
  std::stringstream out;
  dump_tokens(in, out);

  // Then
  EXPECT_EQ(
      out.str(),
      "<1:0>\tVoid\tvoid\n"
      "<1:5>\tIdentifier\tmain\n"
      "<1:9>\tLeftParen\t(\n"
      "<1:10>\tRightParen\t)\n"
      "<2:0>\tLeftBrace\t{\n"
      "<3:4>\tIdentifier\tprintf\n"
      "<3:10>\tLeftParen\t(\n"
      "<3:11>\tString\t\"Hello, world!\"\n"
      "<3:26>\tRightParen\t)\n"
      "<3:27>\tSemi\t;\n"
      "<4:0>\tRightBrace\t}\n");
}

}  // namespace dlang::test
