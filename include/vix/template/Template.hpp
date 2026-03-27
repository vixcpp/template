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
#include <vix/template/ExecutionPlan.hpp>
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
   * - the parsed and optimized AST root
   * - the compiled execution plan
   * - an optional loader used for include resolution
   *
   * In V6, rendering is intended to use the precomputed execution plan
   * instead of traversing the AST recursively for each render call.
   *
   * The AST is still preserved because it remains useful for:
   * - debugging
   * - inspection
   * - future compiler passes
   * - advanced template features
   */
  class Template
  {
  public:
    /**
     * @brief Construct an empty template.
     */
    Template() = default;

    /**
     * @brief Construct a template from a name, AST root, and execution plan.
     *
     * @param name Logical template name.
     * @param root Parsed or optimized root AST.
     * @param plan Compiled execution plan.
     * @param loader Optional loader used for include resolution.
     */
    Template(
        std::string name,
        RootNode root,
        ExecutionPlan plan,
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
     * @brief Get the compiled execution plan.
     *
     * @return Immutable execution plan.
     */
    [[nodiscard]] const ExecutionPlan &plan() const noexcept;

    /**
     * @brief Check whether the template is empty.
     *
     * A template is considered empty when it has no instructions in its
     * execution plan.
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
     * @brief Compiled execution plan.
     */
    ExecutionPlan plan_;

    /**
     * @brief Loader used for include resolution.
     */
    std::shared_ptr<Loader> loader_{nullptr};
  };

} // namespace vix::template_

#endif // VIX_TEMPLATE_TEMPLATE_HPP
