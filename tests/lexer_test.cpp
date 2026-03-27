/**
 *
 *  @file lexer_test.cpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2025, Gaspard Kirira.
 *  All rights reserved.
 *  https://github.com/vixcpp/vix
 *
 *  Use of this source code is governed by a MIT license
 *  that can be found in the License file.
 *
 *  Vix.cpp
 *
 */
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include <vix/template/Lexer.hpp>
#include <vix/template/Token.hpp>

using namespace vix::template_;

static void test_plain_text()
{
  Lexer lexer("Hello world");
  const auto tokens = lexer.tokenize();

  assert(tokens.size() == 2);
  assert(tokens[0].type == TokenType::Text);
  assert(tokens[0].value == "Hello world");
  assert(tokens[1].type == TokenType::EndOfFile);
}

static void test_variable()
{
  Lexer lexer("Hello {{ name }}!");
  const auto tokens = lexer.tokenize();

  assert(tokens.size() == 6);

  assert(tokens[0].type == TokenType::Text);
  assert(tokens[0].value == "Hello ");

  assert(tokens[1].type == TokenType::VariableOpen);
  assert(tokens[1].value == "{{");

  assert(tokens[2].type == TokenType::Identifier);
  assert(tokens[2].value == "name");

  assert(tokens[3].type == TokenType::VariableClose);
  assert(tokens[3].value == "}}");

  assert(tokens[4].type == TokenType::Text);
  assert(tokens[4].value == "!");

  assert(tokens[5].type == TokenType::EndOfFile);
}

static void test_block()
{
  Lexer lexer("{% if user %}Hello{% endif %}");
  const auto tokens = lexer.tokenize();

  assert(tokens.size() >= 8);

  assert(tokens[0].type == TokenType::BlockOpen);
  assert(tokens[0].value == "{%");

  assert(tokens[1].type == TokenType::Identifier);
  assert(tokens[1].value == "if");

  assert(tokens[2].type == TokenType::Identifier);
  assert(tokens[2].value == "user");

  assert(tokens[3].type == TokenType::BlockClose);
  assert(tokens[3].value == "%}");
}

static void test_string()
{
  Lexer lexer("{{ \"hello\" }}");
  const auto tokens = lexer.tokenize();

  bool found_string = false;
  for (const auto &t : tokens)
  {
    if (t.type == TokenType::String && t.value == "hello")
    {
      found_string = true;
    }
  }

  assert(found_string);
}

static void test_number()
{
  Lexer lexer("{{ 123 }}");
  const auto tokens = lexer.tokenize();

  bool found_number = false;
  for (const auto &t : tokens)
  {
    if (t.type == TokenType::Number && t.value == "123")
    {
      found_number = true;
    }
  }

  assert(found_number);
}

static void test_floating_number()
{
  Lexer lexer("{{ 3.14 }}");
  const auto tokens = lexer.tokenize();

  bool found_number = false;
  for (const auto &t : tokens)
  {
    if (t.type == TokenType::Number && t.value == "3.14")
    {
      found_number = true;
    }
  }

  assert(found_number);
}

static void test_member_access()
{
  Lexer lexer("{{ user.name }}");
  const auto tokens = lexer.tokenize();

  assert(tokens.size() == 6);

  assert(tokens[0].type == TokenType::VariableOpen);
  assert(tokens[1].type == TokenType::Identifier);
  assert(tokens[1].value == "user");

  assert(tokens[2].type == TokenType::Punctuation);
  assert(tokens[2].value == ".");

  assert(tokens[3].type == TokenType::Identifier);
  assert(tokens[3].value == "name");

  assert(tokens[4].type == TokenType::VariableClose);
  assert(tokens[5].type == TokenType::EndOfFile);
}

static void test_arithmetic_expression()
{
  Lexer lexer("{{ price * quantity }}");
  const auto tokens = lexer.tokenize();

  assert(tokens.size() == 6);

  assert(tokens[0].type == TokenType::VariableOpen);

  assert(tokens[1].type == TokenType::Identifier);
  assert(tokens[1].value == "price");

  assert(tokens[2].type == TokenType::Operator);
  assert(tokens[2].value == "*");

  assert(tokens[3].type == TokenType::Identifier);
  assert(tokens[3].value == "quantity");

  assert(tokens[4].type == TokenType::VariableClose);
  assert(tokens[5].type == TokenType::EndOfFile);
}

static void test_comparison_expression()
{
  Lexer lexer("{{ a == b }}");
  const auto tokens = lexer.tokenize();

  assert(tokens.size() == 6);

  assert(tokens[0].type == TokenType::VariableOpen);

  assert(tokens[1].type == TokenType::Identifier);
  assert(tokens[1].value == "a");

  assert(tokens[2].type == TokenType::Operator);
  assert(tokens[2].value == "==");

  assert(tokens[3].type == TokenType::Identifier);
  assert(tokens[3].value == "b");

  assert(tokens[4].type == TokenType::VariableClose);
  assert(tokens[5].type == TokenType::EndOfFile);
}

static void test_unary_expression()
{
  Lexer lexer("{{ !enabled }}");
  const auto tokens = lexer.tokenize();

  assert(tokens.size() == 5);

  assert(tokens[0].type == TokenType::VariableOpen);

  assert(tokens[1].type == TokenType::Operator);
  assert(tokens[1].value == "!");

  assert(tokens[2].type == TokenType::Identifier);
  assert(tokens[2].value == "enabled");

  assert(tokens[3].type == TokenType::VariableClose);
  assert(tokens[4].type == TokenType::EndOfFile);
}

static void test_parenthesized_expression()
{
  Lexer lexer("{{ (price + tax) * quantity }}");
  const auto tokens = lexer.tokenize();

  assert(tokens.size() == 10);

  assert(tokens[0].type == TokenType::VariableOpen);

  assert(tokens[1].type == TokenType::Punctuation);
  assert(tokens[1].value == "(");

  assert(tokens[2].type == TokenType::Identifier);
  assert(tokens[2].value == "price");

  assert(tokens[3].type == TokenType::Operator);
  assert(tokens[3].value == "+");

  assert(tokens[4].type == TokenType::Identifier);
  assert(tokens[4].value == "tax");

  assert(tokens[5].type == TokenType::Punctuation);
  assert(tokens[5].value == ")");

  assert(tokens[6].type == TokenType::Operator);
  assert(tokens[6].value == "*");

  assert(tokens[7].type == TokenType::Identifier);
  assert(tokens[7].value == "quantity");

  assert(tokens[8].type == TokenType::VariableClose);
  assert(tokens[9].type == TokenType::EndOfFile);
}

static void test_filter_pipe()
{
  Lexer lexer("{{ user.name | upper }}");
  const auto tokens = lexer.tokenize();

  assert(tokens.size() == 8);

  assert(tokens[0].type == TokenType::VariableOpen);

  assert(tokens[1].type == TokenType::Identifier);
  assert(tokens[1].value == "user");

  assert(tokens[2].type == TokenType::Punctuation);
  assert(tokens[2].value == ".");

  assert(tokens[3].type == TokenType::Identifier);
  assert(tokens[3].value == "name");

  assert(tokens[4].type == TokenType::Operator);
  assert(tokens[4].value == "|");

  assert(tokens[5].type == TokenType::Identifier);
  assert(tokens[5].value == "upper");

  assert(tokens[6].type == TokenType::VariableClose);
  assert(tokens[7].type == TokenType::EndOfFile);
}

static void test_logical_operators()
{
  Lexer lexer("{% if a >= b && enabled || admin %}{% endif %}");
  const auto tokens = lexer.tokenize();

  bool found_gte = false;
  bool found_and = false;
  bool found_or = false;

  for (const auto &t : tokens)
  {
    if (t.type == TokenType::Operator && t.value == ">=")
    {
      found_gte = true;
    }

    if (t.type == TokenType::Operator && t.value == "&&")
    {
      found_and = true;
    }

    if (t.type == TokenType::Operator && t.value == "||")
    {
      found_or = true;
    }
  }

  assert(found_gte);
  assert(found_and);
  assert(found_or);
}

int main()
{
  test_plain_text();
  test_variable();
  test_block();
  test_string();
  test_number();
  test_floating_number();
  test_member_access();
  test_arithmetic_expression();
  test_comparison_expression();
  test_unary_expression();
  test_parenthesized_expression();
  test_filter_pipe();
  test_logical_operators();

  std::cout << "[OK] template lexer tests passed\n";
  return 0;
}
