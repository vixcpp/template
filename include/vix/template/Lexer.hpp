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
     * {{ user }}
     *
     * @return Tokens for the variable expression body.
     */
    [[nodiscard]] std::vector<Token> read_variable();

    /**
     * @brief Read the inside of a block expression.
     *
     * Example:
     * {% if user %}
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
     * @return Identifier token.
     */
    [[nodiscard]] Token read_identifier();

    /**
     * @brief Read a numeric token.
     *
     * @return Number token.
     */
    [[nodiscard]] Token read_number();

    /**
     * @brief Read a quoted string token.
     *
     * @return String token.
     */
    [[nodiscard]] Token read_string();

    /**
     * @brief Read an operator token.
     *
     * @return Operator token.
     */
    [[nodiscard]] Token read_operator();

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
