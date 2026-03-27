/**
 *
 *  @file Parser.cpp
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
#include <vix/template/Parser.hpp>
#include <vix/template/Error.hpp>

#include <memory>
#include <utility>
#include <vector>

namespace vix::template_
{
  Parser::Parser(std::vector<Token> tokens)
      : tokens_(std::move(tokens))
  {
  }

  RootNode Parser::parse()
  {
    return RootNode(parse_nodes());
  }

  bool Parser::eof() const noexcept
  {
    return check(TokenType::EndOfFile);
  }

  const Token &Parser::current() const
  {
    return peek(0);
  }

  const Token &Parser::peek(std::size_t offset) const
  {
    const std::size_t index = pos_ + offset;
    if (index >= tokens_.size())
    {
      return tokens_.back();
    }

    return tokens_[index];
  }

  const Token &Parser::advance()
  {
    const Token &token = current();
    if (!eof())
    {
      ++pos_;
    }

    return token;
  }

  bool Parser::check(TokenType type) const noexcept
  {
    return current().type == type;
  }

  bool Parser::check(TokenType type, const char *value) const noexcept
  {
    return current().type == type && current().value == value;
  }

  const Token &Parser::expect(TokenType type, const char *message)
  {
    if (!check(type))
    {
      throw ParserError(message);
    }

    return advance();
  }

  const Token &Parser::expect(
      TokenType type,
      const char *value,
      const char *message)
  {
    if (!check(type, value))
    {
      throw ParserError(message);
    }

    return advance();
  }

  NodeList Parser::parse_nodes()
  {
    NodeList nodes;

    while (!eof() &&
           !is_endif_block() &&
           !is_endfor_block() &&
           !is_endblock_block())
    {
      nodes.push_back(parse_node());
    }

    return nodes;
  }

  NodePtr Parser::parse_node()
  {
    if (check(TokenType::Text))
    {
      return parse_text();
    }

    if (check(TokenType::VariableOpen))
    {
      return parse_variable();
    }

    if (check(TokenType::BlockOpen))
    {
      return parse_block();
    }

    throw ParserError("unexpected token while parsing node");
  }

  NodePtr Parser::parse_text()
  {
    const Token &token = expect(TokenType::Text, "expected text token");
    return std::make_unique<TextNode>(token.value);
  }

  NodePtr Parser::parse_variable()
  {
    expect(TokenType::VariableOpen, "expected '{{'");

    ExprPtr expression = parse_expression();
    std::vector<FilterNode> filters = parse_filters();

    expect(TokenType::VariableClose, "expected '}}'");

    return std::make_unique<VariableNode>(
        std::move(expression),
        std::move(filters));
  }

  std::vector<FilterNode> Parser::parse_filters()
  {
    std::vector<FilterNode> filters;

    while (check(TokenType::Operator, "|"))
    {
      advance();

      const Token &filter_name =
          expect(TokenType::Identifier, "expected filter name after '|'");
      filters.emplace_back(filter_name.value);
    }

    return filters;
  }

  NodePtr Parser::parse_block()
  {
    expect(TokenType::BlockOpen, "expected '{%'");

    if (check(TokenType::Identifier, "extends"))
    {
      return parse_extends();
    }

    if (check(TokenType::Identifier, "block"))
    {
      return parse_block_node();
    }

    if (check(TokenType::Identifier, "include"))
    {
      return parse_include();
    }

    if (check(TokenType::Identifier, "if"))
    {
      return parse_if();
    }

    if (check(TokenType::Identifier, "for"))
    {
      return parse_for();
    }

    throw ParserError("unsupported block type");
  }

  NodePtr Parser::parse_include()
  {
    expect(TokenType::Identifier, "include", "expected 'include'");

    const Token &template_name =
        expect(TokenType::String, "expected string literal after 'include'");

    expect(TokenType::BlockClose, "expected '%}' after include target");

    return std::make_unique<IncludeNode>(template_name.value);
  }

  NodePtr Parser::parse_extends()
  {
    expect(TokenType::Identifier, "extends", "expected 'extends'");

    const Token &template_name =
        expect(TokenType::String, "expected string literal after 'extends'");

    expect(TokenType::BlockClose, "expected '%}' after extends target");

    return std::make_unique<ExtendsNode>(template_name.value);
  }

  NodePtr Parser::parse_block_node()
  {
    expect(TokenType::Identifier, "block", "expected 'block'");

    const Token &name =
        expect(TokenType::Identifier, "expected block name after 'block'");

    expect(TokenType::BlockClose, "expected '%}' after block name");

    NodeList body = parse_nodes();

    if (!is_endblock_block())
    {
      throw ParserError("expected '{% endblock %}'");
    }

    consume_endblock();

    return std::make_unique<BlockNode>(name.value, std::move(body));
  }

  NodePtr Parser::parse_if()
  {
    expect(TokenType::Identifier, "if", "expected 'if'");

    ExprPtr condition = parse_expression();

    expect(TokenType::BlockClose, "expected '%}' after if condition");

    NodeList body = parse_nodes();

    if (!is_endif_block())
    {
      throw ParserError("expected '{% endif %}'");
    }

    consume_endif();

    return std::make_unique<IfNode>(
        std::move(condition),
        std::move(body));
  }

  NodePtr Parser::parse_for()
  {
    expect(TokenType::Identifier, "for", "expected 'for'");

    const Token &item =
        expect(TokenType::Identifier, "expected loop variable after 'for'");

    expect(TokenType::Identifier, "in", "expected 'in' in for block");

    const Token &iterable =
        expect(TokenType::Identifier, "expected iterable identifier after 'in'");

    expect(TokenType::BlockClose, "expected '%}' after for header");

    NodeList body = parse_nodes();

    if (!is_endfor_block())
    {
      throw ParserError("expected '{% endfor %}'");
    }

    consume_endfor();

    return std::make_unique<ForNode>(
        item.value,
        iterable.value,
        std::move(body));
  }

  ExprPtr Parser::parse_expression()
  {
    return parse_binary_expression(1);
  }

  ExprPtr Parser::parse_binary_expression(int min_precedence)
  {
    ExprPtr left = parse_unary_expression();

    while (is_binary_operator())
    {
      BinaryOperator op = parse_binary_operator();
      const int op_precedence = precedence(op);

      if (op_precedence < min_precedence)
      {
        break;
      }

      advance();

      ExprPtr right = parse_binary_expression(op_precedence + 1);

      left = std::make_unique<BinaryExpression>(
          std::move(left),
          op,
          std::move(right));
    }

    return left;
  }

  ExprPtr Parser::parse_unary_expression()
  {
    if (is_unary_operator())
    {
      UnaryOperator op = parse_unary_operator();
      advance();

      ExprPtr operand = parse_unary_expression();

      return std::make_unique<UnaryExpression>(
          op,
          std::move(operand));
    }

    return parse_primary_expression();
  }

  ExprPtr Parser::parse_primary_expression()
  {
    if (check(TokenType::Identifier, "true"))
    {
      const Token &token = advance();
      return std::make_unique<LiteralExpression>(token.value);
    }

    if (check(TokenType::Identifier, "false"))
    {
      const Token &token = advance();
      return std::make_unique<LiteralExpression>(token.value);
    }

    if (check(TokenType::Number))
    {
      const Token &token = advance();
      return std::make_unique<LiteralExpression>(token.value);
    }

    if (check(TokenType::String))
    {
      const Token &token = advance();
      return std::make_unique<LiteralExpression>(token.value);
    }

    if (check(TokenType::Punctuation, "("))
    {
      advance();

      ExprPtr expr = parse_expression();

      expect(TokenType::Punctuation, ")", "expected ')' after expression");

      return parse_postfix_expression(std::move(expr));
    }

    if (check(TokenType::Identifier))
    {
      return parse_identifier_expression();
    }

    throw ParserError("expected primary expression");
  }

  ExprPtr Parser::parse_postfix_expression(ExprPtr expr)
  {
    while (check(TokenType::Punctuation, "."))
    {
      advance();

      const Token &member =
          expect(TokenType::Identifier, "expected member name after '.'");

      expr = std::make_unique<MemberExpression>(
          std::move(expr),
          member.value);
    }

    return expr;
  }

  ExprPtr Parser::parse_identifier_expression()
  {
    const Token &name =
        expect(TokenType::Identifier, "expected identifier");

    ExprPtr expr = std::make_unique<NameExpression>(name.value);
    return parse_postfix_expression(std::move(expr));
  }

  bool Parser::is_unary_operator() const noexcept
  {
    return check(TokenType::Operator, "!") ||
           check(TokenType::Operator, "-") ||
           check(TokenType::Operator, "+");
  }

  bool Parser::is_binary_operator() const noexcept
  {
    return check(TokenType::Operator, "+") ||
           check(TokenType::Operator, "-") ||
           check(TokenType::Operator, "*") ||
           check(TokenType::Operator, "/") ||
           check(TokenType::Operator, "%") ||
           check(TokenType::Operator, "==") ||
           check(TokenType::Operator, "!=") ||
           check(TokenType::Operator, "<") ||
           check(TokenType::Operator, "<=") ||
           check(TokenType::Operator, ">") ||
           check(TokenType::Operator, ">=") ||
           check(TokenType::Operator, "&&") ||
           check(TokenType::Operator, "||");
  }

  UnaryOperator Parser::parse_unary_operator()
  {
    if (check(TokenType::Operator, "+"))
    {
      return UnaryOperator::Plus;
    }

    if (check(TokenType::Operator, "-"))
    {
      return UnaryOperator::Minus;
    }

    if (check(TokenType::Operator, "!"))
    {
      return UnaryOperator::Not;
    }

    throw ParserError("unsupported unary operator");
  }

  BinaryOperator Parser::parse_binary_operator()
  {
    if (check(TokenType::Operator, "+"))
    {
      return BinaryOperator::Add;
    }

    if (check(TokenType::Operator, "-"))
    {
      return BinaryOperator::Subtract;
    }

    if (check(TokenType::Operator, "*"))
    {
      return BinaryOperator::Multiply;
    }

    if (check(TokenType::Operator, "/"))
    {
      return BinaryOperator::Divide;
    }

    if (check(TokenType::Operator, "%"))
    {
      return BinaryOperator::Modulo;
    }

    if (check(TokenType::Operator, "=="))
    {
      return BinaryOperator::Equal;
    }

    if (check(TokenType::Operator, "!="))
    {
      return BinaryOperator::NotEqual;
    }

    if (check(TokenType::Operator, "<"))
    {
      return BinaryOperator::Less;
    }

    if (check(TokenType::Operator, "<="))
    {
      return BinaryOperator::LessEqual;
    }

    if (check(TokenType::Operator, ">"))
    {
      return BinaryOperator::Greater;
    }

    if (check(TokenType::Operator, ">="))
    {
      return BinaryOperator::GreaterEqual;
    }

    if (check(TokenType::Operator, "&&"))
    {
      return BinaryOperator::And;
    }

    if (check(TokenType::Operator, "||"))
    {
      return BinaryOperator::Or;
    }

    throw ParserError("unsupported binary operator");
  }

  int Parser::precedence(BinaryOperator op) const noexcept
  {
    switch (op)
    {
    case BinaryOperator::Or:
      return 1;

    case BinaryOperator::And:
      return 2;

    case BinaryOperator::Equal:
    case BinaryOperator::NotEqual:
      return 3;

    case BinaryOperator::Less:
    case BinaryOperator::LessEqual:
    case BinaryOperator::Greater:
    case BinaryOperator::GreaterEqual:
      return 4;

    case BinaryOperator::Add:
    case BinaryOperator::Subtract:
      return 5;

    case BinaryOperator::Multiply:
    case BinaryOperator::Divide:
    case BinaryOperator::Modulo:
      return 6;
    }

    return 0;
  }

  bool Parser::is_endif_block() const noexcept
  {
    return check(TokenType::BlockOpen) &&
           peek(1).type == TokenType::Identifier &&
           peek(1).value == "endif" &&
           peek(2).type == TokenType::BlockClose;
  }

  bool Parser::is_endfor_block() const noexcept
  {
    return check(TokenType::BlockOpen) &&
           peek(1).type == TokenType::Identifier &&
           peek(1).value == "endfor" &&
           peek(2).type == TokenType::BlockClose;
  }

  bool Parser::is_endblock_block() const noexcept
  {
    return check(TokenType::BlockOpen) &&
           peek(1).type == TokenType::Identifier &&
           peek(1).value == "endblock" &&
           peek(2).type == TokenType::BlockClose;
  }

  void Parser::consume_endif()
  {
    expect(TokenType::BlockOpen, "expected '{%' before endif");
    expect(TokenType::Identifier, "endif", "expected 'endif'");
    expect(TokenType::BlockClose, "expected '%}' after endif");
  }

  void Parser::consume_endfor()
  {
    expect(TokenType::BlockOpen, "expected '{%' before endfor");
    expect(TokenType::Identifier, "endfor", "expected 'endfor'");
    expect(TokenType::BlockClose, "expected '%}' after endfor");
  }

  void Parser::consume_endblock()
  {
    expect(TokenType::BlockOpen, "expected '{%' before endblock");
    expect(TokenType::Identifier, "endblock", "expected 'endblock'");
    expect(TokenType::BlockClose, "expected '%}' after endblock");
  }

} // namespace vix::template_
