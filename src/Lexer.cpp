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
    {
      return '\0';
    }

    return input_[pos_];
  }

  char Lexer::peek(std::size_t offset) const noexcept
  {
    const std::size_t p = pos_ + offset;
    if (p >= input_.size())
    {
      return '\0';
    }

    return input_[p];
  }

  char Lexer::advance() noexcept
  {
    if (eof())
    {
      return '\0';
    }

    const char c = input_[pos_++];

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
    {
      return false;
    }

    return input_.compare(pos_, pattern.size(), pattern) == 0;
  }

  bool Lexer::consume(const std::string &pattern) noexcept
  {
    if (!starts_with(pattern))
    {
      return false;
    }

    for (std::size_t i = 0; i < pattern.size(); ++i)
    {
      advance();
    }

    return true;
  }

  Token Lexer::read_text()
  {
    std::string value;
    const std::size_t start_line = line_;
    const std::size_t start_col = column_;

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

      if (starts_with("}}"))
      {
        break;
      }

      const char ch = peek();

      if (is_identifier_start(ch))
      {
        tokens.push_back(read_identifier());
      }
      else if (std::isdigit(static_cast<unsigned char>(ch)))
      {
        tokens.push_back(read_number());
      }
      else if (ch == '"' || ch == '\'')
      {
        tokens.push_back(read_string());
      }
      else if (is_punctuation(ch))
      {
        tokens.push_back(read_punctuation());
      }
      else if (starts_operator())
      {
        tokens.push_back(read_operator());
      }
      else
      {
        throw LexerError("unexpected character inside variable expression");
      }

      skip_whitespace();
    }

    if (!starts_with("}}"))
    {
      throw LexerError("unterminated variable expression");
    }

    const std::size_t close_line = line_;
    const std::size_t close_col = column_;
    consume("}}");
    tokens.emplace_back(TokenType::VariableClose, "}}", close_line, close_col);

    return tokens;
  }

  std::vector<Token> Lexer::read_block()
  {
    std::vector<Token> tokens;

    skip_whitespace();

    while (!eof() && !starts_with("%}"))
    {
      skip_whitespace();

      if (starts_with("%}"))
      {
        break;
      }

      const char ch = peek();

      if (is_identifier_start(ch))
      {
        tokens.push_back(read_identifier());
      }
      else if (std::isdigit(static_cast<unsigned char>(ch)))
      {
        tokens.push_back(read_number());
      }
      else if (ch == '"' || ch == '\'')
      {
        tokens.push_back(read_string());
      }
      else if (is_punctuation(ch))
      {
        tokens.push_back(read_punctuation());
      }
      else if (starts_operator())
      {
        tokens.push_back(read_operator());
      }
      else
      {
        throw LexerError("unexpected character inside block expression");
      }

      skip_whitespace();
    }

    if (!starts_with("%}"))
    {
      throw LexerError("unterminated block expression");
    }

    const std::size_t close_line = line_;
    const std::size_t close_col = column_;
    consume("%}");
    tokens.emplace_back(TokenType::BlockClose, "%}", close_line, close_col);

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
    const std::size_t start_line = line_;
    const std::size_t start_col = column_;

    while (!eof() && is_identifier_part(peek()))
    {
      value.push_back(advance());
    }

    return Token(TokenType::Identifier, std::move(value), start_line, start_col);
  }

  Token Lexer::read_number()
  {
    std::string value;
    const std::size_t start_line = line_;
    const std::size_t start_col = column_;
    bool seen_dot = false;

    while (!eof())
    {
      const char ch = peek();

      if (std::isdigit(static_cast<unsigned char>(ch)))
      {
        value.push_back(advance());
        continue;
      }

      if (ch == '.' && !seen_dot)
      {
        const char next = peek(1);
        if (!std::isdigit(static_cast<unsigned char>(next)))
        {
          break;
        }

        seen_dot = true;
        value.push_back(advance());
        continue;
      }

      break;
    }

    return Token(TokenType::Number, std::move(value), start_line, start_col);
  }

  Token Lexer::read_string()
  {
    const std::size_t start_line = line_;
    const std::size_t start_col = column_;
    const char quote = advance();

    std::string value;

    while (!eof() && peek() != quote)
    {
      if (peek() == '\\')
      {
        advance();

        if (eof())
        {
          throw LexerError("unterminated string literal");
        }

        const char escaped = advance();
        switch (escaped)
        {
        case 'n':
          value.push_back('\n');
          break;

        case 't':
          value.push_back('\t');
          break;

        case '\\':
          value.push_back('\\');
          break;

        case '\'':
          value.push_back('\'');
          break;

        case '"':
          value.push_back('"');
          break;

        default:
          value.push_back(escaped);
          break;
        }

        continue;
      }

      value.push_back(advance());
    }

    if (peek() != quote)
    {
      throw LexerError("unterminated string literal");
    }

    advance();

    return Token(TokenType::String, std::move(value), start_line, start_col);
  }

  Token Lexer::read_operator()
  {
    const std::size_t start_line = line_;
    const std::size_t start_col = column_;

    if (starts_with("=="))
    {
      consume("==");
      return Token(TokenType::Operator, "==", start_line, start_col);
    }

    if (starts_with("!="))
    {
      consume("!=");
      return Token(TokenType::Operator, "!=", start_line, start_col);
    }

    if (starts_with("<="))
    {
      consume("<=");
      return Token(TokenType::Operator, "<=", start_line, start_col);
    }

    if (starts_with(">="))
    {
      consume(">=");
      return Token(TokenType::Operator, ">=", start_line, start_col);
    }

    if (starts_with("&&"))
    {
      consume("&&");
      return Token(TokenType::Operator, "&&", start_line, start_col);
    }

    if (starts_with("||"))
    {
      consume("||");
      return Token(TokenType::Operator, "||", start_line, start_col);
    }

    const char ch = peek();
    switch (ch)
    {
    case '+':
    case '-':
    case '*':
    case '/':
    case '%':
    case '<':
    case '>':
    case '!':
    case '|':
      advance();
      return Token(TokenType::Operator, std::string(1, ch), start_line, start_col);

    default:
      throw LexerError("unknown operator");
    }
  }

  Token Lexer::read_punctuation()
  {
    const std::size_t start_line = line_;
    const std::size_t start_col = column_;
    const char ch = advance();

    switch (ch)
    {
    case '(':
    case ')':
    case '.':
    case ',':
      return Token(TokenType::Punctuation, std::string(1, ch), start_line, start_col);

    default:
      throw LexerError("unknown punctuation");
    }
  }

  bool Lexer::is_identifier_start(char ch) const noexcept
  {
    const unsigned char uch = static_cast<unsigned char>(ch);
    return std::isalpha(uch) != 0 || ch == '_';
  }

  bool Lexer::is_identifier_part(char ch) const noexcept
  {
    const unsigned char uch = static_cast<unsigned char>(ch);
    return std::isalnum(uch) != 0 || ch == '_';
  }

  bool Lexer::is_punctuation(char ch) const noexcept
  {
    switch (ch)
    {
    case '(':
    case ')':
    case '.':
    case ',':
      return true;

    default:
      return false;
    }
  }

  bool Lexer::starts_operator() const noexcept
  {
    if (eof())
    {
      return false;
    }

    if (starts_with("==") || starts_with("!=") || starts_with("<=") ||
        starts_with(">=") || starts_with("&&") || starts_with("||"))
    {
      return true;
    }

    switch (peek())
    {
    case '+':
    case '-':
    case '*':
    case '/':
    case '%':
    case '<':
    case '>':
    case '!':
    case '|':
      return true;

    default:
      return false;
    }
  }

  void Lexer::append_eof(std::vector<Token> &tokens) const
  {
    tokens.emplace_back(TokenType::EndOfFile, "", line_, column_);
  }

} // namespace vix::template_
