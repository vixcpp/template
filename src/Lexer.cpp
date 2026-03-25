/**
 *
 *  @file Lexer.cpp
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
#include <vix/template/Lexer.hpp>
#include <vix/template/Error.hpp>

#include <cctype>
#include <utility>

namespace vix::template_
{
  Lexer::Lexer(std::string input)
      : input_(std::move(input))
  {
  }

  std::vector<Token> Lexer::tokenize()
  {
    std::vector<Token> tokens;

    while (!eof())
    {
      if (starts_with("{{"))
      {
        tokens.emplace_back(TokenType::VariableOpen, "{{", line_, column_);
        consume("{{");

        auto inner = read_variable();
        tokens.insert(tokens.end(), inner.begin(), inner.end());
      }
      else if (starts_with("{%"))
      {
        tokens.emplace_back(TokenType::BlockOpen, "{%", line_, column_);
        consume("{%");

        auto inner = read_block();
        tokens.insert(tokens.end(), inner.begin(), inner.end());
      }
      else
      {
        tokens.push_back(read_text());
      }
    }

    append_eof(tokens);
    return tokens;
  }

  bool Lexer::eof() const noexcept
  {
    return pos_ >= input_.size();
  }

  char Lexer::peek() const noexcept
  {
    if (eof())
      return '\0';
    return input_[pos_];
  }

  char Lexer::peek(std::size_t offset) const noexcept
  {
    const std::size_t p = pos_ + offset;
    if (p >= input_.size())
      return '\0';
    return input_[p];
  }

  char Lexer::advance() noexcept
  {
    if (eof())
      return '\0';

    char c = input_[pos_++];
    if (c == '\n')
    {
      ++line_;
      column_ = 1;
    }
    else
    {
      ++column_;
    }

    return c;
  }

  bool Lexer::starts_with(const std::string &pattern) const noexcept
  {
    if (pos_ + pattern.size() > input_.size())
      return false;

    return input_.compare(pos_, pattern.size(), pattern) == 0;
  }

  bool Lexer::consume(const std::string &pattern) noexcept
  {
    if (!starts_with(pattern))
      return false;

    for (std::size_t i = 0; i < pattern.size(); ++i)
    {
      advance();
    }

    return true;
  }

  Token Lexer::read_text()
  {
    std::string value;
    std::size_t start_line = line_;
    std::size_t start_col = column_;

    while (!eof() && !starts_with("{{") && !starts_with("{%"))
    {
      value.push_back(advance());
    }

    return Token(TokenType::Text, std::move(value), start_line, start_col);
  }

  std::vector<Token> Lexer::read_variable()
  {
    std::vector<Token> tokens;

    skip_whitespace();

    while (!eof() && !starts_with("}}"))
    {
      skip_whitespace();

      if (std::isalpha(peek()) || peek() == '_')
      {
        tokens.push_back(read_identifier());
      }
      else if (std::isdigit(peek()))
      {
        tokens.push_back(read_number());
      }
      else if (peek() == '"' || peek() == '\'')
      {
        tokens.push_back(read_string());
      }
      else
      {
        tokens.push_back(read_operator());
      }

      skip_whitespace();
    }

    if (!consume("}}"))
    {
      throw LexerError("unterminated variable expression");
    }

    tokens.emplace_back(TokenType::VariableClose, "}}", line_, column_);
    return tokens;
  }

  std::vector<Token> Lexer::read_block()
  {
    std::vector<Token> tokens;

    skip_whitespace();

    while (!eof() && !starts_with("%}"))
    {
      skip_whitespace();

      if (std::isalpha(peek()) || peek() == '_')
      {
        tokens.push_back(read_identifier());
      }
      else if (std::isdigit(peek()))
      {
        tokens.push_back(read_number());
      }
      else if (peek() == '"' || peek() == '\'')
      {
        tokens.push_back(read_string());
      }
      else
      {
        tokens.push_back(read_operator());
      }

      skip_whitespace();
    }

    if (!consume("%}"))
    {
      throw LexerError("unterminated block expression");
    }

    tokens.emplace_back(TokenType::BlockClose, "%}", line_, column_);
    return tokens;
  }

  void Lexer::skip_whitespace() noexcept
  {
    while (!eof() && std::isspace(static_cast<unsigned char>(peek())))
    {
      advance();
    }
  }

  Token Lexer::read_identifier()
  {
    std::string value;
    std::size_t start_line = line_;
    std::size_t start_col = column_;

    while (!eof() &&
           (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_'))
    {
      value.push_back(advance());
    }

    return Token(TokenType::Identifier, std::move(value), start_line, start_col);
  }

  Token Lexer::read_number()
  {
    std::string value;
    std::size_t start_line = line_;
    std::size_t start_col = column_;

    while (!eof() && (std::isdigit(peek()) || peek() == '.'))
    {
      value.push_back(advance());
    }

    return Token(TokenType::Number, std::move(value), start_line, start_col);
  }

  Token Lexer::read_string()
  {
    char quote = advance(); // ' or "
    std::string value;
    std::size_t start_line = line_;
    std::size_t start_col = column_;

    while (!eof() && peek() != quote)
    {
      value.push_back(advance());
    }

    if (peek() != quote)
    {
      throw LexerError("unterminated string literal");
    }

    advance(); // closing quote

    return Token(TokenType::String, std::move(value), start_line, start_col);
  }

  Token Lexer::read_operator()
  {
    std::size_t start_line = line_;
    std::size_t start_col = column_;

    char c = advance();
    return Token(TokenType::Operator, std::string(1, c), start_line, start_col);
  }

  void Lexer::append_eof(std::vector<Token> &tokens) const
  {
    tokens.emplace_back(TokenType::EndOfFile, "", line_, column_);
  }

} // namespace vix::template_
