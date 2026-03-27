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

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <vix/template/AST.hpp>
#include <vix/template/Builtins.hpp>
#include <vix/template/Context.hpp>
#include <vix/template/Loader.hpp>
#include <vix/template/RenderResult.hpp>
#include <vix/template/Value.hpp>

namespace vix::template_
{
  /**
   * @brief AST renderer for template output generation.
   *
   * Renderer walks the parsed AST and produces the final textual output
   * using the provided runtime context.
   *
   * Supported nodes:
   * - RootNode
   * - TextNode
   * - VariableNode
   * - IfNode
   * - ForNode
   * - IncludeNode
   * - ExtendsNode
   * - BlockNode
   *
   * V5 adds expression support:
   * - {{ user.name }}
   * - {{ price * quantity }}
   * - {{ a == b }}
   * - {% if price * quantity > 0 %} ... {% endif %}
   *
   * Variable nodes may still include a filter pipeline such as:
   * - {{ name | upper }}
   * - {{ items | length }}
   * - {{ price * quantity | string }}
   *
   * Include nodes allow template composition:
   * - {% include "header.html" %}
   *
   * Extends and block nodes allow layout inheritance:
   * - {% extends "base.html" %}
   * - {% block content %} ... {% endblock %}
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
     * @brief Render a parsed template tree.
     *
     * @param root Parsed root AST node.
     * @param context Runtime rendering context.
     * @return Final render result.
     */
    [[nodiscard]] RenderResult render(
        const RootNode &root,
        const Context &context) const;

    /**
     * @brief Render a parsed template tree with block overrides.
     *
     * This overload is mainly used internally to resolve template
     * inheritance chains.
     *
     * @param root Parsed root AST node.
     * @param context Runtime rendering context.
     * @param overrides Active block override map.
     * @return Final render result.
     */
    [[nodiscard]] RenderResult render(
        const RootNode &root,
        const Context &context,
        const BlockMap &overrides) const;

  private:
    /**
     * @brief Render a generic node into the output buffer.
     *
     * @param node AST node to render.
     * @param context Runtime rendering context.
     * @param output Output string buffer.
     */
    void render_node(
        const Node &node,
        const Context &context,
        std::string &output) const;

    /**
     * @brief Render a root node.
     *
     * @param node Root node.
     * @param context Runtime rendering context.
     * @param output Output string buffer.
     */
    void render_root(
        const RootNode &node,
        const Context &context,
        std::string &output) const;

    /**
     * @brief Render a text node.
     *
     * @param node Text node.
     * @param output Output string buffer.
     */
    void render_text(
        const TextNode &node,
        std::string &output) const;

    /**
     * @brief Render a variable node.
     *
     * Variable rendering in V5 works as follows:
     * 1. evaluate the expression
     * 2. apply filters from left to right
     * 3. convert the final value to string
     * 4. escape HTML if enabled
     *
     * @param node Variable node.
     * @param context Runtime rendering context.
     * @param output Output string buffer.
     */
    void render_variable(
        const VariableNode &node,
        const Context &context,
        std::string &output) const;

    /**
     * @brief Render an if node.
     *
     * The condition expression is evaluated first. If the result is truthy,
     * the body is rendered.
     *
     * @param node If node.
     * @param context Runtime rendering context.
     * @param output Output string buffer.
     */
    void render_if(
        const IfNode &node,
        const Context &context,
        std::string &output) const;

    /**
     * @brief Render a for node.
     *
     * @param node For node.
     * @param context Runtime rendering context.
     * @param output Output string buffer.
     */
    void render_for(
        const ForNode &node,
        const Context &context,
        std::string &output) const;

    /**
     * @brief Render an include node.
     *
     * The included template is loaded through the configured Loader,
     * parsed, and rendered with the same runtime context.
     *
     * @param node Include node.
     * @param context Runtime rendering context.
     * @param output Output string buffer.
     */
    void render_include(
        const IncludeNode &node,
        const Context &context,
        std::string &output) const;

    /**
     * @brief Render a block node with override support.
     *
     * If an override exists for this block name from a child template,
     * the override is rendered instead of the current block body.
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
     * Utility used to render block bodies and nested structures.
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
     * @brief Evaluate an expression.
     *
     * This is the core V5 entry point for expression execution.
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
     * Example:
     * - user
     * - price
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
     * Examples:
     * - 42
     * - 3.14
     * - true
     * - false
     * - "hello"
     *
     * @param expr Literal expression.
     * @return Evaluated value.
     */
    [[nodiscard]] Value evaluate_literal_expression(
        const LiteralExpression &expr) const;

    /**
     * @brief Evaluate a member access expression.
     *
     * Examples:
     * - user.name
     * - order.total
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
     * Examples:
     * - !enabled
     * - -price
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
     * Examples:
     * - price * quantity
     * - a == b
     * - total >= limit
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
     * This is used for expression evaluation such as:
     * - user.name
     * - order.total
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
     * Filters are applied from left to right.
     *
     * Example:
     * {{ name | upper | length }}
     *
     * This means:
     * 1. evaluate name
     * 2. apply upper
     * 3. apply length
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
     * Used by condition evaluation and unary logical operations.
     *
     * @param value Value to inspect.
     * @return True if the value is considered truthy.
     */
    [[nodiscard]] bool is_truthy(const Value &value) const noexcept;

    /**
     * @brief Convert a runtime value to a number when possible.
     *
     * This helper is used by arithmetic and comparison operators.
     *
     * @param value Input value.
     * @return Numeric representation.
     */
    [[nodiscard]] double to_number(const Value &value) const;

    /**
     * @brief Check whether a template is already being rendered.
     *
     * This is used to prevent circular includes and inheritance loops.
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
     * Used to build the override map for template inheritance.
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
