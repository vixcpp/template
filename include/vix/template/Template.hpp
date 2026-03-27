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
#include <utility>

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
   * - an optional source signature used by the cache layer
   *
   * In V6, rendering uses the precomputed execution plan instead of
   * traversing the AST recursively for each render call.
   *
   * In V7, Template also carries a source signature so the cache layer
   * can validate freshness and invalidate stale compiled templates.
   *
   * The AST is still preserved because it remains useful for:
   * - debugging
   * - inspection
   * - future compiler passes
   * - advanced template features
   *
   * Template is move-only because the underlying AST owns nodes through
   * unique ownership semantics.
   */
  class Template
  {
  public:
    /**
     * @brief Construct an empty template.
     */
    Template() = default;

    /**
     * @brief Disable copy construction.
     *
     * Template owns a move-only AST, so copying is not allowed.
     *
     * @param other Template to copy from.
     */
    Template(const Template &other) = delete;

    /**
     * @brief Disable copy assignment.
     *
     * Template owns a move-only AST, so copying is not allowed.
     *
     * @param other Template to copy from.
     * @return This template.
     */
    Template &operator=(const Template &other) = delete;

    /**
     * @brief Enable move construction.
     *
     * Transfers ownership of the compiled template state.
     *
     * @param other Template to move from.
     */
    Template(Template &&other) noexcept = default;

    /**
     * @brief Enable move assignment.
     *
     * Transfers ownership of the compiled template state.
     *
     * @param other Template to move from.
     * @return This template.
     */
    Template &operator=(Template &&other) noexcept = default;

    /**
     * @brief Construct a template from a name, AST root, and execution plan.
     *
     * @param name Logical template name.
     * @param root Parsed or optimized root AST.
     * @param plan Compiled execution plan.
     * @param loader Optional loader used for include resolution.
     * @param source_signature Optional source freshness token.
     */
    Template(
        std::string name,
        RootNode root,
        ExecutionPlan plan,
        std::shared_ptr<Loader> loader = nullptr,
        std::string source_signature = {});

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

    /**
     * @brief Get the source signature associated with this template.
     *
     * This value is intended for cache freshness checks.
     *
     * @return Source signature, possibly empty.
     */
    [[nodiscard]] const std::string &source_signature() const noexcept;

    /**
     * @brief Replace the source signature.
     *
     * Useful when the compiler/engine computes a freshness token after
     * construction.
     *
     * @param source_signature New source signature.
     */
    void set_source_signature(std::string source_signature);

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

    /**
     * @brief Source freshness token for cache validation.
     */
    std::string source_signature_;
  };

} // namespace vix::template_

#endif // VIX_TEMPLATE_TEMPLATE_HPP
