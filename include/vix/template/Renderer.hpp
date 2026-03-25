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

#include <string>

#include <vix/template/AST.hpp>
#include <vix/template/Context.hpp>
#include <vix/template/RenderResult.hpp>

namespace vix::template_
{
  /**
   * @brief AST renderer for template output generation.
   *
   * Renderer walks the parsed AST and produces the final textual output
   * using the provided runtime context.
   *
   * Supported V1 nodes:
   * - RootNode
   * - TextNode
   * - VariableNode
   * - IfNode
   * - ForNode
   */
  class Renderer
  {
  public:
    /**
     * @brief Construct a renderer.
     *
     * @param auto_escape_html Whether variable output should be HTML-escaped.
     */
    explicit Renderer(bool auto_escape_html = true);

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
     * @brief Resolve a variable from the current context.
     *
     * @param name Variable name.
     * @param context Runtime rendering context.
     * @return Pointer to the resolved value, or nullptr if missing.
     */
    [[nodiscard]] const Value *resolve_variable(
        const std::string &name,
        const Context &context) const noexcept;

  private:
    /**
     * @brief Whether HTML escaping is enabled for variable output.
     */
    bool auto_escape_html_{true};
  };

} // namespace vix::template_

#endif // VIX_TEMPLATE_RENDERER_HPP
