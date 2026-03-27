/**
 *
 *  @file Token.hpp
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
#ifndef VIX_TEMPLATE_TOKEN_HPP
#define VIX_TEMPLATE_TOKEN_HPP

#include <cstddef>
#include <string>
#include <utility>

namespace vix::template_
{
  /**
   * @brief Token types produced by the lexer.
   */
  enum class TokenType
  {
    Text,          ///< Raw text outside template delimiters.
    VariableOpen,  ///< {{
    VariableClose, ///< }}
    BlockOpen,     ///< {%
    BlockClose,    ///< %}
    Identifier,    ///< Variable names and keywords.
    String,        ///< "text"
    Number,        ///< 123, 3.14
    Operator,      ///< ==, !=, +, -, *, /, &&, ||, |, etc.
    Punctuation,   ///< (, ), ., ,
    EndOfFile      ///< End of input.
  };

  /**
   * @brief Token representation.
   *
   * Tokens are produced by the lexer and consumed by the parser.
   */
  struct Token
  {
    /**
     * @brief Type of the token.
     */
    TokenType type{TokenType::EndOfFile};

    /**
     * @brief Raw token value.
     */
    std::string value;

    /**
     * @brief Line number (1-based).
     */
    std::size_t line{1};

    /**
     * @brief Column number (1-based).
     */
    std::size_t column{1};

    /**
     * @brief Construct a token.
     *
     * @param t Token type.
     * @param v Token value.
     * @param l Line number.
     * @param c Column number.
     */
    Token(TokenType t, std::string v, std::size_t l, std::size_t c)
        : type(t),
          value(std::move(v)),
          line(l),
          column(c)
    {
    }

    /**
     * @brief Default constructor.
     */
    Token() = default;
  };

} // namespace vix::template_

#endif // VIX_TEMPLATE_TOKEN_HPP
