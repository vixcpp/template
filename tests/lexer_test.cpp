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
  auto tokens = lexer.tokenize();

  assert(tokens.size() == 2);
  assert(tokens[0].type == TokenType::Text);
  assert(tokens[0].value == "Hello world");
  assert(tokens[1].type == TokenType::EndOfFile);
}

static void test_variable()
{
  Lexer lexer("Hello {{ name }}!");
  auto tokens = lexer.tokenize();

  assert(tokens.size() >= 5);

  assert(tokens[0].type == TokenType::Text);
  assert(tokens[1].type == TokenType::VariableOpen);
  assert(tokens[2].type == TokenType::Identifier);
  assert(tokens[2].value == "name");
  assert(tokens.back().type == TokenType::EndOfFile);
}

static void test_block()
{
  Lexer lexer("{% if user %}Hello{% endif %}");
  auto tokens = lexer.tokenize();

  assert(!tokens.empty());
  assert(tokens[0].type == TokenType::BlockOpen);
}

static void test_string()
{
  Lexer lexer("{{ \"hello\" }}");
  auto tokens = lexer.tokenize();

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
  auto tokens = lexer.tokenize();

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

int main()
{
  test_plain_text();
  test_variable();
  test_block();
  test_string();
  test_number();

  std::cout << "[OK] template lexer tests passed\n";
  return 0;
}
