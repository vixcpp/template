/**
 *
 *  @file Lexer.hpp
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
#ifndef VIX_TEMPLATE_LEXER_HPP
#define VIX_TEMPLATE_LEXER_HPP

#include <cstddef>
#include <string>
#include <vector>

#include <vix/template/Token.hpp>

namespace vix::template_
{
  /**
   * @brief Template lexer.
   *
   * Lexer transforms raw template source into a linear stream of tokens
   * that can later be consumed by the parser.
   *
   * The lexer is responsible for detecting transitions between:
   * - plain text
   * - variable expressions: {{ ... }}
   * - block expressions: {% ... %}
   *
   * V5 extends expression lexing support with:
   * - identifiers
   * - numbers
   * - strings
   * - operators
   * - punctuation
   *
   * Example supported expressions:
   * - {{ user.name }}
   * - {{ price * quantity }}
   * - {{ a == b }}
   * - {{ (price + tax) * quantity }}
   */
  class Lexer
  {
  public:
    /**
     * @brief Construct a lexer from raw template source.
     *
     * @param input Raw template source.
     */
    explicit Lexer(std::string input);

    /**
     * @brief Tokenize the whole input.
     *
     * @return Full token stream terminated by EndOfFile.
     */
    [[nodiscard]] std::vector<Token> tokenize();

  private:
    /**
     * @brief Check whether the lexer reached the end of input.
     *
     * @return True if no more characters are available.
     */
    [[nodiscard]] bool eof() const noexcept;

    /**
     * @brief Get the current character without consuming it.
     *
     * @return Current character, or '\0' at end of input.
     */
    [[nodiscard]] char peek() const noexcept;

    /**
     * @brief Look ahead by an offset without consuming.
     *
     * @param offset Character offset from current position.
     * @return Lookahead character, or '\0' if out of range.
     */
    [[nodiscard]] char peek(std::size_t offset) const noexcept;

    /**
     * @brief Consume and return the current character.
     *
     * @return Consumed character, or '\0' at end of input.
     */
    char advance() noexcept;

    /**
     * @brief Check whether the current position starts with a pattern.
     *
     * @param pattern Pattern to match.
     * @return True if the input starts with the pattern at current position.
     */
    [[nodiscard]] bool starts_with(const std::string &pattern) const noexcept;

    /**
     * @brief Consume a fixed pattern.
     *
     * @param pattern Pattern to consume.
     * @return True if consumed successfully.
     */
    bool consume(const std::string &pattern) noexcept;

    /**
     * @brief Read raw text until the next template delimiter.
     *
     * @return Text token.
     */
    [[nodiscard]] Token read_text();

    /**
     * @brief Read the inside of a variable expression.
     *
     * Example:
     * {{ user.name }}
     *
     * @return Tokens for the variable expression body.
     */
    [[nodiscard]] std::vector<Token> read_variable();

    /**
     * @brief Read the inside of a block expression.
     *
     * Example:
     * {% if price * quantity > 0 %}
     *
     * @return Tokens for the block expression body.
     */
    [[nodiscard]] std::vector<Token> read_block();

    /**
     * @brief Skip whitespace inside template expressions.
     */
    void skip_whitespace() noexcept;

    /**
     * @brief Read an identifier token.
     *
     * Identifiers are used for variable names, member names,
     * and template keywords such as if, for, include, extends, block.
     *
     * @return Identifier token.
     */
    [[nodiscard]] Token read_identifier();

    /**
     * @brief Read a numeric token.
     *
     * Supported examples:
     * - 123
     * - 3.14
     *
     * @return Number token.
     */
    [[nodiscard]] Token read_number();

    /**
     * @brief Read a quoted string token.
     *
     * Supported examples:
     * - "hello"
     * - 'hello'
     *
     * @return String token.
     */
    [[nodiscard]] Token read_string();

    /**
     * @brief Read an operator token.
     *
     * Supported operators include:
     * - +
     * - -
     * - *
     * - /
     * - %
     * - ==
     * - !=
     * - <
     * - <=
     * - >
     * - >=
     * - !
     * - &&
     * - ||
     * - |
     *
     * @return Operator token.
     */
    [[nodiscard]] Token read_operator();

    /**
     * @brief Read a punctuation token.
     *
     * Supported punctuation includes:
     * - (
     * - )
     * - .
     * - ,
     *
     * @return Punctuation token.
     */
    [[nodiscard]] Token read_punctuation();

    /**
     * @brief Check whether a character may start an identifier.
     *
     * @param ch Character to inspect.
     * @return True if the character can start an identifier.
     */
    [[nodiscard]] bool is_identifier_start(char ch) const noexcept;

    /**
     * @brief Check whether a character may continue an identifier.
     *
     * @param ch Character to inspect.
     * @return True if the character can continue an identifier.
     */
    [[nodiscard]] bool is_identifier_part(char ch) const noexcept;

    /**
     * @brief Check whether a character is recognized as punctuation.
     *
     * @param ch Character to inspect.
     * @return True if the character is punctuation.
     */
    [[nodiscard]] bool is_punctuation(char ch) const noexcept;

    /**
     * @brief Check whether the current position starts a known operator.
     *
     * @return True if the current character sequence is an operator.
     */
    [[nodiscard]] bool starts_operator() const noexcept;

    /**
     * @brief Append an EOF token to the token stream.
     *
     * @param tokens Token output vector.
     */
    void append_eof(std::vector<Token> &tokens) const;

  private:
    /**
     * @brief Raw template source.
     */
    std::string input_;

    /**
     * @brief Current byte position in the input.
     */
    std::size_t pos_{0};

    /**
     * @brief Current line number, 1-based.
     */
    std::size_t line_{1};

    /**
     * @brief Current column number, 1-based.
     */
    std::size_t column_{1};
  };

} // namespace vix::template_

#endif // VIX_TEMPLATE_LEXER_HPP
