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

namespace vix::template_
{
  /**
   * @brief AST renderer for template output generation.
   *
   * Renderer walks the parsed AST and produces the final textual output
   * using the provided runtime context.
   *
   * Supported V3 nodes:
   * - RootNode
   * - TextNode
   * - VariableNode
   * - IfNode
   * - ForNode
   * - IncludeNode
   *
   * Variable nodes may include a filter pipeline such as:
   * - {{ name | upper }}
   * - {{ items | length }}
   *
   * Include nodes allow template composition:
   * - {% include "header.html" %}
   */
  class Renderer
  {
  public:
    /**
     * @brief Construct a renderer.
     *
     * @param auto_escape_html Whether variable output should be HTML-escaped.
     * @param loader Optional template loader used for include nodes.
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
     * Variable rendering resolves the source variable, applies filters
     * in declaration order, then converts the final value to string.
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
     * @brief Resolve a variable from the current context.
     *
     * @param name Variable name.
     * @param context Runtime rendering context.
     * @return Pointer to the resolved value, or nullptr if missing.
     */
    [[nodiscard]] const Value *resolve_variable(
        const std::string &name,
        const Context &context) const noexcept;

    /**
     * @brief Apply a filter pipeline to a value.
     *
     * Filters are applied from left to right.
     *
     * Example:
     * {{ name | upper | length }}
     *
     * This means:
     * 1. resolve name
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
     * @brief Check whether a template is already being rendered.
     *
     * This is used to prevent circular includes.
     *
     * @param template_name Logical template name.
     * @return True if the template is already on the include stack.
     */
    [[nodiscard]] bool is_in_include_stack(
        const std::string &template_name) const noexcept;

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
     * @brief Template loader used to resolve include nodes.
     */
    std::shared_ptr<Loader> loader_;

    /**
     * @brief Current include stack used to detect circular includes.
     */
    mutable std::vector<std::string> include_stack_;
  };

} // namespace vix::template_

#endif // VIX_TEMPLATE_RENDERER_HPP
