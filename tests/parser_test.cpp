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
#include <vix/template/Error.hpp>
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

static const NameExpression *as_name(const Expression &expr)
{
  assert(expr.type() == ExprType::Name);
  return static_cast<const NameExpression *>(&expr);
}

static const LiteralExpression *as_literal(const Expression &expr)
{
  assert(expr.type() == ExprType::Literal);
  return static_cast<const LiteralExpression *>(&expr);
}

static const MemberExpression *as_member(const Expression &expr)
{
  assert(expr.type() == ExprType::Member);
  return static_cast<const MemberExpression *>(&expr);
}

static const UnaryExpression *as_unary(const Expression &expr)
{
  assert(expr.type() == ExprType::Unary);
  return static_cast<const UnaryExpression *>(&expr);
}

static const BinaryExpression *as_binary(const Expression &expr)
{
  assert(expr.type() == ExprType::Binary);
  return static_cast<const BinaryExpression *>(&expr);
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

  assert(var->filters().empty());
  assert(var->expression().type() == ExprType::Name);

  const auto *name = as_name(var->expression());
  assert(name->name() == "name");
}

static void test_parse_variable_with_one_filter()
{
  RootNode root = parse_template("{{ name | upper }}");

  assert(root.children().size() == 1);
  assert(root.children()[0]->type() == NodeType::Variable);

  const auto *var =
      static_cast<const VariableNode *>(root.children()[0].get());

  assert(var->expression().type() == ExprType::Name);
  assert(var->filters().size() == 1);
  assert(var->filters()[0].name() == "upper");

  const auto *name = as_name(var->expression());
  assert(name->name() == "name");
}

static void test_parse_variable_with_multiple_filters()
{
  RootNode root = parse_template("{{ name | upper | length }}");

  assert(root.children().size() == 1);
  assert(root.children()[0]->type() == NodeType::Variable);

  const auto *var =
      static_cast<const VariableNode *>(root.children()[0].get());

  assert(var->expression().type() == ExprType::Name);
  assert(var->filters().size() == 2);
  assert(var->filters()[0].name() == "upper");
  assert(var->filters()[1].name() == "length");

  const auto *name = as_name(var->expression());
  assert(name->name() == "name");
}

static void test_parse_member_expression()
{
  RootNode root = parse_template("{{ user.name }}");

  assert(root.children().size() == 1);
  assert(root.children()[0]->type() == NodeType::Variable);

  const auto *var =
      static_cast<const VariableNode *>(root.children()[0].get());

  assert(var->expression().type() == ExprType::Member);

  const auto *member = as_member(var->expression());
  assert(member->member() == "name");

  const auto *object = as_name(member->object());
  assert(object->name() == "user");
}

static void test_parse_nested_member_expression()
{
  RootNode root = parse_template("{{ order.customer.name }}");

  assert(root.children().size() == 1);
  assert(root.children()[0]->type() == NodeType::Variable);

  const auto *var =
      static_cast<const VariableNode *>(root.children()[0].get());

  assert(var->expression().type() == ExprType::Member);

  const auto *outer = as_member(var->expression());
  assert(outer->member() == "name");

  const auto *inner = as_member(outer->object());
  assert(inner->member() == "customer");

  const auto *base = as_name(inner->object());
  assert(base->name() == "order");
}

static void test_parse_number_literal()
{
  RootNode root = parse_template("{{ 123 }}");

  assert(root.children().size() == 1);
  assert(root.children()[0]->type() == NodeType::Variable);

  const auto *var =
      static_cast<const VariableNode *>(root.children()[0].get());

  assert(var->expression().type() == ExprType::Literal);

  const auto *literal = as_literal(var->expression());
  assert(literal->value() == "123");
}

static void test_parse_string_literal()
{
  RootNode root = parse_template("{{ \"hello\" }}");

  assert(root.children().size() == 1);
  assert(root.children()[0]->type() == NodeType::Variable);

  const auto *var =
      static_cast<const VariableNode *>(root.children()[0].get());

  assert(var->expression().type() == ExprType::Literal);

  const auto *literal = as_literal(var->expression());
  assert(literal->value() == "hello");
}

static void test_parse_bool_literal()
{
  RootNode root = parse_template("{{ true }}");

  assert(root.children().size() == 1);
  assert(root.children()[0]->type() == NodeType::Variable);

  const auto *var =
      static_cast<const VariableNode *>(root.children()[0].get());

  assert(var->expression().type() == ExprType::Literal);

  const auto *literal = as_literal(var->expression());
  assert(literal->value() == "true");
}

static void test_parse_if()
{
  RootNode root = parse_template("{% if user %}Hello{% endif %}");

  assert(root.children().size() == 1);
  assert(root.children()[0]->type() == NodeType::If);

  const auto *if_node =
      static_cast<const IfNode *>(root.children()[0].get());

  assert(if_node->condition().type() == ExprType::Name);

  const auto *name = as_name(if_node->condition());
  assert(name->name() == "user");

  assert(if_node->body().size() == 1);
  assert(if_node->body()[0]->type() == NodeType::Text);

  const auto *text =
      static_cast<const TextNode *>(if_node->body()[0].get());

  assert(text->value() == "Hello");
}

static void test_parse_binary_expression()
{
  RootNode root = parse_template("{{ price * quantity }}");

  assert(root.children().size() == 1);
  assert(root.children()[0]->type() == NodeType::Variable);

  const auto *var =
      static_cast<const VariableNode *>(root.children()[0].get());

  assert(var->expression().type() == ExprType::Binary);

  const auto *binary = as_binary(var->expression());
  assert(binary->op() == BinaryOperator::Multiply);

  const auto *left = as_name(binary->left());
  const auto *right = as_name(binary->right());

  assert(left->name() == "price");
  assert(right->name() == "quantity");
}

static void test_parse_comparison_expression()
{
  RootNode root = parse_template("{{ a == b }}");

  assert(root.children().size() == 1);
  assert(root.children()[0]->type() == NodeType::Variable);

  const auto *var =
      static_cast<const VariableNode *>(root.children()[0].get());

  assert(var->expression().type() == ExprType::Binary);

  const auto *binary = as_binary(var->expression());
  assert(binary->op() == BinaryOperator::Equal);

  const auto *left = as_name(binary->left());
  const auto *right = as_name(binary->right());

  assert(left->name() == "a");
  assert(right->name() == "b");
}

static void test_parse_unary_expression()
{
  RootNode root = parse_template("{{ !enabled }}");

  assert(root.children().size() == 1);
  assert(root.children()[0]->type() == NodeType::Variable);

  const auto *var =
      static_cast<const VariableNode *>(root.children()[0].get());

  assert(var->expression().type() == ExprType::Unary);

  const auto *unary = as_unary(var->expression());
  assert(unary->op() == UnaryOperator::Not);

  const auto *operand = as_name(unary->operand());
  assert(operand->name() == "enabled");
}

static void test_parse_parenthesized_expression()
{
  RootNode root = parse_template("{{ (price + tax) * quantity }}");

  assert(root.children().size() == 1);
  assert(root.children()[0]->type() == NodeType::Variable);

  const auto *var =
      static_cast<const VariableNode *>(root.children()[0].get());

  assert(var->expression().type() == ExprType::Binary);

  const auto *outer = as_binary(var->expression());
  assert(outer->op() == BinaryOperator::Multiply);

  const auto *left_group = as_binary(outer->left());
  assert(left_group->op() == BinaryOperator::Add);

  const auto *left_a = as_name(left_group->left());
  const auto *left_b = as_name(left_group->right());
  const auto *right = as_name(outer->right());

  assert(left_a->name() == "price");
  assert(left_b->name() == "tax");
  assert(right->name() == "quantity");
}

static void test_parse_operator_precedence()
{
  RootNode root = parse_template("{{ a + b * c }}");

  assert(root.children().size() == 1);

  const auto *var =
      static_cast<const VariableNode *>(root.children()[0].get());

  assert(var->expression().type() == ExprType::Binary);

  const auto *outer = as_binary(var->expression());
  assert(outer->op() == BinaryOperator::Add);

  const auto *left = as_name(outer->left());
  assert(left->name() == "a");

  const auto *right = as_binary(outer->right());
  assert(right->op() == BinaryOperator::Multiply);

  const auto *rb_left = as_name(right->left());
  const auto *rb_right = as_name(right->right());

  assert(rb_left->name() == "b");
  assert(rb_right->name() == "c");
}

static void test_parse_if_with_expression()
{
  RootNode root = parse_template(
      "{% if price * quantity > 0 %}Hello{% endif %}");

  assert(root.children().size() == 1);
  assert(root.children()[0]->type() == NodeType::If);

  const auto *if_node =
      static_cast<const IfNode *>(root.children()[0].get());

  assert(if_node->condition().type() == ExprType::Binary);

  const auto *cmp = as_binary(if_node->condition());
  assert(cmp->op() == BinaryOperator::Greater);

  const auto *left = as_binary(cmp->left());
  assert(left->op() == BinaryOperator::Multiply);

  const auto *l1 = as_name(left->left());
  const auto *l2 = as_name(left->right());
  const auto *right = as_literal(cmp->right());

  assert(l1->name() == "price");
  assert(l2->name() == "quantity");
  assert(right->value() == "0");
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

  assert(var->expression().type() == ExprType::Name);
  assert(var->filters().empty());

  const auto *name = as_name(var->expression());
  assert(name->name() == "item");
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

  assert(if_node->condition().type() == ExprType::Name);

  const auto *name = as_name(if_node->condition());
  assert(name->name() == "item");

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

static void test_parse_extends()
{
  RootNode root = parse_template("{% extends \"base.html\" %}");

  assert(root.children().size() == 1);
  assert(root.children()[0]->type() == NodeType::Extends);

  const auto *extends_node =
      static_cast<const ExtendsNode *>(root.children()[0].get());

  assert(extends_node->template_name() == "base.html");
}

static void test_parse_block()
{
  RootNode root = parse_template("{% block content %}Hello{% endblock %}");

  assert(root.children().size() == 1);
  assert(root.children()[0]->type() == NodeType::Block);

  const auto *block_node =
      static_cast<const BlockNode *>(root.children()[0].get());

  assert(block_node->name() == "content");
  assert(block_node->body().size() == 1);
  assert(block_node->body()[0]->type() == NodeType::Text);

  const auto *text =
      static_cast<const TextNode *>(block_node->body()[0].get());

  assert(text->value() == "Hello");
}

static void test_parse_extends_with_block()
{
  RootNode root = parse_template(
      "{% extends \"base.html\" %}{% block content %}Hello{% endblock %}");

  assert(root.children().size() == 2);
  assert(root.children()[0]->type() == NodeType::Extends);
  assert(root.children()[1]->type() == NodeType::Block);

  const auto *extends_node =
      static_cast<const ExtendsNode *>(root.children()[0].get());
  const auto *block_node =
      static_cast<const BlockNode *>(root.children()[1].get());

  assert(extends_node->template_name() == "base.html");
  assert(block_node->name() == "content");
  assert(block_node->body().size() == 1);
  assert(block_node->body()[0]->type() == NodeType::Text);

  const auto *text =
      static_cast<const TextNode *>(block_node->body()[0].get());

  assert(text->value() == "Hello");
}

static void test_parse_nested_block_nodes()
{
  RootNode root = parse_template(
      "{% block content %}Hello {{ name }}{% if user %}X{% endif %}{% endblock %}");

  assert(root.children().size() == 1);
  assert(root.children()[0]->type() == NodeType::Block);

  const auto *block_node =
      static_cast<const BlockNode *>(root.children()[0].get());

  assert(block_node->name() == "content");
  assert(block_node->body().size() == 3);
  assert(block_node->body()[0]->type() == NodeType::Text);
  assert(block_node->body()[1]->type() == NodeType::Variable);
  assert(block_node->body()[2]->type() == NodeType::If);

  const auto *var =
      static_cast<const VariableNode *>(block_node->body()[1].get());
  const auto *if_node =
      static_cast<const IfNode *>(block_node->body()[2].get());

  assert(var->expression().type() == ExprType::Name);
  assert(as_name(var->expression())->name() == "name");

  assert(if_node->condition().type() == ExprType::Name);
  assert(as_name(if_node->condition())->name() == "user");
}

static void test_parse_block_missing_endblock()
{
  bool thrown = false;

  try
  {
    (void)parse_template("{% block content %}Hello");
  }
  catch (const ParserError &)
  {
    thrown = true;
  }

  assert(thrown);
}

int main()
{
  test_parse_text();
  test_parse_variable();
  test_parse_variable_with_one_filter();
  test_parse_variable_with_multiple_filters();
  test_parse_member_expression();
  test_parse_nested_member_expression();
  test_parse_number_literal();
  test_parse_string_literal();
  test_parse_bool_literal();
  test_parse_if();
  test_parse_binary_expression();
  test_parse_comparison_expression();
  test_parse_unary_expression();
  test_parse_parenthesized_expression();
  test_parse_operator_precedence();
  test_parse_if_with_expression();
  test_parse_for();
  test_parse_nested_if_in_for();
  test_parse_include();
  test_parse_include_with_surrounding_text();
  test_parse_extends();
  test_parse_block();
  test_parse_extends_with_block();
  test_parse_nested_block_nodes();
  test_parse_block_missing_endblock();

  std::cout << "[OK] template parser tests passed\n";
  return 0;
}
