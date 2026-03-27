/**
 *
 *  @file Renderer.hpp
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
#ifndef VIX_TEMPLATE_RENDERER_HPP
#define VIX_TEMPLATE_RENDERER_HPP

#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <vix/template/AST.hpp>
#include <vix/template/Builtins.hpp>
#include <vix/template/Context.hpp>
#include <vix/template/ExecutionPlan.hpp>
#include <vix/template/Loader.hpp>
#include <vix/template/RenderResult.hpp>
#include <vix/template/Value.hpp>

namespace vix::template_
{
  /**
   * @brief Execution-plan renderer for template output generation.
   *
   * In V6, Renderer executes a precomputed ExecutionPlan instead of
   * traversing the AST recursively as the primary rendering path.
   *
   * This design improves:
   * - rendering speed
   * - CPU efficiency
   * - cache locality
   *
   * The AST is still used for:
   * - expression evaluation
   * - include / extends support
   * - block inheritance
   * - future advanced compilation features
   */
  class Renderer
  {
  public:
    /**
     * @brief Block override map used during template inheritance.
     */
    using BlockMap = std::unordered_map<std::string, const BlockNode *>;

    /**
     * @brief Construct a renderer.
     *
     * @param auto_escape_html Whether variable output should be HTML-escaped.
     * @param loader Optional template loader used for include and extends.
     */
    explicit Renderer(
        bool auto_escape_html = true,
        std::shared_ptr<Loader> loader = nullptr);

    /**
     * @brief Render a compiled execution plan.
     *
     * @param plan Compiled execution plan.
     * @param context Runtime rendering context.
     * @return Final render result.
     */
    [[nodiscard]] RenderResult render(
        const ExecutionPlan &plan,
        const Context &context) const;

    /**
     * @brief Render a compiled execution plan with block overrides.
     *
     * This overload is mainly used internally for template inheritance.
     *
     * @param plan Compiled execution plan.
     * @param context Runtime rendering context.
     * @param overrides Active block override map.
     * @return Final render result.
     */
    [[nodiscard]] RenderResult render(
        const ExecutionPlan &plan,
        const Context &context,
        const BlockMap &overrides) const;

    [[nodiscard]] RenderResult render(
        const RootNode &root,
        const Context &context) const;

    [[nodiscard]] RenderResult render(
        const RootNode &root,
        const Context &context,
        const BlockMap &overrides) const;

  private:
    /**
     * @brief Execute all instructions in a plan.
     *
     * @param plan Execution plan to execute.
     * @param context Runtime rendering context.
     * @param output Output string buffer.
     */
    void execute_plan(
        const ExecutionPlan &plan,
        const Context &context,
        std::string &output) const;

    /**
     * @brief Execute a single instruction.
     *
     * @param instruction Instruction to execute.
     * @param instruction_index Current instruction index.
     * @param plan Execution plan being interpreted.
     * @param context Runtime rendering context.
     * @param output Output string buffer.
     */
    void execute_instruction(
        const Instruction &instruction,
        std::size_t &instruction_index,
        const ExecutionPlan &plan,
        const Context &context,
        std::string &output) const;

    /**
     * @brief Execute a text emission instruction.
     *
     * @param instr Text instruction payload.
     * @param output Output string buffer.
     */
    void execute_emit_text(
        const TextInstr &instr,
        std::string &output) const;

    /**
     * @brief Execute a variable emission instruction.
     *
     * @param instr Variable instruction payload.
     * @param context Runtime rendering context.
     * @param output Output string buffer.
     */
    void execute_emit_variable(
        const VariableInstr &instr,
        const Context &context,
        std::string &output) const;

    /**
     * @brief Execute a conditional jump instruction.
     *
     * @param instr Conditional jump payload.
     * @param instruction_index Mutable instruction pointer.
     * @param context Runtime rendering context.
     */
    void execute_jump_if_false(
        const JumpIfFalseInstr &instr,
        std::size_t &instruction_index,
        const Context &context) const;

    /**
     * @brief Execute an unconditional jump instruction.
     *
     * @param instr Jump payload.
     * @param instruction_index Mutable instruction pointer.
     */
    void execute_jump(
        const JumpInstr &instr,
        std::size_t &instruction_index) const;

    /**
     * @brief Execute a foreach-begin instruction.
     *
     * In V6 this hook prepares loop execution. The exact iteration strategy
     * can evolve later without changing the public renderer interface.
     *
     * @param instr Loop begin payload.
     * @param instruction_index Mutable instruction pointer.
     * @param context Runtime rendering context.
     */
    void execute_foreach_begin(
        const ForEachInstr &instr,
        std::size_t &instruction_index,
        const Context &context) const;

    /**
     * @brief Execute a foreach-end instruction.
     *
     * @param instr Loop end payload.
     * @param instruction_index Mutable instruction pointer.
     */
    void execute_foreach_end(
        const JumpInstr &instr,
        std::size_t &instruction_index) const;

    /**
     * @brief Execute an include instruction.
     *
     * @param instr Include instruction payload.
     * @param context Runtime rendering context.
     * @param output Output string buffer.
     */
    void execute_include(
        const IncludeInstr &instr,
        const Context &context,
        std::string &output) const;

    /**
     * @brief Evaluate an expression represented as text.
     *
     * In V6, some compiled instructions keep a canonical textual expression
     * representation produced by Compiler.
     *
     * @param expression_text Canonical expression string.
     * @param context Runtime rendering context.
     * @return Evaluated value.
     */
    [[nodiscard]] Value evaluate_compiled_expression(
        const std::string &expression_text,
        const Context &context) const;

    /**
     * @brief Evaluate an expression AST node.
     *
     * This remains the core evaluation entry point for expression execution.
     *
     * @param expr Expression to evaluate.
     * @param context Runtime rendering context.
     * @return Evaluated value.
     */
    [[nodiscard]] Value evaluate_expression(
        const Expression &expr,
        const Context &context) const;

    /**
     * @brief Evaluate a name expression.
     *
     * @param expr Name expression.
     * @param context Runtime rendering context.
     * @return Evaluated value.
     */
    [[nodiscard]] Value evaluate_name_expression(
        const NameExpression &expr,
        const Context &context) const;

    /**
     * @brief Evaluate a literal expression.
     *
     * @param expr Literal expression.
     * @return Evaluated value.
     */
    [[nodiscard]] Value evaluate_literal_expression(
        const LiteralExpression &expr) const;

    /**
     * @brief Evaluate a member access expression.
     *
     * @param expr Member expression.
     * @param context Runtime rendering context.
     * @return Evaluated value.
     */
    [[nodiscard]] Value evaluate_member_expression(
        const MemberExpression &expr,
        const Context &context) const;

    /**
     * @brief Evaluate a unary expression.
     *
     * @param expr Unary expression.
     * @param context Runtime rendering context.
     * @return Evaluated value.
     */
    [[nodiscard]] Value evaluate_unary_expression(
        const UnaryExpression &expr,
        const Context &context) const;

    /**
     * @brief Evaluate a binary expression.
     *
     * @param expr Binary expression.
     * @param context Runtime rendering context.
     * @return Evaluated value.
     */
    [[nodiscard]] Value evaluate_binary_expression(
        const BinaryExpression &expr,
        const Context &context) const;

    /**
     * @brief Resolve a top-level variable from the current context.
     *
     * @param name Variable name.
     * @param context Runtime rendering context.
     * @return Pointer to the resolved value, or nullptr if missing.
     */
    [[nodiscard]] const Value *resolve_variable(
        const std::string &name,
        const Context &context) const noexcept;

    /**
     * @brief Resolve a member from a parent runtime value.
     *
     * @param value Parent value.
     * @param member Member name.
     * @return Pointer to the resolved member value, or nullptr if missing.
     */
    [[nodiscard]] const Value *resolve_member(
        const Value &value,
        const std::string &member) const noexcept;

    /**
     * @brief Apply a filter pipeline to a value.
     *
     * @param input Initial value.
     * @param filters Ordered filter pipeline.
     * @return Transformed value.
     */
    [[nodiscard]] Value apply_filters(
        const Value &input,
        const std::vector<FilterNode> &filters) const;

    /**
     * @brief Check whether a runtime value is truthy.
     *
     * @param value Value to inspect.
     * @return True if the value is considered truthy.
     */
    [[nodiscard]] bool is_truthy(const Value &value) const noexcept;

    /**
     * @brief Convert a runtime value to a number when possible.
     *
     * @param value Input value.
     * @return Numeric representation.
     */
    [[nodiscard]] double to_number(const Value &value) const;

    /**
     * @brief Check whether a template is already being rendered.
     *
     * @param template_name Logical template name.
     * @return True if the template is already on the render stack.
     */
    [[nodiscard]] bool is_in_include_stack(
        const std::string &template_name) const noexcept;

    /**
     * @brief Find an extends node in the root.
     *
     * @param root Root AST node.
     * @return Pointer to ExtendsNode or nullptr if not found.
     */
    [[nodiscard]] const ExtendsNode *find_extends(
        const RootNode &root) const noexcept;

    /**
     * @brief Collect all block nodes from a template root.
     *
     * @param root Root AST node.
     * @return Map of block name to BlockNode pointer.
     */
    [[nodiscard]] BlockMap collect_blocks(
        const RootNode &root) const;

    /**
     * @brief Render a template by name.
     *
     * Used for includes and extends resolution.
     *
     * @param template_name Logical template name.
     * @param context Runtime rendering context.
     * @param output Output string buffer.
     */
    void render_template_by_name(
        const std::string &template_name,
        const Context &context,
        std::string &output) const;

    /**
     * @brief Render a block node with override support.
     *
     * @param node Block node.
     * @param context Runtime rendering context.
     * @param output Output string buffer.
     */
    void render_block(
        const BlockNode &node,
        const Context &context,
        std::string &output) const;

    /**
     * @brief Render a list of AST nodes.
     *
     * Utility used for block rendering and compatibility paths.
     *
     * @param nodes List of nodes.
     * @param context Runtime rendering context.
     * @param output Output string buffer.
     */
    void render_node_list(
        const NodeList &nodes,
        const Context &context,
        std::string &output) const;

    /**
     * @brief Render a generic AST node.
     *
     * This helper remains available for compatibility paths still based on
     * direct AST rendering, especially for inheritance and transitional code.
     *
     * @param node AST node.
     * @param context Runtime rendering context.
     * @param output Output string buffer.
     */
    void render_node(
        const Node &node,
        const Context &context,
        std::string &output) const;

  private:
    /**
     * @brief Whether HTML escaping is enabled for variable output.
     */
    bool auto_escape_html_{true};

    /**
     * @brief Built-in filter registry.
     */
    std::unordered_map<std::string, Filter> filters_;

    /**
     * @brief Template loader used to resolve include and extends nodes.
     */
    std::shared_ptr<Loader> loader_;

    /**
     * @brief Current render stack used to detect circular includes or extends.
     */
    mutable std::vector<std::string> include_stack_;

    /**
     * @brief Active block overrides for template inheritance.
     */
    mutable BlockMap block_overrides_;
  };

} // namespace vix::template_

#endif // VIX_TEMPLATE_RENDERER_HPP
