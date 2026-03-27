/**
 *
 *  @file Template.hpp
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
#ifndef VIX_TEMPLATE_TEMPLATE_HPP
#define VIX_TEMPLATE_TEMPLATE_HPP

#include <memory>
#include <string>

#include <vix/template/AST.hpp>
#include <vix/template/Context.hpp>
#include <vix/template/Loader.hpp>
#include <vix/template/RenderResult.hpp>

namespace vix::template_
{
  class Renderer;

  /**
   * @brief Compiled template ready for rendering.
   *
   * Template owns:
   * - the template logical name
   * - the parsed root AST
   * - an optional loader used for include resolution
   *
   * It provides a simple rendering entry point that delegates
   * the actual output generation to Renderer.
   */
  class Template
  {
  public:
    /**
     * @brief Construct an empty template.
     */
    Template() = default;

    /**
     * @brief Construct a template from a name and parsed AST root.
     *
     * @param name Logical template name.
     * @param root Parsed root AST.
     * @param loader Optional loader used for include resolution.
     */
    Template(
        std::string name,
        RootNode root,
        std::shared_ptr<Loader> loader = nullptr);

    /**
     * @brief Get the template logical name.
     *
     * @return Template name.
     */
    [[nodiscard]] const std::string &name() const noexcept;

    /**
     * @brief Get the parsed AST root.
     *
     * @return Immutable root node.
     */
    [[nodiscard]] const RootNode &root() const noexcept;

    /**
     * @brief Check whether the template is empty.
     *
     * A template is considered empty when it has no child nodes.
     *
     * @return True if empty.
     */
    [[nodiscard]] bool empty() const noexcept;

    /**
     * @brief Render the template with a context.
     *
     * @param context Runtime render context.
     * @param auto_escape_html Whether variable output should be HTML-escaped.
     * @return Render result.
     */
    [[nodiscard]] RenderResult render(
        const Context &context,
        bool auto_escape_html = true) const;

    /**
     * @brief Get the loader associated with this template.
     *
     * @return Loader used for include resolution, or nullptr.
     */
    [[nodiscard]] const std::shared_ptr<Loader> &loader() const noexcept;

  private:
    /**
     * @brief Logical template name.
     */
    std::string name_;

    /**
     * @brief Parsed template AST root.
     */
    RootNode root_;

    /**
     * @brief Loader used for include resolution.
     */
    std::shared_ptr<Loader> loader_{nullptr};
  };

} // namespace vix::template_

#endif // VIX_TEMPLATE_TEMPLATE_HPP
