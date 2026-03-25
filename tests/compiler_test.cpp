/**
 *
 *  @file compiler_test.cpp
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

#include <vix/template/Lexer.hpp>
#include <vix/template/Parser.hpp>
#include <vix/template/Compiler.hpp>

using namespace vix::template_;

static Template compile_template(const std::string &name, const std::string &source)
{
  Lexer lexer(source);
  auto tokens = lexer.tokenize();

  Parser parser(std::move(tokens));
  RootNode root = parser.parse();

  Compiler compiler;
  return compiler.compile(name, std::move(root));
}

static void test_compile_basic()
{
  Template tpl = compile_template("hello", "Hello world");

  assert(tpl.name() == "hello");
  assert(!tpl.empty());
  assert(tpl.root().children().size() == 1);
}

static void test_compile_variable()
{
  Template tpl = compile_template("var", "Hello {{ name }}");

  assert(tpl.name() == "var");
  assert(tpl.root().children().size() == 2);
}

static void test_compile_if()
{
  Template tpl = compile_template("if", "{% if user %}OK{% endif %}");

  assert(tpl.name() == "if");
  assert(tpl.root().children().size() == 1);
}

static void test_compile_for()
{
  Template tpl = compile_template(
      "loop",
      "{% for item in items %}{{ item }}{% endfor %}");

  assert(tpl.name() == "loop");
  assert(tpl.root().children().size() == 1);
}

static void test_compile_empty()
{
  Template tpl = compile_template("empty", "");

  assert(tpl.name() == "empty");
  assert(tpl.empty() || tpl.root().children().empty());
}

int main()
{
  test_compile_basic();
  test_compile_variable();
  test_compile_if();
  test_compile_for();
  test_compile_empty();

  std::cout << "[OK] template compiler tests passed\n";
  return 0;
}
