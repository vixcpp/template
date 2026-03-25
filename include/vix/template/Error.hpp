/**
 *
 *  @file Error.hpp
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
#ifndef VIX_TEMPLATE_ERROR_HPP
#define VIX_TEMPLATE_ERROR_HPP

#include <stdexcept>
#include <string>
#include <utility>

namespace vix::template_
{
  /**
   * @brief Error categories for the template module.
   */
  enum class ErrorCode
  {
    Unknown = 0,
    LoaderError,
    LexerError,
    ParserError,
    CompilerError,
    RendererError,
    CacheError,
    ContextError,
    ValueError,
    SyntaxError,
    RuntimeError
  };

  /**
   * @brief Base exception type for the template module.
   *
   * This error carries:
   * - a stable error code
   * - a human-readable message
   *
   * It is used as the common parent for all template-related failures.
   */
  class Error : public std::runtime_error
  {
  public:
    /**
     * @brief Construct a template error.
     *
     * @param code Stable error code.
     * @param message Human-readable error message.
     */
    Error(ErrorCode code, std::string message)
        : std::runtime_error(std::move(message)),
          code_(code)
    {
    }

    /**
     * @brief Get the error code.
     *
     * @return Stable error code.
     */
    [[nodiscard]] ErrorCode code() const noexcept
    {
      return code_;
    }

  private:
    /**
     * @brief Stable error code associated with this error.
     */
    ErrorCode code_{ErrorCode::Unknown};
  };

  /**
   * @brief Loader-specific template error.
   */
  class LoaderError : public Error
  {
  public:
    /**
     * @brief Construct a loader error.
     *
     * @param message Human-readable error message.
     */
    explicit LoaderError(std::string message)
        : Error(ErrorCode::LoaderError, std::move(message))
    {
    }
  };

  /**
   * @brief Lexer-specific template error.
   */
  class LexerError : public Error
  {
  public:
    /**
     * @brief Construct a lexer error.
     *
     * @param message Human-readable error message.
     */
    explicit LexerError(std::string message)
        : Error(ErrorCode::LexerError, std::move(message))
    {
    }
  };

  /**
   * @brief Parser-specific template error.
   */
  class ParserError : public Error
  {
  public:
    /**
     * @brief Construct a parser error.
     *
     * @param message Human-readable error message.
     */
    explicit ParserError(std::string message)
        : Error(ErrorCode::ParserError, std::move(message))
    {
    }
  };

  /**
   * @brief Compiler-specific template error.
   */
  class CompilerError : public Error
  {
  public:
    /**
     * @brief Construct a compiler error.
     *
     * @param message Human-readable error message.
     */
    explicit CompilerError(std::string message)
        : Error(ErrorCode::CompilerError, std::move(message))
    {
    }
  };

  /**
   * @brief Renderer-specific template error.
   */
  class RendererError : public Error
  {
  public:
    /**
     * @brief Construct a renderer error.
     *
     * @param message Human-readable error message.
     */
    explicit RendererError(std::string message)
        : Error(ErrorCode::RendererError, std::move(message))
    {
    }
  };

  /**
   * @brief Cache-specific template error.
   */
  class CacheError : public Error
  {
  public:
    /**
     * @brief Construct a cache error.
     *
     * @param message Human-readable error message.
     */
    explicit CacheError(std::string message)
        : Error(ErrorCode::CacheError, std::move(message))
    {
    }
  };

  /**
   * @brief Context-specific template error.
   */
  class ContextError : public Error
  {
  public:
    /**
     * @brief Construct a context error.
     *
     * @param message Human-readable error message.
     */
    explicit ContextError(std::string message)
        : Error(ErrorCode::ContextError, std::move(message))
    {
    }
  };

  /**
   * @brief Value-specific template error.
   */
  class ValueError : public Error
  {
  public:
    /**
     * @brief Construct a value error.
     *
     * @param message Human-readable error message.
     */
    explicit ValueError(std::string message)
        : Error(ErrorCode::ValueError, std::move(message))
    {
    }
  };

  /**
   * @brief Syntax-specific template error.
   */
  class SyntaxError : public Error
  {
  public:
    /**
     * @brief Construct a syntax error.
     *
     * @param message Human-readable error message.
     */
    explicit SyntaxError(std::string message)
        : Error(ErrorCode::SyntaxError, std::move(message))
    {
    }
  };

  /**
   * @brief Runtime-specific template error.
   */
  class RuntimeError : public Error
  {
  public:
    /**
     * @brief Construct a runtime error.
     *
     * @param message Human-readable error message.
     */
    explicit RuntimeError(std::string message)
        : Error(ErrorCode::RuntimeError, std::move(message))
    {
    }
  };

} // namespace vix::template_

#endif // VIX_TEMPLATE_ERROR_HPP
