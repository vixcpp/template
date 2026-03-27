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

#include <memory>
#include <string>

#include <vix/template/AST.hpp>
#include <vix/template/ExecutionPlan.hpp>
#include <vix/template/Instruction.hpp>
#include <vix/template/Loader.hpp>
#include <vix/template/Optimizer.hpp>
#include <vix/template/Template.hpp>

namespace vix::template_
{
  /**
   * @brief Template compiler.
   *
   * Compiler is responsible for transforming a parsed AST into an
   * optimized and executable form.
   *
   * Compilation pipeline:
   *
   * 1. AST normalization / optimization
   * 2. Execution plan generation
   * 3. Template construction
   *
   * The resulting Template contains:
   * - the optimized AST
   * - a compiled ExecutionPlan for fast rendering
   * - an optional source signature for cache validation
   *
   * This design enables:
   * - faster rendering
   * - better CPU cache locality
   * - compiled template caching in V7
   *
   * In V8, expressions stored in instructions are kept in compiled form
   * so the renderer can evaluate them directly without reparsing text.
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
     * @param source_signature Optional source freshness token.
     * @return Compiled template.
     */
    [[nodiscard]] Template compile(
        std::string name,
        RootNode root,
        std::shared_ptr<Loader> loader = nullptr,
        std::string source_signature = {}) const;

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

    /**
     * @brief Clone an expression tree into a shared compiled form.
     *
     * Instructions store shared compiled expressions so they remain
     * copyable and cheap to move while avoiding expression reparsing
     * during rendering.
     *
     * @param expression Expression AST to clone.
     * @return Shared pointer to an immutable compiled expression tree.
     */
    [[nodiscard]] CompiledExprPtr clone_expression(
        const Expression &expression) const;

    /**
     * @brief Clone an expression tree into a unique expression node.
     *
     * This helper performs the recursive deep copy of the expression AST.
     *
     * @param expression Expression AST to clone.
     * @return Deep-copied unique expression node.
     */
    [[nodiscard]] ExprPtr clone_expression_node(
        const Expression &expression) const;

  private:
    /**
     * @brief AST optimizer used before execution plan generation.
     */
    Optimizer optimizer_;
  };

} // namespace vix::template_

#endif // VIX_TEMPLATE_COMPILER_HPP
