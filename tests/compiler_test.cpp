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
#include <memory>
#include <string>
#include <utility>

#include <vix/template/Compiler.hpp>
#include <vix/template/Lexer.hpp>
#include <vix/template/Parser.hpp>

using namespace vix::template_;

static Template compile_template(
    const std::string &name,
    const std::string &source,
    std::shared_ptr<Loader> loader = nullptr)
{
  Lexer lexer(source);
  auto tokens = lexer.tokenize();

  Parser parser(std::move(tokens));
  RootNode root = parser.parse();

  Compiler compiler;
  return compiler.compile(name, std::move(root), std::move(loader));
}

static void test_compile_basic()
{
  Template tpl = compile_template("hello", "Hello world");

  assert(tpl.name() == "hello");
  assert(!tpl.empty());
  assert(tpl.root().children().size() == 1);
  assert(tpl.plan().size() == 1);
}

static void test_compile_variable()
{
  Template tpl = compile_template("var", "Hello {{ name }}");

  assert(tpl.name() == "var");
  assert(!tpl.empty());
  assert(tpl.root().children().size() == 2);
  assert(tpl.plan().size() == 2);
}

static void test_compile_if()
{
  Template tpl = compile_template("if", "{% if user %}OK{% endif %}");

  assert(tpl.name() == "if");
  assert(!tpl.empty());
  assert(tpl.root().children().size() == 1);

  // JumpIfFalse + EmitText
  assert(tpl.plan().size() == 2);
}

static void test_compile_for()
{
  Template tpl = compile_template(
      "loop",
      "{% for item in items %}{{ item }}{% endfor %}");

  assert(tpl.name() == "loop");
  assert(!tpl.empty());
  assert(tpl.root().children().size() == 1);

  // ForEachBegin + EmitVariable + ForEachEnd
  assert(tpl.plan().size() == 3);
}

static void test_compile_empty()
{
  Template tpl = compile_template("empty", "");

  assert(tpl.name() == "empty");
  assert(tpl.empty());
  assert(tpl.root().children().empty());
  assert(tpl.plan().empty());
}

static void test_compile_adjacent_text_nodes_are_optimized()
{
  RootNode root;
  root.append(std::make_unique<TextNode>("Hello"));
  root.append(std::make_unique<TextNode>(" "));
  root.append(std::make_unique<TextNode>("world"));

  Compiler compiler;
  Template tpl = compiler.compile("optimized_text", std::move(root));

  assert(tpl.name() == "optimized_text");
  assert(!tpl.empty());

  // Optimizer should merge adjacent text nodes.
  assert(tpl.root().children().size() == 1);
  assert(tpl.plan().size() == 1);

  const Node &node = *tpl.root().children().front();
  assert(node.type() == NodeType::Text);

  const auto &text = static_cast<const TextNode &>(node);
  assert(text.value() == "Hello world");
}

int main()
{
  test_compile_basic();
  test_compile_variable();
  test_compile_if();
  test_compile_for();
  test_compile_empty();
  test_compile_adjacent_text_nodes_are_optimized();

  std::cout << "[OK] template compiler tests passed\n";
  return 0;
}
