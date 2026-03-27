/**
 *
 *  @file parser_test.cpp
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
#include <vector>

#include <vix/template/AST.hpp>
#include <vix/template/Lexer.hpp>
#include <vix/template/Parser.hpp>

using namespace vix::template_;

static RootNode parse_template(const std::string &source)
{
  Lexer lexer(source);
  auto tokens = lexer.tokenize();

  Parser parser(std::move(tokens));
  return parser.parse();
}

static void test_parse_text()
{
  RootNode root = parse_template("Hello world");

  assert(root.children().size() == 1);
  assert(root.children()[0]->type() == NodeType::Text);

  const auto *text =
      static_cast<const TextNode *>(root.children()[0].get());

  assert(text->value() == "Hello world");
}

static void test_parse_variable()
{
  RootNode root = parse_template("Hello {{ name }}");

  assert(root.children().size() == 2);
  assert(root.children()[0]->type() == NodeType::Text);
  assert(root.children()[1]->type() == NodeType::Variable);

  const auto *var =
      static_cast<const VariableNode *>(root.children()[1].get());

  assert(var->name() == "name");
  assert(var->filters().empty());
}

static void test_parse_variable_with_one_filter()
{
  RootNode root = parse_template("{{ name | upper }}");

  assert(root.children().size() == 1);
  assert(root.children()[0]->type() == NodeType::Variable);

  const auto *var =
      static_cast<const VariableNode *>(root.children()[0].get());

  assert(var->name() == "name");
  assert(var->filters().size() == 1);
  assert(var->filters()[0].name() == "upper");
}

static void test_parse_variable_with_multiple_filters()
{
  RootNode root = parse_template("{{ name | upper | length }}");

  assert(root.children().size() == 1);
  assert(root.children()[0]->type() == NodeType::Variable);

  const auto *var =
      static_cast<const VariableNode *>(root.children()[0].get());

  assert(var->name() == "name");
  assert(var->filters().size() == 2);
  assert(var->filters()[0].name() == "upper");
  assert(var->filters()[1].name() == "length");
}

static void test_parse_if()
{
  RootNode root = parse_template("{% if user %}Hello{% endif %}");

  assert(root.children().size() == 1);
  assert(root.children()[0]->type() == NodeType::If);

  const auto *if_node =
      static_cast<const IfNode *>(root.children()[0].get());

  assert(if_node->condition() == "user");
  assert(if_node->body().size() == 1);
  assert(if_node->body()[0]->type() == NodeType::Text);

  const auto *text =
      static_cast<const TextNode *>(if_node->body()[0].get());

  assert(text->value() == "Hello");
}

static void test_parse_for()
{
  RootNode root = parse_template("{% for item in items %}{{ item }}{% endfor %}");

  assert(root.children().size() == 1);
  assert(root.children()[0]->type() == NodeType::For);

  const auto *for_node =
      static_cast<const ForNode *>(root.children()[0].get());

  assert(for_node->item_name() == "item");
  assert(for_node->iterable_name() == "items");
  assert(for_node->body().size() == 1);
  assert(for_node->body()[0]->type() == NodeType::Variable);

  const auto *var =
      static_cast<const VariableNode *>(for_node->body()[0].get());

  assert(var->name() == "item");
  assert(var->filters().empty());
}

static void test_parse_nested_if_in_for()
{
  RootNode root = parse_template(
      "{% for item in items %}{% if item %}X{% endif %}{% endfor %}");

  assert(root.children().size() == 1);
  assert(root.children()[0]->type() == NodeType::For);

  const auto *for_node =
      static_cast<const ForNode *>(root.children()[0].get());

  assert(for_node->body().size() == 1);
  assert(for_node->body()[0]->type() == NodeType::If);

  const auto *if_node =
      static_cast<const IfNode *>(for_node->body()[0].get());

  assert(if_node->condition() == "item");
  assert(if_node->body().size() == 1);
  assert(if_node->body()[0]->type() == NodeType::Text);
}

static void test_parse_include()
{
  RootNode root = parse_template("{% include \"header.html\" %}");

  assert(root.children().size() == 1);
  assert(root.children()[0]->type() == NodeType::Include);

  const auto *include_node =
      static_cast<const IncludeNode *>(root.children()[0].get());

  assert(include_node->template_name() == "header.html");
}

static void test_parse_include_with_surrounding_text()
{
  RootNode root = parse_template("Hello {% include \"header.html\" %} World");

  assert(root.children().size() == 3);
  assert(root.children()[0]->type() == NodeType::Text);
  assert(root.children()[1]->type() == NodeType::Include);
  assert(root.children()[2]->type() == NodeType::Text);

  const auto *text_before =
      static_cast<const TextNode *>(root.children()[0].get());
  const auto *include_node =
      static_cast<const IncludeNode *>(root.children()[1].get());
  const auto *text_after =
      static_cast<const TextNode *>(root.children()[2].get());

  assert(text_before->value() == "Hello ");
  assert(include_node->template_name() == "header.html");
  assert(text_after->value() == " World");
}

int main()
{
  test_parse_text();
  test_parse_variable();
  test_parse_variable_with_one_filter();
  test_parse_variable_with_multiple_filters();
  test_parse_if();
  test_parse_for();
  test_parse_nested_if_in_for();
  test_parse_include();
  test_parse_include_with_surrounding_text();

  std::cout << "[OK] template parser tests passed\n";
  return 0;
}
