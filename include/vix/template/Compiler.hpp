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
#include <memory>

#include <vix/template/AST.hpp>
#include <vix/template/ExecutionPlan.hpp>
#include <vix/template/Optimizer.hpp>
#include <vix/template/Template.hpp>

namespace vix::template_
{
  /**
   * @brief Template compiler.
   *
   * In V6, Compiler is responsible for transforming a parsed AST into an
   * optimized and executable form.
   *
   * Compilation pipeline:
   *
   * 1. AST normalization (Optimizer)
   * 2. AST optimization (merge, cleanup, etc.)
   * 3. Execution plan generation (linear instruction stream)
   * 4. Template construction
   *
   * The resulting Template contains:
   * - the original (or optimized) AST
   * - a compiled ExecutionPlan for fast rendering
   *
   * This design enables:
   * - faster rendering (no recursive AST traversal)
   * - better CPU cache locality
   * - a foundation for future optimizations (V7+)
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
     * @param loader Optional loader used for include and extends resolution.
     * @return Compiled template.
     */
    [[nodiscard]] Template compile(
        std::string name,
        RootNode root,
        std::shared_ptr<Loader> loader = nullptr) const;

  private:
    /**
     * @brief Generate an execution plan from an optimized AST.
     *
     * @param root Optimized AST root.
     * @return Execution plan.
     */
    [[nodiscard]] ExecutionPlan build_plan(const RootNode &root) const;

    /**
     * @brief Compile a list of nodes into instructions.
     *
     * @param nodes AST node list.
     * @param plan Execution plan being built.
     */
    void compile_nodes(
        const NodeList &nodes,
        ExecutionPlan &plan) const;

    /**
     * @brief Compile a single AST node.
     *
     * @param node AST node.
     * @param plan Execution plan being built.
     */
    void compile_node(
        const Node &node,
        ExecutionPlan &plan) const;

    /**
     * @brief Compile a text node.
     *
     * @param node Text node.
     * @param plan Execution plan.
     */
    void compile_text(
        const TextNode &node,
        ExecutionPlan &plan) const;

    /**
     * @brief Compile a variable node.
     *
     * @param node Variable node.
     * @param plan Execution plan.
     */
    void compile_variable(
        const VariableNode &node,
        ExecutionPlan &plan) const;

    /**
     * @brief Compile an if node.
     *
     * @param node If node.
     * @param plan Execution plan.
     */
    void compile_if(
        const IfNode &node,
        ExecutionPlan &plan) const;

    /**
     * @brief Compile a for node.
     *
     * @param node For node.
     * @param plan Execution plan.
     */
    void compile_for(
        const ForNode &node,
        ExecutionPlan &plan) const;

    /**
     * @brief Compile an include node.
     *
     * @param node Include node.
     * @param plan Execution plan.
     */
    void compile_include(
        const IncludeNode &node,
        ExecutionPlan &plan) const;

    /**
     * @brief Compile a block node.
     *
     * @param node Block node.
     * @param plan Execution plan.
     */
    void compile_block(
        const BlockNode &node,
        ExecutionPlan &plan) const;
  };

} // namespace vix::template_

#endif // VIX_TEMPLATE_COMPILER_HPP
