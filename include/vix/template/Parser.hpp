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
   * Supported V2 constructs:
   * - plain text
   * - variable interpolation: {{ name }}
   * - filtered variable interpolation: {{ name | upper }}
   * - chained filters: {{ name | upper | length }}
   * - if blocks: {% if cond %} ... {% endif %}
   * - for blocks: {% for item in items %} ... {% endfor %}
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
     * {{ identifier }}
     * {{ identifier | filter }}
     * {{ identifier | filter1 | filter2 }}
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
     * This dispatches to block-specific parsers such as if/for.
     *
     * @return Parsed block node.
     */
    [[nodiscard]] NodePtr parse_block();

    /**
     * @brief Parse an if block.
     *
     * Expected form:
     * {% if condition %} ... {% endif %}
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
     * @brief Consume the closing endif block.
     */
    void consume_endif();

    /**
     * @brief Consume the closing endfor block.
     */
    void consume_endfor();

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
