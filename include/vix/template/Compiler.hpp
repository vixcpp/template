/**
 *
 *  @file Compiler.hpp
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
#ifndef VIX_TEMPLATE_COMPILER_HPP
#define VIX_TEMPLATE_COMPILER_HPP

#include <string>

#include <vix/template/AST.hpp>
#include <vix/template/Template.hpp>

namespace vix::template_
{
  /**
   * @brief Template compiler.
   *
   * In V1, Compiler is a lightweight component responsible for turning
   * a parsed AST into a Template object ready for rendering.
   *
   * This abstraction is intentionally kept even if the current
   * compilation step is simple, because it provides a stable place for
   * future optimizations such as:
   * - AST normalization
   * - constant folding
   * - precomputed render plans
   * - bytecode or instruction-based rendering
   */
  class Compiler
  {
  public:
    /**
     * @brief Construct a compiler.
     */
    Compiler() = default;

    /**
     * @brief Compile a parsed root AST into a template object.
     *
     * @param name Logical template name.
     * @param root Parsed AST root.
     * @return Compiled template.
     */
    [[nodiscard]] Template compile(
        std::string name,
        RootNode root) const;
  };

} // namespace vix::template_

#endif // VIX_TEMPLATE_COMPILER_HPP
