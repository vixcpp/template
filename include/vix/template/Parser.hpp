/**
 *
 *  @file Parser.hpp
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
#ifndef VIX_TEMPLATE_PARSER_HPP
#define VIX_TEMPLATE_PARSER_HPP

#include <cstddef>
#include <vector>

#include <vix/template/AST.hpp>
#include <vix/template/Token.hpp>

namespace vix::template_
{
  /**
   * @brief Template parser.
   *
   * Parser consumes the token stream produced by Lexer and builds
   * a structured AST representation of the template.
   *
   * Supported constructs:
   * - plain text
   * - variable interpolation
   * - filtered interpolation
   * - if blocks
   * - for blocks
   * - include blocks
   * - extends blocks
   * - named blocks
   * - expression parsing for variables and conditions
   */
  class Parser
  {
  public:
    /**
     * @brief Construct a parser from a token stream.
     *
     * @param tokens Token stream to parse.
     */
    explicit Parser(std::vector<Token> tokens);

    /**
     * @brief Parse the full token stream into a root AST node.
     *
     * @return Parsed root node.
     */
    [[nodiscard]] RootNode parse();

  private:
    /**
     * @brief Check whether the parser reached EOF.
     *
     * @return True if current token is EndOfFile.
     */
    [[nodiscard]] bool eof() const noexcept;

    /**
     * @brief Get the current token.
     *
     * @return Current token.
     */
    [[nodiscard]] const Token &current() const;

    /**
     * @brief Get a token at an offset from the current position.
     *
     * @param offset Offset from the current position.
     * @return Token at offset, or the last token if out of range.
     */
    [[nodiscard]] const Token &peek(std::size_t offset = 0) const;

    /**
     * @brief Advance to the next token.
     *
     * @return Previously current token.
     */
    const Token &advance();

    /**
     * @brief Check whether the current token has the given type.
     *
     * @param type Token type to compare.
     * @return True if the current token matches.
     */
    [[nodiscard]] bool check(TokenType type) const noexcept;

    /**
     * @brief Check whether the current token matches a type and value.
     *
     * @param type Token type.
     * @param value Token value.
     * @return True if both type and value match.
     */
    [[nodiscard]] bool check(TokenType type, const char *value) const noexcept;

    /**
     * @brief Consume a token of a given type.
     *
     * @param type Expected token type.
     * @param message Error message used if the token does not match.
     * @return Consumed token.
     */
    const Token &expect(TokenType type, const char *message);

    /**
     * @brief Consume a token of a given type and value.
     *
     * @param type Expected token type.
     * @param value Expected token value.
     * @param message Error message used if the token does not match.
     * @return Consumed token.
     */
    const Token &expect(
        TokenType type,
        const char *value,
        const char *message);

    /**
     * @brief Parse a sequence of nodes until EOF or a closing block.
     *
     * @return Parsed node list.
     */
    [[nodiscard]] NodeList parse_nodes();

    /**
     * @brief Parse a single node from the current token.
     *
     * @return Parsed node.
     */
    [[nodiscard]] NodePtr parse_node();

    /**
     * @brief Parse a text node.
     *
     * @return Parsed text node.
     */
    [[nodiscard]] NodePtr parse_text();

    /**
     * @brief Parse a variable interpolation node.
     *
     * Expected forms:
     * {{ user.name }}
     * {{ price * quantity }}
     * {{ a == b }}
     * {{ user.name | upper }}
     *
     * @return Parsed variable node.
     */
    [[nodiscard]] NodePtr parse_variable();

    /**
     * @brief Parse a filter pipeline attached to a variable expression.
     *
     * Expected form:
     * | filter_name | other_filter
     *
     * @return Ordered filter list.
     */
    [[nodiscard]] std::vector<FilterNode> parse_filters();

    /**
     * @brief Parse a block expression.
     *
     * This dispatches to block-specific parsers such as if/for/include.
     *
     * @return Parsed block node.
     */
    [[nodiscard]] NodePtr parse_block();

    /**
     * @brief Parse an if block.
     *
     * Expected form:
     * {% if expression %} ... {% endif %}
     *
     * @return Parsed if node.
     */
    [[nodiscard]] NodePtr parse_if();

    /**
     * @brief Parse a for block.
     *
     * Expected form:
     * {% for item in items %} ... {% endfor %}
     *
     * @return Parsed for node.
     */
    [[nodiscard]] NodePtr parse_for();

    /**
     * @brief Parse an include block.
     *
     * Expected form:
     * {% include "header.html" %}
     *
     * @return Parsed include node.
     */
    [[nodiscard]] NodePtr parse_include();

    /**
     * @brief Parse an extends block.
     *
     * Expected form:
     * {% extends "base.html" %}
     *
     * @return Parsed extends node.
     */
    [[nodiscard]] NodePtr parse_extends();

    /**
     * @brief Parse a named block.
     *
     * Expected form:
     * {% block content %} ... {% endblock %}
     *
     * @return Parsed block node.
     */
    [[nodiscard]] NodePtr parse_block_node();

    /**
     * @brief Parse a full expression.
     *
     * This is the entry point for expression parsing.
     *
     * @return Parsed expression tree.
     */
    [[nodiscard]] ExprPtr parse_expression();

    /**
     * @brief Parse an expression using precedence climbing.
     *
     * @param min_precedence Minimum precedence accepted at this level.
     * @return Parsed expression tree.
     */
    [[nodiscard]] ExprPtr parse_binary_expression(int min_precedence);

    /**
     * @brief Parse a unary expression.
     *
     * Examples:
     * - !enabled
     * - -price
     * - +value
     *
     * @return Parsed unary or primary expression.
     */
    [[nodiscard]] ExprPtr parse_unary_expression();

    /**
     * @brief Parse a primary expression.
     *
     * Examples:
     * - user
     * - 42
     * - "hello"
     * - true
     * - false
     * - (a + b)
     *
     * @return Parsed primary expression.
     */
    [[nodiscard]] ExprPtr parse_primary_expression();

    /**
     * @brief Parse postfix expression parts.
     *
     * This currently handles member access such as:
     * - user.name
     * - order.total.amount
     *
     * @param expr Base expression.
     * @return Completed postfix expression.
     */
    [[nodiscard]] ExprPtr parse_postfix_expression(ExprPtr expr);

    /**
     * @brief Parse an identifier-based expression.
     *
     * This starts with a bare identifier and may continue as a
     * member access chain.
     *
     * @return Parsed expression.
     */
    [[nodiscard]] ExprPtr parse_identifier_expression();

    /**
     * @brief Check whether the current token is a unary operator.
     *
     * @return True if current token starts a unary expression.
     */
    [[nodiscard]] bool is_unary_operator() const noexcept;

    /**
     * @brief Check whether the current token is a binary operator.
     *
     * @return True if current token starts a binary operator.
     */
    [[nodiscard]] bool is_binary_operator() const noexcept;

    /**
     * @brief Convert the current token into a unary operator.
     *
     * @return Unary operator.
     */
    [[nodiscard]] UnaryOperator parse_unary_operator();

    /**
     * @brief Convert the current token into a binary operator.
     *
     * @return Binary operator.
     */
    [[nodiscard]] BinaryOperator parse_binary_operator();

    /**
     * @brief Get precedence for a binary operator.
     *
     * Higher numbers bind more strongly.
     *
     * @param op Binary operator.
     * @return Operator precedence.
     */
    [[nodiscard]] int precedence(BinaryOperator op) const noexcept;

    /**
     * @brief Check whether the current position starts a closing endif block.
     *
     * @return True if the current tokens represent {% endif %}.
     */
    [[nodiscard]] bool is_endif_block() const noexcept;

    /**
     * @brief Check whether the current position starts a closing endfor block.
     *
     * @return True if the current tokens represent {% endfor %}.
     */
    [[nodiscard]] bool is_endfor_block() const noexcept;

    /**
     * @brief Check whether the current position starts a closing endblock block.
     *
     * @return True if the current tokens represent {% endblock %}.
     */
    [[nodiscard]] bool is_endblock_block() const noexcept;

    /**
     * @brief Consume the closing endif block.
     */
    void consume_endif();

    /**
     * @brief Consume the closing endfor block.
     */
    void consume_endfor();

    /**
     * @brief Consume the closing endblock block.
     */
    void consume_endblock();

  private:
    /**
     * @brief Token stream being parsed.
     */
    std::vector<Token> tokens_;

    /**
     * @brief Current token index.
     */
    std::size_t pos_{0};
  };

} // namespace vix::template_

#endif // VIX_TEMPLATE_PARSER_HPP
