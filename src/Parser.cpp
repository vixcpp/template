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

    while (!eof() && !is_endif_block() && !is_endfor_block())
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

    const Token &name =
        expect(TokenType::Identifier, "expected identifier inside variable expression");

    std::vector<FilterNode> filters = parse_filters();

    expect(TokenType::VariableClose, "expected '}}'");

    return std::make_unique<VariableNode>(name.value, std::move(filters));
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

  NodePtr Parser::parse_if()
  {
    expect(TokenType::Identifier, "if", "expected 'if'");

    const Token &condition =
        expect(TokenType::Identifier, "expected condition identifier after 'if'");

    expect(TokenType::BlockClose, "expected '%}' after if condition");

    NodeList body = parse_nodes();

    if (!is_endif_block())
    {
      throw ParserError("expected '{% endif %}'");
    }

    consume_endif();

    return std::make_unique<IfNode>(condition.value, std::move(body));
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

} // namespace vix::template_
